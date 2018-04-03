#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>

#define NEED_P 2
#define NEED_C 3
#define WORKS_P 6
#define WORKS_C 4

#define BUF_LENGTH (sizeof(struct mybuffer))
#define LETTER_NUM 3
#define SHM_MODE 0600

//用户指定的信号量集合的关键字
#define SEM_ALL_KEY 1234
#define SEM_EMPTY 0
#define SEM_FULL 1
#define SEM_MUTEX 2

//缓冲区结构（循环队列）
struct mybuffer
{
    char letter[LETTER_NUM];
    int head;
    int tail;
    int is_empty;
    int index;
};

//得到5以内的一个随机数
int get_random()
{
    int t;
    srand((unsigned)(getpid() + time(NULL)));
    t = rand() % 5;
    return t;
}

//得到A~Z的一个随机字母
char get_letter()
{
    char a;
    srand((unsigned)(getpid() + time(NULL)));
    a = (char)((char)(rand() % 26) + 'A');
    return a;
}

//P操作
void p(int sem_id, int sem_num)
{
    struct sembuf xx;
    xx.sem_num = sem_num;   //要操作的信号量的索引
    xx.sem_op = -1;         //操作值
    xx.sem_flg = 0;         //访问标志
    /*  semop() 进程对信号集合的一个或者多个信号量执行P/V操作
    *   sem_id 为信号量的标识符，xx是用户提供的模板数组指针
    *   1为数组中的元素数量   
    */
    semop(sem_id, &xx, 1);
}

//V操作
void v(int sem_id, int sem_num)
{
    struct sembuf xx;
    xx.sem_num = sem_num;
    xx.sem_op = 1;
    xx.sem_flg = 0;
    semop(sem_id, &xx, 1);
}

void printInfo(time_t now, mybuffer * shmptr, int num, char lt, pid_t mypid, int type){
    printf("[%02d]\tProcess ID: %d\t", shmptr->index, mypid);
    
    printf("Time: %02d:%02d:%02d\n", localtime(&now)->tm_hour, localtime(&now)->tm_min, localtime(&now)->tm_sec);
    
    if(type == 0) printf("Producer %d puts '%c'.\n", num, lt);
    else{  printf("Consumer %d gets '%c'.\n", num, lt); }
    int i, j;

    printf("+---+---+---+\n");
    for (i = 0, j = (shmptr->tail - 1 >= shmptr->head) ? (shmptr->tail - 1) : (shmptr->tail - 1 + LETTER_NUM); !(shmptr->is_empty) && j >= shmptr->head; j--)
    {
        i++;
        printf("|");
        printf("%3c", shmptr->letter[j % LETTER_NUM]);
    }
    for(int k = i + 1; k <= 3; k++){
        printf("|   ");
    }
    printf("|\n");
    printf("+---+---+---+\n");
   
    printf("-----------------------------------------------------------------------\n");
}

//主函数
int main(int argc, char * argv[])
{
    int i, j;
    //信号量集合的标志号
    int shm_id, sem_id;
    int num_p = 0, num_c = 0;
    struct mybuffer * shmptr;
    char lt;
    time_t now;
    pid_t pid_p, pid_c;
    
    /*  创建了一个信号量集合，关键字为SEM_ALL_KEY 1234,
    *   信号量集合中的信号量数目为2，第三个参数为创
    *   建或者打开的标志
    *   返回值为信号量集合的标志号，出错返回1
    */

    sem_id = semget(SEM_ALL_KEY, 3, IPC_CREAT | 0660);

    //创建成功
    if (sem_id >= 0)
    {
        printf("Main process starts. Semaphore created.\n");
    }


    /*  对信号量执行控制操作
    *   信号量标志为sem_id,信号量索引为SEM_EMPTY与SEM_FULL
    *   SETVAL 为需要执行的操作命令，为 SETVAL 时表示设置信号量为
    *   一个初始值，为arg.val(第4个参数中的一个值)，第四个参数一般是一个semun的
    *   union,此处直接进行了赋值，并且初始化给了信号量
    *   在上一个函数中创建了的信号量为2，此处第二个参数分别对其
    *   索引值为0和1的两个信号量进行初始化
    */
    semctl(sem_id, SEM_EMPTY, SETVAL, LETTER_NUM);
    semctl(sem_id, SEM_FULL, SETVAL, 0);
    semctl(sem_id, SEM_MUTEX, SETVAL, 1);
    

    /*  建立共享内存区，如果 key == IPC_PRIVATE时，表示总是会创建一个新的共享内核对象
    *   第二个参数表示共享内存区域的长度，第三个参数表示共享内存区域的创建或者打开标志
    *   表示对共享内存区域的特殊要求
    *   函数返回共享段标识
    */
    if ((shm_id = shmget(IPC_PRIVATE, BUF_LENGTH, SHM_MODE)) < 0)
    {
        printf("Error on shmget.\n");
        exit(1);
    }

    /*  shamt()API将共享内存空间映射到了当前进程的虚拟空间，返回的是对应的内存的地址
    *   第二个参数为0表示附加到的地址为进程虚拟空间的
    *   第一个可用地址空间，第三个参数则为允许对共享段的访问方式
    *   shmper 是一个缓冲区类型的指针
    */
    if ((shmptr = (mybuffer*)shmat(shm_id, 0, 0)) == (void *)-1)
    {
        printf("Error on shmat.\n");
        exit(1);
    }

    //  初始化
    shmptr->head = 0;
    shmptr->tail = 0;
    shmptr->is_empty = 1;
    shmptr->index = 0;
    
    while ((num_p++) < NEED_P)
    {
        if ((pid_p = fork()) < 0)
        {
            printf("Error on fork.\n");
            exit(1);
        }
        //如果是子进程，开始创建生产者
        if (pid_p == 0)
        {
            if ((shmptr = (mybuffer*)shmat(shm_id, 0, 0)) == (void *)-1)
            {
                printf("Error on shmat.\n");
                exit(1);
            }

            for (i = 0; i < WORKS_P; i++)
            {
                p(sem_id, SEM_EMPTY);
                p(sem_id, SEM_MUTEX);
                sleep(get_random());    
                shmptr->letter[shmptr->tail] = lt = get_letter();
                shmptr->tail = (shmptr->tail + 1) % LETTER_NUM;
                shmptr->is_empty = 0;
                shmptr->index++;
                pid_t pid = getpid();
                now = time(NULL);

                printInfo(now, shmptr, num_p, lt, pid, 0);

                fflush(stdout);
                v(sem_id, SEM_FULL);
                v(sem_id, SEM_MUTEX);
            }
            shmdt(shmptr);
            exit(0);
        }
    }

    while (num_c++ < NEED_C)
    {
        if ((pid_c = fork()) < 0)
        {
            printf("Error on fork.\n");
            exit(1);
        }
        //如果是子进程，开始创建消费者
        if (pid_c == 0)
        {
            if ((shmptr = (mybuffer*)shmat(shm_id, 0, 0)) == (void *)-1)
            {
                printf("Error on shmat.\n");
                exit(1);
            }
            for (i = 0; i < WORKS_C; i++)
            {
                p(sem_id, SEM_FULL);
                p(sem_id, SEM_MUTEX);

                sleep(get_random());
                
                lt = shmptr->letter[shmptr->head];
                shmptr->head = (shmptr->head + 1) % LETTER_NUM;
                shmptr->is_empty = (shmptr->head == shmptr->tail);
                shmptr->index++;
                pid_t pid = getpid();
                now = time(NULL);
                printInfo(now, shmptr, num_p, lt, pid, 1);
                fflush(stdout);

                v(sem_id, SEM_EMPTY);
                v(sem_id, SEM_MUTEX);
            }
            shmdt(shmptr);
            exit(0);
        }
    }
    
    //主控程序最后退出
    while(wait(0) != -1);
    shmdt(shmptr);
    shmctl(shm_id, IPC_RMID, 0);
    semctl(sem_id, IPC_RMID, 0);
    printf("Main process ends.\n");
    fflush(stdout);
    sleep(60);
    exit(0);
}