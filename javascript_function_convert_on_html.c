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

#define BUFFER_16_SIZE 16
#define BUFFER_1024_SIZE 1024
#define MAX_BUFFER BUFFER_1024_SIZE

ssize_t readn(int fd, void *vptr, size_t n)
{
	size_t nleft;
	size_t nread;
	char *ptr;

	ptr = vptr;
	nleft = n;
	while(nleft > 0)
	{
		if((nread = read(fd, ptr, nleft)) < 0)
		{
			if(errno == EINTR) nread = 0; // and call read() again
			else return (-1);
		}
		else if(nread == 0) break; // EOF
		nleft -= nread;
		ptr += nread;
	}
	return (n - nleft); // return >= 0
}

ssize_t readline(int fd, void *vptr, size_t maxlen)
{
	ssize_t n, rc;
	char c, *ptr;

	ptr = vptr;
	for (n = 1; n < maxlen; n++) {
		again:
			if ( (rc = read(fd, &c, 1)) == 1) {
				*ptr++ = c;
			if ( c == 10 ) break;
			} else if (rc == 0) {
					*ptr = 0;
					return (n - 1);
			} else {
				if (errno == EINTR)
					goto again;
				return (-1);
			}
		}
	*ptr = 0;
	return (n);
}

int ParseByHtml(int fd, char **vptr, int *pmax)
{
	char c;
	unsigned int dec = 0;
	int base, rc, cur = 0, is_a_num = 0, is_a_txt = 0;

	base = MAX_BUFFER;
	for(;;)
	{
		rc = read(fd, &c, 1);
		if(rc == 1)
		{
			if(cur >= (*pmax))
			{
printf("check point ... ParseByHtml cur[%d], pmax[%d]\n", cur, (*pmax));
				(*pmax) += base;
				(*vptr) = realloc((*vptr),(*pmax));
				memset((*vptr)+cur,0x00,base);
			}
			*((*vptr)+(cur++)) = c;

			if(c == 13 || c == 10)
			{
				if(is_a_txt == is_a_num && is_a_num > 0)
				{
					rc = read(fd, &c, 1); // is read ascii '10';; dummy clear
					if(dec > 0)
					{
						base = dec;
						if(cur+dec >= (*pmax))
						{
							(*pmax) += base;
							(*vptr) = realloc((*vptr),(*pmax));
							memset((*vptr)+cur,0x00,base);
						}
						rc = readn(fd, (*vptr)+cur-3-is_a_num, base);
						base = MAX_BUFFER;
						cur += (rc-3-is_a_num);
					}
					else if(dec == 0)
					{
						memset((*vptr)+cur-3-is_a_num, 0x00, (*pmax)-(cur-3-is_a_num));
						break;
					}
				}

				is_a_num = 0;
				is_a_txt = 0;
				dec = 0;
				continue;
			}

			is_a_txt += 1;

			if((c >= '0' && c <= '9') ||
					(c >= 'A' && c <='F') || (c >= 'a' && c <='f'))
			{
				is_a_num += 1;

				dec <<= 4;
				if(c >= '0' && c <= '9')
				{
					dec += c & 0x0F;
				}
				else if((c >= 'A' && c <='F') || (c >= 'a' && c <='f'))
				{
					if(c >= 'a' && c <='f') c &= 0xDF;
					dec += (c & 0x07) + 9;
				}
				continue;
			}
			is_a_num = 0;
			dec = 0;
		}
	}

	if(is_a_txt == is_a_num && is_a_num > 0) cur = cur-3-is_a_num;

	return cur;
}

int connect_server(char *hostname, int hostport)
{
	int socket_fd;
	struct sockaddr_in name;
	struct hostent *hostinfo;

	socket_fd = socket(AF_INET, SOCK_STREAM, 0);

	name.sin_family = AF_INET;
	hostinfo = gethostbyname(hostname);

	if(hostinfo == NULL) return 1;
	else name.sin_addr = *((struct in_addr *)hostinfo->h_addr);
	name.sin_port = htons(hostport);

	// Connect to web server.
	if(connect(socket_fd, (struct sockaddr *)&name, sizeof(struct sockaddr)) == -1)
	{
		perror("connect");
		return 1;
	}
	return socket_fd;
}

int main(int argc, char **argv)
{
	char *ptrPoint = NULL;
	char *ptrIdx_f = NULL;
	char *ptrIdx_b = NULL;
	char *ptrReadFile = NULL;
	char *ptrMemoryBlock;

	char sReadFile[ BUFFER_1024_SIZE + 1 ];
	char sReadFileOld[ BUFFER_1024_SIZE + 1 ];
	char sReadFileNew[ BUFFER_1024_SIZE + 1 ];
	char sWriteFile[ BUFFER_1024_SIZE + 1 ];
	char sReadLine[ BUFFER_1024_SIZE + 1 ];
	char sUrl[ BUFFER_1024_SIZE + 1 ];
	char sAddrCokie[ BUFFER_1024_SIZE + 1 ];

	int nBufMax = BUFFER_1024_SIZE;
	int fdRead = 0;
	int fdWrite = 0;
	int fdWebSock = 0;
	int fCondition = 0;
	int nBufLen = 0;
	int nMemorySize = 0;
	int pMemorySize = 0;
	int nUsageLength = 0;
	int nChk = 0;

	char rcpno[ BUFFER_16_SIZE ];
	char dcmno[ BUFFER_16_SIZE ];
	char eleId[ BUFFER_16_SIZE ];
	char offset[ BUFFER_16_SIZE ];
	char length[ BUFFER_16_SIZE ];
	char dtd[ BUFFER_16_SIZE ];

	DIR *dir_info;
	struct dirent *dir_entry;

	nChk = chdir("/home/hakyu/Project/WebCrawler/contents/st1");
	if(nChk == -1)
	{
		perror("chdir ");
		return -1;
	}

	dir_info = opendir(".");
	if( dir_info == NULL )
	{
		perror("opendir ");
		return -1;
	}

	ptrMemoryBlock = malloc(1);
	memset(ptrMemoryBlock, 0x00, 1);

	while(dir_entry = readdir( dir_info ))
	{
		ptrReadFile = NULL;
		if( (ptrReadFile = strstr(dir_entry->d_name, "read_list_crawler_url_dart_")) != NULL )
		{
			if( strncmp(ptrReadFile, "read_", 5) ) continue;
/* processed start */
// processed ing

			memset( sReadFile, 0x00, BUFFER_1024_SIZE );
			sprintf( sReadFile, "%s", ptrReadFile);
			fdRead = open(sReadFile, O_RDONLY, 0);
			if(fdRead == -1)
			{
				perror("fdRead ");
				return -1;
			}

			fCondition = 0;
			while( 1 )
			{
				if( fCondition )
				{
					ptrPoint == NULL;
					if( (ptrPoint = strstr(sReadLine, "viewDoc")) != NULL )
					{
						ptrIdx_f = strstr( ptrPoint, "'");
						ptrIdx_b = strstr( ptrIdx_f + 1 , "'");
						memset(rcpno, 0x00, BUFFER_16_SIZE);
						memcpy(rcpno, ptrIdx_f + 1, ptrIdx_b - ptrIdx_f - 1);

						ptrIdx_f = strstr( ptrIdx_b + 1, "'");
						ptrIdx_b = strstr( ptrIdx_f + 1 , "'");
						memset(dcmno, 0x00, BUFFER_16_SIZE);
						memcpy(dcmno, ptrIdx_f + 1, ptrIdx_b - ptrIdx_f - 1);

						ptrIdx_f = strstr( ptrIdx_b + 1, "'");
						ptrIdx_b = strstr( ptrIdx_f + 1 , "'");
						memset(eleId, 0x00, BUFFER_16_SIZE);
						memcpy(eleId, ptrIdx_f + 1, ptrIdx_b - ptrIdx_f - 1);

						ptrIdx_f = strstr( ptrIdx_b + 1, "'");
						ptrIdx_b = strstr( ptrIdx_f + 1 , "'");
						memset(offset, 0x00, BUFFER_16_SIZE);
						memcpy(offset, ptrIdx_f + 1, ptrIdx_b - ptrIdx_f - 1);

						ptrIdx_f = strstr( ptrIdx_b + 1, "'");
						ptrIdx_b = strstr( ptrIdx_f + 1 , "'");
						memset(length, 0x00, BUFFER_16_SIZE);
						memcpy(length, ptrIdx_f + 1, ptrIdx_b - ptrIdx_f - 1);

						ptrIdx_f = strstr( ptrIdx_b + 1, "'");
						ptrIdx_b = strstr( ptrIdx_f + 1 , "'");
						memset(dtd, 0x00, BUFFER_16_SIZE);
						memcpy(dtd, ptrIdx_f + 1, ptrIdx_b - ptrIdx_f - 1);

						fCondition = 0;
						break;
					}
				}

				if( strstr(sReadLine, " 연결재무제표") != NULL ) fCondition = 1;

				memset(sReadLine, 0x00, BUFFER_1024_SIZE + 1);
				if( readline(fdRead, sReadLine, nBufMax)  <= 0 ) break;
			}
			close(fdRead);

			memset(sAddrCokie, 0x00, BUFFER_1024_SIZE + 1);
			sprintf(sAddrCokie,"/report/viewer.do");
			sprintf(sAddrCokie,"%s?rcpNo=%s",sAddrCokie, rcpno);
			sprintf(sAddrCokie,"%s&dcmNo=%s",sAddrCokie, dcmno);
			if(eleId != NULL) sprintf(sAddrCokie,"%s&eleId=%s",sAddrCokie, eleId);
			if(offset != NULL) sprintf(sAddrCokie,"%s&offset=%s",sAddrCokie, offset);
			if(length != NULL) sprintf(sAddrCokie,"%s&length=%s",sAddrCokie, length);
			sprintf(sAddrCokie,"%s&dtd=%s",sAddrCokie, dtd);

			memset(sUrl, 0x00, BUFFER_1024_SIZE + 1);
			sprintf(sUrl,"GET http://dart.fss.or.kr%s HTTP/1.1\r\n", sAddrCokie);
			sprintf(sUrl,"%sHost: dart.fss.or.kr\r\n", sUrl);
			sprintf(sUrl,"%sAccept-Encoding: text/html, deflate\r\n",sUrl);
			sprintf(sUrl,"%sAccept: text/html\r\n",sUrl);
			sprintf(sUrl,"%sAccept-Language: ko-kr\r\n",sUrl);

			fdWebSock = connect_server("dart.fss.or.kr", 80);
			if( fdWebSock <= 1 )
			{
				perror("WebSocket ");
				return -1;
			}

			nBufLen = write(fdWebSock, sUrl, nBufMax);
			if( nBufLen <= 0 )
			{
				perror("fdWebSock Write ");
				return -1;
			}

printf("check point ... 1 URL[%s]", sUrl);
			nMemorySize = ParseByHtml(fdWebSock, &ptrMemoryBlock, &nUsageLength);
			memset(sWriteFile, 0x00, BUFFER_1024_SIZE + 1);
			sprintf( sWriteFile, "../st2/read_list_crawler_parser_dart_%s.html", rcpno);
			fdWrite = open(sWriteFile, O_WRONLY|O_CREAT, 0644 );
			if(fdWrite == -1)
			{
				perror("Fie..");
				return -1;
			}
			write(fdWrite, ptrMemoryBlock, nMemorySize);
			close(fdWrite);

			/*// processed finish */
			memset(sReadFileOld, 0x00, BUFFER_1024_SIZE + 1);
			memset(sReadFileNew, 0x00, BUFFER_1024_SIZE + 1);
			sprintf(sReadFileOld, "%s", ptrReadFile);
			sprintf(sReadFileNew, "readed_%s", ptrReadFile + 5);

			nChk = rename(ptrReadFile, sReadFileNew);
			if(nChk == -1)
			{
				perror("rename ");
				return -1;
			}
		}
	}
	closedir( dir_info );
}
