#include <Windows.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
	
int main(int argc, char *argv[]){
	int t = atoi(argv[1]);

	printf("This process' running time will be %d s\n", t);
	
	int truetime = t * 1000;
	Sleep(truetime);
	
	return 0;
}
