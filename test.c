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
#include <string.h>

///home/matheus/Desktop/arquivo.txt

static void hexdump(unsigned char *buf, unsigned int len) {

        while (len--)
	{
		printf("%c", *buf++);
	}

        printf("\n");
}

int main()
{  
	void *buf;
	int fd, i, qtd_caracteres, k = 0;
	long int ret_status;
	int multiplo;
	char string[512], string1[512], local[100], c;
	FILE *fp;

	printf("\nDigite o local do arquivo: ex: /home...: ");
	__fpurge(stdin);
	fgets(local, sizeof(local), stdin);
	
	printf("\nDigite a string para ser criptografada: ");
	__fpurge(stdin);
	fgets(string, sizeof(string), stdin);

	qtd_caracteres = strlen(string) -1;

	printf("\nString a ser criptografada: %s\n", string);

	__fpurge(stdin);

	buf = string;
	remove(local);
	fd = open(local, O_WRONLY|O_CREAT, 0644);
	ret_status = syscall(334, fd, buf, qtd_caracteres); // 334 write
	close(fd);

	__fpurge(stdin);
	
	multiplo = qtd_caracteres;

	while(multiplo >31) {

		multiplo -= 32;
		k++;
	}

	if(multiplo != 0)
		k++;

	__fpurge(stdin);

	fp = fopen(local, "rt");
	if(fp == NULL) {
		printf("Problema leitura");
		return 1;
	}

	fseek(fp, 0, SEEK_SET);
	fscanf(fp, "%s", string1);
	printf("\nString criptografada: %s\n", string1);
	fclose(fp);

	printf("\nString Descriptografada: ");

	__fpurge(stdin);

	fd = open(local, O_RDONLY);
        buf = string;
	ret_status = syscall(333, fd, buf, k*32); // 333 read
	close(fd);

	__fpurge(stdin);

	for(i = 0; i < qtd_caracteres; i++)
		printf("%c", ((uint8_t*)buf)[i]);
	printf("\n");

	__fpurge(stdin);

	return 0;
}























