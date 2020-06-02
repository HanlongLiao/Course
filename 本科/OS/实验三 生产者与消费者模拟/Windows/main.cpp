#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <process.h>
#include <tchar.h>

#define ID_M 0										//主控进程编号
#define ID_P_FROM 1							//生产者进程起始编号
#define ID_P_TO 2								//生产者进程结束编号
#define ID_C_FROM 3							//消费者进程起始编号
#define ID_C_TO 5								//消费者进程结束编号
#define PROCESS_NUM 5						//子进程数（生产者进程+消费者进程）
#define WORKS_P 6								//生产者循环次数
#define WORKS_C 4								//消费者循环次数

#define LETTER_NUM 3							//缓冲区长度
#define SHM_NAME "BUFFER"				//缓冲区名

//缓冲区结构
struct mybuffer
{
	char letter[LETTER_NUM];
	int head;
	int tail;
	int is_empty;
	int index;            // 步骤标记
	HANDLE semEmpty;      //空信号量句柄 
	HANDLE semFull;		  //满信号量句柄	
	HANDLE semMutex;	  //互斥访问信号量
};


//文件映射对象句柄
static HANDLE hMapping;

//子进程句柄数组
static HANDLE hs[PROCESS_NUM + 1];

//得到0-1000以内的一个随机数
int get_random()
{
	int t;
	srand((unsigned)(GetCurrentProcessId() + time(NULL)));
	t = rand() % 3001;
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

//进程克隆（通过参数传递进程的序列号）
void StartClone(int nCloneID)
{
	char szFilename[MAX_PATH];
	char szCmdLine[MAX_PATH];
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	GetModuleFileName(NULL, szFilename, MAX_PATH);
	sprintf(szCmdLine, "\"%s\" %d", szFilename, nCloneID);
	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);
	//创建子进程
	BOOL bCreateOK = CreateProcess(szFilename, szCmdLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
	hs[nCloneID] = pi.hProcess;
	return;
}

/*	创建一个共享文件映射对象
*	返回共享文件映射对象的句柄
*/
HANDLE MakeSharedFile()
{
	/*	创建文件映射对象
	*	INVALID_HANDLE_VALUE 表示 0xFFFFFFFF 表示无效的句柄，
	*	此处可以创建一个进程间共享的内存映射对象
	*/
	HANDLE hMapping = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(struct mybuffer), SHM_NAME);

	if (hMapping != INVALID_HANDLE_VALUE)
	{
		/*	将文件对象映射到进程的地址空间
		*	第一个参数为映射对象的句柄，第二个参数为制定文件的访问权限FILE_MAP_ALL_ACCESS表示映射
		*	文件可读可写
		*	第三个参数表示文件映射起点的高32位地址，第四个参数为低32位地址
		*	第四个参数表示文件中要映射的字节数，0表示映射整个文件
		*	如果调用成功，返回值为在文件在内存中的其实地址
		*/
		LPVOID pData = MapViewOfFile(hMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		if (pData != NULL)
		{
			ZeroMemory(pData, sizeof(struct mybuffer));
		}
		//关闭文件视图
		UnmapViewOfFile(pData);
	}
	return (hMapping);
}

void printInfo(SYSTEMTIME systime, struct mybuffer * shmp , int nClone, char lt, int type){
	printf("[%02d]\t", shmp->index);
	printf("Process ID: %d\t", GetCurrentProcessId());
	printf("Time: %02d:%02d:%02d\n", systime.wHour, systime.wMinute, systime.wSecond);
	if(type == 0) printf("Producer %d puts '%c'.\n", nClone - ID_M, lt);
	else{ printf("Consumer %d gets '%c'.\n", nClone - ID_P_TO, lt);}

	
	printf("+---+---+---+\n");

	int n, j;
	for (n = 0, j = (shmp->tail - 1 >= shmp->head) ? (shmp->tail - 1) : (shmp->tail - 1 + LETTER_NUM); !(shmp->is_empty) && j >= shmp->head; j--)
	{
	n++;
	printf("|");
	printf("%3c", shmp->letter[j % LETTER_NUM]);
	}
	for(int k = n + 1; k <= 3; k++ ){
		printf("|   ");
	}
	printf("|\n");
	printf("+---+---+---+\n");

	printf("-------------------------------------------------------------------------\n");
}

//主函数
int main(int argc, char * argv[])
{
	int i, j, k;
	int pindex = 1; //下一个要创建的子进程的序列号
	int nClone = ID_M; //本进程序列号
	char lt;
	SYSTEMTIME systime;

	//如果有参数，就作为子进程的序列号
	if (argc > 1)
	{
		sscanf(argv[1], "%d", &nClone);
	}

	//对于所有进程
	// printf("Process ID: %d , Serial No: %d\n", GetCurrentProcessId(), nClone);

	//对于主控进程
	if (nClone == ID_M)
	{
		printf("Main process starts.\n");
		//创建数据文件
		printf("The main process ID: %d\n", GetCurrentProcessId());
		hMapping = MakeSharedFile();
		/*	打开一个已经存在的文件映射对象
		*	FILE_MAP_ALL_ACCESS 指定文件映射对象的存取访问方式
		*	指定继承标记，用于确定返回的句柄是否在进程创建期间由其他进程继承
		*	SHM_NAME 指定了要打开的文件映射对象的名称
		*	返回要打开的文件映射对象句柄
		*/
		HANDLE hFileMapping = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, SHM_NAME);
		//	返回映射对象的内存开始地址
		LPVOID pFile = MapViewOfFile(hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		
		if (pFile != NULL)
		{	
			//初始化共享内存区域
			struct mybuffer * shmp = (struct mybuffer *)(pFile);
			shmp->head = 0;
			shmp->tail = 0;
			shmp->index = 0;
			/*	创建一个信号量对象
			*	第一个参数表示安全属性
			*	第二个参数为信号量的初始值，必须大于或者等于0
			*	第三个参数为信号量的最大值
			*	第四个参数为信号量的名称
			*/
			shmp->semEmpty = CreateSemaphore(NULL, LETTER_NUM, LETTER_NUM, "SEM_EMPTY");
			shmp->semFull = CreateSemaphore(NULL, 0, LETTER_NUM, "SEM_FULL");
			shmp->semMutex = CreateSemaphore(NULL, 1, 1, "SEM_MUTEX");

			UnmapViewOfFile(pFile);
			pFile = NULL;
		}

		else
		{
			printf("Error on OpenFileMapping.\n");
		}
		CloseHandle(hFileMapping);

		//依次创建子进程
		while (pindex <= PROCESS_NUM)
		{
			StartClone(pindex++);
		}
		//等待子进程完成
		for (k = 1; k < PROCESS_NUM + 1; k++)
		{
			WaitForSingleObject(hs[k], INFINITE);
			CloseHandle(hs[k]);
		}
		printf("Main process ends.\n");
	}
	//对于生产者进程
	else if (nClone >= ID_P_FROM && nClone <= ID_P_TO)
	{
		//printf("Producer %d process starts.\n", nClone - ID_M);
		//映射视图
		HANDLE hFileMapping = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, SHM_NAME);
		LPVOID pFile = MapViewOfFile(hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		
		if (pFile != NULL)
		{
			struct mybuffer * shmp = (struct mybuffer *)(pFile);

			HANDLE semEmpty = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, "SEM_EMPTY");
			HANDLE semFull = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, "SEM_FULL");
			HANDLE semMutex = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, "SEM_MUTEX");

			for (i = 0; i < WORKS_P; i++)
			{
				Sleep(get_random());

				WaitForSingleObject(semEmpty, INFINITE);
				WaitForSingleObject(semMutex, INFINITE);

				shmp->index++;
				shmp->letter[shmp->tail] = lt = get_letter();
				shmp->tail = (shmp->tail + 1) % LETTER_NUM;
				shmp->is_empty = 0;
				GetLocalTime(&systime);

				printInfo(systime, shmp, nClone, lt, 0);

				ReleaseSemaphore(semFull, 1, NULL);
				ReleaseSemaphore(semMutex, 1, NULL);
			}
			
			UnmapViewOfFile(pFile);
			pFile = NULL;
		}
		else
		{
			printf("Error on OpenFileMapping.\n");
		}
		CloseHandle(hFileMapping);
		//printf("Producer %d process ends.\n", nClone - ID_M);
	}
	//对于消费者进程
	else if (nClone >= ID_C_FROM && nClone <= ID_C_TO)
	{
		//printf("Consumer %d process starts.\n", nClone - ID_P_TO);
		//映射视图
		HANDLE hFileMapping = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, SHM_NAME);
		LPVOID pFile = MapViewOfFile(hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		if (pFile != NULL)
		{
			struct mybuffer * shmp = (struct mybuffer *)(pFile);

			HANDLE semEmpty = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, "SEM_EMPTY");
			HANDLE semFull = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, "SEM_FULL");
			HANDLE semMutex = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, "SEM_MUTEX");

			for (i = 0; i < WORKS_C; i++)
			{
				Sleep(get_random());
				WaitForSingleObject(semFull, INFINITE);
				WaitForSingleObject(semMutex, INFINITE);
				
				shmp->index++;
				lt = shmp->letter[shmp->head];
				shmp->head = (shmp->head + 1) % LETTER_NUM;
				shmp->is_empty = (shmp->head == shmp->tail);
				GetLocalTime(&systime);

				printInfo(systime, shmp, nClone, lt, 1);
				
				ReleaseSemaphore(semEmpty, 1, NULL);
				ReleaseSemaphore(semMutex, 1, NULL);
			}
			UnmapViewOfFile(pFile);
			pFile = NULL;
		}
		else
		{
			printf("Error on OpenFileMapping.\n");
		}
		CloseHandle(hFileMapping);
		//printf("Consumer %d process ends.\n", nClone - ID_P_TO);
	}

	CloseHandle(hMapping);
	hMapping = INVALID_HANDLE_VALUE;

	//system("pause\n");
	Sleep(30000);
	return 0;
}