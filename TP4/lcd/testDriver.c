#include "lcd_driver.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <string.h>

#define SYMBOL "\xb6\xb2\xd9\xdd & \xd4\xbd\xcf"
#define TEXT "une trs longue phraseeeeee as dsd !"

void set_cus(struct CusChar *cus, int num, char buff[8]) {
	cus -> num = 1;
	memcpy(cus->str, buff, 8);
}

int main(int argc, char const *argv[])
{
	struct Pos p;
	struct CusChar *cus;
	char test[80];
	char F1 = CHINA_FLAG_1;
	char F2 = CHINA_FLAG_2;
	char F3 = CHINA_FLAG_3;
	char F4 = CHINA_FLAG_4;


	int fd = open("/dev/lcd_driverkz",O_RDWR);

	set_cus(cus, 0, F1);
	ioctl(fd, LCDIOCT_SETCUSTOM, cus);

	set_cus(cus, 1, F2);
	ioctl(fd, LCDIOCT_SETCUSTOM, cus);

	set_cus(cus, 2, F3);
	ioctl(fd, LCDIOCT_SETCUSTOM, cus);

	set_cus(cus, 3, F4);
	ioctl(fd, LCDIOCT_SETCUSTOM, cus);

	test[0] = 0;
	test[1] = 1;
	test[2] = 2;
	test[3] = 3;
	test[4] = '\0';

	write(fd, test, 4);

	close(fd);
	
	return 0;
}