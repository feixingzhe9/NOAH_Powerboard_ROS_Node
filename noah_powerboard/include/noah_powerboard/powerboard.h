#include "ros/ros.h"
#include "std_msgs/String.h"
#include "json.hpp"
using json = nlohmann::json;
#ifndef LED_H
#define LED_H


#define PROTOCOL_HEAD               0x5a
#define PROTOCOL_TAIL               0xa5
#define POWER_CURRENT_LEN           33

#define BUF_LEN                    256

#define FRAME_TYPE_LEDS_CONTROL         0x01
#define FRAME_TYPE_SYS_STATUS           0x02
#define FRAME_TYPE_BAT_STATUS           0x03
#define FRAME_TYPE_GET_MODULE_STATE     0x04
//#define FRAME_TYPE_READ_ERR_CURRENT     0x05//
#define FRAME_TYPE_MODULE_CONTROL       0x06
#define FRAME_TYPE_IRLED_CONTROL        0x07
#define FRAME_TYPE_GET_CURRENT          0x0a
#define FRAME_TYPE_GET_VERSION          0x0e

#define COM_ERR_REPEAT_TIME             3 

typedef enum
{
    POWER_5V_MOTOR            = 0x00000001,
    POWER_5V_RECHARGE         = 0x00000002,
    POWER_5V_SENSOR_BOARD     = 0x00000004,
    POWER_5V_SWITCH           = 0x00000008,
    POWER_5V_ROUTER           = 0x00000010,  
    POWER_5V_EN               = 0x00000020,

    POWER_12V_PAD             = 0x00000040,
    POWER_12V_2_1_PA          = 0x00000080,
    POWER_12V_EXTEND          = 0x00000100,
    POWER_12V_X86             = 0x00000200,
    POWER_12V_NV              = 0x00000400, 
    POWER_12V_EN              = 0x00000800,


    POWER_24V_EN              = 0x00001000,
    POWER_24V_PRINTER         = 0x00002000,
    POWER_24V_EXTEND          = 0x00004000,
    POWER_VSYS_24V_NV         = 0x00008000,


    POWER_485                 = 0x00010000,
    POWER_SYS_LED             = 0x00020000,
    POWER_RECHARGE_LED        = 0x00040000,
    POWER_SLAM                = 0x00080000,


    POWER_LED_MCU             = 0x00100000,
    POWER_CHARGE_FAN          = 0x00200000,
    POWER_POLE_MOTOR          = 0x00400000,
    POWER_5V_KEYPAD           = 0x00800000,
    POWER_CAMERA_LED          = 0x01000000,

    POWER_ALL                 = 0x0FFFFFFF,

} module_ctrl_e;

typedef struct
{
#define MODULE_CTRL_ON      1  
#define MODULE_CTRL_OFF     0 
    uint8_t     on_off;
#define HW_NO_SUPPORT         0xFFFFFFFF
    volatile uint32_t    module;
} module_ctrl_t;

typedef struct
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
}color_t;

#pragma pack(1)
typedef struct 
{
    //  uint8_t               ctype;
    uint8_t               cur_light_mode;
    color_t               color;
    uint8_t               period;
} rcv_serial_leds_frame_t;

typedef struct 
{
    uint8_t               ctype;
    uint8_t               cur_light_mode;
    color_t               color;
    uint8_t               period;
} ack_serial_leds_frame_t;
#pragma pack()


#pragma pack(1)
typedef struct _VoltageData_t 
{
    uint16_t              _5V_reserve1_currents;
    uint16_t              _24V_nv_currents;
    uint16_t              _12V_nv_currents;
    uint16_t              _48V_extend_currents;

    uint16_t              _12V_extend_currents;
    uint16_t              motor_currents;
    uint16_t              slam_currents;
    uint16_t              _2_1_pa_currents;

    uint16_t              pad_currents;
    uint16_t              printer_currents;
    uint16_t              x86_currents;
    uint16_t              ir_led_currents;

    uint16_t              _5V_leds_currents;

    uint16_t              recharge_currents;
    uint16_t              _24V_extend_currents;
    uint16_t              charge_currents;
    uint16_t              batin_currents;

    uint16_t              vbus_currents;
    uint16_t              bat_motor_currents;
    //  uint16_t              multi_channel_adc;

    uint16_t              _24V_temp;
    uint16_t              _12V_temp;
    uint16_t              _5V_temp;
    uint16_t              air_temp;

    uint16_t              _24V_all_currents;
    uint16_t              _12V_all_currents;
    uint16_t              _5V_all_currents;
    uint16_t              _24V_voltage;

    uint16_t              _12V_voltage;
    uint16_t               _5V_voltage;
    uint16_t               bat_voltage;
    uint16_t               sensor_board_currents;

    int16_t               _5V_router_currents;
} voltage_data_t;

typedef struct 
{
    //    uint8_t               ctype;
    //    uint8_t               cmdType;
    voltage_data_t         voltage_data;  
    uint8_t               fault_bit[4];
    uint8_t               send_rate;
    uint8_t               reserve[7];
} voltage_info_t;  
#pragma pack()


typedef struct _recModuleControlFrame_t
{
    uint8_t               module;
#define             SYSTEM_MODULE         0x00
#define             MOTOR_MODULE          0x01
#define             SENSOR_MODULE         0x02
#define             LEDS_MODULE           0x03
#define             _5VRESERVE_MODULE     0x04
#define             PAD_MODULE            0x05
#define             _12V_ROUTER_MODULE    0x06
#define             _2_1_PA_MODULE        0x07
#define             DYP_MODULE            0x08
#define             X86_MODULE            0x09
#define             NV_MODULE             0x0A
#define             DLP_MODULE            0x0B
#define             _12V_RESERVE_MODULE   0x0C
#define             PRINTER_MODULE        0x0D
#define             _24V_RESERVE_MODULE   0x0E
#define             BAT_NV_MODULE         0x0F
#define             _5V_ALL_MODULE        0x10
#define             _12V_ALL_MODULE       0x11
#define             _24V_ALL_MODULE       0x12
#define             AIUI_MODULE           0x13
#define             _5V_ROUTER_MODULE     0x14
    uint8_t               control;
} rcv_module_control_frame_t;

typedef struct 
{
    uint8_t      effect;
    color_t      color;
    uint8_t      period;
}led_t;

typedef struct
{
#define CMD_BAT_PERCENT     2
#define CMD_BAT_VOLTAGE     1
    uint8_t cmd;
    uint16_t bat_info;
}bat_info_t;

typedef struct _recTestCurrentCmdFrame_t 
{
    uint8_t               cmd;
    uint8_t               sendRate;
#define                         SEND_RATE_SINGLE        ((uint8_t)0x00)
#define                         SEND_RATE_1HZ           ((uint8_t)0x01)
#define                         SEND_RATE_2HZ           ((uint8_t)0x02)
#define                         SEND_RATE_5HZ           ((uint8_t)0x03)
#define                         SEND_RATE_10HZ          ((uint8_t)0x04)
#define                         SEND_RATE_50HZ          ((uint8_t)0x05)
#define                         SEND_RATE_100HZ         ((uint8_t)0x06)
#define                         SEND_RATE_0_5HZ         ((uint8_t)0x07)
#define                         SEND_RATE_0_2HZ         ((uint8_t)0x08)
#define                         SEND_RATE_0_1HZ         ((uint8_t)0x09) 
} current_cmd_frame_t;

typedef struct
{
#define IR_CMD_READ     0
#define IR_CMD_WRITE    1
    uint8_t cmd;
    uint8_t set_ir_percent;
    uint8_t lightness_percent;
}ir_cmd_t;
typedef struct
{
#define DEV_STRING_LEN              50
    char                        dev[DEV_STRING_LEN]; 
    int                         device;
    led_t                       led;
    led_t                       led_set;
    rcv_serial_leds_frame_t     rcv_serial_leds_frame;
    bat_info_t                  bat_info;

    current_cmd_frame_t         current_cmd_frame;  
    voltage_info_t               voltage_info;

#define VERSION_TYPE_FW             0
#define VERSION_TYPE_PROTOCOL       1
    uint8_t                     get_version_type;

#define HW_VERSION_SIZE             3
#define SW_VERSION_SIZE             16
#define PROTOCOL_VERSION_SIZE       15 
    char                        hw_version[HW_VERSION_SIZE];
    char                        sw_version[SW_VERSION_SIZE];
    char                        protocol_version[];

#define SYS_STATUS_OFF              0
#define SYS_STATUS_TURNING_ON       1
#define SYS_STATUS_ON               2
#define SYS_STATUS_TURNING_OFF      3
#define SYS_STATUS_ERR              4
#define                 STATE_IS_CHARGING       0x10
#define                 STATE_IS_LOW_POWER      0x20
#define                 STATE_IS_AUTO_UPLOAD    0x40
#define                 STATE_IS_CHARGER_IN     0x80
#define                 SYSTEM_IS_SLEEP         0x00 //set 0x00 to no use

#define                 STATE_IS_RECHARGE_IN    0x0100

    uint16_t                     sys_status;

    ir_cmd_t                    ir_cmd;
    module_ctrl_t               module_status_set;
    module_ctrl_t               module_status;

#define SEND_DATA_BUF_LEN           255
    uint8_t                     send_data_buf[SEND_DATA_BUF_LEN];
}powerboard_t;

typedef enum 
{
    LIGHTS_MODE_DEFAULT                 = 0,
    LIGHTS_MODE_NOMAL                   = 1,
    LIGHTS_MODE_ERROR                   = 2,
    LIGHTS_MODE_LOW_POWER,
    LIGHTS_MODE_CHARGING,
    LIGHTS_MODE_TURN_LEFT,
    LIGHTS_MODE_TURN_RIGHT,
    LIGHTS_MODE_COM_ERROR,
    LIGHTS_MODE_EMERGENCY_STOP,

    LIGHTS_MODE_SETTING                 = 0xff,
}light_mode_t;

//#define DEV_PATH                "/dev/noah_powerboard"
extern powerboard_t    *sys_powerboard;
class NoahPowerboard
{
    public:
        NoahPowerboard()
        {
            noah_powerboard_pub = n.advertise<std_msgs::String>("tx_noah_powerboard_node",1000);
            pub_charge_status_to_move_base = n.advertise<std_msgs::UInt8MultiArray>("charge_status_to_move_base",1000);
            resp_navigation_camera_leds = n.advertise<std_msgs::String>("resp_lane_follower_node/camera_using_n",1000);
            power_pub_to_app = n.advertise<std_msgs::UInt8MultiArray>("app_sub_power",1);
            power_sub_from_app = n.subscribe("app_pub_power",1000,&NoahPowerboard::power_from_app_rcv_callback,this);
            noah_powerboard_sub = n.subscribe("rx_noah_powerboard_node",1000,&NoahPowerboard::from_app_rcv_callback,this);
            sub_navigation_camera_leds = n.subscribe("lane_follower_node/camera_using_n",1000,&NoahPowerboard::from_navigation_rcv_callback,this);
            
        }
        int PowerboardParamInit(void);
        int SetLedEffect(powerboard_t *powerboard);
        int GetBatteryInfo(powerboard_t *sys);
        int GetAdcData(powerboard_t *sys);
        int GetVersion(powerboard_t *sys);
        int GetSysStatus(powerboard_t *sys);
        int InfraredLedCtrl(powerboard_t *sys);
        int SetModulePowerOnOff(powerboard_t *sys);
        int GetModulePowerOnOff(powerboard_t *sys);
        int send_serial_data(powerboard_t *sys);
        int handle_receive_data(powerboard_t *sys);
        void from_app_rcv_callback(const std_msgs::String::ConstPtr &msg);
        void from_navigation_rcv_callback(const std_msgs::String::ConstPtr &msg);
        void power_from_app_rcv_callback(std_msgs::UInt8MultiArray data);
        void PubPower(void);
        void PubChargeStatus(uint8_t status);

    private:
        uint8_t CalCheckSum(uint8_t *data, uint8_t len);
        int handle_rev_frame(powerboard_t *sys,unsigned char * frame_buf);
        ros::NodeHandle n;
        ros::Publisher noah_powerboard_pub;
        ros::Subscriber noah_powerboard_sub;
        ros::Subscriber sub_navigation_camera_leds;
        ros::Publisher resp_navigation_camera_leds;
        ros::Publisher power_pub_to_app;
        ros::Subscriber power_sub_from_app;
        ros::Publisher pub_charge_status_to_move_base;
        json j;
        void pub_json_msg_to_app(const nlohmann::json j_msg);

};
int handle_receive_data(powerboard_t *sys);
void set_speed(int fd, int speed);
int set_parity(int fd,int databits,int stopbits,int parity);
int open_com_device(char *dev);

#endif



