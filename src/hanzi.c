#include <stdio.h>
#include <math.h>
#include "lcd.h"
#include "zimo.h"
#include "hanzi.h"

int draw_color = 0x000000;
int back_color = 0xFFF1C0;
int old_wei = 0;

/*
    color_init：初始化打印颜色的函数
    返回值：
        无。
*/
void color_init()
{
    draw_color = 0x000000;
    back_color = 0xFFF1C0;
}

/*
    change_color：改变打印颜色的函数
    @draw：定义画的颜色
    @back：定义背景颜色
    返回值：
        无。
*/
void change_color(int draw, int back)
{
    draw_color = draw;
    back_color = back;
}

/*
    max：求取两个点哪个x更大
    返回值：
        RGB_Point：点坐标
*/
RGB_Point max(RGB_Point a, RGB_Point b)
{
    if(a.x > b.x) return a;
    else return b;
}

/*
    min：求取两个点哪个x更小
    返回值：
        RGB_Point：点坐标
*/
RGB_Point min(RGB_Point a, RGB_Point b)
{
    if(a.x < b.x) return a;
    else return b;
}

/*
    get_line_y：求取R、G、B直线并且返回对应的R、G、B值
    @a：直线点1
    @b：直线点2
    @x：求取的x坐标
    返回值：
        int：求取的R、G、B值。
*/
int get_line_y(RGB_Point a, RGB_Point b, double x)
{
    RGB_Point max_p = max(a,b), min_p = min(a,b); //x范围
    if(x >= max_p.x) return max_p.y;  //如果不在x范围直接返回最大/最小值
    else if(x <= min_p.x) return min_p.y; 

    double k = (a.y - b.y) / (a.x - b.x); //求k
    double h_b = a.y - k * a.x; //求b
    return k * x + h_b; //返回x对应的y值
}

/*
    RGB_color：根据x坐标改变RGB颜色
    @mini：起始点
    @maxi：结束点
    @best：最佳点
    @x：求取的x坐标
    返回值：
        int类型的RGB数字。
*/
int RGB_color(double mini, double maxi, double best, double x)
{
    int B, G, R, color; //定义R、G、B和颜色
    RGB_Point tmp1, tmp2; //两个临时坐标

    tmp1.x = mini, tmp1.y = 255, tmp2.x = best, tmp2.y = 0; //设置B的直线的两个坐标
    B = get_line_y(tmp1, tmp2, x);

    tmp1.x = best, tmp1.y = 0, tmp2.x = maxi, tmp2.y = 255; //设置R的直线的两个坐标
    R = get_line_y(tmp1, tmp2, x);

    if(x <= best) //如果x在最佳值的左边
    {
        tmp1.x = mini, tmp1.y = 0, tmp2.x = best, tmp2.y = 255; //设置G的直线的两个坐标
        G = get_line_y(tmp1, tmp2, x);
    }
    else //如果x在最佳值的右边
    {
        tmp1.x = best, tmp1.y = 255, tmp2.x = maxi, tmp2.y = 0; //设置G的直线的两个坐标
        G = get_line_y(tmp1, tmp2, x);
    }

    color = (R << 16) | (G << 8) | B; //组合RGB
    //printf("%d:%d:%d\n", R, G, B);
    return color; //返回颜色
}

/*
    hanzi_display：在屏幕上显示一个汉字或数字
    @x0：显示区域的左上顶点的x轴坐标
    @y0：显示区域的左上顶点的y轴坐标
    @size：要显示的汉字的宽和高的数组
    @data：要显示的汉字或数字的取模后的数据
    返回值：
        无。
*/
void hanzi_display(int x0, int y0, int size[], char data[])
{
    //总共要显示w*h个像素点
    //按扫描的顺序给像素点编号，编号范围[0, w*h)
    int w = size[0];
    int h = size[1];
    int dian; //保存点的编号， [0, w*h)
    int color;
    for(dian = 0; dian < w * h; dian++)
    {
        //显示第dian个像素点
        int index; //第 dian 这个像素点的取模后的数据在 data 中的第 index 个字节中
        int bit; //第 dian 这个像素点的取模后的数据在 data[index] 这个字节中的 第bit个位置
        int s;
        index = dian / 8;
        bit = 7 - dian % 8;
        if((data[index] >> bit) & 1)
        {
            color = draw_color; //白色
        }
        else
        {
            color = back_color; //黑色
        }

        int x = x0 + dian % w;
        int y = y0 + dian / w;

        lcd_draw_point(x, y, color);
    }
}

/*
    hanzi_display_wh：在屏幕上显示一个汉字或数字
    @x0：显示区域的左上顶点的x轴坐标
    @y0：显示区域的左上顶点的y轴坐标
    @w：要显示的汉字或数字的宽
    @h：要显示的汉字或数字的高
    @data：要显示的汉字或数字的取模后的数据
    返回值：
        无。
*/
void hanzi_display_wh(int x0, int y0, int w, int h, char data[])
{
    //总共要显示w*h个像素点
    //按扫描的顺序给像素点编号，编号范围[0, w*h)
    int dian; //保存点的编号， [0, w*h)
    int color;
    for(dian = 0; dian < w * h; dian++)
    {
        //显示第dian个像素点
        int index; //第 dian 这个像素点的取模后的数据在 data 中的第 index 个字节中
        int bit; //第 dian 这个像素点的取模后的数据在 data[index] 这个字节中的 第bit个位置
        int s;
        index = dian / 8;
        bit = 7 - dian % 8;
        if((data[index] >> bit) & 1)
        {
            color = draw_color; //黑色
        }
        else
        {
            color = back_color; //白色
        }

        int x = x0 + dian % w;
        int y = y0 + dian / w;

        lcd_draw_point(x, y, color);
    }
}

/*
    show_num：打印数字的函数
    @x0：要打印的数字的初始x轴坐标
    @y0：要打印的数字的初始y轴坐标
    @num：要打印的数字
    返回值：
        无。
*/
void show_num(int x0, int y0, int num)
{
	if(num == 1) hanzi_display(x0, y0, size_er, yi_erhao);
	else if(num == 2) hanzi_display(x0, y0, size_er, er_erhao);
	else if(num == 3) hanzi_display(x0, y0, size_er, san_erhao);
	else if(num == 4) hanzi_display(x0, y0, size_er, si_erhao);
	else if(num == 5) hanzi_display(x0, y0, size_er, wu_erhao);
	else if(num == 6) hanzi_display(x0, y0, size_er, liu_erhao);
	else if(num == 7) hanzi_display(x0, y0, size_er, qi_erhao);
	else if(num == 8) hanzi_display(x0, y0, size_er, ba_erhao);
	else if(num == 9) hanzi_display(x0, y0, size_er, jiu_erhao);
	else if(num == 0) hanzi_display(x0, y0, size_er, ling_erhao);
}

/*
    tital_display：显示标题的函数
    @x0：标题的初始x轴坐标
    @y0：标题的初始y轴坐标
    返回值：
        无。
*/
void tital_display(int x0, int y0)
{
    hanzi_display(x0, y0, size_yi, zhi_yi);
    x0 += size_yi[0];

    hanzi_display(x0, y0, size_yi, neng_yi);
    x0 += size_yi[0];

    hanzi_display(x0, y0, size_yi, jia_yi);
    x0 += size_yi[0];

    hanzi_display(x0, y0, size_yi, ju_yi);
    x0 += size_yi[0];

    hanzi_display(x0, y0, size_yi, kong_yi);
    x0 += size_yi[0];

    hanzi_display(x0, y0, size_yi, zhi2_yi);
    x0 += size_yi[0];

    hanzi_display(x0, y0, size_yi, xi_yi);
    x0 += size_yi[0];

    hanzi_display(x0, y0, size_yi, tong_yi);
}

/*
    digital_display：显示数字的函数
    @x0：要显示的数字的初始x轴坐标
    @y0：要显示的数字的初始y轴坐标
    @num：要显示的数字
    返回值：
        无。
*/
void digital_display(int x0, int y0, double num)
{   
    int zh[9], i = 8;
    int zheng = (int)num;
    double xiao = num - zheng;
    int wei = 0;
    //printf("%d\n%lf\n", zheng, xiao);
    while(zheng > 0)
    {
        zh[i--] = zheng % 10;
        zheng = zheng / 10;
        wei++;
    }

    if(wei < old_wei)
    {
        lcd_clear_area(x0 + (wei + 3) * size_er[0], y0, x0 + (old_wei + 3) * size_er[0], y0 + size_er[1], back_color);
    }

    int x = x0;
    for(int j = i + 1; j <= 8; j++)
    {
        show_num(x, y0, zh[j]);
        x += size_er[0];
        //printf("%d\n", zh[j]);
    }


    xiao = xiao * 100;
    int xiao_int = (int)xiao, xs[2];
    
    hanzi_display(x, y0, size_er, point_erhao);
    x += size_er[0];
    
    xs[0] = (xiao_int / 10) % 10;
    xs[1] = xiao_int % 10;

    for(int j = 0; j <= 1; j++){
        show_num(x, y0, xs[j]);
        x += size_er[0];
        //printf("%d\n", xs[j]);
    }
}