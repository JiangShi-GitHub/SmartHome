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
#include <dirent.h>
#include "lcd.h"
#include "bmp.h"
#include "hanzi.h"
#include "zimo.h"
#include "ts.h"
#include "UART.h"

/*
    声明结构体变量{@sign:   bmp图片的代号, @__FILE: bmp图片的文件名}
    @now_Pages: 当前页面
*/
Pages ps[] = {{0x01, "main.bmp"}, {0x02, "shufu.bmp"}, {0x04, "hutao_beijing.bmp"}, {0x08, "qiya.bmp"}, 
              {0x10, "duoyun.bmp"}, {0x20, "music.bmp"}, {0x40, "yanwu.bmp"}, {0xFF, "warning.bmp"}};
Pages now_Page = {0x01, "main.bmp"};

/*
    定义一个数组
    数组成员为一串连续的图片
*/
const char* Hutao[] = {"hutao00.bmp", "hutao01.bmp", "hutao02.bmp", "hutao03.bmp", "hutao04.bmp", "hutao05.bmp", 
                       "hutao06.bmp", "hutao07.bmp", "hutao08.bmp", "hutao09.bmp", "hutao10.bmp", "hutao11.bmp", 
                       "hutao12.bmp", "hutao13.bmp", "hutao14.bmp", "hutao15.bmp", "hutao16.bmp", "hutao17.bmp", 
                       "hutao18.bmp", "hutao19.bmp", "hutao20.bmp", "hutao21.bmp", "hutao22.bmp", "hutao23.bmp", 
                       "hutao24.bmp", "hutao25.bmp", "hutao26.bmp", "hutao27.bmp", "hutao28.bmp", "hutao29.bmp", 
                       "hutao30.bmp", "hutao31.bmp", "hutao32.bmp", "hutao33.bmp", "hutao34.bmp", "hutao35.bmp", 
                       "hutao36.bmp", "hutao37.bmp", "hutao38.bmp", "hutao39.bmp", "hutao40.bmp", "hutao41.bmp", 
                       "hutao42.bmp"};

/*
    线程id
*/
pthread_t music, wenshi, tv, deng, qiya, guangzhao, yanwu;

/*
    标志位
*/
int start = 0, stop = 0, voice_up = 0, voice_down = 0, sw_left = 0, sw_right = 0, conti = 0;
int music_exit = 0, wenshi_exit = 0, tv_exit = 0, deng_exit = 0, qiya_exit = 0, guangzhao_exit = 0, yanwu_exit = 0;
int wen_low = 0, wen_high = 0, shi_low = 0, shi_high = 0;

/*
    stop_thread：停止线程
*/
void stop_thread()
{
    if(now_Page.sign == ps[1].sign) wenshi_exit = 1;
    if(now_Page.sign == ps[2].sign)
    {
        tv_exit = 1;
        deng_exit = 1;
    }
    if(now_Page.sign == ps[3].sign) qiya_exit = 1;
    if(now_Page.sign == ps[4].sign) guangzhao_exit = 1;
    if(now_Page.sign == ps[5].sign) music_exit = 1;
    if(now_Page.sign == ps[6].sign) yanwu_exit = 1;
}

/*
    thread_exit：回收线程
*/
void thread_exit()
{
    if(now_Page.sign == ps[1].sign) pthread_join(wenshi, NULL);
    if(now_Page.sign == ps[2].sign) 
    {
        pthread_join(tv, NULL);
        pthread_join(deng, NULL);
    }
    if(now_Page.sign == ps[3].sign) pthread_join(qiya, NULL);
    if(now_Page.sign == ps[4].sign) pthread_join(guangzhao, NULL);
    if(now_Page.sign == ps[5].sign) pthread_join(music, NULL);
    if(now_Page.sign == ps[6].sign) pthread_join(yanwu, NULL);
}

/*
    Page_main：主页面的显示函数
*/
void Page_main()
{
    thread_exit();                          //回收线程

    bmp_change_pages(now_Page, ps[0]);      //改变当前界面为now_Page
    bmp_display(ps[0].__FILE, 0, 0);        //bmp显示
    change_color(0x000000, 0xFFF1C0);       //改变打印颜色的函数
    tital_display(245, 32);                 //显示标题的函数
    color_init();                           //初始化打印颜色的函数
    now_Page = ps[0];                       //设置当前界面为main界面
}


/*
    detect_wenshi：温湿度线程
*/
void* detect_wenshi(void* data)
{
    while(1)
    {
        if(wenshi_exit)  //判断标志位是否为 1，如果为 1，则退出线程
        {
            wenshi_exit = 0;
            printf("Wenshi exit!!\n");
            pthread_exit(NULL);
        }

        change_color(RGB_color(0, 50, 25, temp), 0xFFF1C0);
        digital_display(326, 115, temp); //显示温度

        change_color(RGB_color(0, 100, 50, hum), 0xFFF1C0);
        digital_display(326, 338, hum); //显示湿度

        if(hum >= 45 && hum <= 65) shi_low = 0, shi_high = 0; //根据不同的温湿度显示不同的表情
        else if(hum < 45) shi_low = 1, shi_high = 0;
        else if(hum > 65) shi_low = 0, shi_high = 1;

        if(temp >= 22 && temp <= 30) wen_low = 0, wen_high = 0;
        else if(temp < 22) wen_low = 1, wen_high = 0;
        else if(temp > 30) wen_low = 0, wen_high = 1;

        if(wen_low) //根据标志位切换图片
        {
            if(shi_low) bmp_display("leng_gan.bmp", 0, 0);
            else if(shi_high) bmp_display("leng_shi.bmp", 0, 0);
            else bmp_display("leng_shu.bmp", 0, 0);
        }
        else if(wen_high)
        {
            if(shi_low) bmp_display("re_gan.bmp", 0, 0);
            else if(shi_high) bmp_display("re_shi.bmp", 0, 0);
            else bmp_display("re_shu.bmp", 0, 0);
        }
        else
        {
            if(shi_low) bmp_display("shu_gan.bmp", 0, 0);
            else if(shi_high) bmp_display("shu_shi.bmp", 0, 0);
            else bmp_display("shufu.bmp", 0, 0);
        }
    }
}

/*
    Page_wenshi：温湿度显示界面
*/
void Page_wenshi()
{
    /*
        hum = 45% ~ 65%
        temp = 22 ~ 26%
    */
    bmp_change_pages(now_Page, ps[1]);
    now_Page = ps[1];

    int ret = pthread_create(&wenshi, NULL, detect_wenshi, NULL);
    if(ret == -1)
	{
		printf("Sorry, faild to Create thread Wenshi\n");
	}
}

/*
    waterful_light：流水灯线程
*/
void* waterful_light(void* data)
{
    /*
        insmod kobject_led.ko
        led_d7~10       beep
        /sys/kernel/gec_ctrl/
    */
    while(1)
    {
        int on = 1, close = 0, time = 100000;
        if(deng_exit)
        {
            int fd = open("/sys/kernel/gec_ctrl/led_all", O_RDWR);
            write(fd, &close, 4);

            deng_exit = 0;
            printf("Deng exit!!\n");
            pthread_exit(NULL);
        }

        int fd = open("/sys/kernel/gec_ctrl/led_d7", O_RDWR);
        write(fd, &on, 4);
        usleep(time);
        write(fd, &close, 4);

        fd = open("/sys/kernel/gec_ctrl/led_d8", O_RDWR);
        write(fd, &on, 4);
        usleep(time);
        write(fd, &close, 4);

        fd = open("/sys/kernel/gec_ctrl/led_d9", O_RDWR);
        write(fd, &on, 4);
        usleep(time);
        write(fd, &close, 4);

        fd = open("/sys/kernel/gec_ctrl/led_d10", O_RDWR);
        write(fd, &on, 4);
        usleep(time);
        write(fd, &close, 4);
    }
}

/*
    hutao_rock：胡桃摇线程
*/
void* hutao_rock(void* data)
{
    while(1)
    {
        if(tv_exit)
        {
            tv_exit = 0;
            printf("Hutao exit!!\n");
            pthread_exit(NULL);
        }

        for(int i = 0; i <= 42; i++)
        {
            bmp_display(Hutao[i], 320, 160);
            usleep(41667);
        }
    }
}

/*
    Page_tv：   TV界面显示
*/
void Page_tv()
{
    bmp_change_pages(now_Page, ps[2]);
    now_Page = ps[2];

    int ret = pthread_create(&tv, NULL, hutao_rock, NULL);
    if(ret == -1)
    {
        printf("Sorry, faild to Create thread tv\n");
    }

    int ret2 = pthread_create(&deng, NULL, waterful_light, NULL);
    if(ret2 == -1)
    {
        printf("Sorry, faild to Create thread Deng\n");
    }
}


/*
    detect_qiya：气压线程
*/
void* detect_qiya(void* data)
{
    while(1)
    {
        if(qiya_exit) //退出线程
        {
                qiya_exit = 0;
                printf("Qiya exit!!\n");
                pthread_exit(NULL);
        }

        color_init();
        digital_display(448, 305, atmo); //显示气压
        digital_display(448, 400, hight); //显示海拔
    }
}

/*
    Page_atmo： 气压显示界面
*/
void Page_atmo()
{
    bmp_change_pages(now_Page, ps[3]);
    now_Page = ps[3];

    int ret = pthread_create(&qiya, NULL, detect_qiya, NULL);
    if(ret == -1)
    {
        printf("Sorry, faild to Create thread qiya\n");
    }
}


/*
    detect_light：  光照线程
*/
void* detect_light(void* data)
{
    /*
        多云:300~500
    */
    while(1)
    {
        if(guangzhao_exit)
        {
                guangzhao_exit = 0;
                printf("Light exit!!\n");
                pthread_exit(NULL);
        }

        change_color(RGB_color(0, 1000, 400, light), 0xFFF1C0);
        digital_display(144, 209, light); //显示光照

        if(light < 300) bmp_display("yintian.bmp", 0, 0);
        else if(light > 500) bmp_display("qingtian.bmp", 0, 0);
        else bmp_display("duoyun.bmp", 0, 0);
    }
}

/*
    Page_light：光照显示
*/
void Page_light()
{
    bmp_change_pages(now_Page, ps[4]);
    now_Page = ps[4];

    int ret = pthread_create(&guangzhao, NULL, detect_light, NULL);
    if(ret == -1)
    {
        printf("Sorry, faild to Create thread light\n");
    }
}

//设置节点
struct node
{
    char *data;         //数据域         
    struct node *next;  //后继指针
    struct node *prev;  //前驱指针
};

//初始化链表
struct node *list_init(char *newdata)
{
    struct node *head = malloc(sizeof(struct node));    //分配头节点的内存
    head->data = newdata;
    head->next = head;   //初始把头结点的prev和next指针都指向自己
    head->prev = head;
    return head;
}

//创建新节点
struct node *newnode(char *newdata)
{
    struct node *new = malloc(sizeof(struct node));
    new->data = newdata;
    new->next = NULL;   //将指针域置空
    new->prev = NULL;
}

/*
    addnode：加入新节点
    @new:   新结点
    @list:  当前结点
*/
int addnode(struct node *new,struct node *list)
{
    struct node *p = list;
    while(p->next != list)
    {
        p = p->next;
    }
    new->prev = list->prev; //新结点的前结点为当前结点的前结点
    new->next = list;       //新结点的后结点指向当前结点
    list->prev = new;       //当前结点的前结点为新结点
    new->prev->next = new;  //新结点的前结点的后结点为新结点
}

int voice = 0;

/*
    音乐线程
*/
void *play_music(void *data)
{
    char* start_song = "wotui.mp3";
	struct node *list = list_init(start_song);						//初始化双向循环链表
	char *path = "./";										//music为音乐目录文件名
	DIR *dp = opendir(path);
	struct dirent *p;
	
	while(p = readdir(dp))										//遍历目录文件
	{
		if(p->d_type == DT_REG)
		{
			if(strstr(p->d_name,".mp3") && !strstr(p->d_name, start_song)) 						//判断是否为.mp3文件
			{
				struct node *new = newnode(p->d_name); 			//创建新节点
				addnode(new,list);								//插入新节点
			}				
		}
	}
	
	char buf[50];												//用于存放拼接后的字符串
	struct node *head = list->next;


    while(1)
    {
        if(music_exit)
        {
            printf("Music exit!!\n");
            system("killall -9 madplay");
            music_exit = 0;
            pthread_exit(NULL);
        }
        if(start && conti)
        {
            system("killall -CONT madplay &");
            conti = 0;
        }
        else if(start)
        {
            bzero(buf, 50);
            sprintf(buf,"madplay -a %d %s/%s -r &",voice, path, head->data);
            printf("%s\n", buf);
            system(buf);
            //system("madplay -a +10 ./*.mp3 -r &"); //列表循环
            start = 0;
        }
        else if(stop)
        {
            system("killall -STOP madplay &");
            stop = 0;
            conti = 1;
        }
        else if(sw_left)
        {
            printf("GET Left!!\n\n");
            system("killall -9 madplay");
			usleep(500000);
            system("killall -9 madplay");
			head = head->prev;
			bzero(buf,50);
			sprintf(buf,"madplay -a %d %s/%s -r &",voice, path, head->data);
            printf("%s\n", buf);
			system(buf);
            sw_left = 0;
        }
        else if(sw_right)
        {
            printf("GET Right!!\n\n");
            system("killall -9 madplay");
			usleep(500000);
            system("killall -9 madplay");
			head = head->next;
			bzero(buf,50);
			sprintf(buf,"madplay -a %d %s/%s -r &",voice, path, head->data);
            printf("%s\n", buf); 
			system(buf);
            sw_right = 0;
        }
        else if(voice_up)
        {
            system("killall -9 madplay");
            usleep(500000);
            system("killall -9 madplay");
            if(voice <= 16) voice += 2;
            bzero(buf,50);
			sprintf(buf,"madplay -a %d %s/%s -r &",voice, path, head->data);
            printf("%s\n", buf);
			system(buf);
            voice_up = 0;
        }
        else if(voice_down)
        {
            system("killall -9 madplay");
            usleep(500000);
            system("killall -9 madplay");
            if(voice >= -175) voice -= 2;
            bzero(buf,50);
			sprintf(buf,"madplay -a %d %s/%s -r &",voice, path, head->data);
            printf("%s\n", buf);
			system(buf);
            voice_down = 0;
        }
    }
}

/*
    Page_music：音乐播放界面
*/
void Page_music()
{
    bmp_change_pages(now_Page, ps[5]);
    now_Page = ps[5];

    int ret = pthread_create(&music, NULL, play_music, NULL);
    if(ret == -1)
	{
		printf("Sorry, faild to Create thread music\n");
	}
}

/*
    烟雾线程
*/
void* detect_yanwu(void* data)
{
    while(1)
    {
        if(yanwu_exit)
        {
            printf("Yanwu exit!!\n");
            yanwu_exit = 0;
            pthread_exit(NULL);
        }
        change_color(RGB_color(0, 100, 30, gas), 0xFFF1C0);
	    digital_display(582, 209, gas); //显示烟雾
        //printf("gas= %d\n", gas);
        //usleep(100000);
    }
}

/*
    Page_alarm：烟雾界面
*/
void Page_alarm()
{
    bmp_change_pages(now_Page, ps[6]);
    now_Page = ps[6];

    int ret = pthread_create(&yanwu, NULL, detect_yanwu, NULL);
    if(ret == -1)
    {
        printf("Sorry, faild to Create thread yanwu\n");
    }
}


/*
    Page_touch：触摸检测
*/
void Page_touch(Point *p)
{
    Point tmp = *p;
    if(now_Page.sign == 0x01) //主页面
    {
        if(tmp.x >= 96 && tmp.x <= 192 && tmp.y >= 96 && tmp.y <= 192) Page_wenshi();
        else if (tmp.x >= 96 && tmp.x <= 192 && tmp.y >= 224 && tmp.y <= 320) Page_tv();
        else if(tmp.x >= 96 && tmp.x <= 192 && tmp.y >= 352 && tmp.y <= 448) Page_atmo();
        else if(tmp.x >= 608 && tmp.x <= 704 && tmp.y >= 96 && tmp.y <= 192) Page_light();
        else if(tmp.x >= 608 && tmp.x <= 704 && tmp.y >= 224 && tmp.y <= 320) Page_music();
        else if(tmp.x >= 608 && tmp.x <= 704 && tmp.y >= 352 && tmp.y <= 448) Page_alarm();
    }
    else if(now_Page.sign == 0x02) //温湿度
    {
        if(tmp.x >= 0 && tmp.x <= 64 && tmp.y >= 0 && tmp.y <= 64)
        {
            wenshi_exit = 1;
            Page_main();
        }
    }
    else if(now_Page.sign == 0x04) //tv
    {
        if(tmp.x >= 0 && tmp.x <= 64 && tmp.y >= 0 && tmp.y <= 64)
        {
            tv_exit = 1;
            deng_exit = 1;
            Page_main();
        }
    }
    else if(now_Page.sign == 0x08) //气压
    {
        if(tmp.x >= 0 && tmp.x <= 64 && tmp.y >= 0 && tmp.y <= 64)
        {
            qiya_exit = 1;
            Page_main();
        }
    }
    else if(now_Page.sign == 0x10) //光照
    {
        if(tmp.x >= 0 && tmp.x <= 64 && tmp.y >= 0 && tmp.y <= 64)
        {
            guangzhao_exit = 1;
            Page_main();
        }
    }
    else if(now_Page.sign == 0x20) //音乐
    {
        if(tmp.x >= 384 && tmp.x <= 480 && tmp.y >= 128 && tmp.y <= 224) start = 1;
        else if(tmp.x >= 512 && tmp.x <= 608 && tmp.y >= 128 && tmp.y <= 224) sw_left = 1;
        else if(tmp.x >= 640 && tmp.x <= 734 && tmp.y >= 128 && tmp.y <= 224) sw_right= 1;
        else if(tmp.x >= 384 && tmp.x <= 480 && tmp.y >= 256 && tmp.y <= 352) stop = 1;
        else if(tmp.x >= 512 && tmp.x <= 608 && tmp.y >= 256 && tmp.y <= 352) voice_up = 1;
        else if(tmp.x >= 640 && tmp.x <= 736 && tmp.y >= 256 && tmp.y <= 352) voice_down = 1;
        else if(tmp.x >= 0 && tmp.x <= 64 && tmp.y >= 0 && tmp.y <= 64)
        {
            music_exit = 1;
            Page_main();
        }
    }
    else if(now_Page.sign == 0x40) //烟雾
    {
        if(tmp.x >= 0 && tmp.x <= 64 && tmp.y >= 0 && tmp.y <= 64)
        {
            yanwu_exit = 1;
            Page_main();
        }
    }
}