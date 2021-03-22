/*
 * @Author: zgw
 * @email: liang.zhang@tuya.com
 * @LastEditors: zgw
 * @file name: sht21.h
 * @Description: SHT21 IIC drive src file
 * @Copyright: HANGZHOU TUYA INFORMATION TECHNOLOGY CO.,LTD
 * @Company: http://www.tuya.com
 * @Date: 2020-12-16 18:51:29
 * @LastEditTime: 2021-03-13 12:22:49
 */

#include "app_control.h"
#include "tuya_gpio.h"
#include "tuya_uart.h"
#include "BkDriverUart.h"
#include "sys_timer.h"
#include "uni_time.h"
#include "app_pwm.h"
/***********************************************************
*************************types define***********************
***********************************************************/
typedef enum
{
    LOW = 0,
    HIGH,
}default_level;


USER_PWM_DUTY_T user_pwm_duty = {0};
APP_CTRL_DATA_T app_ctrl_data = {0};

/***********************************************************
*************************IO control device define***********
***********************************************************/


/***********************************************************
*************************about adc init*********************
***********************************************************/


/***********************************************************
*************************about iic init*********************
***********************************************************/


/***********************************************************
*************************function***************************
***********************************************************/

STATIC VOID __ctrl_gpio_init(CONST TY_GPIO_PORT_E port, CONST BOOL_T high)
{   
    //Set IO port as output mode
    tuya_gpio_inout_set(port, FALSE);
    //Set IO port level
    tuya_gpio_write(port, high);
}


VOID app_device_init(VOID)
{
    INT_T op_ret = 0;

    app_pwm_init();
}

STATIC VOID __set_pwm_duty(VOID)
{   
    user_pwm_duty.duty_red = (USHORT_T)(((float)app_ctrl_data.Red_value/255.0)*1000);
    user_pwm_duty.duty_green = (USHORT_T)(((float)app_ctrl_data.Green_value/255.0)*1000);
    user_pwm_duty.duty_blue = (USHORT_T)(((float)app_ctrl_data.Blue_value/255.0)*1000);
    
    if(user_pwm_duty.duty_red != user_pwm_duty.duty_red_old) {
        app_pwm_set(red,user_pwm_duty.duty_red);
        user_pwm_duty.duty_red_old = user_pwm_duty.duty_red;
    }else if(user_pwm_duty.duty_green != user_pwm_duty.duty_green_old) {
        app_pwm_set(green,user_pwm_duty.duty_green_old);
        user_pwm_duty.duty_green_old = user_pwm_duty.duty_green;
    }else if(user_pwm_duty.duty_blue != user_pwm_duty.duty_blue_old) {
        app_pwm_set(blue,user_pwm_duty.duty_blue_old);
        user_pwm_duty.duty_blue_old = user_pwm_duty.duty_blue;
    }
}

VOID app_ctrl_handle(VOID)
{
    __set_pwm_duty();
}

VOID app_ctrl_all_off(VOID)
{   
    app_pwm_off();
}

 