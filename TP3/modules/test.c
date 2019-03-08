#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define LED0 4
#define BUTTON 18
#define MAIN


#ifdef MAIN2
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

#else

#include <stdio.h>

#define NBLED 2
#define NBBP 1
char led[NBLED];
char bp[NBBP];
 
int main()
{
   int i;
   int fd = open("/dev/ledKZ2", O_RDWR);
   if (fd < 0) {
      fprintf(stderr, "Erreur d'ouverture du pilote LED et Boutons\n");
      exit(1);
   }
   for( i = 0; i < NBLED; i ++) {
      led[i] = '0';
   }
   do { 
      led[0] = (led[0] == '0') ? '1' : '0';
      write( fd, led, NBLED);
      sleep( 1);
      read( fd, bp, 1);
   } while (bp[0] == '1');
   return 0;
}
#endif