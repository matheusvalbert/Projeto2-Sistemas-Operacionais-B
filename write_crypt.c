#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/sched.h>
#include<linux/syscalls.h>
#include <linux/file.h>
#include <linux/fs.h>
#include <linux/fcntl.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include "read_write_crypt.h"

static char key_getu = "0123456789ABCDEF";

asmlinkage ssize_t write_crypt(int fd, const void *buf, size_t nbytes)
{
	struct crypto_skcipher *tfm;
	struct skcipher_request *req;
	struct scatterlist sg;
	struct tcrypt_result result
	char *file;
	char key[80], *input = NULL, *output = NULL;
	int i = 0, ret, j=0;

	file = kmalloc(nbytes, GFP_KERNEL);

	if (!file) {

                printk("kmalloc(file) failed\n");
                goto out;
        }

	sprintf(file, "%s", ((uint8_t*) buf));

	if (fd >= 0) {
		mm_segment_t old_fs = get_fs();
		set_fs(KERNEL_DS);
		sys_write(fd, file, nbytes);
		set_fs(old_fs);
		kfree(file);
		return 0;
  	}


	Serial.begin(115200);
 
	int size = sizeof(buf);
	 
	 
	for (int i = 0; i < size; i++)
	{
	input[i] = buf[i];
	}
	 
	for (int i = size; i < 16; i++)
	{
	input[i] = 0;
	}
	 
	mbedtls_aes_init(&aes);
	mbedtls_aes_setkey_enc(&aes, key, 128);
	mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_ENCRYPT, input, output);

	sprintf(file, "%s", ((uint8_t*) output));

	printk("input criptografia: "); hexdump(input, DATA_SIZE);

	ret = crypto_skcipher_encrypt(req);

	printk("output criptografia: "); hexdump(input, DATA_SIZE);

out:
	kfree(file);
	return 1;
}
