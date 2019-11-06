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
	int fd, j;
	long int ret_status;

	printf("leitura e escrita\n");

	fd = open("/home/matheus/Desktop/arquivo.txt", O_RDONLY);
         
	ret_status = syscall(333, fd, buf, 4); // 333 read

	close(fd);

	if(ret_status == 0)
		for(j = 0; j < 4; ++j)
			printf("%c\n", ((uint8_t*) buf)[j]);
	else
		printf("Erro na leitura\n");

	remove("/home/matheus/Desktop/arquivo.txt");

	fd = open("/home/matheus/Desktop/arquivo.txt", O_WRONLY|O_CREAT, 0644);
         
	ret_status = syscall(334, fd, "aaaaa\n", 4); // 334 write

	close(fd);
         
	if(ret_status == 0)
		printf("escrita efetuada\n");
    	else 
		printf("Erro na escrita\n");
          
	return 0;
}
