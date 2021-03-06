#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/sched.h>
#include<linux/syscalls.h>
#include <linux/file.h>
#include <linux/fs.h>
#include <linux/fcntl.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/err.h>
#include <linux/scatterlist.h>
#include <linux/string.h>
#include <crypto/md5.h>
#include <crypto/internal/skcipher.h>
#include <crypto/rng.h>
#include "read_write_crypt.h"

#define DATA_SIZE       16

struct tcrypt_result //Struct para armazenar o resultado
{
	struct completion completion;
	int err;
};

static void test_skcipher_cb(struct crypto_async_request *req, int error) //Função de callback
{

	struct tcrypt_result *result = req->data;

	if (error == -EINPROGRESS)
		return;
	result->err = error;
	complete(&result->completion);
}

static void hexdump(unsigned char *buf, unsigned int len) //Função para impressão dos resultados no buffer do sistema
{

        while (len--)
	{
		printk("%02x", *buf++);
	}

        printk("\n");
}

int encrypt(unsigned char cifrado[], int numop, int nbytes) //Função para criptografia
{
	struct crypto_skcipher *tfm;
	struct skcipher_request *req;
	struct scatterlist sg;
	struct tcrypt_result result;
	unsigned char *input = NULL;
	int ret, i;
	unsigned char key[16] = "0123456789ABCDEF";//Chave definida

	tfm = crypto_alloc_skcipher("ecb(aes)", 0, 0);//Algoritmo de criptografia definido
	if (IS_ERR(tfm)) {

		ret = PTR_ERR(tfm);
		return ret;
	}

	req = skcipher_request_alloc(tfm, GFP_KERNEL);
	if (IS_ERR(req)) {

		pr_err("ERROR: skcipher_request_alloc\n");
		ret = PTR_ERR(req);
		goto error_tfm;
	}

	skcipher_request_set_callback(req, CRYPTO_TFM_REQ_MAY_BACKLOG, test_skcipher_cb, &result);//Setando a função de callback

	ret = crypto_skcipher_setkey(tfm, key, DATA_SIZE);
	if (ret != 0) {

		pr_err("ERROR: crypto_skcipher_setkey\n");
		goto error_req;
	}

	input = kmalloc(DATA_SIZE, GFP_KERNEL);
        if (!input) {

                printk("kmalloc(input) failed\n");
                goto out;
        }

	i=0;
	while(i!=16) {//Realizando a cópia dos dados criptografados para o input

		input[i] = cifrado[i+16*numop];
		i++;
	}

	sg_init_one(&sg, input, DATA_SIZE);
	skcipher_request_set_crypt(req, &sg, &sg, DATA_SIZE, NULL);
	init_completion(&result.completion);

	ret = crypto_skcipher_encrypt(req);//Realizando a criptografia

	switch (ret) {
	case 0:
		break;
	case -EINPROGRESS:
	case -EBUSY:
		ret = wait_for_completion_interruptible(&result.completion);
		break;
	}

	i=0;
	while(i!=16) {//Realizando a passagem dos dados de volta para o vetor inicial

		cifrado[i+16*numop] = input[i];
		i++;
	}

error_req:
	skcipher_request_free(req);
error_tfm:
	crypto_free_skcipher(tfm);
	return ret;
out:
	kfree(input);
	return 0;
}

asmlinkage ssize_t write_crypt(int fd, const void *buf, size_t nbytes)
{
	int i, numop;
	unsigned char string[256], file[512];
	ssize_t retorno;
	if (fd >= 0) {
		for(i = 0; i< nbytes; i++) {
			sprintf(&string[i], "%c",((char *)buf)[i]);
		}

		printk("string copiada: %s\n", string);
	
		i = nbytes;
	
		while(i % 16 != 0) {//Completando a string com zeros caso ela não seja múltiplo de 16 bytes
			
			string[i] = 0x00;	
			i++;
		}

		nbytes = i;
		numop = nbytes/16;

		i = 0;
		while(i!=numop) {//Realizando a função de criptografia

			encrypt(string, i, nbytes);
			i++;
		}	

		printk("output criptografia: "); hexdump(string, nbytes);//Impressão dos resultados no buffer do sistema

		for(i = 0; i< nbytes; i++) {//Transformando os dados criptografados em número hexadecimal na tabela ASCII
			sprintf(&file[i*2], "%02x", string[i]);
		}

		printk("output file: "); hexdump(file, nbytes*2);//Impressão dos resultados no buffer do sistema

		mm_segment_t old_fs = get_fs();
		set_fs(KERNEL_DS);
		retorno = sys_write(fd, file, nbytes*2);	
		set_fs(old_fs);
		return retorno;
  	}
	else
		return -1;
}
























