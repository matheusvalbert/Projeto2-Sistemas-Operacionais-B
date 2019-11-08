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

int encrypt(unsigned char cifrado[]) {
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

	input = kmalloc(DATA_SIZE, GFP_KERNEL);
        if (!input) {

                printk("kmalloc(input) failed\n");
                goto out;
        }

	input[0] = 0x30;
	input[1] = 0x31;
	input[2] = 0x32;
	input[3] = 0x33;
	input[4] = 0x34;
	input[5] = 0x35;
	input[6] = 0x36;
	input[7] = 0x37;
	input[8] = 0x38;
	input[9] = 0x39;
	input[10] = 0x41;
	input[11] = 0x42;
	input[12] = 0x43;
	input[13] = 0x44;
	input[14] = 0x45;
	input[15] = 0x46;

	sg_init_one(&sg, input, DATA_SIZE);
	skcipher_request_set_crypt(req, &sg, &sg, DATA_SIZE, NULL);
	init_completion(&result.completion);

	ret = crypto_skcipher_encrypt(req);

	//printk("output criptografia: "); hexdump(input, DATA_SIZE);

	switch (ret) {
	case 0:
		break;
	case -EINPROGRESS:
	case -EBUSY:
		ret = wait_for_completion_interruptible(&result.completion);
		break;
	}

	for(i = 0; i < DATA_SIZE; i++)
		cifrado[i] = input[i];

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
	unsigned char cifrado[16], *file;
	int i;

	encrypt(cifrado);

	printk("output criptografia: "); hexdump(cifrado, DATA_SIZE);

	file = kmalloc(nbytes*2, GFP_KERNEL);

	if (!file) {

                printk("kmalloc(file) failed\n");
                goto out;
        }

	for(i = 0; i< nbytes; i++) {
		sprintf(&file[i*2], "%02x",cifrado[i]);
	}

	if (fd >= 0) {
		mm_segment_t old_fs = get_fs();
		set_fs(KERNEL_DS);
		sys_write(fd, file, nbytes*2);
		set_fs(old_fs);
		kfree(file);
		return 0;
  	}

out:
	kfree(file);
	return 1;
}
























