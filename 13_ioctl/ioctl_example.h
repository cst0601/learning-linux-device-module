#ifndef IOCTL_EXAMPLE_H
#define IOCTL_EXAMPLE_H


struct mystruct {
    int repeat;
    char name[64];
};

struct TestData {
    int* list;
    int n;  // size
};

#define WR_VALUE _IOW('a', 'a',  int32_t *)
#define RD_VALUE _IOW('a', 'b',  int32_t *)
#define GREETER _IOW('a', 'c',  struct mystruct *)
#define READ    _IOW('a', 'd', struct TestData *)


#endif