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

//����Ŀ�꣺�����½��̲����½�������ʾ��ǰʱ�� 
int main(int argc,TCHAR* argv[]) {
	//printf("%s\n",argv[0]);
	//printf("%s\n",argv[1]);

	// ����ʱ�����
    LARGE_INTEGER timeStart, timeEnd;
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    double quadpart = (double)frequency.QuadPart;
  	 
	//��ʾ����λ��
	printf("PID: %d\n", GetCurrentProcessId());
	

	//��ȡ���ڵ�ǰ��ִ���ļ����ļ���
	TCHAR szFilename[MAX_PATH];
	//GetModuleFileName(NULL, szFilename, MAX_PATH);
	
	//�����ӽ��������еĸ�ʽ�������Ӧ�ó����EXE�ļ����Ϳ�¡���̵�IDֵ
	TCHAR szCmdLine[MAX_PATH];
	//sprintf((char *)szCmdLine, "\"%s\"%d", szFilename, nCloneID);

	//�����ӽ��̵�STARTUPINFO�ṹ
	//reinterpret_cast��C++���ǿ������ת������ֻ���޸��˲��������ͣ����������½����˸����Ķ���ı���ģ�Ͷ�û�н��ж�����ת����
	STARTUPINFO si;
	ZeroMemory(reinterpret_cast<void *>(&si), sizeof(si));
	si.cb = sizeof(si);

	//˵��һ�����ڼ�¼�ӽ��̵������Ϣ�Ľṹ����
	PROCESS_INFORMATION pi;						
	// typedef struct _PROCESS_INFORMATION {
	// 	HANDLE hProcess;
	// 	HANDLE hThread;
	// 	DWORD dwProcessId;
	// 	DWORD dwThreadId;
	// } PROCESS_INFORMATION

	//������ͬ�Ŀ�ִ���ļ��������д������̵ķ���
	//����mytime�����ֵ��÷�ʽ�����ԣ���Ҫ�ж������ĸ�����ʵ�ֲ�ͬ�ĵ���
	if(argc == 3) CreateArgv(argv[1], argv[2]);

	QueryPerformanceCounter(&timeStart);

	BOOL bCreateOK = CreateProcess(
		NULL,								    //��ִ�е�Ӧ�ó�������
		argv[1],								//ָ������һ���ӽ���ʶ���������Ϊ�ӽ��̴������
		NULL,									//ȱʡ�Ľ��̰�ȫ��
		NULL,									//ȱʡ���̰߳�ȫ��
		FALSE,									//���̳д��ļ��ľ��
		CREATE_NEW_CONSOLE,						//ʹ���µĿ���̨
		NULL,									//�µĻ���
		NULL,									//��ǰĿ¼
		&si,									//������Ϣ
		&pi);									//���ؽ��̺��߳���Ϣ
	
	//���н������رս��̺����̵߳ľ��
	// if (bCreateOK) {
	// 	CloseHandle(pi.hProcess);               //�رս��� 
	// 	CloseHandle(pi.hThread);                //�ر��߳̾�� 
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
