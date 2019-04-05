#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

int main()
{
	int		fd, err;
	char    *myfifo = "/tmp/myfifo";

	/* create the FIFO (named pipe) */
	err = mkfifo(myfifo, 0666);
	if (err == -1) {
		perror(myfifo);
		return err;
	}

	/* write "Hi" to the FIFO */
	fd = open(myfifo, O_WRONLY);
	if (fd == -1) {
		perror(myfifo);
		return -1;
	}
	
    for(int i=0; i<5; i++) {
        char    buf[80];
        int len=sprintf(buf, "hello %d fois from c\n", i+1);
		write(fd, buf, len);
		sleep(1);
	}
	write(fd, "end\n", sizeof("end"));
	close(fd);

	/* remove the FIFO */
	unlink(myfifo);

	return 0;
}
