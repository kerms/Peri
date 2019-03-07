#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define LED0 4
#define BUTTON 18

int main(void)
{
	/* code */
	int file = open("/dev/ledKZ2", O_RDWR);
	char buff_read[16];
	char buff_write[16];
	buff_write[0] = (char)0;
	buff_write[1] = (char)1;
	if (file < 0)
	{
		perror("open");
		exit(errno);
	}

	while (1) {
		write(file, &buff_write, 16);
		read(file, &buff_read, 16);
		printf("read buff : [0]: %d - [1]: %d - [2]: %d - [3]: %d\n", buff_read[0], buff_read[1], buff_read[2],buff_read[3] );
		sleep(1);
		
		write(file, &buff_write[1], 16);
		read(file, &buff_read, 16);
		printf("read buff : [0]: %d - [1]: %d - [2]: %d - [3]: %d\n", buff_read[0], buff_read[1], buff_read[2],buff_read[3] );
		sleep(1);
	}

	return 0;
}