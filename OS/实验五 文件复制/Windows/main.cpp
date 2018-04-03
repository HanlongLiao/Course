
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include <wchar.h>
#include <windows.h>
#include <windowsx.h>

typedef struct _DIRWALK
{
	int depth;		//深度
	BOOL FindNext;	//查找成功
	TCHAR DesPath[MAX_PATH];		//目标路径名
	TCHAR DestFullName[MAX_PATH];	//目标文件名
	WIN32_FIND_DATA FindData;		//存放查找到的文件信息
}DIRWALK, *pDIRWALK;

//判断是否为父路径
BOOL IsParentDir(LPCTSTR pszRootPath, LPCTSTR DesRootPath)
{
	TCHAR PathTemp[MAX_PATH];
	_tcscpy_s(PathTemp, DesRootPath);
	PathTemp[_tcslen(pszRootPath)] = '\0';
	return (_tcscmp(PathTemp, pszRootPath) <= 0);//字符串比较 是否为前缀
}


void CopyTime(TCHAR * Src, TCHAR * Des)
{
	FILETIME CreationTime, LastAccessTime, LastWriteTime;
	HANDLE hSrc = CreateFile(Src, 
		GENERIC_READ,			 //进行读访问
		FILE_SHARE_READ,		 //共享读
		NULL,					 //默认安全特性
		OPEN_EXISTING,			 //文件必须已经存在
		FILE_FLAG_BACKUP_SEMANTICS,	//打开文件夹 适当的检查
		NULL
		);
	HANDLE hDes = CreateFile(Des,
		GENERIC_READ | GENERIC_WRITE, //进行读写访问
		FILE_SHARE_READ,		//共享读
		NULL,					//默认安全特性
		OPEN_ALWAYS,			//如文件不存在则创建
		FILE_FLAG_BACKUP_SEMANTICS,	//打开文件夹 适当的检查
		//You must set this flag to obtain a handle to a directory. 
		//A directory handle can be passed to some functions instead of a file handle.
		NULL
		);
	GetFileTime(hSrc, &CreationTime, &LastAccessTime, &LastWriteTime);//获取文件时间
	SetFileTime(hDes, &CreationTime, &LastAccessTime, &LastWriteTime);//设置文件时间
	CloseHandle(hSrc);
	CloseHandle(hDes);	//关闭文件句柄
}

void myCopyFile(TCHAR * src, TCHAR * des)
{
	FILETIME CreationTime, LastAccessTime, LastWriteTime;
	HANDLE hSrc = CreateFile(src,
		GENERIC_READ,			//进行读访问
		FILE_SHARE_READ,		//共享读
		NULL,					//默认安全特性
		OPEN_EXISTING,			//文件必须已经存在
		FILE_ATTRIBUTE_NORMAL,	//默认属性
		NULL
		);
	HANDLE hDes = CreateFile(des,
		GENERIC_READ | GENERIC_WRITE, //进行读写访问
		FILE_SHARE_READ,		//共享读
		NULL,					//默认安全特性
		CREATE_ALWAYS,			//创建文件,改写前一个文件
		FILE_ATTRIBUTE_NORMAL,	//默认属性
		NULL
		);
	DWORD FileLength = GetFileSize(hSrc, NULL);	//获取文件大小
	DWORD dwWritenSize, dwReadSize;
	char * Buffer = new char[FileLength + 1];
	ReadFile(hSrc, Buffer, FileLength, &dwReadSize, NULL);		//读取文件
	WriteFile(hDes, Buffer, FileLength, &dwWritenSize, NULL);	//写入文件
	GetFileTime(hSrc, &CreationTime, &LastAccessTime, &LastWriteTime);//获取文件时间
	SetFileTime(hDes, &CreationTime, &LastAccessTime, &LastWriteTime);//设置文件时间
	delete[] Buffer;	//销毁内存
	CloseHandle(hSrc);	//关闭文件句柄
	CloseHandle(hDes);
}

static void DirWalk(pDIRWALK DW)
{
	DW->depth++;
	//par1: 表示查找的文件的类型
	//par2: 查找到的文件的属性保存位置，包括文件名，文件的创建时间等等
	HANDLE hFind = FindFirstFile(_TEXT("*.*"), &DW->FindData);
	while (DW->FindNext)
	{
		if ((lstrcmp(DW->FindData.cFileName, _TEXT(".")) == 0) ||
			(lstrcmp(DW->FindData.cFileName, _TEXT("..")) == 0));
		//是父路径或当前路径  取下一个
		//判断当前是否是文件夹
		else if (DW->FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			_stprintf_s(DW->DesPath, _TEXT("%s\\%s"), DW->DesPath, 	DW->FindData.cFileName);//目标文件名
			//swprintf(DW->DesPath, L"%s\\%s", DW->DesPath, DW->FindData.cFileName);
			//swprintf(DW->DesPath, "%s\\%s", DW->DesPath, DW->FindData.cFileName);
			printf("%*sCreateDirectory %s\n", 4 * DW->depth, "", DW->DesPath);
			CreateDirectory(DW->DesPath, NULL);			//创建文件夹
			//切换当前的进程的当前工作目录	
			SetCurrentDirectory(DW->FindData.cFileName);
			DirWalk(DW);								//递归遍历复制
			SetCurrentDirectory(_TEXT(".."));			//返回上一层文件夹
			_tcscpy(DW->FindData.cFileName, _tcsrchr(DW->DesPath, _TEXT('\\')) + 1);
			CopyTime(DW->FindData.cFileName, DW->DesPath);//复制文件夹时间
			_tcsrchr(DW->DesPath, _TEXT('\\'))[0] = 0;
		}
		else
		{
			_stprintf(DW->DestFullName, _TEXT("%s\\%s"), DW->DesPath, DW->FindData.cFileName);
			//swprintf(DW->DestFullName, L"%s\\%s", DW->DesPath, DW->FindData.cFileName);
			printf("%*sCopyFile %s\n", 4 * DW->depth, "",DW->FindData.cFileName);
			myCopyFile(DW->FindData.cFileName, DW->DestFullName);		//复制文件函数
		} 
		DW->FindNext = FindNextFile(hFind, &DW->FindData);//查找下一个文件
	}
	if (hFind != INVALID_HANDLE_VALUE)
		FindClose(hFind);
	DW->depth--;
}

int main(int argc, TCHAR *argv[])
{
	if (argc != 3)
	{
		printf_s("argv error\n");
		exit(0);
	}

	TCHAR szCurrDir[_MAX_DIR];
	DIRWALK DW;
	//保存原来的目录信息
	//par1: the length of buffer for current directory string 
	//par2: a pointer to the buffer that recives the current directory string
	//return value: the number of character that are wtitten to buffer
	GetCurrentDirectory(sizeof(szCurrDir) / sizeof(szCurrDir[0]), szCurrDir);

	//源目录不存在
	if (!SetCurrentDirectory(argv[1]))
	{
		printf_s("源目录不存在 不能复制\n");
		exit(-1);
	}
	//存在递归关系，不能复制
	if (IsParentDir(argv[1], argv[2]))
	{
		printf_s("目标目录是源目录的子目录 不能复制\n");
		exit(-1);
	}

	DW.depth = -1;
	DW.FindNext = TRUE;

	_tcscpy_s(DW.DesPath, argv[2]);
	//par1: the path of the directory to be created
	//par2: a pointer to security_attributes structure
	//return value: if succeed, return nozero, if faild, return zero
	CreateDirectory(DW.DesPath, NULL);
	DirWalk(&DW);//遍历

	SetCurrentDirectory(szCurrDir);	//恢复当前目录
	return 0;
}