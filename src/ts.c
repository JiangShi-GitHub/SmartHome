#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include "ts.h"

/*
    get_ts_point: 获取用户的点击事件的坐标信息
    @p : 指向一个“点击事件 point”结构体，用来
         保存用户点击事件的信息的
    返回值：
        无。
*/
void get_ts_point(Point *p)
{
    int fd;
    fd = open("/dev/input/event0", O_RDONLY);
    if(fd == -1)
    {
        printf("Sorry, failed to open /dev/input/event0\n");
        return;
    }

    struct input_event ev;
    while(1)
    {
        int ret = read(fd, &ev, sizeof(ev));
        if(ret != sizeof(ev))
        {
            continue;
        }

        if(ev.type == EV_ABS && ev.code == ABS_X)
        {
            p -> x = ev.value;

            p -> x = p -> x * 800 / 1024.0;
        }

        if(ev.type == EV_ABS && ev.code == ABS_Y)
        {
            p -> y = ev.value;

            p -> y = p -> y * 480 / 600.0;
        }

        if((ev.type == EV_KEY && ev.code == BTN_TOUCH && ev.value == 0) ||
           (ev.type == EV_ABS && ev.code == ABS_PRESSURE && ev.value == 0))
        {
            close(fd);
            return;
        }
    }
    
}