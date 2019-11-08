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

char convert(char op) {

	switch(op) {

	case '0':
		return 0x0;
	case '1':
		return 0x1;
	case '2':
		return 0x2;
	case '3':
		return 0x3;
	case '4':
		return 0x4;
	case '5':
		return 0x5;
	case '6':
		return 0x6;
	case '7':
		return 0x7;
	case '8':
		return 0x8;
	case '9':
		return 0x9;
	case 'A':
	case 'a':
		return 0xa;
	case 'B':
	case 'b':
		return 0xb;
	case 'C':
	case 'c':
		return 0xc;
	case 'D':
	case 'd':
		return 0xd;
	case 'E':
	case 'e':
		return 0xe;
	case 'F':
	case 'f':
		return 0xf;
	}

	return 0;
}

struct tcrypt_result {
	struct completion completion;
	int err;
};

static void test_skcipher_cb(struct crypto_async_request *req, int error) {

	struct tcrypt_result *result = req->data;

	if (error == -EINPROGRESS)
		return;
	result->err = error;
	complete(&result->completion);
}

static void hexdump(unsigned char *buf, unsigned int len) {

        while (len--)
	{
		printk("%02x", *buf++);
	}

        printk("\n");
}

int decrypt(unsigned char cifrar[], int numop, int nbytes) {

struct crypto_skcipher *tfm;
	struct skcipher_request *req;
	struct scatterlist sg;
	struct tcrypt_result result;
	unsigned char *input = NULL;
	int ret, i;
	unsigned char key[16] = "0123456789ABCDEF";

	tfm = crypto_alloc_skcipher("ecb(aes)", 0, 0);
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

	skcipher_request_set_callback(req, CRYPTO_TFM_REQ_MAY_BACKLOG, test_skcipher_cb, &result);

	ret = crypto_skcipher_setkey(tfm, key, DATA_SIZE);
	if (ret != 0) {

		pr_err("ERROR: crypto_skcipher_setkey\n");
		goto error_req;
	}

	input = kmalloc(DATA_SIZE + 1, GFP_KERNEL);
        if (!input) {

                printk("kmalloc(input) failed\n");
                goto out;
        }

	i=0;
	while(i!=16) {

		input[i] = cifrar[i+16*numop];
		i++;
	}

	sg_init_one(&sg, input, DATA_SIZE);
	skcipher_request_set_crypt(req, &sg, &sg, DATA_SIZE, NULL);
	init_completion(&result.completion);

	ret = crypto_skcipher_decrypt(req);

	//printk("output descriptografado: "); hexdump(input, DATA_SIZE);

	switch (ret) {
	case 0:
		break;
	case -EINPROGRESS:
	case -EBUSY:
		ret = wait_for_completion_interruptible(&result.completion);
		break;
	}

	i=0;
	while(i!=16) {

		cifrar[i+16*numop] = input[i];
		if(input[i] == 0) {

			nbytes = i+16*numop;
		}
		else {
			nbytes = i+16*numop+1;
		}
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

asmlinkage ssize_t read_crypt(int fd, void *buf, size_t nbytes) {

	int i=0, j=0, numop = 0;
	mm_segment_t old_fs;

	if (fd >= 0) {

		old_fs = get_fs();
  		set_fs(KERNEL_DS);
		sys_read(fd, buf, nbytes*2);
		printk("leitura: %s", ((uint8_t*)buf));
		printk("\n");

		i=0;
		//Transforma a string em numero hexa
		while(i != nbytes*2) {
			
			((uint8_t*)buf)[i] = convert(((uint8_t*)buf)[i]);
			i++;
		}
		//Uniao dos numeros em hexa
		i=0;
		while(i != nbytes) {

			((uint8_t*)buf)[i] = (((uint8_t*)buf)[j]<<4) + ((uint8_t*)buf)[j+1];		
			
			i++;
			j+=2;
		}

		numop = nbytes/16;
		i = 0;
		while(i!=numop) {

			decrypt(buf, i, nbytes);
			i++;
		}
		for(i = 0; i < nbytes; i++)
			printk("string: %c\n", ((uint8_t*)buf)[i]);
		printk("output geral: "); hexdump(buf, nbytes);
		set_fs(old_fs);
		return 0;
	}
	else
		return 1;
}























