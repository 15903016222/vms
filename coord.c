#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

int main (int argc, char *argv[]) {
	int sim_fd;
	int x, y;
	char buffer[12];

	sim_fd = open ("/dev/vmouse", O_RDWR);
	if (sim_fd < 0) {
		perror ("open");
		exit (-1);
	}
	
	int i = 0;

	for (i = 0; i < 10; ++i) {
		memset (buffer, i * 2, sizeof (buffer));
		write (sim_fd, buffer, sizeof (buffer));
		fsync (sim_fd);
		sleep (3);
	}

	close (sim_fd);

	return 0;
}
