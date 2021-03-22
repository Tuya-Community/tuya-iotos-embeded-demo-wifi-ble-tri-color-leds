/*
 * @Author: zgw
 * @email: liang.zhang@tuya.com
 * @LastEditors: zgw
 * @file name: app_plant.c
 * @Description: light init process include file
 * @Copyright: HANGZHOU TUYA INFORMATION TECHNOLOGY CO.,LTD
 * @Company: http://www.tuya.com
 * @Date: 2020-12-21 11:30:03
 * @LastEditTime: 2021-03-13 14:18:40
 */

#include "app_rgb.h"
#include "app_control.h"
#include "uni_time_queue.h"
#include "sys_timer.h"
#include "tuya_iot_wifi_api.h"
#include "FreeRTOS.h"
#include "tuya_hal_thread.h"
#include "uni_thread.h"
#include "queue.h"
#include "semphr.h"

#define TASKDELAY_SEC         1000



extern APP_CTRL_DATA_T app_ctrl_data;

STATIC VOID light_control_thread(PVOID_T pArg);
STATIC VOID sensor_data_report_thread(PVOID_T pArg);







OPERATE_RET app_rgb_init(IN APP_RGB_MODE mode)
{
    OPERATE_RET op_ret = OPRT_OK;

    if(APP_NORMAL == mode) {
        app_device_init();

        tuya_hal_thread_create(NULL, "light_control_thread", 512*4, TRD_PRIO_4, light_control_thread, NULL);

        tuya_hal_thread_create(NULL, "thread_data_report", 512*4, TRD_PRIO_4, sensor_data_report_thread, NULL);
    }else {
        //not factory test mode
    }

    return op_ret;
}

STATIC VOID light_control_thread(PVOID_T pArg)
{   
    while(1) {
        if(app_ctrl_data.Light_switch == TRUE) {
            app_ctrl_handle();
        }else {
            app_ctrl_all_off();
        }

        tuya_hal_system_sleep(TASKDELAY_SEC);
    }
}


STATIC VOID sensor_data_report_thread(PVOID_T pArg)
{   
    while(1) {
        app_report_all_dp_status();
        tuya_hal_system_sleep(TASKDELAY_SEC*10);
    }
}


VOID app_report_all_dp_status(VOID)
{
    OPERATE_RET op_ret = OPRT_OK;

    INT_T dp_cnt = 0;
    dp_cnt = 4;

    TY_OBJ_DP_S *dp_arr = (TY_OBJ_DP_S *)Malloc(dp_cnt*SIZEOF(TY_OBJ_DP_S));
    if(NULL == dp_arr) {
        PR_ERR("malloc failed");
        return;
    }

    memset(dp_arr, 0, dp_cnt*SIZEOF(TY_OBJ_DP_S));

    dp_arr[0].dpid = DPID_SWITCH;
    dp_arr[0].type = PROP_BOOL;
    dp_arr[0].time_stamp = 0;
    dp_arr[0].value.dp_value = app_ctrl_data.Light_switch;

    dp_arr[1].dpid = DPID_RED_VALUE;
    dp_arr[1].type = PROP_VALUE;
    dp_arr[1].time_stamp = 0;
    dp_arr[1].value.dp_value = app_ctrl_data.Red_value;

    dp_arr[2].dpid = DPID_GREEN_VALUE;
    dp_arr[2].type = PROP_VALUE;
    dp_arr[2].time_stamp = 0;
    dp_arr[2].value.dp_value = app_ctrl_data.Green_value;
    
    dp_arr[3].dpid = DPID_BLUE_VALUE;
    dp_arr[3].type = PROP_VALUE;
    dp_arr[3].time_stamp = 0;
    dp_arr[3].value.dp_value = app_ctrl_data.Blue_value;

    op_ret = dev_report_dp_json_async(NULL,dp_arr,dp_cnt);
    Free(dp_arr);
    if(OPRT_OK != op_ret) {
        PR_ERR("dev_report_dp_json_async relay_config data error,err_num",op_ret);
    }

    PR_DEBUG("dp_query report_all_dp_data");
    return;
}


VOID deal_dp_proc(IN CONST TY_OBJ_DP_S *root)
{
    UCHAR_T dpid;

    dpid = root->dpid;
    PR_DEBUG("dpid:%d",dpid);
    
    switch (dpid) {
    
    case DPID_SWITCH:
        app_ctrl_data.Light_switch = root->value.dp_bool;
        break;
    case DPID_RED_VALUE:
        app_ctrl_data.Red_value = root->value.dp_value;
        break;
    case DPID_GREEN_VALUE:
        app_ctrl_data.Green_value = root->value.dp_value;
        break;
    case DPID_BLUE_VALUE:
        app_ctrl_data.Blue_value = root->value.dp_value;
        break;
    default:
        break;
    }

    app_report_all_dp_status();

    return;

}