/*
 * User program calls kernel and put data into kernel.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "linked_list_example.h"

int main(int argc, char * argv[]) {

    int dev = open("/dev/linked_list_example", O_RDONLY);
    if (dev == -1) {
        printf("opening was not possible.\n");
        return -1;
    }

    int data = 10;
    int del_target = 20;

    
    for (int i = 0; i < 10; i++) {
        ioctl(dev, INSERT, &data);
        ioctl(dev, INSERT, &data);
        data += 10;
    }
    ioctl(dev, LIST, &data);

    ioctl(dev, REMOVE, &del_target);

    ioctl(dev, LIST, &data);

    close(dev);

    return 0;
}