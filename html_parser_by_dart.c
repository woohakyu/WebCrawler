#include "../libsrc/string/fn_string.h"

#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#include <iconv.h>
#include <errno.h>

#include <time.h>

#include <dirent.h>

#define BUFFER_1024_SIZE 1024

int htmltag_out(int fd, char **vptr, int *pmax)
{
	ssize_t n, rc, base, cur = 0;
	char c;

	base = BUFFER_1024_SIZE;
	cur = 0;
	while(1)
	{
		again:
		if((rc = read(fd, &c, 1)) == 1)
		{
			if( c == '<' || c == '>')
			{
				*((*vptr)+(cur)) = 0;
				lseek(fd, -1, SEEK_CUR);
				break;
			}
			if(cur >= (*pmax))
			{
				(*pmax) += base;
				(*vptr) = realloc((*vptr),(*pmax));
				memset((*vptr)+cur,0x00,base);
			}
			*((*vptr)+(cur++)) = c;
		} 
		else if (rc == 0) 
		{
			memset((*vptr)+cur, 0x00, (*pmax));
			return (cur - 1); 
		} 
		else 
		{
			if(errno == EINTR) 
				goto again;
			return (-1);
		}
	}
	return (cur);
}

int htmltag_in(int fd, char **vptr, int *pmax)
{
	ssize_t n, rc, base, cur = 0, deep = 0;
	char c;

	base = BUFFER_1024_SIZE;
	cur = 0;
	while(1)
	{
		again:
		if((rc = read(fd, &c, 1)) == 1)
		{
			if(cur >= (*pmax))
			{
				(*pmax) += base;
				(*vptr) = realloc((*vptr),(*pmax));
				memset((*vptr)+cur,0x00,base);
			}
			*((*vptr)+(cur++)) = c;
			if( c == '<' ) deep += 1;
			else if( c == '>' ) deep -= 1;
			if(deep == 0)
			{
				*((*vptr)+(cur)) = 0;
				break;
			}
		} 
		else if (rc == 0) 
		{
			memset((*vptr)+cur, 0x00, (*pmax));
			return (cur - 1); 
		} 
		else 
		{
			if(errno == EINTR) 
				goto again;
			return (-1);
		}
	}
	return (cur);
}

int main(int argc, char **argv)
{
	int nChk = 0;
	int fdRead = 0;
	int fdConf = 0;
	int nIdx = 0;
	int nLength = 0, nltrim;
	int nBufMax = BUFFER_1024_SIZE;
	int fVal = 0;
	int fGet = 0;

	char sReadFile[BUFFER_1024_SIZE + 1];
	char sReadLine[BUFFER_1024_SIZE + 1];
	char ptrMemoryBlock[BUFFER_1024_SIZE + 1];
	char sConfVal[BUFFER_1024_SIZE + 1];

	char *ptrIdx = NULL;
	char *ptrIdx_f = NULL;
	char *ptrIdx_b = NULL;
	char *ptrMem;

	//fdConf = open(sConfFile, O_RDONLY, 0);
	fdConf = open("/home/hakyu/Project/WebCrawler/common_005930.conf", O_RDONLY, 0);
	if(fdConf == -1)
	{
		perror("fdConf ");
		return -1;
	}

	memset(sConfVal,0x00,BUFFER_1024_SIZE + 1);
//	while(1)
//	{
//		nChk = readline(fdConf, sConfVal, BUFFER_1024_SIZE);
//		if(nChk <= 0) break;
//	}

	nChk = readline(fdConf, sConfVal, BUFFER_1024_SIZE);
	printf("check point ... Chk:[%d], Conf:[%d]%s\n", nChk, strlen(sConfVal), sConfVal);

	nChk = chdir("/home/hakyu/Project/WebCrawler/contents/st2");
	if(nChk == -1)
	{
		perror("chdir ");
		return -1;
	}

	memset(sReadFile, 0x00, BUFFER_1024_SIZE + 1);
	sprintf(sReadFile, "read_list_crawler_parser_dart_20171114001694.html");
	fdRead = open(sReadFile, O_RDONLY, 0);
	if(fdRead == -1)
	{
		perror("fdRead ");
		return -1;
	}

	ptrMem = malloc(nBufMax);
	memset(ptrMem, 0x00, nBufMax);
	while(1)
	{
/*
		nIdx = 0;
		if(*(ptrMem+nIdx) == '<')
		{
			nIdx = 1;
			if(*(ptrMem+nIdx) == 'P' || *(ptrMem+nIdx) == 'p') fVal = 1;
			else if(*(ptrMem+nIdx) == '/')
			{
				nIdx = 2;
				if(*(ptrMem+nIdx) == 'P' || *(ptrMem+nIdx) == 'p') fVal = 0;
			}
		}
*/

		if(nLength == 1)
		{
			if(!(*ptrMem == 10 || *ptrMem == 13))
			{
				lseek(fdRead, -1, SEEK_CUR);
				nLength = htmltag_out(fdRead, &ptrMem, &nBufMax);
				nltrim  = ltrim(ptrMem);
				//if(nLength-nltrim) printf("[%d]%s\n",nLength, ptrMem);
		
				//printf("check point ... compare: 1.[%d] 2.[%d]", nLength, nLength-nltrim);
				//printf("3.[%s] 4.[%s], 5.[%d]\n", ptrMem, sConfVal, strlen(sConfVal));
				//output. parsing...
				if(nLength-nltrim && strncmp(ptrMem, sConfVal, strlen(sConfVal))==0) fGet = 1;
				if(nLength-nltrim && fGet == 1 && *ptrMem) printf("[%d][%d]%s\n",nLength, nLength-nltrim, ptrMem);
				if(nLength-nltrim && fGet == 1) printf("[%d][%d]%s\n",nLength, nLength-nltrim, ptrMem);
			}
		}
		//else
			//printf("check point ... %d\n\n[%s]\n\n",nLength, ptrMem);

		//if(fGet == 1)
		//		if(nLength-nltrim) printf("[%d]%s\n",nLength, ptrMem);


		nLength = htmltag_in(fdRead, &ptrMem, &nBufMax);
		if(nLength <= 0) break;
	}

	free(ptrMem);
	close(fdRead);
	return 0;
}
/*
		switch(c) 
		{
			case '<' : 
			case '>' : 
			case '/' : 
			case '!' : 
			case ' ' : 
			case '=' : 
			case '"' :
			case ''' : 
			default: break;
		}
*/
