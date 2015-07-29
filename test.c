#include <stdio.h>
#include <pty.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#define PATH_MAX 512
#define BUFFER_SIZE	1024
#define SH_CMD	"/bin/bash"
#define INPUT_CMD	"pwd\n"

int main(void){
	int rc = 0;
	char slaveName[PATH_MAX] = {0};
	char buffer[BUFFER_SIZE] = {0};
	int count = 0;
	int masterFD;
	int slaveFD = forkpty(&masterFD, (char*)&slaveName, NULL, NULL);
	if (slaveFD<0){
		rc = errno;
		goto out;
	}

	if (!slaveFD){
		execlp(SH_CMD, SH_CMD, NULL);
		_exit(0);
	}

	printf("slaveName = %s\n", slaveName);
	write(masterFD, INPUT_CMD, strlen(INPUT_CMD));
	while(1){
		count = read(masterFD, buffer, 1024);	
		if (count){
			printf("buffer = %s", buffer);
		}
	}

out:
	if (rc){
		printf("err:%s\n", strerror(errno));
	}
}
