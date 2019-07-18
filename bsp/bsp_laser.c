#include "bsp_laser.h"

#include "lwip/init.h"
#include "lwip/netif.h"
#include "lwip/tcpip.h"
#include "lwip/ip_addr.h"
#include "lwip/udp.h"
#include "lwip/sockets.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/opt.h"

#include <string.h>
#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"

int sock_fd;
struct sockaddr_in sock_addr;

char ret_data[65535];
int8_t is_socket_connected = 0;

char device_indet[] = "\x02sRI0\x03\0";
char serial_number[] = "\x02sRN SerialNumber\x03\0";
char firmware_version[] = "\x02sRN FirmwareVersion\x03\0";
char device_state[] = "\x02sRN SCdevicestate\x03\0";
char scandata_start[] = "\x02sEN LMDscandata 1\x03\0";
char scandata_stop[] = "\x02sEN LMDscandata 0\x03\0";

char device_state0[] = "sRA SCdevicestate 0";
char device_state1[] = "sRA SCdevicestate 1";
char device_state2[] = "sRA SCdevicestate 2";

int8_t bsp_laser_send_sopa_command(char* req_data, unsigned int req_data_len, 
                                   char* ret_data, unsigned int ret_data_len, 
                                   uint16_t* read_len)
{
  int ret;
  struct timeval timeout;
  fd_set sock_rd;
  char ch;
  int i = 0;
  
  timeout.tv_usec = 0;
  timeout.tv_sec = 1;
  
  FD_ZERO(&sock_rd);
  FD_SET(sock_fd, &sock_rd);
  
  if(!is_socket_connected)
  {
    return -1;
  }
  else
  {
    ret = send(sock_fd, req_data, req_data_len, 0);
    if(ret < req_data_len)
    {
      return -1;
    }
    while(1)
    {
      ret = select(sock_fd + 1, &sock_rd, NULL, NULL, &timeout);
      if(ret == 0)
      {
        return -1;
      }
      else if(ret == -1)
      {
        return -1;
      }
      else
      {
        if(FD_ISSET(sock_fd, &sock_rd))
        {
          ret = read(sock_fd, &ch, 1);
          if(ret < 0)
          {
            return -1;
          }
          else if(ret == 1)
          {
            if(ch == 0x03)
            {
              *read_len = i;
              break;
            }
            else
            {
              ret_data[i] = ch;
              i++;
              continue;
            }
          }
        }
        else
        {
          return -1;
        }
      }
    }
  }  
  return 0;
}

int8_t bsp_laser_device_init(void)
{
	int err;
  
	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_fd == -1)
	{
		is_socket_connected = 0;
		close(sock_fd);
		return -1;
	}
  
	memset(&sock_addr, 0, sizeof(struct sockaddr_in));
	sock_addr.sin_family = AF_INET;
	sock_addr.sin_addr.s_addr = inet_addr("192.168.2.10");
	sock_addr.sin_port = htons(2111);
	
	err = connect(sock_fd, (struct sockaddr *)&sock_addr, sizeof(sock_addr));
	if (err < 0) 
	{
		is_socket_connected = 0;
		close(sock_fd);
		return -1;
	}
  
	is_socket_connected = 1;
  
	return 0;
}

int8_t bsp_laser_scan_init(void)
{
  int8_t ret;
  char recv_data[100];
  TickType_t tick_delay = 5;
  uint16_t read_data_len;
  
  ret = bsp_laser_send_sopa_command(device_indet, sizeof(device_indet), recv_data, 100, &read_data_len);
  if(ret < 0)
  {
    return -1;
  }
  vTaskDelay(tick_delay);
  
  ret  = bsp_laser_send_sopa_command(serial_number, sizeof(serial_number), recv_data, 100, &read_data_len);
  if(ret < 0)
  {
    return -1;
  }
  vTaskDelay(tick_delay);
  
  ret = bsp_laser_send_sopa_command(firmware_version, sizeof(firmware_version), recv_data, 100, &read_data_len);
  if(ret < 0)
  {
    return -1;
  }
  vTaskDelay(tick_delay);
  
  ret = bsp_laser_send_sopa_command(device_state, sizeof(device_state), recv_data, 100, &read_data_len);
  if(ret < 0)
  {
    return -1;
  }
  vTaskDelay(tick_delay);
  
  ret = bsp_laser_send_sopa_command(scandata_start, sizeof(scandata_start), recv_data, 100, &read_data_len);
  if(ret < 0)
  {
    return -1;
  }
  vTaskDelay(tick_delay);
  return 0;
}

int8_t bsp_laser_get_datagram(char* recv_buff, uint16_t buff_len, uint16_t * actual_len)
{
  int ret;
  struct timeval timeout;
  fd_set sock_rd;
  char ch;
  int i = 0;
  
  timeout.tv_usec = 0;
  timeout.tv_sec = 1;
  
  FD_ZERO(&sock_rd);
  FD_SET(sock_fd, &sock_rd);
  
  if(!is_socket_connected)
  {
    return -1;
  }
  else
  {
    while(1)
    {
      ret = select(sock_fd + 1, &sock_rd, NULL, NULL, &timeout);
      if(ret == 0)
      {
        return -1;
      }
      else if(ret == -1)
      {
        return -1;
      }
      else
      {
        if(FD_ISSET(sock_fd, &sock_rd))
        {
          ret = read(sock_fd, &ch, 1);
          if(ret < 0)
          {
            return -1;
          }
          else if(ret == 1)
          {
			  if(ch == 0x02)
			  {
				  i = 0;
				  continue;
			  }
			  else if(ch == 0x03)
			  {
				  *actual_len = i;
				  break;
			  }
			  else
			  {
				  recv_buff[i] = ch;
				  i++;
				  continue;
			  }
          }
        }
      }
    }
  }
  
  return 0;
}

