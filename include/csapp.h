//
// Created by 杨君鹏 on 2023/1/17.
//

#ifndef NETWORKPROGRAMINGINC_CSAPP_H
#define NETWORKPROGRAMINGINC_CSAPP_H
#define RIO_BUFSIZE 8192
#define MAXLINE 1000
#define MAXARGS 128
#define MAXBUF 1024
#define MAXN 15213
#define LISTENQ 1024

#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>     //socket
#include <netdb.h>          //getnameinfo


extern char **environ;

/* Virtual Memory */
void *mmap(void *start,size_t length,int prot,int flags,int fd,off_t offset);
int munmap(void *start, size_t length);


/*  System IO */
typedef struct {
    int rio_fd;         //Descriptor for this internal buf
    int rio_cnt;        //Unread bytes in internal buf
    char *rio_bufptr;   //Next unread byte in internal buf
    char rio_buf[RIO_BUFSIZE];      //Internal buffer
} rio_t;

ssize_t rio_readn(int fd,void *usrbuf,size_t n);
ssize_t rio_writen(int fd,void *usrbuf,size_t n);
void rio_readinitb(rio_t *rp,int fd);
ssize_t rio_readlineb(rio_t *rp,void *usrbuf,size_t maxlen);
ssize_t rio_readnb(rio_t *rp,void *usrbuf,size_t n);



/*  Network Programing */
//  Request Line Construct
//-----------------------------------------------
//|  Http Method       URI          HTTP VERSION |
//-----------------------------------------------
//       |              |                 |
//      GET           /uri              HTTP/1.1
//-------------------------------------------------

typedef struct sockaddr SA;
int open_clientfd(char *hostname,char *port);
int open_listenfd(char *port);
void echo(int connfd);
extern void mem_set();
#endif //NETWORKPROGRAMINGINC_CSAPP_H
