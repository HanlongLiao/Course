#include <windows.h>
#include <iostream>
#include <iomanip>			//操纵运算子
//setbase(int n) : 将数字转换为 n 进制.
//setw(n)： 预设宽度
#include <Tlhelp32.h>		
//引用ToolHelp API ，调用PROCESSENTRY32结构体
#include <stdio.h>
#include <shlwapi.h>
#include <string.h>
#include <stdlib.h>

#pragma comment(lib,"Shlwapi.lib")
#pragma comment(lib,"Kernel32.lib")
//Kernel32.dll提供应用程序一些Win32下的基底API，包括内存管理、输入/输出操作和同步函式。

using namespace std;

int ProcessInfoList[1005];
int ProcessTotalNum = 0;

//显示保护标记，该标记表示允许应用程序对内存进行访问的类型 
inline bool TestSet(DWORD dwTarget, DWORD dwMask) {
	return ((dwTarget &dwMask) == dwMask);
}

#define SHOWMASK(dwTarget, type) if(TestSet(dwTarget, PAGE_##type))   {	cout<<","<<#type;}

//显示当前块页面保护方式
void ShowProtection(DWORD dwTarget) {
	//定义的页面保护方式 
	SHOWMASK(dwTarget, READONLY);						//只读
	SHOWMASK(dwTarget, GUARD);							//保护
	SHOWMASK(dwTarget, NOCACHE);						//无缓存
	SHOWMASK(dwTarget, READWRITE);					//读写
	SHOWMASK(dwTarget, WRITECOPY);					//写时复制
	SHOWMASK(dwTarget, EXECUTE);						//
	SHOWMASK(dwTarget, EXECUTE_READ);				//
	SHOWMASK(dwTarget, EXECUTE_READWRITE);	//
	SHOWMASK(dwTarget, EXECUTE_WRITECOPY);	//
	SHOWMASK(dwTarget, NOACCESS);						//未访问
}

//遍历整个虚拟内存，并显示各内存区属性的工作程序
void WalkVM(HANDLE hProcess)//获得单个内存的使用情况
{
	SYSTEM_INFO si;    //系统信息结构 
	ZeroMemory(&si, sizeof(si));		//清空
	GetSystemInfo(&si);    //获得系统信息 

	MEMORY_BASIC_INFORMATION mbi;    //进程虚拟内存空间的基本信息结构 
	ZeroMemory(&mbi, sizeof(mbi));    //分配缓冲区，用于保存信息 

	/*typedef struct _MEMORY_BASIC_INFORMATION {
		PVOID BaseAddress;
		PVOID AllocationBase;
		DWORD AllocationProtect;
		SIZE_T RegionSize;
		DWORD State;
		DWORD Protect;
		DWORD Type;
	} MEMORY_BASIC_INFORMATION, *PMEMORY_BASIC_INFORMATION;*/

	//循环整个应用程序地址空间 
	LPCVOID pBlock = (LPVOID)si.lpMinimumApplicationAddress;
	int count = 0;
	while (pBlock<(LPVOID)si.lpMaximumApplicationAddress) {
		//获得下一个虚拟内存块的信息                                        
		if (VirtualQueryEx(
			hProcess,                 //相关的进程
			pBlock,                   //开始位置
			&mbi,                     //缓冲区
			sizeof(mbi)) == sizeof(mbi))//长度的确认
		{
			//计算块的结尾及其长度 
			LPCVOID pEnd = (PBYTE)pBlock + mbi.RegionSize;
			char szSize[MAX_PATH];
			StrFormatByteSizeA(mbi.RegionSize, (PSTR)szSize, MAX_PATH);

			//LWSTDAPI_(PSTR)     StrFormatByteSizeA(DWORD dw, _Out_writes_(cchBuf) PSTR pszBuf, UINT cchBuf);
			//显示块地址和长度 
			cout.fill('0');
			printf("%8x - %8x(%s)\t|", (DWORD)pBlock, (DWORD)pEnd, szSize);

			//显示块的状态 
			switch (mbi.State) {
			case MEM_COMMIT:
				printf("已被提交, ");
				break;
			case MEM_FREE:
				printf("空闲, ");
				break;
			case MEM_RESERVE:
				printf("保留, ");
				break;
			}

			//显示保护 
			if (mbi.Protect == 0 && mbi.State != MEM_FREE) {
				mbi.Protect = PAGE_READONLY;
			}
			ShowProtection(mbi.Protect);

			//显示类型 邻近页面物理存储器类型指的是与给定地址所在页面相同的存储器类型
			switch (mbi.Type) {
			case MEM_IMAGE://加载到内存的模块
			
				printf("Image, ");
				break;
			case MEM_PRIVATE://私有
			
				printf("Private, ");
				break;
			case MEM_MAPPED://内存映射
				printf("Mapped ");
				break;
			}
			//检验可执行的映像 
			TCHAR szFilename[MAX_PATH];
			if (GetModuleFileName((HMODULE)pBlock,		//实际虚拟内存的模块句柄 
				szFilename,				//完全指定的文件名称 
				MAX_PATH)				//实际使用的缓冲区长度
	>0) {
				//除去路径并显示 
				PathStripPath(szFilename);
				printf("\tModule: %s\t", szFilename);
			}
			cout << endl;//换行
			count++;
			//移动块指针以获得下一个块 
			pBlock = pEnd;
		}
		else {
			printf("访问受限，可能是因为权限不够!");
			break;
		}
	}
	cout << "共枚举" << count << "内存块" << endl;
}



/**
* 获得当前的内存的配置信息
*/
void GetSystemInfo_List() {

	//获得系统信息 
	SYSTEM_INFO si;

	ZeroMemory(&si, sizeof(si));
	GetSystemInfo(&si);

	DWORD dwMemSize = (DWORD)si.lpMaximumApplicationAddress - (DWORD)si.lpMinimumApplicationAddress;
	TCHAR szMemSize[MAX_PATH];
	StrFormatByteSizeA(dwMemSize, (PSTR)szMemSize, MAX_PATH);

	//将内存信息显示出来 
	//hex以16进制的格式输出
	printf("+-------------------------------+---------------+\n");
	printf("|The physical page size         |%08x kb\t|\n", si.dwPageSize / 1024);
	printf("|The minimum address of program |%08x \t|\n", (DWORD)si.lpMinimumApplicationAddress);
	printf("|The maximum address of program |%08x \t|\n", (DWORD)si.lpMaximumApplicationAddress);
	printf("|The avialable address length   |%08x \t|\n", dwMemSize);
	printf("+-------------------------------+---------------+\n");
	return;
}

/**
* 总的内存的使用情况
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

//用于实时获取各个进程信息
void GetProcessInfo_List() {
	PROCESSENTRY32 pe32;//定义进程信息结构
						/*typedef struct tagPROCESSENTRY32{
						DWORD dwSize;						//结构的大小
						DWORD cntUsage;					    //此进程的引用数
						DWORD th32ProcessID;				//PID
						ULONG_PTR th32DefaultHeapID;		//进程默认堆，默认为0
						DWORD th32ModuleID;					//进程模块ID
						DWORD cntThreads;					//此进程开启的进程数
						DWORD th32ParentProcessID;			//父进程的PID
						LONG pcPriClassBase;						//线程优先权
						DWORD dwtemps;								//此成员不再被使用，默认为0
						TCHAR szExeFile[MAX_PATH];			//进程全名（字符数组）
						} PROCESSENTRY32, *PPROCESSENTRY32;*/
	pe32.dwSize = sizeof(PROCESSENTRY32);

	HANDLE hProcessShot;
	hProcessShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);    //获取进程列表，最后一个参数是0
																	   // 创建系统当前进程快照
	BOOL bRet;
	int i = 0;
	//从第一个进程开始
	bRet = Process32First(hProcessShot, &pe32);//从第一个进程开始
	for (int i = 0; i < 1000; i++)
		ProcessInfoList[i] = 0;
	printf("+-------+-------------------------------------+-------------+-------------------+\n");
	while (bRet) {
		//wcslen 返回宽字节字符串的字节个数
		size_t len = wcslen(pe32.szExeFile) + 1;
		size_t converted = 0;
		char *CStr;
		CStr = (char*)malloc(len * sizeof(char));
		//类型转换
		wcstombs_s(&converted, CStr, len, pe32.szExeFile, _TRUNCATE);
		ProcessInfoList[i] = pe32.th32ProcessID;
		
		printf("|[%03d]\t|%37s|PID: [%05d] |cntThreads: %d\t|\n", i, CStr, pe32.th32ProcessID, pe32.cntThreads);//序号，进程名
		bRet = Process32Next(hProcessShot, &pe32);
		i++;
	}//遍历进程快照
	printf("+-------+-------------------------------------+-------------+-------------------+\n");
	ProcessTotalNum = i - 1;
	CloseHandle(hProcessShot);
}

int main() {
	int temp = 4;
	cout << "操作系统实验四-内存监视器";
	while (1) {
		cout << endl << "请选择功能: " << endl
			<< "0 - 退出" << endl
			<< "1 - 查看内存配置信息" << endl
			<< "2 - 查看内存使用信息" << endl
			<< "3 - 查看当前运行进程信息" << endl
			<< "请按照数字选取当前操作" << endl;
		cin >> temp;
		if (temp == 0) {
			return 0;
		}
		else if (temp == 1) {
			GetSystemInfo_List();	//获取实时内存配置信息
		}
		else if (temp == 2) {
			GetMemoryInfo_List();
		}
		else if (temp == 3) {
			GetProcessInfo_List();
			int f, i, PID;
			cout << "查看某一进程的虚拟内存信息，按序号查询输入1，按PID查询输入2" << endl;
			cin >> f;
			
			if (f == 1) {
				cout << "请输入序号" << endl;
				cin >> i;
				if (i > ProcessTotalNum || i < 0) { 
					printf("无当前序号对应的进程");
					continue;
				}
				else {
					PID = ProcessInfoList[i];
					HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, PID);
					if (hProcess == 0x00000000) {
						printf("发生错误！ 返回的句柄为空，可能是因为无权限访问！");
						continue;
					}
					WalkVM(hProcess);
					continue;
				}
			}
			else if(f == 2){
				cout << "请输入进程PID" << endl;
				cin >> PID;
				HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, PID);	//获取PID值
				WalkVM(hProcess);
				continue;
			}
			else {
				printf("请输入正确的指令！");
				continue;
			}
		}
	}
	return 0;
	
}
