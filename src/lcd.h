#ifndef __LCD_H__
#define __LCD_H__

/*
    lcd_open:打开lcd
    返回值：
        无。
*/
extern void lcd_open(void);

/*
    lcd_close:关闭lcd
    返回值：
        无。
*/
extern void lcd_close(void);

/*
    lcd_draw_point:在屏幕坐标(x, y)这个像素点，画一个颜色color
    @x：点的x坐标
    @y：点的y坐标
    @color：画的颜色
    返回值：
        无。
*/
extern void lcd_draw_point(int x, int y, int color);

/*
    lcd_clear_screen：将屏幕颜色清空为color
    @color：画的颜色
    返回值：
        无。
*/
extern void lcd_clear_screen(int color);

extern void lcd_clear_area(int x0, int y0, int x1, int y1, int color);

#endif