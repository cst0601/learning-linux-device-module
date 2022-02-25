#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "task_example.h"

int main() {

    int dev = open("/dev/task_example", O_RDONLY);
    if (dev == -1) {
        printf("Opening was not possible!\n");
        return -1;
    }

    printf("User - PID = %d\n", getpid());

    ioctl(dev, INFO, NULL);
    
    close(dev);

    return 0;
}