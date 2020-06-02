#include <windows.h>
#include <iostream>
#include <iomanip>			//����������
//setbase(int n) : ������ת��Ϊ n ����.
//setw(n)�� Ԥ����
#include <Tlhelp32.h>		
//����ToolHelp API ������PROCESSENTRY32�ṹ��
#include <stdio.h>
#include <shlwapi.h>
#include <string.h>
#include <stdlib.h>

#pragma comment(lib,"Shlwapi.lib")
#pragma comment(lib,"Kernel32.lib")
//Kernel32.dll�ṩӦ�ó���һЩWin32�µĻ���API�������ڴ��������/���������ͬ����ʽ��

using namespace std;

int ProcessInfoList[1005];
int ProcessTotalNum = 0;

//��ʾ������ǣ��ñ�Ǳ�ʾ����Ӧ�ó�����ڴ���з��ʵ����� 
inline bool TestSet(DWORD dwTarget, DWORD dwMask) {
	return ((dwTarget &dwMask) == dwMask);
}

#define SHOWMASK(dwTarget, type) if(TestSet(dwTarget, PAGE_##type))   {	cout<<","<<#type;}

//��ʾ��ǰ��ҳ�汣����ʽ
void ShowProtection(DWORD dwTarget) {
	//�����ҳ�汣����ʽ 
	SHOWMASK(dwTarget, READONLY);						//ֻ��
	SHOWMASK(dwTarget, GUARD);							//����
	SHOWMASK(dwTarget, NOCACHE);						//�޻���
	SHOWMASK(dwTarget, READWRITE);					//��д
	SHOWMASK(dwTarget, WRITECOPY);					//дʱ����
	SHOWMASK(dwTarget, EXECUTE);						//
	SHOWMASK(dwTarget, EXECUTE_READ);				//
	SHOWMASK(dwTarget, EXECUTE_READWRITE);	//
	SHOWMASK(dwTarget, EXECUTE_WRITECOPY);	//
	SHOWMASK(dwTarget, NOACCESS);						//δ����
}

//�������������ڴ棬����ʾ���ڴ������ԵĹ�������
void WalkVM(HANDLE hProcess)//��õ����ڴ��ʹ�����
{
	SYSTEM_INFO si;    //ϵͳ��Ϣ�ṹ 
	ZeroMemory(&si, sizeof(si));		//���
	GetSystemInfo(&si);    //���ϵͳ��Ϣ 

	MEMORY_BASIC_INFORMATION mbi;    //���������ڴ�ռ�Ļ�����Ϣ�ṹ 
	ZeroMemory(&mbi, sizeof(mbi));    //���仺���������ڱ�����Ϣ 

	/*typedef struct _MEMORY_BASIC_INFORMATION {
		PVOID BaseAddress;
		PVOID AllocationBase;
		DWORD AllocationProtect;
		SIZE_T RegionSize;
		DWORD State;
		DWORD Protect;
		DWORD Type;
	} MEMORY_BASIC_INFORMATION, *PMEMORY_BASIC_INFORMATION;*/

	//ѭ������Ӧ�ó����ַ�ռ� 
	LPCVOID pBlock = (LPVOID)si.lpMinimumApplicationAddress;
	int count = 0;
	while (pBlock<(LPVOID)si.lpMaximumApplicationAddress) {
		//�����һ�������ڴ�����Ϣ                                        
		if (VirtualQueryEx(
			hProcess,                 //��صĽ���
			pBlock,                   //��ʼλ��
			&mbi,                     //������
			sizeof(mbi)) == sizeof(mbi))//���ȵ�ȷ��
		{
			//�����Ľ�β���䳤�� 
			LPCVOID pEnd = (PBYTE)pBlock + mbi.RegionSize;
			char szSize[MAX_PATH];
			StrFormatByteSizeA(mbi.RegionSize, (PSTR)szSize, MAX_PATH);

			//LWSTDAPI_(PSTR)     StrFormatByteSizeA(DWORD dw, _Out_writes_(cchBuf) PSTR pszBuf, UINT cchBuf);
			//��ʾ���ַ�ͳ��� 
			cout.fill('0');
			printf("%8x - %8x(%s)\t|", (DWORD)pBlock, (DWORD)pEnd, szSize);

			//��ʾ���״̬ 
			switch (mbi.State) {
			case MEM_COMMIT:
				printf("�ѱ��ύ, ");
				break;
			case MEM_FREE:
				printf("����, ");
				break;
			case MEM_RESERVE:
				printf("����, ");
				break;
			}

			//��ʾ���� 
			if (mbi.Protect == 0 && mbi.State != MEM_FREE) {
				mbi.Protect = PAGE_READONLY;
			}
			ShowProtection(mbi.Protect);

			//��ʾ���� �ڽ�ҳ������洢������ָ�����������ַ����ҳ����ͬ�Ĵ洢������
			switch (mbi.Type) {
			case MEM_IMAGE://���ص��ڴ��ģ��
			
				printf("Image, ");
				break;
			case MEM_PRIVATE://˽��
			
				printf("Private, ");
				break;
			case MEM_MAPPED://�ڴ�ӳ��
				printf("Mapped ");
				break;
			}
			//�����ִ�е�ӳ�� 
			TCHAR szFilename[MAX_PATH];
			if (GetModuleFileName((HMODULE)pBlock,		//ʵ�������ڴ��ģ���� 
				szFilename,				//��ȫָ�����ļ����� 
				MAX_PATH)				//ʵ��ʹ�õĻ���������
	>0) {
				//��ȥ·������ʾ 
				PathStripPath(szFilename);
				printf("\tModule: %s\t", szFilename);
			}
			cout << endl;//����
			count++;
			//�ƶ���ָ���Ի����һ���� 
			pBlock = pEnd;
		}
		else {
			printf("�������ޣ���������ΪȨ�޲���!");
			break;
		}
	}
	cout << "��ö��" << count << "�ڴ��" << endl;
}



/**
* ��õ�ǰ���ڴ��������Ϣ
*/
void GetSystemInfo_List() {

	//���ϵͳ��Ϣ 
	SYSTEM_INFO si;

	ZeroMemory(&si, sizeof(si));
	GetSystemInfo(&si);

	DWORD dwMemSize = (DWORD)si.lpMaximumApplicationAddress - (DWORD)si.lpMinimumApplicationAddress;
	TCHAR szMemSize[MAX_PATH];
	StrFormatByteSizeA(dwMemSize, (PSTR)szMemSize, MAX_PATH);

	//���ڴ���Ϣ��ʾ���� 
	//hex��16���Ƶĸ�ʽ���
	printf("+-------------------------------+---------------+\n");
	printf("|The physical page size         |%08x kb\t|\n", si.dwPageSize / 1024);
	printf("|The minimum address of program |%08x \t|\n", (DWORD)si.lpMinimumApplicationAddress);
	printf("|The maximum address of program |%08x \t|\n", (DWORD)si.lpMaximumApplicationAddress);
	printf("|The avialable address length   |%08x \t|\n", dwMemSize);
	printf("+-------------------------------+---------------+\n");
	return;
}

/**
* �ܵ��ڴ��ʹ�����
*/
void GetMemoryInfo_List() {

	MEMORYSTATUS stat;
	long int DIV = 1024 * 1024;
	long int DIV2 = 1;
	//GlobalMemoryStatusEx(&stat);
	GlobalMemoryStatus(&stat);
	//VOID GlobalMemoryStatus
	//(
	//	LPMEMORYSTATUS lpBuffer
	//);
	//typedef struct _MEMORYSTATUS { // mst
	//	DWORD dwLength; // sizeof(MEMORYSTATUS)
	//	DWORD dwMemoryLoad; // percent of memory in use
	//	DWORD dwTotalPhys; // bytes of physical memory
	//	DWORD dwAvailPhys; // free physical memory bytes
	//	DWORD dwTotalPageFile; // bytes of paging file
	//	DWORD dwAvailPageFile; // free bytes of paging file
	//	DWORD dwTotalVirtual; // user bytes of address space
	//	DWORD dwAvailVirtual; // free user bytes
	//} MEMORYSTATUS, *LPMEMORYSTATUS;
	printf("+------------------------------------------------+--------------+\n");
	printf("|Percentage of total memory been used            |%d(100)\t|\n", stat.dwMemoryLoad);
	printf("|Total physical memory                           |%d MB\t|\n", stat.dwTotalPhys / DIV);
	printf("|Total available physical memory                 |%d MB\t|\n", stat.dwAvailPhys / DIV);
	printf("|Total paging file                               |%d MB\t|\n", stat.dwTotalPageFile / DIV);
	printf("|Total available paging file                     |%d MB\t|\n", stat.dwAvailPageFile / DIV);
	printf("|Total virtual file                              |%d MB\t|\n", stat.dwTotalVirtual / DIV);
	printf("|Total available virtual file                    |%d MB\t|\n", stat.dwAvailVirtual / DIV);
	printf("+------------------------------------------------+--------------+\n");
}

//����ʵʱ��ȡ����������Ϣ
void GetProcessInfo_List() {
	PROCESSENTRY32 pe32;//���������Ϣ�ṹ
						/*typedef struct tagPROCESSENTRY32{
						DWORD dwSize;						//�ṹ�Ĵ�С
						DWORD cntUsage;					    //�˽��̵�������
						DWORD th32ProcessID;				//PID
						ULONG_PTR th32DefaultHeapID;		//����Ĭ�϶ѣ�Ĭ��Ϊ0
						DWORD th32ModuleID;					//����ģ��ID
						DWORD cntThreads;					//�˽��̿����Ľ�����
						DWORD th32ParentProcessID;			//�����̵�PID
						LONG pcPriClassBase;						//�߳�����Ȩ
						DWORD dwtemps;								//�˳�Ա���ٱ�ʹ�ã�Ĭ��Ϊ0
						TCHAR szExeFile[MAX_PATH];			//����ȫ�����ַ����飩
						} PROCESSENTRY32, *PPROCESSENTRY32;*/
	pe32.dwSize = sizeof(PROCESSENTRY32);

	HANDLE hProcessShot;
	hProcessShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);    //��ȡ�����б����һ��������0
																	   // ����ϵͳ��ǰ���̿���
	BOOL bRet;
	int i = 0;
	//�ӵ�һ�����̿�ʼ
	bRet = Process32First(hProcessShot, &pe32);//�ӵ�һ�����̿�ʼ
	for (int i = 0; i < 1000; i++)
		ProcessInfoList[i] = 0;
	printf("+-------+-------------------------------------+-------------+-------------------+\n");
	while (bRet) {
		//wcslen ���ؿ��ֽ��ַ������ֽڸ���
		size_t len = wcslen(pe32.szExeFile) + 1;
		size_t converted = 0;
		char *CStr;
		CStr = (char*)malloc(len * sizeof(char));
		//����ת��
		wcstombs_s(&converted, CStr, len, pe32.szExeFile, _TRUNCATE);
		ProcessInfoList[i] = pe32.th32ProcessID;
		
		printf("|[%03d]\t|%37s|PID: [%05d] |cntThreads: %d\t|\n", i, CStr, pe32.th32ProcessID, pe32.cntThreads);//��ţ�������
		bRet = Process32Next(hProcessShot, &pe32);
		i++;
	}//�������̿���
	printf("+-------+-------------------------------------+-------------+-------------------+\n");
	ProcessTotalNum = i - 1;
	CloseHandle(hProcessShot);
}

int main() {
	int temp = 4;
	cout << "����ϵͳʵ����-�ڴ������";
	while (1) {
		cout << endl << "��ѡ����: " << endl
			<< "0 - �˳�" << endl
			<< "1 - �鿴�ڴ�������Ϣ" << endl
			<< "2 - �鿴�ڴ�ʹ����Ϣ" << endl
			<< "3 - �鿴��ǰ���н�����Ϣ" << endl
			<< "�밴������ѡȡ��ǰ����" << endl;
		cin >> temp;
		if (temp == 0) {
			return 0;
		}
		else if (temp == 1) {
			GetSystemInfo_List();	//��ȡʵʱ�ڴ�������Ϣ
		}
		else if (temp == 2) {
			GetMemoryInfo_List();
		}
		else if (temp == 3) {
			GetProcessInfo_List();
			int f, i, PID;
			cout << "�鿴ĳһ���̵������ڴ���Ϣ������Ų�ѯ����1����PID��ѯ����2" << endl;
			cin >> f;
			
			if (f == 1) {
				cout << "���������" << endl;
				cin >> i;
				if (i > ProcessTotalNum || i < 0) { 
					printf("�޵�ǰ��Ŷ�Ӧ�Ľ���");
					continue;
				}
				else {
					PID = ProcessInfoList[i];
					HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, PID);
					if (hProcess == 0x00000000) {
						printf("�������� ���صľ��Ϊ�գ���������Ϊ��Ȩ�޷��ʣ�");
						continue;
					}
					WalkVM(hProcess);
					continue;
				}
			}
			else if(f == 2){
				cout << "���������PID" << endl;
				cin >> PID;
				HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, PID);	//��ȡPIDֵ
				WalkVM(hProcess);
				continue;
			}
			else {
				printf("��������ȷ��ָ�");
				continue;
			}
		}
	}
	return 0;
	
}
