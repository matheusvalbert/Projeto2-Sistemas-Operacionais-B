#include <stdio.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stdint.h>
#include <inttypes.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include <stdio_ext.h>

int main()
{  
	void *buf;
	int fd, i;
	long int ret_status;
	
	printf("String: ");

	__fpurge(stdin);

	fd = open("/home/matheus/Desktop/arquivo.txt", O_RDONLY);
         
	ret_status = syscall(333, fd, buf, 32); // 333 read

	close(fd);

	for(i = 0; i < 32; i++)
		printf("%c", ((uint8_t*)buf)[i]);
	printf("\n");

/*
	remove("/home/matheus/Desktop/arquivo.txt");

	fd = open("/home/matheus/Desktop/arquivo.txt", O_WRONLY|O_CREAT, 0644);
         
	ret_status = syscall(334, fd, "aaaaa\n", 16); // 334 write

	close(fd);
         
	if(ret_status == 0)
		printf("escrita efetuada\n");
    	else 
		printf("Erro na escrita\n");

	__fpurge(stdin);
*/

	return 0;
}
