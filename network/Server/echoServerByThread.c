//
// Created by 杨君鹏 on 2023/9/19.
//

#include "../../include/csapp.h"
#include <pthread.h>

void echo(int connfd);
void *thread(void *vargp);

int main(int argc,char **argv){
    int listenfd,*connfdp;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    pthread_t tid;

    if(argc != 2){
        fprintf(stderr,"usage: %s <port>\n",argv[0]);
    }
    listenfd = open_listenfd(argv[1]);

    while (1){
        clientlen = sizeof (struct sockaddr_storage);
        connfdp = malloc(sizeof (int ));
        *connfdp = accept(listenfd,(SA *)&clientaddr,&clientlen);
        pthread_create(&tid,NULL,thread,connfdp);
    }

}

/* Thread routine(线程例程) */
void *thread(void *vargp){
    int connfd = *((int *)vargp);
    pthread_detach(pthread_self());
    free(vargp);
    echo(connfd);
    close(connfd);
    return NULL;
}

