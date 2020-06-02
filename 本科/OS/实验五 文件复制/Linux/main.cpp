#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <utime.h>
#include <time.h>

#define MAX_PATH 260
#define BUFFER_MAX 1024

int copyFile(char *source,char *target, int deepth)
{
	int fpSource, fpTarget;
	int nLenSource, nLenTarget;
	char buffer[BUFFER_MAX];
	struct stat statbuf;

	//stat结构用于存储文件类型
	//获取源文件相关信息，成功返回0，失败返回-1 
	lstat(source, &statbuf);
	//open为打开文件函数
	//par1: 文件的指针
	//par2: 指定了打开文件的方式
	//如果打开成功，返回文件的描述符，否则，返回-1   
	if ((fpSource = open(source, 0)) == -1)
	{
		printf("The file '%s' can not be opened! \n", source);
		return 0;
	}
	if ((fpTarget = creat(target, statbuf.st_mode)) == -1)
	{
		//printf("The file '%s' can not be opened! \n", target);
		close(fpSource);
		return 0;
	}
	memset(buffer, 0, BUFFER_MAX);
	//read和wirte函数
	//par1: 文件的描述符
	//par2: 读出或者写入文件的字符数组
	//par3: 要传送的字节个数
	//返回值：正确的时候返回0或者读写的字节数，错误的时候返回0

	while ((nLenSource = read(fpSource,buffer,BUFFER_MAX)) > 0)
	{
		if ((nLenTarget = write(fpTarget,buffer,nLenSource)) != nLenSource)
		{
			printf("Write to file '%s' failed!\n", target);
			close(fpSource);
			close(fpTarget);
			return 0;
		}
		memset(buffer, 0, BUFFER_MAX);
	}
	printf("%*s Copyfile: %s\n", 4 * deepth, "", source);
	close(fpSource);
	close(fpTarget);
	return 1;
}

void SetTime(char * source, char* target)
{
	struct stat statbuf;	
	lstat(source, &statbuf);
	struct timespec filetime[2] = {statbuf.st_atim,statbuf.st_mtim};	//得到源文件的相关时间 
	//纳秒级数量级改变时间戳
	//通过两个时间来改变时间戳，最后访问时间和最后改动时间
	utimensat(AT_FDCWD,target, filetime,AT_SYMLINK_NOFOLLOW);			//通过filetime[2]改变目标文件时间 
}

void CopyAllFile(char* source, char* target, int deepth)
{
	char cSource_temp[MAX_PATH];
	char cTarget_temp[MAX_PATH];
	char cSource_temp_all[MAX_PATH];

	DIR *dp;

	struct dirent *entry;	
	struct stat statbuf;

	if ((dp = opendir(source)) == NULL)	
	{
		printf("opendir Error.\n");
	}

	while ((entry = readdir(dp)) != NULL)		
	{
		lstat(entry->d_name, &statbuf);	//文件名
		if ((strcmp(".", entry->d_name) != 0) && (strcmp("..", entry->d_name) != 0))  //文件夹
		{
			if (entry->d_type == 4)	//目录类型 
			{
				strcpy(cSource_temp, source);
				strcat(cSource_temp, "/");
				strcat(cSource_temp, entry->d_name);
				strcpy(cTarget_temp, target);
				strcat(cTarget_temp, "/");
				strcat(cTarget_temp, entry->d_name);
				lstat(cSource_temp, &statbuf);
				mkdir(cTarget_temp, statbuf.st_mode);
				printf("%*s Copyfolder: %s\n", 4 * deepth, "", cSource_temp);
				int a = deepth;
				a ++;
				CopyAllFile(cSource_temp, cTarget_temp, a);		
				SetTime(cSource_temp, cTarget_temp);
			}
			if (entry->d_type == 10)//链接文件 
			{
				char buf[1024];
				memset(buf,0,1024);
				ssize_t len = 0;
				
				strcpy(cSource_temp, source);
				strcat(cSource_temp, "/");
				strcat(cSource_temp, entry->d_name);
				strcpy(cTarget_temp, target);
				strcat(cTarget_temp, "/");
				strcat(cTarget_temp, entry->d_name);
				
				len = readlink(cSource_temp, buf, 1024 - 1);
				buf[len] = '\0';
				symlink(buf, cTarget_temp);
				SetTime(cSource_temp, cTarget_temp);				
			 } 
			else   //当前为文件
			{
				strcpy(cSource_temp, source);
				strcat(cSource_temp, "/");
				strcat(cSource_temp, entry->d_name);
				strcpy(cTarget_temp, target);
				strcat(cTarget_temp, "/");
				strcat(cTarget_temp, entry->d_name);
				int a = deepth;
				copyFile(cSource_temp, cTarget_temp, a);
				SetTime(cSource_temp, cTarget_temp);
			}
		}
	}
}


int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		printf("Arguments is not appropriate.\n");
	}
	else
	{
		struct stat statbuf;
		DIR *dp;
		lstat(argv[1], &statbuf);
		//opendir()打开指定目录，如果失败返回NULL,成功的话返回指针
		//此处判断需要复制的文件是文件还是目录
		if ((dp = opendir(argv[1])) == NULL)	
		{
			copyFile(argv[1], argv[2], 0);
			SetTime(argv[1], argv[2]);
		}
		else
		{
			mkdir(argv[2], statbuf.st_mode);	//建立目标目录文件，保护模式和源文件保持相同dd
			CopyAllFile(argv[1], argv[2], 0);		
			SetTime(argv[1], argv[2]);
		}

	}
}