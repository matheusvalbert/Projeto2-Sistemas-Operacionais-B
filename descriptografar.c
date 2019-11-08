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


asmlinkage ssize_t read_crypt(int fd, void *buf, size_t nbytes) {

struct crypto_skcipher *tfm;
	struct skcipher_request *req;
	struct scatterlist sg;
	struct tcrypt_result result;
	char *input = NULL;
	int ret;
	void *a = NULL;
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

	input[0] = 0x85;
	input[1] = 0xd8;
	input[2] = 0x12;
	input[3] = 0x76;
	input[4] = 0x13;
	input[5] = 0xd6;
	input[6] = 0xdc;
	input[7] = 0x35;
	input[8] = 0xba;
	input[9] = 0x90;
	input[10] = 0xab;
	input[11] = 0x08;
	input[12] = 0x65;
	input[13] = 0x67;
	input[14] = 0x4a;
	input[15] = 0xcf;

	sg_init_one(&sg, input, DATA_SIZE);
	skcipher_request_set_crypt(req, &sg, &sg, DATA_SIZE, a);
	init_completion(&result.completion);

	ret = crypto_skcipher_decrypt(req);

	printk("output criptografia: "); hexdump(input, DATA_SIZE);

	switch (ret) {
	case 0:
		break;
	case -EINPROGRESS:
	case -EBUSY:
		ret = wait_for_completion_interruptible(&result.completion);
		break;
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