#ifndef SRC_BPS_LASER_H
#define SRC_BSP_LASER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"{
#endif

//typedef struct laser_scan_cfg
//{
//  int scaning_Frequency;
//  int angle_resolution;
//  int start_angle;
//  int stop_angle;
//}laser_scan_cfg_t;
//
//typedef struct laser_scan_data_cfg
//{
//  int output_channel;
//  uint8_t remission;
//  int resolution;
//  int encoder;
//  uint8_t position;
//  uint8_t device_name;
//  uint8_t time_stamp;
//  int output_interval;
//}laser_scan_data_cfg_t
//
//typedef struct laser_scan_output_range
//{
//  int angle_resolution;
//  int start_angle;
//  int stop_angle;
//}laser_scan_output_range_t;
//
//typedef laser_scan_data
//{
//  int dist_len1;
//  uint16_t dist1[1082];
//  int dist_len2;
//  uint16_t dist2[1082];
//  int rssi_len1;
//  uint16_t rssi1[1082];
//  int rssi_len2;
//  uint16_t rssi2[1082];  
//}laser_scan_data_t;

//uint8_t bsp_laser_connect(char *ip, int port);
//uint8_t bsp_laser_isconnected(void);
//void    bsp_laser_disconnect(void);
//
//void    bsp_laser login(void);
//
//void    bsp_laser_start_measure(void);
//void    bsp_laser_stop_measure(void);
//void    bsp_laser_query_status(void);
//
//laser_scan_cfg  bsp_laser_get_scan_cfg(void);
//void bsp_laser_set_scan_cfg(laser_scan_cfg cfg);
//
//laser
  
int8_t bsp_laser_device_init(void);
int8_t bsp_laser_scan_init(void);

int8_t bsp_laser_get_datagram(char* recv_buff, uint16_t buff_len, uint16_t * actual_len);

#ifdef __cplusplus
}
#endif

#endif
