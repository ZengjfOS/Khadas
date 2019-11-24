#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/rtc.h>
#include <linux/ioctl.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>

#define FPGA_DEV "/dev/fpga"

int main(int, const char*[]) {
	int fd = open(FPGA_DEV, O_RDWR);
	if (fd ==-1) 
		perror("open device node error");                  

	ioctl(fd, 0x001, 0);

	close(fd);
}
