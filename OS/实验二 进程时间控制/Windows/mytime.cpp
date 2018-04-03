#include <Windows.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <tchar.h> 

void CreateArgv(TCHAR t1[], TCHAR t2[]){
    int i = 0;
    while(t1[i] != '\0'){
        i ++;
    }
    t1[i] = ' ';
    i++;
    int j = 0;
    while(t2[j] != '\0'){
        t1[i++] = t2[j++];
    }
    t1[i] = '\0'; 
}
 
void printtime(LARGE_INTEGER starttime, LARGE_INTEGER endtime, double quadpart){
    double elapsed = (endtime.QuadPart - starttime.QuadPart) / quadpart;

	int h = elapsed / 3600;
	int m = (elapsed - h * 3600) / 60;
	int s = elapsed - h * 3600 - m * 60;
	int ms = (elapsed - h * 3600 - m * 60 - s) * 1000;
	int us = (elapsed - h * 3600 - m * 60 - s - ms / 1000) * 1000;

	printf("The child process' running time is  %dh %dm %ds %dms %dus\n", h, m, s, ms, us);
}

//程序目标：创建新进程并在新进程中显示当前时间 
int main(int argc,TCHAR* argv[]) {
	//printf("%s\n",argv[0]);
	//printf("%s\n",argv[1]);

	// 定义时间参数
    LARGE_INTEGER timeStart, timeEnd;
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    double quadpart = (double)frequency.QuadPart;
  	 
	//显示进程位置
	printf("PID: %d\n", GetCurrentProcessId());
	

	//获取用于当前可执行文件的文件名
	TCHAR szFilename[MAX_PATH];
	//GetModuleFileName(NULL, szFilename, MAX_PATH);
	
	//创建子进程命令行的格式化，获得应用程序的EXE文件名和克隆进程的ID值
	TCHAR szCmdLine[MAX_PATH];
	//sprintf((char *)szCmdLine, "\"%s\"%d", szFilename, nCloneID);

	//用于子进程的STARTUPINFO结构
	//reinterpret_cast是C++里的强制类型转换符，只是修改了操作数类型，但仅是重新解释了给出的对象的比特模型而没有进行二进制转换。
	STARTUPINFO si;
	ZeroMemory(reinterpret_cast<void *>(&si), sizeof(si));
	si.cb = sizeof(si);

	//说明一个用于记录子进程的相关信息的结构变量
	PROCESS_INFORMATION pi;						
	// typedef struct _PROCESS_INFORMATION {
	// 	HANDLE hProcess;
	// 	HANDLE hThread;
	// 	DWORD dwProcessId;
	// 	DWORD dwThreadId;
	// } PROCESS_INFORMATION

	//利用相同的可执行文件和命令行创建进程的符号
	//由于mytime有两种调用方式，所以，需要判定参数的个数，实现不同的调用
	if(argc == 3) CreateArgv(argv[1], argv[2]);

	QueryPerformanceCounter(&timeStart);

	BOOL bCreateOK = CreateProcess(
		NULL,								    //可执行的应用程序名称
		argv[1],								//指定创建一个子进标识，可以理解为子进程传入参数
		NULL,									//缺省的进程安全性
		NULL,									//缺省的线程安全性
		FALSE,									//不继承打开文件的句柄
		CREATE_NEW_CONSOLE,						//使用新的控制台
		NULL,									//新的环境
		NULL,									//当前目录
		&si,									//启动信息
		&pi);									//返回进程和线程信息
	
	//运行结束，关闭进程和其线程的句柄
	// if (bCreateOK) {
	// 	CloseHandle(pi.hProcess);               //关闭进程 
	// 	CloseHandle(pi.hThread);                //关闭线程句柄 
	// }
	
	if(bCreateOK){
		printf("Success\n");
		WaitForSingleObject(pi.hProcess, INFINITE);
		QueryPerformanceCounter(&timeEnd);

		printtime(timeStart, timeEnd, quadpart);
		}
	
	else printf("Fail\n");

    system("pause");
	return 0;
}
