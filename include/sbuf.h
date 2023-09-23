//
// Created by 杨君鹏 on 2023/9/23.
//

#ifndef NETWORKPROCEDURE_SBUF_H
#define NETWORKPROCEDURE_SBUF_H

#include <semaphore.h>


/* Semaphore Programing */
typedef struct{
    int *buf;       /* Buffer array */
    int n;          /* Maximum number of slots */
    int front;      /* buf[(front + 1)%n] is first item */
    int rear;       /* BUF[rear%n] is last item */
    sem_t mutex;    /* Protects accesses to buf */
    sem_t slots;    /* Counts available slots */
    sem_t items;    /* Counts available items */
} sbuf_t;

void sbuf_init(sbuf_t *sp,int n );
void sbuf_deinit(sbuf_t *sp);
void sbuf_insert(sbuf_t *sp,int item);
int sbuf_remove(sbuf_t *sp);

#endif //NETWORKPROCEDURE_SBUF_H
