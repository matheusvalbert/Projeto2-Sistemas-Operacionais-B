#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/sched.h>
#include<linux/syscalls.h>
#include "processInfo.h"

asmlinkage long sys_listProcessInfo(void) {    

	  int fd, i = 0;
	  char buf[1];

	  mm_segment_t old_fs = get_fs();
	  set_fs(KERNEL_DS);

	  fd = sys_open("/home/matheus/Desktop/arquivo.txt", O_RDONLY, 0);
	  if (fd >= 0) {
	    printk(KERN_DEBUG);
	    while (sys_read(fd, buf, 1) == 1)
	      printk("%c", buf[0]);
	    printk("\n");
	    sys_close(fd);
	  }
	  set_fs(old_fs);

   return 0;
}
