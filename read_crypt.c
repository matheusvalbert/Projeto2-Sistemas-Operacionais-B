#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/sched.h>
#include<linux/syscalls.h>
#include "read_write_crypt.h"


asmlinkage ssize_t read_crypt(int fd, void *buf, size_t nbytes) {

	if (fd >= 0) {
		sys_read(fd, buf, nbytes);
		return 0;
	}
	else
		return 1;
}
