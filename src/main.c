#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <pthread.h>
#include "lcd.h"
#include "bmp.h"
#include "hanzi.h"
#include "zimo.h"
#include "ts.h"
#include "UART.h"
#include "main.h"
#include "page.h"

void* GY39_loop(void* data)
{
	GY39_getvalue();
}

void* MQ01_loop(void* data)
{
	MQ01_getvalue();
}

int main() 
{
	

    lcd_open();

	lcd_clear_screen(0xffffff);
	
	pthread_t tid;
	int ret = pthread_create(&tid, NULL, GY39_loop, NULL);
	if(ret == -1)
	{
		printf("Sorry, faild to ret\n");
		return -1;
	}

	pthread_t tid2;
	int ret2 = pthread_create(&tid2, NULL, MQ01_loop, NULL);
	if(ret2 == -1)
	{
		printf("Sorry, faild to ret2\n");
		return -1;
	}
	
	Page_main();
	
	while(1)
	{
		Point p;
		get_ts_point(&p);
		Page_touch(&p);
	}
	

	lcd_close();
	
    return 0;
}
