
#include "sync_types.h"

/* 임계영역에서 조작할 여러 쓰레드와 공유되는 변수 */
int shared_variable=0;
/* 뮤텍스에서 상호 배제를 보장하기 위한 락 변수 */
int lock=0;
/* 뮤텍스 변수를 정적 초기화 수행 정적 초기화는 
 * (기본 셋팅으로 초기화 된다. deadlock 에 대한 설정을 하려면 
 * 동적으로pthread_mutex_init 함수를 통해 초기화 하여야 한다) */
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

/* 
 * INFO
 * mtx_ex_usage : sync_mtx 프로그램 사용법 함수  
 *  @cmd : 사용자가 입력한 명령어
 */
void mtx_ex_usage(char *cmd)
{
    printf("\n Usage for %s : \n",cmd);
    printf("    -t: num thread, must be bigger than 0 ( e.g. 4 )\n");
    printf("    -i: iteration count, must be bigger than 0 ( e.g. 100000 ) \n");
    printf("    -s: sync mode setting ( e.g. m : pthread mutex , o : original ) \n");

}

/* 
 * INFO
 * mtx_ex_example : sync_mtx 프로그램 사용법 예시 함수  
 *  @cmd : 사용자가 입력한 명령어
 */
void mtx_ex_example(char *cmd)
{
    printf("\n Example : \n");
    printf("    # %s -t 4 -i 1000000 -s o \n", cmd);
    printf("    # %s -t 4 -i 1000000 -s m \n\n", cmd);
}

/* 
 * INFO
 * mtx_print_result : sync_mtx 프로그램에서 동기화의 결과를 보여주기 위한 함수  
 *  @num_threads : 사용자 입력으로 sync_cond 프로그렘 수행에 사용된 쓰레드의 수
 *  @num_iterations : 사용자 입력으로 공유변수 shared_variable 에 대해 덧셈연산을 반복한 횟수
 *  @is_sync : 사용자 입력으로 설정한 동기화의 여부
 */
void mtx_print_result(int num_threads, int num_iterations, int is_sync)
{
    char *cond[] = {"original(race condition)","pthread mutex"};

    printf("\n Experiment info \n");
    printf("    num_threads         : %d \n",num_threads);
    printf("    num_iterations      : %d \n",num_iterations);
    printf("    experiment type     : %s \n",cond[is_sync]);

    printf("\n Experiment result : \n");
    printf("    expected result     : %d \n",num_threads * num_iterations);
    printf("    result              : %d \n\n", shared_variable);

}

/*   
 * INFO
 * mtx_add_shared_variable : 각 쓰레드에서 수행할 함수로 공윻변수 shared_variable 에 대하여 
 * num_iterations 의 수 만큼 반복하여shared_variable 증가시키기 위한 함수.
 *   @arg : 쓰레드에 전달될 매개변수
 *        (쓰레드가 수행할 함수의 형식은void* 형으로 반환하며, void* 형
 *        으로 매개변수가 전달되도록 형식이 고정되어 있다)
 *
 * NOTE 전달될 매개변수는 아래와 같다. 
 *  - struct thread_arg
 *      @num_iterations : add_shared_variable 함수에서 공유변수인
 *                        shared_variable 에 대해 반복하여 덧셈을
 *                        수행 할 횟수.
 *      @is_sync        : add_shared_variable 함수에서 공유변수인
 *                        shared_variable 에 대해 동기화를 수행 
 *                        할 지의 여부.
 *
 */
void* mtx_add_shared_variable(void *arg){
    thread_arg *th_arg = (thread_arg*)arg;
    int num_iterations = th_arg->num_iterations;
    int is_sync = th_arg->is_sync;
    int i=0;

    /* 공유변수를 통한 동기화를 수행 하도록 입력받은 경우( -s c 옵션을 통해 설정 )  */
    if(is_sync){
        for(i=0; i < num_iterations ;i++){
            /* pthread_mutex_lock 함수를 통해 임계영역에 이미 존재하고 있는 다른 쓰레도가 
             * 있을경우 대기하고 쓰레드가 없을 경우 현재 쓰레드가 임계영역을 수행하기 위해 락을 잠금. 
             */
            pthread_mutex_lock(&mutex);
            /* 
             * NOTE 임계영역 구간
             * 공유변수 shared_variable 을 증가시킴
             */
            shared_variable++;
            /*
             * 현재 쓰레드가 임계영역 수행을 마쳤으므로, 다른 쓰레드가 임계영역에 접근 할 수 있도록 하기 위해
             * 락을 되돌려줌.
             */
            pthread_mutex_unlock(&mutex);
        }    
    }else{
        /* 
         * 공유변수를 통한 동기화를 수행 하지 않도록 입력받은 경우( -s o 옵션을 통해 설정 )  
         * 동기화를 수행하지 않으므로 shared_variable 의 값이 정상적으로 계산되지 않는다. 
         */  
        for(i=0; i < num_iterations ;i++){
            shared_variable++;
        }    
    }
}

/*
 * INFO
 * mtx_test : 쓰레드를 생성하고 각 쓰레드에게 사용자부터 입력받은 변수들을 넘겨주기 위한 함수
 *  @num_threads : 생성할 쓰레드의 수
 *  @num_iterations : 각 쓰레드에서 공유변수를 증가시킬 횟수
 *  @is_sync : 동기화 수행 여부(수행 후와 수행 전 비교용)
 */

int mtx_test(int num_threads, int num_iterations,int is_sync)
{
    /* pthread_create 함수를 통해 반환될 쓰레드 식별 번호  */
    pthread_t *pthreads = NULL;
    int res=ERROR, i;
    /* 각 쓰레드에게 전달할 매개변수*/
    thread_arg arg;

    /* 쓰레드에게 전달 할 매개변수들을 초기화 */
    arg.is_sync = is_sync;
    arg.num_iterations = num_iterations;

    /* 쓰레드식별 번호를 저장할 메모리 공간 확보 및 초기화  */
    pthreads = (pthread_t*)malloc(sizeof(pthread_t)*num_threads);
    memset(pthreads, 0x0, sizeof(pthread_t) * num_threads);

    /* 사용자에게 입력받은 수 만큼 쓰레드 생성*/
    for(i = 0 ; i < num_threads; i++){ 
        /* 
         * pthread_create 함수를 통해 새로운 쓰레드 생성하고
         * 쓰레드 식별번호가 pthreads[i] 변수에 저장되며, 
         * 생성한 쓰레드에서 수행 할 일을 mtx_add_shared_variable 로 지정하고
         * add_shared_variable 함수에 넘겨질 매개변수를 arg 변수로 전달.
         */
        res = pthread_create(&pthreads[i], NULL, mtx_add_shared_variable,(void*)&arg);
        if(res == ERROR){
            printf(" Error: _perf_metadata - pthread_create error \n");
            goto TEST_ERROR;
        }
    }
    /* 
     * pthreads[i] 의 식별번호를 가지는 쓰레드가 종료되는 것을 기다림.
     *  NOTE 
     *  현재 프로그램의 main 이 종료되면 pthread_create 함수를 통해 생성한 쓰레드 들이 
     *  모두 종료되므로pthread_join 함수를 통해 생성한 쓰레드가 종료되기를 기다려야 한다. 
     */
    for(i = 0 ; i < num_threads ; i++){
        pthread_join(pthreads[i], NULL);
    }

    /* 공유 변수 출력 함수  */
    mtx_print_result(num_threads, num_iterations, is_sync);

    /* 
     * 리눅스에서 쓰레드는 뮤택스와 별개로 되어 있으므로, 쓰레드가 종료되더라도, 뮤텍스 객체는 남아있음  
     * 아래의 pthread_mutex_destroy 함수를 통해 뮤텍스 객체를 삭제.
     */
    pthread_mutex_destroy(&mutex);

    return SUCCESS;
TEST_ERROR:
    free(pthreads);
    return ERROR;

}


int main(int argc, char *argv[]){
    char op;
    int num_threads=0, num_iterations=0, is_sync=0;
    int fd;

    optind = 0;

    /* 사용자로부터 입력을 받는 부분  */
    while ((op = getopt(argc, argv, "t:i:s:")) != -1) {
        switch (op) {
            case 't':
                /* -t 옵션을 통해 생성할 쓰레드의 수를 받아 저장  */
                num_threads=atoi(optarg);
                break;
            case 'i':
                /* -i 옵션을 통해 공유변수를 몇번 증가시킬 것인지를 받아 저장  */
                num_iterations = atoi(optarg);
                break;
            case 's':
                /* -s 옵션을 통해 동기화를 사용할 것인지 여부를 받아 저장
                 * (동기화 사용시, 사용하지 않을 시 비교용)  */
                if(!strcmp(optarg, "o")){
                    is_sync=0;
                    break;
                }
               else if(!strcmp(optarg, "m")){
                    is_sync=1;              
                    break;
               } 
            default:
                goto INVALID_ARGS;
        }
    }
    if((num_threads > 0) && (num_iterations > 0))
    {        
        mtx_test(num_threads,num_iterations,is_sync);
    }
    else{
        goto INVALID_ARGS;
    }

    return SUCCESS;
INVALID_ARGS:
    mtx_ex_usage(argv[0]);
    mtx_ex_example(argv[0]);

    return ERROR;
}
