//
// Created by 杨君鹏 on 2023/1/16.
//
#include <netdb.h>
#include "../include/csapp.h"
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <errno.h> //errorno
#include <sys/types.h>

/* Virtual Memory */
//void *mmap(void *start,size_t length,int prot,int flags,int fd,off_t offset){
//
//}
//
//int munmap(void *start, size_t length){
//
//}


/*  System IO */
ssize_t rio_readn(int fd,void *usrbuf,size_t n){
    size_t nleft = n;
    ssize_t nread;
    char *bufp = (char *)usrbuf;
    while (nleft > 0){
        if ((nread == read(fd,bufp,nleft)) < 0){
            if (errno == EINTR)     /* Interrupted by sig handler return */
                nread = 0;          /* and call read() again */
            else
                return -1;          /* errno set by read() */
        }else if(nread == 0)
            break;                  /* EOF */
        nleft -= nread;
        bufp += nread;
    }
    return (n-nleft);               /* Return >= 0 */
}

ssize_t rio_writen(int fd,void *usrbuf,size_t n){
    size_t nleft = n;
    ssize_t nwritten;
    char *bufp = (char *)usrbuf;

    while(nleft > 0){
        if ((nwritten == write(fd,bufp,nleft)) <= 0 ){
            if (errno == EINTR)     /* Interrupted by sig handler return */
                nwritten = 0;       /* and call read() again */
            else
                return -1;          /* errno set by write() */
        }
        nleft -= nwritten;
        bufp += nwritten;
    }
    return n;
}

static ssize_t rio_read(rio_t *rp,char *usrbuf,size_t n){
    int cnt;

    while(rp->rio_cnt <= 0){    /* Refill if buf is empty*/
        rp->rio_cnt = read(rp->rio_fd,rp->rio_buf,sizeof(rp->rio_buf));

        if (rp->rio_cnt < 0){
            if (errno != EINTR) {    /* Interrupted by sig handler return */
                return -1;
            }
        }else if (rp->rio_cnt == 0)     /* EOF */
            return 0;
        else
            rp->rio_bufptr = rp->rio_buf;   /* Reset buffer ptr*/
    }

    /* Copy min(n,rp->rio_cnt) bytes from internal buf to user buf */
    cnt = n;
    if (rp->rio_cnt < n)
        cnt = rp->rio_cnt;
    memcpy(usrbuf,rp->rio_bufptr,cnt);
    rp->rio_bufptr += cnt;
    rp->rio_cnt -= cnt;
    return cnt;
}

void rio_readinitb(rio_t *rp,int fd){
    rp->rio_fd = fd;
    rp->rio_cnt = 0;
    rp->rio_bufptr = rp->rio_buf;
}

ssize_t rio_readlineb(rio_t *rp,void *usrbuf,size_t maxlen){
    int n,rc;
    char c,*bufp = (char*)usrbuf;

    for(n = 1;n < maxlen; n++){
        if((rc = rio_read(rp,&c,1)) == 1 ){
            *bufp++ = c;
            if (c == '\n'){
                n ++;
                break;
            }
        }else if (rc == 0){
            if (n == 1)
                return 0;       /* EOF,no data read */
            else
                break;          /* EOF,some data was read */
        }else
            return -1;          /* Error */
    }
    *bufp = 0;
    return n-1;
}

ssize_t rio_readnb(rio_t *rp,void *usrbuf,size_t n){
    size_t nleft = n;
    ssize_t nread;
    char *bufp = (char *)usrbuf;

    while(nleft > 0){
        if((nread = rio_read(rp,bufp,nleft)) < 0)
            return -1;                              /* errno set by read() */
        else if (nread == 0)
            break;                                  /* EOF */
        nleft -= nread;
        bufp += nread;
    }
    return (n - nleft);                             /* Return >= 0 */
}

/*  Network Programing */
int open_clientfd(char *hostname,char *port){
    int clientfd;
    struct addrinfo hints,*listp,*p;

    /* Get a list of potential server addresses */
    memset(&hints,0,sizeof (struct addrinfo));
    hints.ai_socktype = SOCK_STREAM;   /*Open a connection */
    hints.ai_flags = AI_NUMERICSERV;   /*...using a numeric port arg.*/
    hints.ai_flags |= AI_ADDRCONFIG;   /*Recommended for connections */
    getaddrinfo(hostname,port,&hints,&listp);

    /*Walk the list for one that we can successfully connect to */
    for (p = listp; p; p= p->ai_next) {
        /*Create a socket descriptor*/
        if ((clientfd = socket(p->ai_family,p->ai_socktype,p->ai_protocol)) < 0)continue;/* Socket failed,try the next */

        /* Connect to the server */
        if (connect(clientfd,p->ai_addr,p->ai_addrlen) != -1) break; /* Success */
        close(clientfd); /* Connect failed ,try another */

    }

    /* Clean up */
    freeaddrinfo(listp);
    if (!p) /* All connects failed*/
        return -1;
    else    /* The last connect succeeded */
        return clientfd;
}




//struct addrinfo {
//    int	ai_flags;	/* AI_PASSIVE, AI_CANONNAME, AI_NUMERICHOST */
//    int	ai_family;	/* PF_xxx */
//    int	ai_socktype;	/* SOCK_xxx */
//    int	ai_protocol;	/* 0 or IPPROTO_xxx for IPv4 and IPv6 */
//    socklen_t ai_addrlen;	/* length of ai_addr */
//    char	*ai_canonname;	/* canonical name for hostname */
//    struct	sockaddr *ai_addr;	/* binary address */
//    struct	addrinfo *ai_next;	/* next structure in linked list */
//};


int open_listenfd(char *port){

    struct addrinfo hints, *listp, *p;
    int listenfd, optval = 1;

    /*Get a list of potential server addresses */
    memset(&hints,0,sizeof(struct addrinfo));

    hints.ai_socktype=SOCK_STREAM;                      //Accept connections
    hints.ai_flags=AI_PASSIVE | AI_ADDRCONFIG;          //on any IP address
    hints.ai_flags |= AI_NUMERICSERV;                   //using port number
    getaddrinfo(NULL,port,&hints,&listp);

    /* Walk the list for one that we can bind to */
    for(p = listp;p;p = p->ai_next){
        /* Create a socket descriptor */
        if((listenfd = socket(p->ai_family,p->ai_socktype,p->ai_protocol)) < 0) continue;   /* Socket failed,try the next */

        /* Eliminates "Address already in use" error from bind */
        setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,(const void *)&optval,sizeof(int));

        /* Bind the descriptor to the address */
        if (bind(listenfd,p->ai_addr,p->ai_addrlen) == 0)
            break;      /* Success */
        close(listenfd);        /* Bind failed, try the next */
    }

    /* Clean up */
    freeaddrinfo(listp);
    if(!p)  /* No address worked */
        return -1;

        /* Make it a listening socket ready to accept connection requests */
    if (listen(listenfd,LISTENQ) < 0 ){
        close(listenfd);
        return -1;
    }
    return listenfd;
}


void echo(int connfd){
    size_t n;
    char buf[MAXLINE];
    rio_t rio;

    rio_readinitb(&rio,connfd);
    while((n = rio_readlineb(&rio,buf,MAXLINE)) != 0){
        printf("server received %d bytes\n",(int)n);
        rio_writen(connfd,buf,n);
    }

}