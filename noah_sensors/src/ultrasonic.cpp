/* 
 *  ultrasonic.cpp 
 *  Communicate with ultrasonics.
 *  Author: Kaka Xie 
 *  Date:2017/11/28
 */

#include "ros/ros.h"
#include "std_msgs/String.h"
#include "std_msgs/UInt8MultiArray.h" 
#include "pthread.h"
#include <math.h>
#include <stdio.h>     
#include <stdlib.h>     
#include <unistd.h>     
#include <sys/types.h>  
#include <sys/stat.h>   
#include <fcntl.h>      
#include <termios.h>   
#include <errno.h>     
#include <string.h>
#include <time.h>
#include <signal.h>
#include <iostream>
#include <vector>
#include "stdlib.h"
#include "cstdlib"
#include "string"
#include "sstream"
#include <mrobot_driver_msgs/vci_can.h>
#include <roscan/can_long_frame.h>
#include <ultrasonic.h>
#include <common.h>


void Ultrasonic::update_status(void)
{
    for(uint8_t i = 0; i < ULTRASONIC_NUM_MAX; i++)
    {
        if(ros::Time::now() - start_measure_time[i] >= ros::Duration(1.5))
        {
            this->err_status[i] = ERR_COMMUNICATE_TIME_OUT;
            this->distance[i] = DISTANCE_ERR_TIME_OUT;
        }
    }
}

void Ultrasonic::get_version(uint8_t ul_id)     
{
    if(ul_id > 15)
    {
        return ;
    }
    mrobot_driver_msgs::vci_can can_msg;
    CAN_ID_UNION id;
    memset(&id, 0x0, sizeof(CAN_ID_UNION));
    id.CanID_Struct.SourceID = CAN_SOURCE_ID_GET_VERSION;
    id.CanID_Struct.SrcMACID = 1;
    id.CanID_Struct.DestMACID = ULTRASONIC_CAN_SRC_MAC_ID_BASE+ ul_id;
    id.CanID_Struct.FUNC_ID = 0x02;
    id.CanID_Struct.ACK = 0;
    id.CanID_Struct.res = 0;

    can_msg.ID = id.CANx_ID;
    can_msg.DataLen = 2;
    can_msg.Data.resize(2);
    can_msg.Data[0] = 0x00;
    can_msg.Data[1] = 0;
    this->pub_to_can_node.publish(can_msg);
}

int Ultrasonic::start_measurement(uint8_t ul_id)     
{
    int error = 0; 
    if(ul_id > 15)
    {
        return -1;
    }
    mrobot_driver_msgs::vci_can can_msg;
    CAN_ID_UNION id;
    memset(&id, 0x0, sizeof(CAN_ID_UNION));
    id.CanID_Struct.SourceID = CAN_SOURCE_ID_START_MEASUREMENT;
    id.CanID_Struct.SrcMACID = 1;
    id.CanID_Struct.DestMACID = ULTRASONIC_CAN_SRC_MAC_ID_BASE+ ul_id;
    id.CanID_Struct.FUNC_ID = 0x02;
    id.CanID_Struct.ACK = 0;
    id.CanID_Struct.res = 0;

    can_msg.ID = id.CANx_ID;
    can_msg.DataLen = 2;
    can_msg.Data.resize(2);
    can_msg.Data[0] = 0x00;
    can_msg.Data[1] = 0;
    this->pub_to_can_node.publish(can_msg);
    return error;
}

void Ultrasonic::ultrasonic_en(uint8_t ul_id, bool en)     
{
    if(ul_id > 15)
    {
        return ;
    }
    mrobot_driver_msgs::vci_can can_msg;
    CAN_ID_UNION id;
    memset(&id, 0x0, sizeof(CAN_ID_UNION));
    id.CanID_Struct.SourceID = CAN_SOURCE_ID_MEASUREMENT_EN;
    id.CanID_Struct.SrcMACID = 1;
    id.CanID_Struct.DestMACID = ULTRASONIC_CAN_SRC_MAC_ID_BASE+ ul_id;
    id.CanID_Struct.FUNC_ID = 0x02;
    id.CanID_Struct.ACK = 0;
    id.CanID_Struct.res = 0;

    can_msg.ID = id.CANx_ID;
    can_msg.DataLen = 2;
    can_msg.Data.resize(2);
    can_msg.Data[0] = 0x00;
    can_msg.Data[1] = en;
    this->pub_to_can_node.publish(can_msg);
}
#define BROADCAST_CAN_SRC_ID    0x60
int Ultrasonic::broadcast_test(void)     
{
    int error = 0; 
    mrobot_driver_msgs::vci_can can_msg;
    CAN_ID_UNION id;
    memset(&id, 0x0, sizeof(CAN_ID_UNION));
    id.CanID_Struct.SourceID = CAN_SOURCE_ID_START_MEASUREMENT;
    id.CanID_Struct.SrcMACID = 1;
    id.CanID_Struct.DestMACID = BROADCAST_CAN_SRC_ID;
    id.CanID_Struct.FUNC_ID = 0x02;
    id.CanID_Struct.ACK = 0;
    id.CanID_Struct.res = 0;

    can_msg.ID = id.CANx_ID;
    can_msg.DataLen = 2;
    can_msg.Data.resize(2);
    can_msg.Data[0] = 0x00;
    can_msg.Data[1] = 0;
    this->pub_to_can_node.publish(can_msg);
    return error;
}

void pub_json_msg_to_app( const nlohmann::json j_msg)
{
    std_msgs::String pub_json_msg;
    std::stringstream ss;

    ss.clear();
    ss << j_msg;
    pub_json_msg.data = ss.str();
    //this->noah_powerboard_pub.publish(pub_json_msg);
}

void Ultrasonic::update_measure_en(uint32_t ul_en)
{
    if(ul_en<<(32 - ULTRASONIC_NUM_MAX) == measure_en_ack<<(32 - ULTRASONIC_NUM_MAX)) 
        return;

    for(uint8_t i = 0; i < ULTRASONIC_NUM_MAX; i++)
    {
        if( (ul_en^measure_en_ack) &(1<<i) ) 
        {
            this->ultrasonic_en(i,(ul_en>>i) &0x01);
            ROS_ERROR("set %d to %d",i, (ul_en>>i) &0x01);
        }
    }

}
bool Ultrasonic::is_ultrasonic_can_id(CAN_ID_UNION id)
{
    if((id.CanID_Struct.SrcMACID >= 0x60)&&(id.CanID_Struct.SrcMACID <= 0x6f))
    {
        return true ;
    }
    return false;
}


#define NOT_ULTRASONIC_ID      0xff
uint8_t Ultrasonic::parse_ultrasonic_id(CAN_ID_UNION id)
{
    if((id.CanID_Struct.SrcMACID >= 0x60)&&(id.CanID_Struct.SrcMACID <= 0x6f))
    {
        return id.CanID_Struct.SrcMACID - ULTRASONIC_CAN_SRC_MAC_ID_BASE;
    }
    return NOT_ULTRASONIC_ID;
}


void Ultrasonic::rcv_from_can_node_callback(const mrobot_driver_msgs::vci_can::ConstPtr &c_msg)
{
    mrobot_driver_msgs::vci_can can_msg;
    mrobot_driver_msgs::vci_can long_msg;
    CAN_ID_UNION id;
    uint8_t ul_id;
    //ROS_INFO("%s",__func__);
    long_msg = this->long_frame.frame_construct(c_msg);
    mrobot_driver_msgs::vci_can* msg = &long_msg;
    if( msg->ID == 0 ) 
    {
        return;
    }
    if(this->is_log_on == true)
    {
        for(uint8_t i = 0; i < msg->DataLen; i++)
        {
            ROS_INFO("msg->Data[%d] = 0x%x",i,msg->Data[i]);
        }
    }
    can_msg.ID = msg->ID;
    id.CANx_ID = can_msg.ID;
    can_msg.DataLen = msg->DataLen;

    if(this->is_ultrasonic_can_id(id) == false)
    {
        ROS_ERROR("not ultrasonic CAN id");
        return ;
    }

    if((ul_id = this->parse_ultrasonic_id(id)) == NOT_ULTRASONIC_ID)
    {
        ROS_ERROR("ultrasonic CAN id not right");
        return ; 
    }
    if(ul_id >= ULTRASONIC_NUM_MAX)
    {
        ROS_ERROR("wtf ! ! !");
        return;
    }
    if(id.CanID_Struct.SourceID == CAN_SOURCE_ID_START_MEASUREMENT)
    {
        if(id.CanID_Struct.ACK == 1)
        {
            uint16_t distance = 0;
            static uint32_t cnt = 0;
            this->start_measure_time[ul_id] = ros::Time::now();

            distance = msg->Data[0];
            distance += msg->Data[1]<<8;
            this->distance[ul_id] = double(distance)/100;

            if((this->distance[ul_id] >= DISTANCE_MAX - 0.00001) || (abs(this->distance[ul_id]) <= 0.00001))  //distance > DISTANCE_MAX or do not have obstacle
            {
                this->distance[ul_id] = DISTANCE_MAX;
                //this->distance_buf[ul_id][cnt] = DISTANCE_MAX;
                this->distance_buf_proc[ul_id][cnt] = 0;
            }
            else
            {
                uint8_t filter_flag = 1;
                this->distance_buf_proc[ul_id][cnt] = 1;
                for(uint8_t j = 0; j < FILTER_BUF_SIZE; j++) 
                {
                    filter_flag *= this->distance_buf_proc[ul_id][cnt];
                }
                if(filter_flag == 0)
                {
                    this->distance[ul_id] = DISTANCE_MAX;
                }
            }

            measure_en_ack |= 1<<ul_id; //we can receive measurement data, so this ultrasonic is enable !

            if(cnt < FILTER_BUF_SIZE - 1)
            {
                cnt++;
            }
            else
            {
                cnt = 0;
            }



            //if(this->is_log_on == true)
            {
#if 0
                printf("\n");
                printf("\n");
                printf("\n");
                printf("ultrasonic:                                                           laser:\n");
                for(uint8_t i = 0; i < ULTRASONIC_NUM_MAX; i++)
                {
                    printf("%4d ",i + 1);
                }
                for(uint8_t i = 0; i < 13; i++)
                {
                    printf("%4d ",i + 1);
                }
                printf("\n");
#endif





#if 1
		{

			for(uint8_t i = 0; i < ULTRASONIC_NUM_MAX; i++)
			{
				printf("%4f ",this->distance[i]);
			}
			printf("\n");
		}		
#endif






#if 0
extern uint16_t laser_test_data[13];
                for(uint8_t i = 0; i <13; i++)
                {
                    printf("%4d ",laser_test_data[i]);
                }
                printf("\n");
#endif

            }
        }
    }
    
    if(id.CanID_Struct.SourceID == CAN_SOURCE_ID_MEASUREMENT_EN)
    {
        if(id.CanID_Struct.ACK == 1)
        {   
            if(msg->Data[0] == 0)
            {
                measure_en_ack &= ~(1<<ul_id);
            }
            else if(msg->Data[0] == 1)
            {
                measure_en_ack |= 1<<ul_id;
            }
            ROS_WARN("get ultrasonic id %d enable is %d ",ul_id,msg->Data[0]);
            ROS_WARN("measure_en_ack: %x ",measure_en_ack);
            ROS_WARN("measure_en: %x ",sonar_en);

        }
    }

    if(id.CanID_Struct.SourceID == CAN_SOURCE_ID_GET_VERSION)
    {
        uint8_t len;
        if(id.CanID_Struct.ACK == 1)
        {   
            if(msg->Data[0] > 0)
            {
                len = msg->Data[0];
                version[ul_id].resize(len);
                version[ul_id].clear();
                for(uint8_t i = 0; i < len; i++)
                {
                    version[ul_id].push_back(*(char *)&(msg->Data[i+1]));
                }
                //memcpy(version[ul_id].cbegin(),&msg->Data[1], len);
                ROS_WARN("ultrasonic %d version is %s",ul_id,version[ul_id].data());
            }

        }
    }
}

void Ultrasonic::pub_ultrasonic_data_to_navigation(double * ul_data)
{
    uint32_t en_sonar = sonar_en;
    static bool close_all_flag = 0;
    this->ultrasonic_data.header.stamp = ros::Time::now();
    this->ultrasonic_data.radiation_type = ULTRASOUND;
    this->ultrasonic_data.field_of_view = 0.61;
    this->ultrasonic_data.min_range = 0.03;
    this->ultrasonic_data.max_range = 2.0;
    if(close_all_flag == 0)
    {
        for(int i=0;i<ultrasonic_real_num;i++)
        {
            if(en_sonar == 0)
            {
                close_all_flag = 1;

                if(i >= 3)
                {
                    this->ultrasonic_data.min_range = 0.03;
                    this->ultrasonic_data.max_range = 2.0;
                }
                this->ultrasonic_data.header.frame_id = this->ultrasonic_frames[i];
                this->ultrasonic_data.range = 5.0;
                usleep(2000);
                this->ultrasonic_pub_to_navigation.publish(this->ultrasonic_data);
            }
            else if(en_sonar & (0x00000001<<i))
            {
                close_all_flag = 0;

                if(i >= 3)
                {
                    this->ultrasonic_data.min_range = 0.03;
                    this->ultrasonic_data.max_range = 2.0;
                }
                this->ultrasonic_data.header.frame_id = this->ultrasonic_frames[i];
                this->ultrasonic_data.range = this->distance[i];
                usleep(2000);
                this->ultrasonic_pub_to_navigation.publish(this->ultrasonic_data);
            }
        }
    }

    if(en_sonar == 0)
    {
        close_all_flag = 1;
    }
    else
    {
        close_all_flag = 0;
    }

}


