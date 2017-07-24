/*
 *
 */

#ifndef _HEADER_H
#define _HEADER_H

#ifndef __USE_GNU
#define __USE_GNU /* RUSAGE_THREADA  */
#endif

#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

#define SUCCESS     0
#define ERROR       1

extern const char *sync_name[];

typedef enum { 
    SYNC_NON, 
    SYNC_MTX, 
    SYNC_SPL, 
    SYNC_SEM, 
    SYNC_CDV, 
    SYNC_RWL 
} sync_type;

typedef enum { 
    THREAD_READER, 
    THREAD_WRITER 
} thread_type;

typedef struct semaphore_s {
    int r_count;                           //  4 B
    sem_t r_sem_t, w_sem_t;              // 64 B 
} semaphore;                                     // 68 B

typedef struct condition_variable_s{
    int writer_count, reader_count;             //  8 B
    pthread_mutex_t     cv_mutex_t;             // 40 B
    pthread_cond_t      cv_t;                   // 48 B
} condition_variable;                            // 96 B

typedef struct sync_thread_args{
    sync_type type;
    union syncronization_variable{
        pthread_mutex_t     mutex_t;            // 40 B
        pthread_spinlock_t  spinlock_t;         //  4 B 
        semaphore semaphore_t;                  // 72 B
        condition_variable condition_variable_t;     // 96 B       
        pthread_rwlock_t    rwlock_t;           // 56 B
    } sync;                                     // 96 B

    void* (*reader_func)(void*);
    void* (*writer_func)(void*);
    int shd_v;    
} thread_args;

typedef struct sync_thread_rets {
    struct timeval tv_start, tv_end;
} thread_rets;

/* Synchronization functions */
void* read_mutex(void* arg);
void* write_mutex(void* arg);
void* read_spl(void* arg);
void* write_spl(void* arg);
void* read_sem(void* arg);
void* write_sem(void* arg);
void* read_cdv(void* arg);
void* write_cdv(void* arg);
void* read_rwl(void* arg);
void* write_rwl(void* arg);


/* communication functions */

/* time usage related function */
#define THRET_START(th_ret) \
    gettimeofday(th_ret->tv_start, NULL);

#define THRET_END(th_ret) \
    gettimeofday(th_ret->tv_end, NULL);

/* time related functions  */
static double get_time_usage(struct timeval *tv_start, struct timeval *tv_end){

    double tv_s = tv_start->tv_sec + (tv_start->tv_usec / 1000000.0);
    double tv_e = tv_end->tv_sec + (tv_end->tv_usec / 1000000.0);

    return (tv_e - tv_s);
}


#endif /* _HEADER_H*/
