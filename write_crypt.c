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

asmlinkage ssize_t write_crypt(int fd, const void *buf, size_t nbytes)
{
	char *file;

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

out:
	kfree(file);
	return 1;
}
