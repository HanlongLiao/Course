一、实验目的
1. 熟悉文件操作的相关系统函数
熟悉Linux和Windows两种环境中文件操作的相关函数，掌握创建文件、复制文件、读写文件的方法。理解各个函数的作用，理解函数参数。
2. 理解文件系统的概念
通过对文件的复制操作，理解Linux和Windows两种环境下文件系统的作用。

二、实验内容
完成一个目录复制命令mycp，包括目录下的文件和子目录, 运行结果如下：
beta@bugs.com [~/]# ls –la sem
total 56
drwxr-xr-x  3 beta beta 4096 Dec 19 02:53 ./
drwxr-xr-x  8 beta beta 4096 Nov 27 08:49 ../
-rw-r--r--  1 beta beta  128 Nov 27 09:31 Makefile
-rwxr-xr-x  1 beta beta 5705 Nov 27 08:50 consumer*
-rw-r--r--  1 beta beta  349 Nov 27 09:30 consumer.c
drwxr-xr-x  2 beta beta 4096 Dec 19 02:53 subdir/
beta@bugs.com [~/]# mycp sem target
beta@bugs.com [~/]# ls –la target
total 56
drwxr-xr-x  3 beta beta 4096 Dec 19 02:53 ./
drwxr-xr-x  8 beta beta 4096 Nov 27 08:49 ../
-rw-r--r--  1 beta beta  128 Nov 27 09:31 Makefile
-rwxr-xr-x  1 beta beta 5705 Nov 27 08:50 consumer*
-rw-r--r--  1 beta beta  349 Nov 27 09:30 consumer.c
drwxr-xr-x  2 beta beta 4096 Dec 19 02:53 subdir/
并且要求：
Linux: creat，read，write等系统调用，要求支持软链接
Windows: CreateFile(), ReadFile(), WriteFile(), CloseHandle()等函数
特别注意复制后，不仅权限一致，而且时间属性也一致。
三、实验环境及配置方法
1.	Windows环境：
操作系统：Windows 10 家庭中文版
编译环境：Visual Studio Code 
		  gcc 4.9.2
2.	Linux环境：
操作系统：Ubuntu 16.04 LTS
编译环境：GCC 5.2.0
编写代码软件：Visual Studio Code
四、实验实现步骤
1.实验基本思路
使用文件（夹）复制函数在给定的文件或文件夹目录中进行遍历：
a) 如果遍历到的是单一文件，则直接将源文件复制到目标目录
下。
b) 如果遍历到的是文件夹，那么就在目标目录中创建一个和源
文件夹相同名称的文件夹。并且再以遍历到的文件夹路径作
为源目录参数，将新建的文件夹目录作为目标路径递归调用
文件（夹）复制函数。
c) 当源文件（夹）被遍历结束后， 程序完成全部文件的复制，
结束运行。
2. windows系统调用API介绍
(1). CreateDirectory() 创建目录
BOOL WINAPI CreateDirectory(
	LPCTSTR               lpPathName,             //是新创建目录的路径名
	LPSECURITY_ATTRIBUTES lpSecurityAttributes    //是为目录设置的安全属性结构，若该参数为NULL,
												  //则设置默认为默认的安全属性
);
成功调用时返回True, 否则返回false, 且通过函数GetLastError()可以获得错误码
(2). RemoveDirecotory() 删除目录
BOOL WINAPI RemoveDirectory(
	LPCTSTR lpPathName        //为要删除的目录名
);
成功时返回true, 否则返回false。当进程没有删除权限或者目录中没有文件或其他子目录时，删除失败。
(3). GetCurrentDirectory() 得到当前目录
DWORD WINAPI GetCurrentDirectory(
DWORD  nBufferLength,            //为缓冲区的最大字符数
LPTSTR lpBuffer                  //是指向存放当前目录的缓冲区的指针
);
函数调用成功时，用进程的当前目录填充有参数lpBuffer指向的缓冲区，并返回复制到缓冲区中的字符数（不包括结尾的0）。使用
函数GetLastError() 函数来获得错误信息。
为了确保函数调用成功，通常使用如下的形式：
TCHAR szCurDir[MAX_PATH];
DWORD dwCurDir = GetCurrentDirectory(sizeof(dzCurDir)/sizeof(TCHAR), szCurDir);
其中，MAX_PATH定义在文件WINDEF中，大小为260。如果没有定义，可以自行第一该常量。
(4).  SetCurrentDiretory() 改变当前目录
BOOL WINAPI SetCurrentDirectory(
	LPCTSTR lpPathName          //是指存放当前目录字符串的缓冲区的指针
);
返回非0表示成功，0表示失败，且通过函数GetLastError()可以获得错误码
(5). FindFirstFile() 查找指定文件路径的文件
HANDLE WINAPI FindFirstFile(
	LPCTSTR           lpFileName,        //指向一个以0结尾的字符串的文件名，文件名可以包含通配符(* 和 ?)
	LPWIN32_FIND_DATA lpFindFileData     //是一个Win32_FIND_DATA结构的地址
);
如果调用成功，返回值为非0，否则返回值为0，且可通过GetLastError()函数来获得错误信息。
其中Win32_FIND_DATA结构的定义如下：
typedef struct _WIN32_FIND_DATA {
  DWORD    dwFileAttributes;      //文件属性
  FILETIME ftCreationTime;        //文件创建时间
  FILETIME ftLastAccessTime;      //文件最后一次访问时间
  FILETIME ftLastWriteTime;       //文件最后一次修改时间
  DWORD    nFileSizeHigh;         //文件长度高32位
  DWORD    nFileSizeLow;          //文件长度低32位
  DWORD    dwReserved0;           //系统保留
  DWORD    dwReserved1;           //系统保留
  TCHAR    cFileName[MAX_PATH];   //长文件名
  TCHAR    cAlternateFileName[14];//8.3 格式文件名
} WIN32_FIND_DATA, *PWIN32_FIND_DATA, *LPWIN32_FIND_DATA;
(6). FindNextFile() 查找FindFirstFile()函数搜索之后的下一个文件
BOOL WINAPI FindNextFile(
	HANDLE            hFindFile,       //是调用FindFirstFile时，返回的文件句柄
	LPWIN32_FIND_DATA lpFindFileData   //是一个Win32_Find_Data结构的地址
);
如果调用函数成功，返回值为非0，否则返回值为0，且可以通过GetLastError()函数来获取错误信息。
(7). FindClose() 关闭由FindFirstFile()函数搜索到的文件句柄
BOOL WINAPI FindClose(
	HANDLE hFindFile            //为要关闭的由FindFirstFile()返回的文件句柄
);
如果调用成功，返回值为非0，否则返回值为0，且可通过GetLastError()来获得错误信息
(8). GetFileTime 检索文件或目录，上次访问和上次修改的日期和时间
BOOL WINAPI GetFileTime(
	HANDLE     hFile,            //要检索日期和时间的文件或目录的句柄
	LPFILETIME lpCreationTime,   //指向FILETIME结构的指针，用于接收创建文件或目录的日期和时间。
								 //如果应用程序不需要此信息，则此参数可以为NULL
	LPFILETIME lpLastAccessTime, //指向FILETIME结构的指针，用于接收上次访问文件或目录的日期和时间
	LPFILETIME lpLastWriteTime   //指向FILETIME结构的指针，用于接收上次写入文件或目录的日期和时间，
								 //截断或覆盖（例如，使用WriteFile或 SetEndOfFile）
);

如果函数成功，返回值为非零。
如果函数失败，返回值为零。要获得扩展的错误信息，可调用 GetLastError。
(8). SetFileTime() 设置创建，上次访问或上次修改指定文件或目录的日期和时间
BOOL WINAPI SetFileTime(
	HANDLE   hFile,
	const FILETIME *lpCreationTime,      //文件或目录的句柄。该句柄必须使用具有FILE_WRITE_ATTRIBUTES
										 //访问权限的CreateFile函数 创建 
	const FILETIME *lpLastAccessTime,    //指向包含文件或目录的新创建日期和时间的FILETIME结构的指针
	const FILETIME *lpLastWriteTime      //指向FILETIME结构的指针，该结构包含文件或目录的新的上次修改日期和时间
);
如果函数成功，返回值为非零。
如果函数失败，返回值为零。要获得扩展的错误信息，可调用 GetLastError。
(9). CreateFile() 创建获得打开文件
HANDLE WINAPI CreateFile(
	LPCTSTR               lpFileName,             //为指向一个以NULL结束的字符串的指针，该字符串是打开或者创建
												  //文件等对象的名字
	DWORD                 dwDesiredAccess,        //为指定对象的访问类型，可能的访问类型为读，写，读写访问或者
												  //查询设备等类型
	DWORD                 dwShareMode,            //为规定与其他进程共享文件的方式，0为不允许共享。FILE_SHARE_READ 
												  //或 FILE_SHARE_WRITE 表示运行对文件进程共享访问
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,   // 为指向一个SECURITY_ARRTIBUTES结构的指针，该结构指定一个安全对象，
												  //在创建文件时使用。如果为NULL,表示使用默认安全对象
	DWORD                 dwCreationDisposition,  //为指定如何创建文件
	DWORD                 dwFlagsAndAttributes,   //为指定文件的属性和标志
	HANDLE                hTemplateFile           //如果不为0，则表示一个文件句柄。新文件将从这个文件中赋值扩展属性
);
如果函数调用成功，返回值为指向指定文件的打开句柄；如果函数调用失败，返回值为INVALD_HENDLE_VALUE，且会设置GetLastError.
(10). ReadFile() 从文件中读取数据
BOOL WINAPI ReadFile(
	HANDLE       hFile,                         //设备的句柄
	LPVOID       lpBuffer,                      //指向接收从文件或设备读取的数据的缓冲区的指针。
												//这个缓冲区在读操作期间必须保持有效。在读取操作完成之前，调用方
												//不得使用此缓冲区。
	DWORD        nNumberOfBytesToRead,          //指向接收使用同步hFile参数时读取的字节数的变量的指针 
	LPDWORD      lpNumberOfBytesRead,           //如果使用FILE_FLAG_OVERLAPPED打开hFile参数， 则需要指向OVERLAPPED
												//结构的指针，否则它可以为NULL
	LPOVERLAPPED lpOverlapped                   //如果使用FILE_FLAG_OVERLAPPED打开hFile参数， 则需要指向OVERLAPPED结构的指针，
												//否则它可以为NULL。
												//如果使用FILE_FLAG_OVERLAPPED打开hFile，则 lpOverlapped参数必须指向有效且唯一
												//的OVERLAPPED结构，
												//否则该函数可能会错误地报告读取操作已完成。
);
如果函数成功，则返回值为非零（TRUE），如果函数失败或异步完成，则返回值为零（FALSE）。要获得扩展的错误信息，可调用 GetLastError函数。
(11). WriteFile() 向文件中写数据
BOOL WINAPI WriteFile(
	HANDLE       hFile,                    //文件或I / O设备的句柄
	LPCVOID      lpBuffer,                 //指向包含要写入文件或设备的数据的缓冲区的指针。
										   //这个缓冲区在写操作期间必须保持有效，写操作完成之前，调用方不得使用此缓冲区
	DWORD        nNumberOfBytesToWrite,    //要写入文件或设备的字节数
	LPDWORD      lpNumberOfBytesWritten,   //指向接收使用同步hFile参数时写入的字节数的变量的指针 
	LPOVERLAPPED lpOverlapped              //如果使用FILE_FLAG_OVERLAPPED打开hFile参数， 则需要指向OVERLAPPED结构的指针，
										   //否则此参数可以为 NULL
);
如果函数成功，则返回值为非零（TRUE），如果函数失败或异步完成，则返回值为零（FALSE）。要获得扩展的错误信息，可调用 GetLastError函数。