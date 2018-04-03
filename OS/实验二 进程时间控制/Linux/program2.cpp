#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/time.h>
using namespace std;

int main(int argc, char *argv[]){
    int t = atoi(argv[1]);
    
    printf("This process' running time will be %ds\n", t);
    sleep(t);

}