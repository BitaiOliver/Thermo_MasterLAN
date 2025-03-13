#ifndef COMPILER_SWITCH_H
#define COMPILER_SWITCH_H 1
/* This header shall be included in everi software component.
    This header includes defines which are used as compiler switches. */

//#define BLYNK_PRINT Serial // Defines the object that is used for printing
//#define BLYNK_DEBUG 

//#include <BlynkSimpleEsp8266.h>

#define SERIAL_PRINT_MAIN                       1U /* If this define exist serial prints are working for Main module */
#define SERIAL_PRINT_TASKMGR                    1U /* If this define exist serial prints are working for TaskMgr module */
#define SERIAL_PRINT_TCP                        1U /* If this define exist serial prints are working for TcpClient module */
#define SERIAL_PRINT_TEMP                       1U /* If this define exist serial prints are working for Temperature module */
#define SERIAL_PRINT_BLYNK                      1U /* If this define exist serial prints are working for BlynkClient module */
#define SERIAL_PRINT_THERMO                     1U /* If this define exist serial prints are working for Thermo module */
#define SERIAL_PRINT_NVM                        1U /* If this define exist serial prints are working for NvM module */
#define SERIAL_PRINT_WIFIMGR                    1U /* If this define exist serial prints are working for WifiMgr module */
#define SERIAL_PRINT_DIO                        1U /* If this define exist serial prints are working for DIO module */

#define EEPROM_ADDR_S1_TEMP_THRESH          0U
#define EEPROM_ADDR_S2_TEMP_THRESH          1U
#define EEPROM_ADDR_S3_TEMP_THRESH          2U

#define EEPROM_ADDR_S1_ONOFF                3U
#define EEPROM_ADDR_S2_ONOFF                4U
#define EEPROM_ADDR_S3_ONOFF                5U

#define EEPROM_NB_USED_BLOCKS               6U

/* If this define exist then IO pin is seted to LOW in order to activate relay/heating. */
/* Comment/delete define for reqular active on HIGH relay*/
#define RELAY_TYPE_ACTIVE_ON_LOW            1U

#ifdef RELAY_TYPE_ACTIVE_ON_LOW
#define RELAY_DEFAULT_INIT_VALUE            HIGH
#else
#define RELAY_DEFAULT_INIT_VALUE            LOW
#endif

/* Some other defines used by all the modules */
#define TRUE                    1U
#define FALSE                   0U

#define TEMPERATURE_INVALID_VALUE       0U


#define TEMPERATURE_QF_INIT             0U
#define TEMPERATURE_QF_GOOD             1U
#define TEMPERATURE_QF_FAILING          2U
#define TEMPERATURE_QF_BAD              3U

#define WIFI_STATUS_INIT                0U
#define WIFI_STATUS_CONNECTED           1U
#define WIFI_STATUS_NOT_CONNECTED       2U

#define BLYNK_SERVER_STATUS_INIT                0U
#define BLYNK_SERVER_STATUS_CONNECTED           1U
#define BLYNK_SERVER_STATUS_NOT_CONNECTED       2U


#define TEMPERATURE_TOLERANCE           4U /* Temperature tolerance below threshold 0.4C*/

/***************** Configuration *****************/
/// Blynk configuration ///
#define BLYNK_TEMPLATE_ID "T******t"            /* add your template ID */
#define BLYNK_DEVICE_NAME "Thermo"              /* add your device name */
const char Blynk_Auth_Token[] = "D******G";     /* add your Authentication token of the lynk server */

const char WiFi_Name[] = "N***i";           /* add your WiFi name (SSID) */
const char WiFi_Password[] = "t***3";       /* add your WiFi password */


#define BLYNKCLIENT_NB_SLAVES                   3U
#define BLYNKCLIENT_NB_MASTER                   1U
#define THERMO_MASTER_RELAY_PIN                 4U /* D02 */

#define BLYNKCLIENT_MASTER_HEATING_STATUS_PIN       V5 
#define BLYNKCLIENT_MASTER_OPERATING_STATUS_PIN     V6 

#define BLYNKCLIENT_SLAVE1_TEMP_THRES_PIN       V1
#define BLYNKCLIENT_SLAVE2_TEMP_THRES_PIN       V2
#define BLYNKCLIENT_SLAVE3_TEMP_THRES_PIN       V3


#define BLYNKCLIENT_SLAVE1_ID                  0U
#define THERMO_SLAVE1_RELAY_PIN                14U /* D05 */
#define BLYNKCLIENT_SLAVE1_TEMP_PIN            V11
#define BLYNKCLIENT_SLAVE1_QF_PIN              V12
#define BLYNKCLIENT_SLAVE1_ONOFF_PIN           V13

#define BLYNKCLIENT_SLAVE2_ID                  1U
#define THERMO_SLAVE2_RELAY_PIN                12U /* D06 */
#define BLYNKCLIENT_SLAVE2_TEMP_PIN            V16
#define BLYNKCLIENT_SLAVE2_QF_PIN              V17
#define BLYNKCLIENT_SLAVE2_ONOFF_PIN           V18

#define BLYNKCLIENT_SLAVE3_ID                  2U
#define THERMO_SLAVE3_RELAY_PIN                13U /* D07 */
#define BLYNKCLIENT_SLAVE3_TEMP_PIN            V21
#define BLYNKCLIENT_SLAVE3_QF_PIN              V22
#define BLYNKCLIENT_SLAVE3_ONOFF_PIN           V23



/*************************************************/
#endif