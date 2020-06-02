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

	//stat�ṹ���ڴ洢�ļ�����
	//��ȡԴ�ļ������Ϣ���ɹ�����0��ʧ�ܷ���-1 
	lstat(source, &statbuf);
	//openΪ���ļ�����
	//par1: �ļ���ָ��
	//par2: ָ���˴��ļ��ķ�ʽ
	//����򿪳ɹ��������ļ��������������򣬷���-1   
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
	//read��wirte����
	//par1: �ļ���������
	//par2: ��������д���ļ����ַ�����
	//par3: Ҫ���͵��ֽڸ���
	//����ֵ����ȷ��ʱ�򷵻�0���߶�д���ֽ����������ʱ�򷵻�0

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
	struct timespec filetime[2] = {statbuf.st_atim,statbuf.st_mtim};	//�õ�Դ�ļ������ʱ�� 
	//���뼶�������ı�ʱ���
	//ͨ������ʱ�����ı�ʱ�����������ʱ������Ķ�ʱ��
	utimensat(AT_FDCWD,target, filetime,AT_SYMLINK_NOFOLLOW);			//ͨ��filetime[2]�ı�Ŀ���ļ�ʱ�� 
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
		lstat(entry->d_name, &statbuf);	//�ļ���
		if ((strcmp(".", entry->d_name) != 0) && (strcmp("..", entry->d_name) != 0))  //�ļ���
		{
			if (entry->d_type == 4)	//Ŀ¼���� 
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
			if (entry->d_type == 10)//�����ļ� 
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
			else   //��ǰΪ�ļ�
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
		//opendir()��ָ��Ŀ¼�����ʧ�ܷ���NULL,�ɹ��Ļ�����ָ��
		//�˴��ж���Ҫ���Ƶ��ļ����ļ�����Ŀ¼
		if ((dp = opendir(argv[1])) == NULL)	
		{
			copyFile(argv[1], argv[2], 0);
			SetTime(argv[1], argv[2]);
		}
		else
		{
			mkdir(argv[2], statbuf.st_mode);	//����Ŀ��Ŀ¼�ļ�������ģʽ��Դ�ļ�������ͬdd
			CopyAllFile(argv[1], argv[2], 0);		
			SetTime(argv[1], argv[2]);
		}

	}
}