#include"stdio.h"
#include"stdlib.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/input.h>
#include <string.h>
	   
#define DEVICE_KEYBOARD "/dev/input/event1"
int main(void)
{
	//file *fops ;
	int count;
   // fops = open(DEVICE_KEYBOARD,r);	
    int fp = -1;
	struct input_event *key_event;
	fp = open(DEVICE_KEYBOARD,O_RDONLY);
	if(fp < 0){
		printf("open file failed\n");
		exit(-1);
	}
	
	key_event = (struct input_event *)malloc(sizeof(struct input_event));
	while (1) {
		memset(key_event,0,sizeof(struct input_event));
	
		count = read(fp,key_event,sizeof(struct input_event));
		if(count != sizeof(struct input_event)) {
			close(fp);
			perror("get data wrong\n");
			return -1;
		} else {
			printf("count = %d\n",count);
		}
		
		printf("%s.\n",key_event);
	}
	close(fp);
	return 0;
}