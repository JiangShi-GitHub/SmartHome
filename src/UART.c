#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>		//延时函数头文件
#include <fcntl.h>
#include <termios.h>	//串口初始化头文件,单独把这个串口初始化放一个模块中
#include "UART.h"
#include "hanzi.h"
#include "lcd.h"
#include "page.h"
#include "bmp.h"

double gas = 0, light = 0, temp = 0, atmo = 0, hum = 0, hight = 0;
int gas_alarm = 0;

/*
    init_serial：初始化串口
    @file：文件名称
    @baudrate：波特率
*/
int init_serial(const char *file, int baudrate)
{
	int fd;
    fd = open(file, O_RDWR);
    if (fd == -1)
    {
    perror("open device error:");
    return -1;
    }
    struct termios myserial;
    //清 空 结 构 体
    memset(&myserial, 0, sizeof (myserial));
    //O_RDWR
    myserial.c_cflag |= (CLOCAL | CREAD);
    //设 置 控 制 模 式 状 态 ， 本 地 连 接 ， 接 受 使 能
    //设 置 数 据 位
    myserial.c_cflag &= ~CSIZE; //清 空 数 据 位
    myserial.c_cflag &= ~CRTSCTS; //无 硬 件 流 控 制
    myserial.c_cflag |= CS8; //数 据 位 :8
    myserial.c_cflag &= ~CSTOPB;// //1位 停 止 位
    myserial.c_cflag &= ~PARENB; //不 要 校 验
    //myserial.c_iflag |= IGNPAR; //不 要 校 验
    //myserial.c_oflag = 0; //输 入 模 式
    //myserial.c_lflag = 0; //不 激 活 终 端 模 式
    switch (baudrate)
    {
    case 9600:
    cfsetospeed(&myserial, B9600); //设 置 波 特 率
    cfsetispeed(&myserial, B9600);
    break;
    case 115200:
    cfsetospeed(&myserial, B115200); //设 置 波 特 率
    cfsetispeed(&myserial, B115200);
    break;
    case 19200:
    cfsetospeed(&myserial, B19200); //设 置 波 特 率
    cfsetispeed(&myserial, B19200);
    break;
    }
    /* 刷 新 输 出 队 列 ,清 楚 正 接 受 的 数 据 */
    tcflush(fd, TCIFLUSH);
    /* 改 变 配 置 */
    tcsetattr(fd, TCSANOW, &myserial);
    return fd;
}

/*
    MQ01_alarm：报警
*/
void MQ01_alarm()
{
    int on = 1, close = 0, fd = 0;
    if(gas >= 100 && !gas_alarm)
	{
		stop_thread();
		//thread_exit();
		gas_alarm = 1;
        bmp_display("warning.bmp", 0, 0);
        fd = open("/sys/kernel/gec_ctrl/beep", O_RDWR);
        write(fd, &on, 4);
	}
    else if(gas_alarm && gas < 100)
    {
        gas_alarm = 0;
        fd = open("/sys/kernel/gec_ctrl/beep", O_RDWR);
        write(fd, &close, 4);

        if(now_Page.sign == ps[0].sign)
        {
            now_Page = ps[7];
            Page_main();
        }
        if(now_Page.sign == ps[1].sign)
        {
            now_Page = ps[7];
            Page_wenshi();
        }
        if(now_Page.sign == ps[2].sign)
        {
            now_Page = ps[7];
            Page_tv();
        }
        if(now_Page.sign == ps[3].sign)
        {
            now_Page = ps[7];
            Page_atmo();
        }
        if(now_Page.sign == ps[4].sign)
        {
            now_Page = ps[7];
            Page_light();
        }
        if(now_Page.sign == ps[5].sign)
        {
            now_Page = ps[7];
            Page_music();
        }
        if(now_Page.sign == ps[6].sign)
        {
            now_Page = ps[7];
            Page_alarm();
        }
    }
}

/*
    MQ01_getvalue：获取MQ01传感器的数值
*/
int MQ01_getvalue(void)
{
	/* 用一个空间来保存我们的传感器控制指令 */
	unsigned char cmd[9] = {0xff,0x01,0x86,0x00,0x00,0x00,0x00,0x00,0x79};
	unsigned char buf[15] = {0};		//存数据的空间，大小自己看着来
	int mq01fd = 0;
	
	/* 打开串口文件，串口自己选 */
	mq01fd = init_serial("/dev/ttySAC2",9600);
	
	while(1)
	{
		/* 循环里面把数据读出来保存 */
			usleep(500000);
			int w_back = write(mq01fd,cmd,9);		//MQ-02，你发一次命令，它回一次数据
            if(w_back == -1)
            {
                printf("Write yanwu cmd Error!!");
            }


            int r_back = read(mq01fd, buf, 9);      //获取数值写到buf中
            if(r_back == -1)
            {
                printf("Read yanwu cmd Error!!");
            }
            
            gas = (buf[2] << 8) | buf[3];           //计算gas
            MQ01_alarm();

            //printf("gas= %lf\n", gas);
	}
	close(mq01fd);
}

/*
    GY39_getvalue：获取GY39传感器的数值
*/
int GY39_getvalue(void)
{
    unsigned char cmd_light[] = {0xA5, 0x83, 0x28};     //指令
    unsigned char buf[25] = {0};
    int GY39_fd = init_serial("/dev/ttySAC1", 9600);    //初始化串口
    
    while(1)
    {   
        usleep(500000);
        write(GY39_fd, cmd_light, 3);   //发送命令
        read(GY39_fd, buf, 24);         //获取结果，写到buf中
        for(int i = 0; i < 24; i++)
        {
            if(buf[i] == 0x5a && buf[i + 1] == 0x5a)
            {
                i += 2;
                if(buf[i] == 0x15)  //buf[i]（帧第三位）对应值为0x15的话帧为光照数据
                {
                    i += 2;
                    light = (buf[i] << 24) | (buf[i + 1] << 16) | (buf[i + 2] << 8) | buf[i + 3];   //计算光照强度
                    light = light / 100;
                    i += 4;
                    // printf("light= %lf\n", light);
                }
                else if(buf[i] == 0x45) //buf[i]（帧第三位）对应值为0x45的话帧为温湿度、气压、海拔数据
                {
                    i += 2;
                    temp = (buf[i] << 8) | buf[i + 1];  
                    temp = temp / 100;
                    
                    i += 2;
                    atmo = (buf[i] << 24) | (buf[i + 1] << 16) | (buf[i + 2] << 8) | buf[i + 3];
                    atmo = atmo / 100;

                    i += 4;
                    hum = (buf[i] << 8) | buf[i + 1];
                    hum = hum / 100;
                    
                    i += 2;
                    hight = (buf[i] << 8) | buf[i + 1];

                    //printf("temp= %lf\natmo= %lf\nhum= %lf\nheight= %lf\n", temp, atmo, hum, hight);
                }
            }
        }
    }
    close(GY39_fd);
}