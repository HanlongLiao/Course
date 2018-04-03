#include<cstdlib>
#include<cstdio>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<sys/time.h>
using namespace std;

void printtime(struct timeval start, struct timeval end){
    long time_sec = end.tv_sec - start.tv_sec;
    long time_usec = end.tv_usec - start.tv_usec;
    if(time_usec < 0){
        time_usec += 1000000;
        time_sec -= 1;
    } 

    long h = time_sec / 3600;
    long m = (time_sec - h * 3600) / 60;
    long s = time_sec - h * 3600 - m * 60;
    long ms = time_usec / 1000;
    long us = time_usec - ms * 1000;

    printf("The child process' running time is  %ldh %ldm %lds %ldms %ldus \n", h, m, s, ms, us);
}


int main(int argc, char *argv[]){
    struct timeval start, end;
    pid_t pid = fork();
    gettimeofday(&start, NULL);
    if(pid == -1){
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if(pid == 0){
        char *arg1 = argv[1];
        if(argc == 3){
            char *arg2 = argv[2];
            execl(argv[1], arg1, arg2, NULL);}
        else if(argc == 2){
            execl(argv[1], arg1, NULL);
        }
    }
    else{
        wait(NULL);
        gettimeofday(&end, NULL);
        printtime(start, end);

        return 0;
    }
}