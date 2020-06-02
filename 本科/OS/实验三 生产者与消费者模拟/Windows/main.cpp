#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <process.h>
#include <tchar.h>

#define ID_M 0										//���ؽ��̱��
#define ID_P_FROM 1							//�����߽�����ʼ���
#define ID_P_TO 2								//�����߽��̽������
#define ID_C_FROM 3							//�����߽�����ʼ���
#define ID_C_TO 5								//�����߽��̽������
#define PROCESS_NUM 5						//�ӽ������������߽���+�����߽��̣�
#define WORKS_P 6								//������ѭ������
#define WORKS_C 4								//������ѭ������

#define LETTER_NUM 3							//����������
#define SHM_NAME "BUFFER"				//��������

//�������ṹ
struct mybuffer
{
	char letter[LETTER_NUM];
	int head;
	int tail;
	int is_empty;
	int index;            // ������
	HANDLE semEmpty;      //���ź������ 
	HANDLE semFull;		  //���ź������	
	HANDLE semMutex;	  //��������ź���
};


//�ļ�ӳ�������
static HANDLE hMapping;

//�ӽ��̾������
static HANDLE hs[PROCESS_NUM + 1];

//�õ�0-1000���ڵ�һ�������
int get_random()
{
	int t;
	srand((unsigned)(GetCurrentProcessId() + time(NULL)));
	t = rand() % 3001;
	return t;
}

//�õ�A~Z��һ�������ĸ
char get_letter()
{
	char a;
	srand((unsigned)(getpid() + time(NULL)));
	a = (char)((char)(rand() % 26) + 'A');
	return a;
}

//���̿�¡��ͨ���������ݽ��̵����кţ�
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
	//�����ӽ���
	BOOL bCreateOK = CreateProcess(szFilename, szCmdLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
	hs[nCloneID] = pi.hProcess;
	return;
}

/*	����һ�������ļ�ӳ�����
*	���ع����ļ�ӳ�����ľ��
*/
HANDLE MakeSharedFile()
{
	/*	�����ļ�ӳ�����
	*	INVALID_HANDLE_VALUE ��ʾ 0xFFFFFFFF ��ʾ��Ч�ľ����
	*	�˴����Դ���һ�����̼乲����ڴ�ӳ�����
	*/
	HANDLE hMapping = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(struct mybuffer), SHM_NAME);

	if (hMapping != INVALID_HANDLE_VALUE)
	{
		/*	���ļ�����ӳ�䵽���̵ĵ�ַ�ռ�
		*	��һ������Ϊӳ�����ľ�����ڶ�������Ϊ�ƶ��ļ��ķ���Ȩ��FILE_MAP_ALL_ACCESS��ʾӳ��
		*	�ļ��ɶ���д
		*	������������ʾ�ļ�ӳ�����ĸ�32λ��ַ�����ĸ�����Ϊ��32λ��ַ
		*	���ĸ�������ʾ�ļ���Ҫӳ����ֽ�����0��ʾӳ�������ļ�
		*	������óɹ�������ֵΪ���ļ����ڴ��е���ʵ��ַ
		*/
		LPVOID pData = MapViewOfFile(hMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		if (pData != NULL)
		{
			ZeroMemory(pData, sizeof(struct mybuffer));
		}
		//�ر��ļ���ͼ
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

//������
int main(int argc, char * argv[])
{
	int i, j, k;
	int pindex = 1; //��һ��Ҫ�������ӽ��̵����к�
	int nClone = ID_M; //���������к�
	char lt;
	SYSTEMTIME systime;

	//����в���������Ϊ�ӽ��̵����к�
	if (argc > 1)
	{
		sscanf(argv[1], "%d", &nClone);
	}

	//�������н���
	// printf("Process ID: %d , Serial No: %d\n", GetCurrentProcessId(), nClone);

	//�������ؽ���
	if (nClone == ID_M)
	{
		printf("Main process starts.\n");
		//���������ļ�
		printf("The main process ID: %d\n", GetCurrentProcessId());
		hMapping = MakeSharedFile();
		/*	��һ���Ѿ����ڵ��ļ�ӳ�����
		*	FILE_MAP_ALL_ACCESS ָ���ļ�ӳ�����Ĵ�ȡ���ʷ�ʽ
		*	ָ���̳б�ǣ�����ȷ�����صľ���Ƿ��ڽ��̴����ڼ����������̼̳�
		*	SHM_NAME ָ����Ҫ�򿪵��ļ�ӳ����������
		*	����Ҫ�򿪵��ļ�ӳ�������
		*/
		HANDLE hFileMapping = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, SHM_NAME);
		//	����ӳ�������ڴ濪ʼ��ַ
		LPVOID pFile = MapViewOfFile(hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		
		if (pFile != NULL)
		{	
			//��ʼ�������ڴ�����
			struct mybuffer * shmp = (struct mybuffer *)(pFile);
			shmp->head = 0;
			shmp->tail = 0;
			shmp->index = 0;
			/*	����һ���ź�������
			*	��һ��������ʾ��ȫ����
			*	�ڶ�������Ϊ�ź����ĳ�ʼֵ��������ڻ��ߵ���0
			*	����������Ϊ�ź��������ֵ
			*	���ĸ�����Ϊ�ź���������
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

		//���δ����ӽ���
		while (pindex <= PROCESS_NUM)
		{
			StartClone(pindex++);
		}
		//�ȴ��ӽ������
		for (k = 1; k < PROCESS_NUM + 1; k++)
		{
			WaitForSingleObject(hs[k], INFINITE);
			CloseHandle(hs[k]);
		}
		printf("Main process ends.\n");
	}
	//���������߽���
	else if (nClone >= ID_P_FROM && nClone <= ID_P_TO)
	{
		//printf("Producer %d process starts.\n", nClone - ID_M);
		//ӳ����ͼ
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
	//���������߽���
	else if (nClone >= ID_C_FROM && nClone <= ID_C_TO)
	{
		//printf("Consumer %d process starts.\n", nClone - ID_P_TO);
		//ӳ����ͼ
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