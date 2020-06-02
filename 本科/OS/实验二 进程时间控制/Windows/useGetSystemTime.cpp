#include<Windows.h>
#include<iostream>
#include<cstdio>
#include<cstdlib>
using namespace std;

void printtime(SYSTEMTIME start, SYSTEMTIME end){
    int ms = end.wMilliseconds - start.wMilliseconds;
    int s = end.wSecond - start.wSecond;
    int m = end.wMinute - start.wMinute;
    int h = end.wHour - start.wHour;

    if(ms < 0){ms += 1000; s -= 1;}
    if(s < 0){s += 60; m -= 1;}
    if(m < 0){m += 60; h += 1;}
    
    printf("Time: %dh %dm %ds %dms\n", h, m, s, ms);
}

int main(){
    SYSTEMTIME starttime, endtime;
    GetSystemTime(&starttime);
    Sleep(5000);
    GetSystemTime(&endtime);
    printtime(starttime, endtime);
    
    system("pause");
    return 0;
}