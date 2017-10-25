#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#define SOCKET_BUFFER_SIZE  (32768U)

/* �ο�:
 * frameworks\native\libs\input\InputTransport.cpp
 */
void *function_thread1(void *args)
{
	int fd = (int)args;
	char buf[512];
	int len;
	int cnt = 0;

	while(1)
	{
		/* �����̷߳���: Hello, main thread */
		len = sprintf(buf, "Hello, main thread, cnt=%d", cnt++);
		write(fd, buf, len);

		/* ��ȡ����(main�̷߳��ص�����) */
		len = read(fd, buf, 512);
		buf[len] = '\0';
		printf("main thread return: %s\n",buf);

	}
	return NULL;

}


int main(int argc, char **argv)
{
	int sockets[2];
	
	socketpair(AF_UNIX, SOCK_SEQPACKET, 0, sockets);
	
	int bufferSize = SOCKET_BUFFER_SIZE;	
	
	setsockopt(sockets[0], SOL_SOCKET, SO_SNDBUF, &bufferSize, sizeof(bufferSize));    
	setsockopt(sockets[0], SOL_SOCKET, SO_RCVBUF, &bufferSize, sizeof(bufferSize));	  
	setsockopt(sockets[1], SOL_SOCKET, SO_SNDBUF, &bufferSize, sizeof(bufferSize));	 
	setsockopt(sockets[1], SOL_SOCKET, SO_RCVBUF, &bufferSize, sizeof(bufferSize));


	/* �����߳�1 */
	pthread_t threadID;

	int return_create_thread1 = pthread_create(&threadID, NULL, function_thread1, (void *)sockets[1]);

	if(return_create_thread1 == 0)
	{
		int len;
		char buf[512];
		int fd = sockets[0];
		int cnt = 0;

		while(1)
		{
			/* ��ȡthread1���������� */
			len = read(fd,buf,512);
			buf[512] = '\0';
			printf("read data from thread1: %s\n",buf);

			/* main thread ��thread1��������: Hello, thread1 */
			len = sprintf(buf,"Hello, thread1, cnt = %d\n",cnt++);
			write(fd, buf, len);	
		}
	}
	return 0;
}





