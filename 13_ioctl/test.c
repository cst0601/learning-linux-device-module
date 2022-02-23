#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "ioctl_example.h"

int main() {
    int answer;
    struct mystruct test = {35, "chikuma"};

    int dev = open("/dev/ioctl_test", O_RDONLY);
    if (dev == -1) {
        printf("Opening was not possible!\n");
        return -1;
    }

    ioctl(dev, RD_VALUE, &answer);
    printf("The answer is %d\n", answer);

    answer = 123;

    ioctl(dev, WR_VALUE, &answer);
    ioctl(dev, RD_VALUE, &answer);
    printf("The answer is %d\n", answer);

    ioctl(dev, GREETER, &test);

    printf("Opening was successful!\n");

    int n = 10;
    int* list = malloc(sizeof(int) * n);


    for (size_t i = 0; i < n; i++) list[i] = i;

    struct TestData data = {list, n};
    ioctl(dev, READ, &data);
    
    close(dev);

    free(list);

    return 0;
}