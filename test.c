#include <stdio.h>
#include <pty.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/select.h>

#define PATH_MAX 512
#define BUFFER_SIZE	1024
#define SH_CMD	"/bin/bash"

void* showOutputInfo(void* args){
	int rc;
	char buffer[BUFFER_SIZE] = {0};
	int count = 0;
	fd_set fds;
	int fd = (int)args;

	FD_ZERO(&fds);
	FD_SET(fd, &fds);

	while(1){
		rc = select(fd+1, &fds, NULL, NULL, NULL);
		if (rc == -1){
			break;
		}

		if (!rc){
			continue;
		}

		if (FD_ISSET(fd, &fds)){
			while(1){
				count = read(fd, buffer, BUFFER_SIZE);
				if (!count){
					break;
				}
				printf("%.*s", count, buffer);
			}
		}
	}

	return NULL;
}

int main(void){
	int rc = 0;
	char slaveName[PATH_MAX] = {0};
	char input[BUFFER_SIZE] = {0};
	int count = 0;
	int masterFD;
	pthread_t pid;
	int slaveFD = forkpty(&masterFD, (char*)&slaveName, NULL, NULL);
	if (slaveFD<0){
		rc = errno;
		goto out;
	}

	if (!slaveFD){
		execlp(SH_CMD, SH_CMD, NULL);
		_exit(0);
	}

	pthread_create(&pid, NULL, showOutputInfo, (void*)masterFD);

	while(1){
		count = 0;
		while(1){
			input[count++] = getchar();
			if (input[count-1] == '\n'){
				break;
			}
		}

		if (!memcmp(input, "exit", 4)){
			break;
		}

		write(masterFD, input, count);
		fsync(masterFD);
	}

out:
	if (rc){
		printf("err:%s\n", strerror(errno));
	}

	close(masterFD);
}
