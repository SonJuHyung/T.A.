/*
 * ITRC Kernel Lecture. 
 *
 *  Unix communication & synchronization Primitives.
 *
 *  communication : exchanging data between processes.
 *      - data transfer
 *          - byte stread
 *              - PIPE(unamed pipe)
 *              - FIFO(named pipe)
 *              - stream socket
 *          - pesudoterminal
 *          - message
 *              - System V message queue
 *              - POSIX message queue
 *              - datagram socket 
 *      - shared memory
 *          - System V shared memory
 *          - POSIX shared memoryu
 *          - memory mapping
 *              - anonymous mapping
 *              - mapped file
 *
 *  signal 
 *      - standard signal
 *      - realtime signal
 *
 *  synchronization : synchronizing the actions of processes or threads.
 *      - semaphore 
 *          - System V semaphore
 *          - POSIX semaphore
 *              - named
 *              - unnamed
 *      - file lock
 *          - "record" lock(fcntl())
 *          - file lock(flock())
 *      - mutex(threads)
 *      - condition variable(threads)
 *            
 *
 *      - mutex                 : pthread_mutex_t
 *      - condition variable    : pthread_cond_t
 *      - rwlock                : pthread_rxlock_t
 *      - semaphore             : pthread_sem_t
 *      - pthread_key_t 
 *
 * Consuer & Producer problem
 * Reader & Writer problem
 *
 *
 */ 
#include "sync_ipc_funcs.h"

const char *sync_name[]  = {    
    "Non Synchronization",                                 
    "Mutex Synchronization",                                
    "Spinlock Synchronization",                                 
    "Semaphore Synchronization",                                 
    "Condition Variable Synchronization",                                 
    "Read/Write lock Synchronization" 
};

void ex_usage(char *cmd)
{
    printf("\n Usage for %s : \n",cmd);
    printf("    -r: number of reader thread (should be positive number)\n");
    printf("    -w: number of writer thread (should be positive number)\n");
    printf("    -t: pthread synchronization mode     \n\
           mtx : pthread mutex             \n\
           spl : spinlock                  \n\
           sem : semaphore                 \n\
           cnd : condition variable        \n\
           rxl : read/write lock           \n ");

}

void ex_example(char *cmd)
{
    printf("\n Example : \n");
    printf("    #sudo %s -r 2 -w 4 -t mtx \n", cmd);
    printf("    #sudo %s -r 10 -w 3 -t rxl \n\n", cmd);
}


static void init_sync_print(int reader_num, int writer_num, sync_type sync){
    printf("\n %s Experiment \n", sync_name[sync]);
    printf("    - reader count : %d \n", reader_num);
    printf("    - writer count : %d \n\n", writer_num);

}

void init_sync_func(sync_type sync, thread_args *arg){

    arg->shd_v = 0;
    arg->type = sync;
    arg->reader_func = NULL;
    arg->writer_func = NULL;

    switch(sync){
        case SYNC_MTX:
            pthread_mutex_init(&arg->sync.mutex_t,NULL);
            arg->reader_func = read_mutex;
            arg->writer_func = write_mutex;           
            break;
        case SYNC_SPL:
            pthread_spin_init(&arg->sync.spinlock_t,PTHREAD_PROCESS_PRIVATE);
            arg->reader_func = read_spl;
            arg->writer_func = write_spl;
            break;
        case SYNC_SEM:
            sem_init(&arg->sync.semaphore_t.r_sem_t, 0, 1);
            sem_init(&arg->sync.semaphore_t.w_sem_t, 0, 1);
            arg->sync.semaphore_t.r_count = 0;
            arg->reader_func = read_sem;
            arg->writer_func = write_sem;
            break;
        case SYNC_CDV:
            pthread_mutex_init(&arg->sync.condition_variable_t.cv_mutex_t,NULL);
            pthread_cond_init(&arg->sync.condition_variable_t.cv_t, NULL);
            arg->sync.condition_variable_t.reader_count = 0;
            arg->sync.condition_variable_t.writer_count = 0;
            arg->reader_func = read_cdv;
            arg->writer_func = write_cdv;
            break;
        case SYNC_RWL:
            pthread_rwlock_init(&arg->sync.rwlock_t, NULL);
            arg->reader_func = read_rwl;
            arg->writer_func = write_rwl;

            break;
    }
}

void exit_sync_func(sync_type sync, thread_args *arg){

    switch(sync){
        case SYNC_MTX:
            pthread_mutex_destroy(&arg->sync.mutex_t);   
            break;
        case SYNC_SPL:
            pthread_spin_destroy(&arg->sync.spinlock_t);
            break;
        case SYNC_SEM:
            sem_destroy(&arg->sync.semaphore_t.r_sem_t);
            sem_destroy(&arg->sync.semaphore_t.w_sem_t);
            break;
        case SYNC_CDV:
            pthread_mutex_destroy(&arg->sync.condition_variable_t.cv_mutex_t);
            pthread_cond_destroy(&arg->sync.condition_variable_t.cv_t);
            break;
        case SYNC_RWL:
            pthread_rwlock_destroy(&arg->sync.rwlock_t);
            break;
    }
}
#define FNAME_SIZE 128

int sync_test(int reader_num, int writer_num, sync_type sync){
     
    pthread_t *r_pthreads = NULL, *w_pthreads = NULL;
    thread_args th_arg = {0,};
    thread_rets **r_th_usage, **w_th_usage;
    double th_exe_time = 0.0;
    int ret=ERROR, thread_num = reader_num + writer_num, i;
    void *th_ret;

    r_pthreads = (pthread_t*)calloc(reader_num, sizeof(pthread_t));
    w_pthreads = (pthread_t*)calloc(writer_num, sizeof(pthread_t));

    r_th_usage = (thread_rets**)calloc(reader_num, sizeof(thread_rets*));
    w_th_usage = (thread_rets**)calloc(writer_num, sizeof(thread_rets*));

    init_sync_print(reader_num, writer_num, sync);
    init_sync_func(sync, &th_arg);

    for(i=0 ; i < writer_num; i++){ 
        ret = pthread_create(&w_pthreads[i], NULL, th_arg.writer_func,(void*)&th_arg);
        if(ret == ERROR){
            printf("    error in ptyhread create \n");
            goto FUNC_SYNC_ERROR;
        }
    }

    for(i = 0 ; i < reader_num; i++){ 
        ret = pthread_create(&r_pthreads[i], NULL, th_arg.reader_func,(void*)&th_arg);
        if(ret == ERROR){
            printf("    error in ptyhread create \n");
            goto FUNC_SYNC_ERROR;
        }
    }

    for(i = 0 ; i < writer_num ; i++){
        ret = pthread_join(w_pthreads[i], &th_ret);
        if(ret)
            goto FUNC_SYNC_ERROR;
        w_th_usage[i] = (thread_rets*)th_ret;
    }

    for(i = 0 ; i < reader_num ; i++){
        ret = pthread_join(r_pthreads[i], &th_ret);
        if(ret)
            goto FUNC_SYNC_ERROR;
        r_th_usage[i] = (thread_rets*)th_ret;
    }
    puts("");
    if(writer_num > 0){
        for(i=0, th_exe_time = 0.0 ; i < writer_num; i++){
            th_exe_time += get_time_usage(&w_th_usage[i]->tv_start, &w_th_usage[i]->tv_end);
        }
        printf("    Writer average lock acquire time   : %lf   seconds \n",(double)th_exe_time / writer_num);
    } 

    if(reader_num > 0){
        for(i=0, th_exe_time = 0.0 ; i < reader_num; i++){            
            th_exe_time += get_time_usage(&r_th_usage[i]->tv_start, &r_th_usage[i]->tv_end);
        }
        printf("    Reader average lock acquire time   : %lf   seconds \n\n",(double)th_exe_time / reader_num);
    }

    exit_sync_func(sync, &th_arg);

    return SUCCESS;

FUNC_SYNC_ERROR:
   return ERROR; 

}

int main(int argc, char *argv[]){
    char op;
    int num_reader=0,num_writer=0, count=0;
    sync_type type;
    

    optind = 0;

    while ((op = getopt(argc, argv, "r:w:t:")) != -1) {
        switch (op) {
            case 'r':
                num_reader = atoi(optarg);
                break;
            case 'w':
                num_writer = atoi(optarg);
                break;
            case 't':
                if(!strcmp(optarg, "mtx")){
                    type = SYNC_MTX;
                    break;
                } else if(!strcmp(optarg, "spl")){
                    type = SYNC_SPL;
                    break;
                } else if(!strcmp(optarg, "sem")){
                    type = SYNC_SEM;
                    break;
                } else if(!strcmp(optarg, "cnd")){
                     type = SYNC_CDV;
                    break;                   
                } else if(!strcmp(optarg, "rwl")){
                      type = SYNC_RWL;
                    break;      
                } else{
                    type = SYNC_NON;
                    break;
                }       
            default:
                goto INVALID_ARGS;
        }
    }

    if(num_reader >= 0 && num_writer >= 0 && type != SYNC_NON && (num_reader + num_writer) > 0){
        sync_test(num_reader, num_writer, type);
    }else
        goto INVALID_ARGS;

    return SUCCESS;
INVALID_ARGS:
    ex_usage(argv[0]);
    ex_example(argv[0]);
    return ERROR;
}


