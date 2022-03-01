obj-m += linked_list_example.o


all: 
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	clang user.c -o user

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -f user

