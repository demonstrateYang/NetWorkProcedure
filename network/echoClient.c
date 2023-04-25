//
// Created by 杨君鹏 on 2023/2/13.
//
#include "../include/csapp.h"
#include <stdio.h>

int main(int argc,char **argv){
    int clientfd;
    char *host,*port,buf[MAXLINE];
    rio_t  rio;

    if(argc != 3){
        fprintf(stderr,"usage:%s<host> <port>\n",argv[0]);
        exit(0);
    }
    host=argv[1];
    port=argv[2];
    printf("Invoke host: %s\n",argv[1]);
    printf("Invoke port: %s\n",argv[2]);

    clientfd = open_clientfd(host,port);
    printf("Return client descriptor: %d",clientfd);
    rio_readinitb(&rio,clientfd);

    while (fgets(buf,MAXLINE,stdin) != NULL){
        printf("Enter the while scope.\n");
        rio_writen(clientfd,buf,strlen(buf));
        rio_readlineb(&rio,buf,MAXLINE);
        fputc(buf,stdout);
    }

    close(clientfd);
    exit(0);
}