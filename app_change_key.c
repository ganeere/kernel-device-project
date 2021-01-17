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
        char value_to_read[20];
        // char* value_to_write;
        char value_to_write[20] = "jhjhjh";
        printf("\n*********************************\n");
 
        printf("\nOpening Driver\n");
        fd = open("/dev/vault", O_RDWR);
        if(fd < 0) {
                printf("Cannot open device file...\n");
                return 0;
        }
		
		/*
			CHANGE_KEY
		*/
        printf("Enter the Value to send\n");
        scanf("%s",value_to_write);
        printf("Writing Value to Driver\n");
        ioctl(fd, CHANGE_KEY, (char*) value_to_write); 


		/*
			GET_KEY
		*/ 
        printf("Reading Value from Driver\n");
        ioctl(fd, GET_KEY, (char*) value_to_read);
        printf("Value is %s\n", value_to_read);
 
        printf("Closing Driver\n");
        close(fd);
}























