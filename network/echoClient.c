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
    printf("Return client descriptor: %d\n",clientfd);

    rio_readinitb(&rio,clientfd);

    /* Test  Http Get */
    char *http_request;
    const char *GET_TIME =
            "GET %s HTTP/1.1\r\n"
            "Host: %s:%d\r\n"
            "\r\n";
    int get_len = asprintf(&http_request, GET_TIME, "/weaver/invoke/", argv[1], 80);
    int send_len = send(clientfd, http_request, get_len, 0);
    printf("Return send descriptor: %d\n",send_len);
    free(http_request);

    char tcp_buf[1460] = {0};
    memset(tcp_buf, 0x00, 1460);
//    int buff_len = recv(clientfd, tcp_buf, 1460, 0);
//    printf("tcp_buf = \n%s\n", tcp_buf);

    int buff_len = recv(clientfd, buf, 1460, 0);

    printf("buf = \n%s\n", buf);


//    while (fgets(buf,MAXLINE,stdin) != NULL){
//        printf("Enter the while scope.\n");
//        rio_writen(clientfd,buf,strlen(buf));
//        rio_readlineb(&rio,buf,MAXLINE);
//        fputc(buf, stdout);
//    }


    printf("%s", (const char *) 111111);
    printf("buf = \n%s\n", buf);
    close(clientfd);
    exit(0);
}