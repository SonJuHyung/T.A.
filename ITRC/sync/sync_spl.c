
#include "sync_types.h"

/*
 * NOTE 여러 쓰레드들과 공유되는 부분
 * 전역변수로 여러 쓰레드간 공유가 되는 부분.
 */

/* 스핀락 변수  */
pthread_spinlock_t  spl; 
/* 임계영역에서 조작할 여러 쓰레드와 공유되는 변수 */
int shared_variable=0;

/* 
 * INFO
 * spl_ex_usage : sync_spl 프로그램 사용법 함수  
 *  @cmd : 사용자가 입력한 명령어
 */
void spl_ex_usage(char *cmd)
{
    printf("\n Usage for %s : \n",cmd);
    printf("    -t: num thread, must be bigger than 0 ( e.g. 4 )\n");
    printf("    -i: iteration count, must be bigger than 0 ( e.g. 100000 ) \n");
    printf("    -s: sync mode setting ( e.g. l : spinlock , o : original ) \n");

}

/* 
 * INFO
 * spl_ex_example : sync_spl 프로그램 사용법 예시 함수  
 *  @cmd : 사용자가 입력한 명령어
 */
void spl_ex_example(char *cmd)
{
    printf("\n Example : \n");
    printf("    # %s -t 4 -i 1000000 -s o \n", cmd);
    printf("    # %s -t 4 -i 1000000 -s l \n\n", cmd);
}

/* 
 * INFO
 * spl_print_result : sync_spl 프로그램에서 동기화의 결과를 보여주기 위한 함수  
 *  @num_threads : 사용자 입력으로 sync_cond 프로그렘 수행에 사용된 쓰레드의 수
 *  @num_iterations : 사용자 입력으로 공유변수 shared_variable 에 대해 덧셈연산을 반복한 횟수
 *  @is_sync : 사용자 입력으로 설정한 동기화의 여부
 */
void spl_print_result(int num_threads, int num_iterations, int is_sync)
{
    char *cond[] = {"original(race condition)","spinlock"};

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
 * spl_add_shared_variable : 각 쓰레드에서 수행할 함수로 공윻변수 shared_variable 에 대하여 
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
void* spl_add_shared_variable(void *arg){
    thread_arg *th_arg = (thread_arg*)arg;
    int num_iterations = th_arg->num_iterations;
    int is_sync = th_arg->is_sync;
    int i=0;

    /* 공유변수를 통한 동기화를 수행 하도록 입력받은 경우( -s c 옵션을 통해 설정 )  */
    if(is_sync){
        for(i=0; i < num_iterations ;i++){
            /* 임계영역에 진입하기 위해 스핀락을 얻을 수 있는지 확인. 얻을 수 없을 경우 
             * busy waiting 하며 계속 락을 얻기까지 대기하고 락을 얻을 수 있을 경우, 임계 
             * 영역으로 진입 */
            pthread_spin_lock(&spl);

            /* 
             * NOTE 임계영역 구간
             * 공유변수 shared_variable 을 증가시킴
             */
            shared_variable++;

            /* 임계영역을 수행하였으므로, 락을 얻기 위해 기다리는 다른 쓰레드들이 임계영역 
             * 으로 진입 핧 수 있도록 락을 놓아줌. */
            pthread_spin_unlock(&spl);
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
 * spl_test : 쓰레드를 생성하고 각 쓰레드에게 사용자부터 입력받은 변수들을 넘겨주기 위한 함수
 *  @num_threads : 생성할 쓰레드의 수
 *  @num_iterations : 각 쓰레드에서 공유변수를 증가시킬 횟수
 *  @is_sync : 동기화 수행 여부(수행 후와 수행 전 비교용)
 */

int spl_test(int num_threads, int num_iterations,int is_sync)
{
    /* pthread_create 함수를 통해 반환될 쓰레드 식별 번호  */
    pthread_t *pthreads = NULL;
    int res=ERROR, i;
    /* 각 쓰레드에게 전달할 매개변수*/
    thread_arg arg;

    /* 스핀락 변수를 초기화. 두번재 변수는 스핀락에 대한 보호 레벨 설정. 
     * PTHREAD_PROCESS_SHARED 로 설정할 경우, 서로 다른 프로세스간에 존재하는
     * 쓰레드간에도 스핀락 변수에 접근이 가능하도록 설정  
     * PTHREAD_PROCESS_PRIVATE 로 설정 할 경우, 같은 프로세스 내의 쓰레드만 
     * 스핀락 변수에 접근이 가능하도록 설정.
     * 본 예시에서는 같은 프로세스에서만 사용 할 것이므로 PTHREAD_PROCESS_PRIVATE 으로 설정.
     */
    pthread_spin_init(&spl, PTHREAD_PROCESS_PRIVATE);

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
         * 생성한 쓰레드에서 수행 할 일을 spl_add_shared_variable 로 지정하고
         * add_shared_variable 함수에 넘겨질 매개변수를 arg 변수로 전달.
         */          res = pthread_create(&pthreads[i], NULL, spl_add_shared_variable,(void*)&arg);
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
    spl_print_result(num_threads, num_iterations, is_sync);

    /* 스핀락 변수를 삭제  */
    pthread_spin_destroy(&spl);

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
               else if(!strcmp(optarg, "l")){
                    is_sync=1;              
                    break;
               } 
            default:
                goto INVALID_ARGS;
        }
    }
    if((num_threads > 0) && (num_iterations > 0))
    {        
        spl_test(num_threads,num_iterations,is_sync);
    }
    else{
        goto INVALID_ARGS;
    }

    return SUCCESS;
INVALID_ARGS:
    spl_ex_usage(argv[0]);
    spl_ex_example(argv[0]);

    return ERROR;
}
