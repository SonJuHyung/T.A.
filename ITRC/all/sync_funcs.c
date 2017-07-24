#include "sync_ipc_funcs.h"

#define BUF_SIZE 512
#define FILE_NAME "./sample_text/SampleTextFile_1000kb.txt"

int read_file(const char *file_name, int read_size){

    int fd, ret = ERROR;
    char *buf = NULL;
    buf = (char*)calloc(read_size+1, sizeof(char));

    fd = open(file_name,O_RDONLY);
    if (fd < 0){ 
        perror("Error in meta check(IS_OPEN) : file open error (before open/close) \n");
        return ret;
    }

    ret = read(fd,buf,read_size);
    if(ret == ERROR)
        return ret;
    
    printf("        Reader Thread read %d from %s \n", read_size, FILE_NAME);

    return SUCCESS;
}

int write_file(const char *file_name, char *buf, int write_size){
   
   int fd, ret = ERROR;

    fd = open(FILE_NAME,O_WRONLY);
    if (fd < 0){ 
        perror("Error in meta check(IS_OPEN) : file open error (before open/close) \n");
        return ret;
    }
    lseek(fd,0,SEEK_END);
    ret = write(fd,buf, write_size);
    if(ret == ERROR)
        return ret;

    printf("        Writer Thread write %d to %s\n", write_size ,FILE_NAME);

    return SUCCESS;

}

void* read_mutex(void* arg){

    thread_args *th_arg = (thread_args*)arg;
    pthread_mutex_t *mutex_t = &th_arg->sync.mutex_t;
    thread_rets *th_ret = NULL;
    int *shd_v = &th_arg->shd_v, ret = ERROR;

    th_ret = (thread_rets*)calloc(1, sizeof(thread_rets));

    /* get time usage at thread start time. */
    THRET_START(&th_ret);

    pthread_mutex_lock(mutex_t);

    /* get time usage at lock acquire time */
    THRET_END(&th_ret);

    /* critical section  */
    read_file(FILE_NAME, BUF_SIZE);        
    pthread_mutex_unlock(mutex_t);



    pthread_exit(th_ret);
}

void* write_mutex(void* arg){
    
    thread_args *th_arg = (thread_args*)arg;
    pthread_mutex_t *mutex_t = &th_arg->sync.mutex_t;
    thread_rets *th_ret = NULL;
    int *shd_v = &th_arg->shd_v;

    th_ret = (thread_rets*)calloc(1, sizeof(thread_rets));

    /* get time usage at thread start time. */
    THRET_START(&th_ret);

    pthread_mutex_lock(mutex_t);

    /* get time usage at lock acquire time */
    THRET_END(&th_ret);

    /* critical section  */    
    write_file(FILE_NAME,"ITRC multi threading test\n" ,BUF_SIZE);
    pthread_mutex_unlock(mutex_t);

    pthread_exit(th_ret);
}

void* read_spl(void* arg){

    thread_args *th_arg = (thread_args*)arg;
    pthread_spinlock_t *spinlock_t = &th_arg->sync.spinlock_t;
    thread_rets *th_ret = NULL;
    int *shd_v = &th_arg->shd_v;
    
    th_ret = (thread_rets*)calloc(1, sizeof(thread_rets));

    /* get time usage at thread start time. */
    THRET_START(&th_ret);

    pthread_spin_lock(spinlock_t);

    /* get time usage at lock acquire time */
    THRET_END(&th_ret);

    read_file(FILE_NAME, BUF_SIZE);        
    pthread_spin_unlock(spinlock_t);

    pthread_exit(th_ret);
}

void* write_spl(void* arg){

    thread_args *th_arg = (thread_args*)arg;
    pthread_spinlock_t *spinlock_t = &th_arg->sync.spinlock_t;
    thread_rets *th_ret = NULL;
    int *shd_v = &th_arg->shd_v;
    
    th_ret = (thread_rets*)calloc(1, sizeof(thread_rets));

    /* get time usage at thread start time. */
    THRET_START(&th_ret);

    pthread_spin_lock(spinlock_t);

    /* get time usage at lock acquire time */
    THRET_END(&th_ret);
   
    write_file(FILE_NAME,"ITRC multi threading test\n" ,BUF_SIZE);

    pthread_spin_unlock(spinlock_t);

    pthread_exit(th_ret);
}

void* read_sem(void* arg){

    thread_args *th_arg = (thread_args*)arg;
    semaphore *sem = &th_arg->sync.semaphore_t;
    sem_t *r_sem_t = &sem->r_sem_t, *w_sem_t = &sem->w_sem_t;
    thread_rets *th_ret = NULL;
    int *shd_v = &th_arg->shd_v, *r_count = &sem->r_count;

    th_ret = (thread_rets*)calloc(1, sizeof(thread_rets));

    /* get time usage at thread start time. */
    THRET_START(&th_ret);

    sem_wait(r_sem_t);
    (*r_count)++;
    if(*r_count == 1)
        sem_wait(w_sem_t);
    sem_post(r_sem_t);

    /* get time usage at lock acquire time */
    THRET_END(&th_ret);

    /* critical section */
    read_file(FILE_NAME, BUF_SIZE);        
   
    sem_wait(r_sem_t);
    (*r_count)--;
    if(*r_count == 0)
        sem_post(w_sem_t);
    sem_post(r_sem_t);

    pthread_exit(th_ret);
}
void* write_sem(void* arg){

    thread_args *th_arg = (thread_args*)arg;
    semaphore *sem = &th_arg->sync.semaphore_t;
    sem_t *r_sem_t = &sem->r_sem_t, *w_sem_t = &sem->w_sem_t;
    thread_rets *th_ret = NULL;
    int *shd_v = &th_arg->shd_v;

    th_ret = (thread_rets*)calloc(1, sizeof(thread_rets));

    /* get time usage at thread start time. */
    THRET_START(&th_ret);

    sem_wait(w_sem_t);

    /* get time usage at lock acquire time */
    THRET_END(&th_ret);

    write_file(FILE_NAME,"ITRC multi threading test\n" ,BUF_SIZE);

    sem_post(w_sem_t);

    pthread_exit(th_ret);
}

/*
 * TODO
 * optimize.
 * variable name
 */
void* read_cdv(void* arg){

    thread_args *th_arg = (thread_args*)arg;
    condition_variable *cv = &th_arg->sync.condition_variable_t;
    pthread_mutex_t *mutex_t = &cv->cv_mutex_t;
    pthread_cond_t *cv_t =  &cv->cv_t;
    thread_rets *th_ret = NULL;
    int *shd_v = &th_arg->shd_v;
    int *writer_count = &cv->writer_count, *reader_count = &cv->reader_count;
    

    th_ret = (thread_rets*)calloc(1, sizeof(thread_rets));

    /* get time usage at thread start time. */
    THRET_START(&th_ret);
       
    pthread_mutex_lock(mutex_t);
    while(*writer_count)
        pthread_cond_wait(cv_t, mutex_t);
    (*reader_count)++;
    pthread_mutex_unlock(mutex_t);

    /* get time usage at lock acquire time */
    THRET_END(&th_ret);

    /* critical section */
    read_file(FILE_NAME, BUF_SIZE);        

    pthread_mutex_lock(mutex_t);
    (*reader_count)--;
    pthread_cond_signal(cv_t);
    pthread_mutex_unlock(mutex_t);

    pthread_exit(th_ret);
}

void* write_cdv(void* arg){

    thread_args *th_arg = (thread_args*)arg;
    condition_variable *cv = &th_arg->sync.condition_variable_t;
    pthread_mutex_t *mutex_t = &cv->cv_mutex_t;
    pthread_cond_t *cv_t =  &cv->cv_t;
    thread_rets *th_ret = NULL;
    int *shd_v = &th_arg->shd_v;
    int *writer_count = &cv->writer_count, *reader_count = &cv->reader_count;
    
    th_ret = (thread_rets*)calloc(1, sizeof(thread_rets));

    /* get time usage at thread start time. */
    THRET_START(&th_ret);
        
    pthread_mutex_lock(mutex_t);
    while(*reader_count || *writer_count)
        pthread_cond_wait(cv_t, mutex_t);
    (*writer_count)++;
    pthread_mutex_unlock(mutex_t);

    /* get time usage at lock acquire time */
    THRET_END(&th_ret);

    /* critical section */
    write_file(FILE_NAME,"ITRC multi threading test\n" ,BUF_SIZE);

    pthread_mutex_lock(mutex_t);
    (*writer_count)--;
    pthread_cond_signal(cv_t);
    pthread_mutex_unlock(mutex_t);

    /* get time usage at thread end time */
    THRET_END(&th_ret);


    pthread_exit(th_ret);
}

void* read_rwl(void* arg){

    thread_args *th_arg = (thread_args*)arg;
    pthread_rwlock_t *rwlock_t = &th_arg->sync.rwlock_t;
    thread_rets *th_ret = NULL;
    int *shd_v = &th_arg->shd_v;
        
    th_ret = (thread_rets*)calloc(1, sizeof(thread_rets));

    /* get time usage at thread start time. */
    THRET_START(&th_ret);

    pthread_rwlock_rdlock(rwlock_t);

    /* get time usage at lock acquire time */
    THRET_END(&th_ret);

    /* critical section  */
    read_file(FILE_NAME, BUF_SIZE);        

    pthread_rwlock_unlock(rwlock_t);

    pthread_exit(th_ret);
}

void* write_rwl(void* arg){

    thread_args *th_arg = (thread_args*)arg;
    pthread_rwlock_t *rwlock_t = &th_arg->sync.rwlock_t;
    thread_rets *th_ret = NULL;
    int *shd_v = &th_arg->shd_v;
    
    th_ret = (thread_rets*)calloc(1, sizeof(thread_rets));

    /* get time usage at thread start time. */
    THRET_START(&th_ret);

    pthread_rwlock_wrlock(rwlock_t);

    /* get time usage at lock acquire time */
    THRET_END(&th_ret);

    /* critical section  */
    write_file(FILE_NAME,"ITRC multi threading test\n" ,BUF_SIZE);

    pthread_rwlock_unlock(rwlock_t);

    pthread_exit(th_ret);
}

