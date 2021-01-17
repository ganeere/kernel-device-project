#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include<sys/ioctl.h>
 
#define CHANGE_KEY _IOW('k',13,char*)
#define CLEAR_CONTENT _IOW('k', 14,char*)
#define GET_KEY _IOR('k',15,char*)
 
int main()
{
        int fd;
        char value_to_write[20] = "jhjhjh";
        printf("*********************************\n");
 
        printf("\n Opening Driver \n");
        fd = open("/dev/vault", O_RDWR);
        if(fd < 0) {
                printf("Cannot open device file...\n");
                return 0;
        }
		

		/*
			CLEAR_CONTENT
		*/
        // printf("Enter the Value to send\n");
        // scanf("%s",value_to_write);
        printf("Clearing Content of the  Driver\n");
        ioctl(fd, CLEAR_CONTENT, (char*) value_to_write); 

 
        printf("Closing Driver\n");
        close(fd);
}























