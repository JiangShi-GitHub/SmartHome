#ifndef __UART_H__
#define __UART_H__

/*
    @gas：烟雾浓度
    @light：光照强度
    @temp：温度
    @atmo：气压
    @hum：湿度
    @hight：海拔
*/
extern double gas, light, temp, atmo, hum, hight;

/*
    MQ01_getvalue：获取MQ01传感器的数值
*/
extern int MQ01_getvalue(void);

/*
    GY39_getvalue：获取GY39传感器的数值
*/
extern int GY39_getvalue(void);

#endif