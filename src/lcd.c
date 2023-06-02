/*
分辨率：800*480
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include "lcd.h"

static int lcd_fd = -1;
static int* plcd = NULL;

/*
    lcd_open：打开lcd
    返回值：
        无。
*/
void lcd_open()
{
    lcd_fd = open("/dev/fb0", O_RDWR);
    if(lcd_fd == -1)
    {
        printf("fd failed!!");
        return;
    }

    plcd = (int *)mmap(NULL, 800*480*4, PROT_READ|PROT_WRITE, MAP_SHARED, lcd_fd, 0);
    if(plcd == MAP_FAILED)
    {
        printf("map Failed!!");
        close(lcd_fd);
        return;
    }

}

/*
    lcd_close：关闭lcd
    返回值：
        无。
*/
void lcd_close()
{
    munmap(plcd, 800*480*4);
    close(lcd_fd);
}

/*
    lcd_draw_point：在坐标(x, y)处画一个color颜色
    @x：lcd的x坐标
    @y：lcd的y坐标
    @color：颜色
    返回值：
        无。
*/
void lcd_draw_point(int x, int y, int color)
{
    if(plcd != NULL && x > 0 && x < 800 && y > 0 && y < 480)
    {
        *(plcd + y * 800 + x) = color;
    }
}

/*
    lcd_clear_screen：将屏幕设置为color颜色
    @color：颜色
    返回值：
        无。
*/
void lcd_clear_screen(int color)
{
    int x, y;
    for(y = 0; y < 480; y++)
    {
        for(x = 0; x < 800; x++)
        {
            lcd_draw_point(x, y, color);
        }
    }
}

void lcd_clear_area(int x0, int y0, int x1, int y1, int color)
{
    if(x0 < x1)
    {
        switch(x0, x1);
        switch(y0, y1);
    }
    for(int j = y0; j < y1; j++)
    {
        for(int i = x0; i < x1; i++)
        {
            lcd_draw_point(i, j, color);
        }
    }
}