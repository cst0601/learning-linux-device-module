/*
 * User program calls kernel and put data into kernel.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "hash_table_example.h"

int main(int argc, char * argv[]) {

    int dev = open("/dev/hash_table_example", O_RDONLY);
    if (dev == -1) {
        printf("opening was not possible.\n");
        return -1;
    }

    int data = 10;

    ioctl(dev, INSERT, &data);
    ioctl(dev, LIST, &data);
    ioctl(dev, REMOVE, &data);

    close(dev);

    return 0;
}