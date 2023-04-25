//
// Created by 杨君鹏 on 2023/2/15.
//
#include <stdio.h>
#include "../include/csapp.h"

typedef struct{ /* Represents a pool of connected descriptions */
    int maxfd;         /* Largest descriptor in read_set */
    fd_set read_set;   /* Set of all active descriptors */
    fd_set ready_set;  /* Subset of all active descriptors */
    int nready;        /* Number of ready descriptors from select */
    int maxi;          /* High water index into client array */
    int clientfd[FD_SETSIZE];   /* Set of active descriptors */
    rio_t clientrio[FD_SETSIZE];   /* Set of active read buffers */
} pool;

void init_pool(int listenfd,pool *p);
void add_client(int connfd,pool *p);
void check_clients(pool *p);

int byte_cnt = 0; /* Counts total bytes received by server */

int main(int argc,char **argv){
    int listenfd,connfd;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    static pool pool;

    if (argc != 2){
        fprintf(stderr,"usage: %s <port>\n",argv[0]);
        exit(0);
    }

    //Monitoring pthread port
    listenfd= open_listenfd(argv[1]);

    //Initially Reuse pool
    init_pool(listenfd,&pool);

    while(1){
        /* Wait for listening/connected descriptor(s) to become ready */
        pool.ready_set = pool.read_set;
        pool.nready = select(pool.maxfd+1,&pool.ready_set,NULL,NULL,NULL);


        /* If listening descriptor ready,add new client to pool */
        if (FD_ISSET(listenfd,&pool.ready_set)){
            clientlen = sizeof(struct sockaddr_storage);
            connfd = accept(listenfd,(SA*)&clientaddr,&clientlen);
            add_client(connfd,&pool);
        }


        /* Echo a text line from each ready connected descriptor */
        check_clients(&pool);
    }
}

void init_pool(int listenfd,pool *p){
    /* Initially,there are no connected descriptors */
    int i;
    p->maxi = -1;
    for ( i = 0; i < FD_SETSIZE; ++i) {
        p->clientfd[i] = -1;
    }

    /* Initially,listenfd is only member of select read set */
    p->maxfd = listenfd;
    FD_ZERO(&p->read_set);
    FD_SET(listenfd,&p->read_set);
}


void add_client(int connfd,pool *p){
    int i;
    p->nready--;
    for (int i  = 0; i < FD_SETSIZE; i++) {
        if (p->clientfd[i] < 0){
            /* Add connected descriptor to the pool */
            p->clientfd[i] = connfd;
            rio_readinitb(&p->clientrio[i],connfd);

            /* Add the descriptor to descriptor set */
            FD_SET(connfd,&p->read_set);

            /* Update max descriptor and pool high water mark */
            if (connfd > p->maxfd)
                p->maxfd = connfd;
            if (i > p->maxi)
                p->maxi = i;
            break;
        }

        if (i == FD_SETSIZE) /* Couldn't find an empty slot */
            printf("None");
//            app_error("add client error: Too many clients");
    }
}


void check_clients(pool *p){
    int i ,connfd,n;
    char buf[MAXLINE];
    rio_t  rio;

    for ( i = 0; (i <= p->maxi) && (p->nready > 0); ++i) {
        connfd = p->clientfd[i];
        rio = p->clientrio[i];

        /* If the descriptor is ready,echo a text line from it */
        if ((connfd > 0) && (FD_ISSET(connfd,&p->ready_set))){
            p->nready--;
            if ((n = rio_readlineb(&rio,buf,MAXLINE)) != 0){
                byte_cnt += n;
                printf("Server received %d (%d total) bytes on fd %d\n",n,byte_cnt,connfd);
                rio_writen(connfd,buf,n);
            }

            /* EOF detected,remove descriptor from pool */
            else{
                close(connfd);
                FD_CLR(connfd,&p->read_set);
                p->clientfd[i] = -1;
            }
        }
    }
}

