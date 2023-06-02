#ifndef __PAGE_H__
#define __PAGE_H__
#include "main.h"
#include "ts.h"

extern Pages ps[]; //声明结构体变量{@sign:   bmp图片的代号, @__FILE: bmp图片的文件名}

extern Pages now_Page; //当前页面

/*
    Page_main：主页面的显示函数
*/
extern void Page_main();

/*
    Page_wenshi：温湿度显示界面
*/
extern void Page_wenshi();

/*
    Page_tv：   TV界面显示
*/
extern void Page_tv();

/*
    Page_atmo： 气压显示界面
*/
extern void Page_atmo();

/*
    Page_light：光照显示
*/
extern void Page_light();

/*
    Page_music：音乐播放界面
*/
extern void Page_music();

/*
    Page_alarm：烟雾界面
*/
extern void Page_alarm();

/*
    Page_touch：触摸检测
*/
extern void Page_touch(Point *p);

/*
    stop_thread：停止线程
*/
extern void stop_thread();

/*
    thread_exit：回收线程
*/
extern void thread_exit();

#endif