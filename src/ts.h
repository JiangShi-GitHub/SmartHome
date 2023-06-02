#ifndef __TS_H__
#define __TS_H__

typedef struct
{
    int x;
    int y;
}Point;

/*
    get_ts_point: 获取用户的点击事件的坐标信息
    @p : 指向一个“点击事件 point”结构体，用来
         保存用户点击事件的信息的
    返回值：
        无。
*/
extern void get_ts_point(Point *p);

#endif