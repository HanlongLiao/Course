#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/time.h>
using namespace std;

int main(){

    printf("This process' running time will be 5s\n");
    sleep(5);

    return 0;
}