/*
 * HomeHub.h - An TNM Home Automation Library
 * Created by Vikhyat Chauhan @ TNM on 9/11/19
 * www.thenextmove.in
 * Revision #10 - See readMe
 */

#ifndef HomeHub_h
#define HomeHub_h


/* Setup debug printing macros. */
#define HomeHub_DEBUG

#define HomeHub_DEBUG_PORT Serial
#define HomeHub_DEBUG_PORT_BAUD 9600

#define HomeHub_SLAVE_DATA_PORT Serial
#define HomeHub_SLAVE_DATA_PORT_BAUD 9600

#include "Arduino.h"
#include <ArduinoJson.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <WiFiClient.h>
#include <Ticker.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <pgmspace.h>
#include <Wire.h>
#include <DS3231.h>
#include <vector>

#ifdef ESP8266
#include <functional>

#define MQTT_CALLBACK_SIGN std::function<void(String)> mqttCallback
#else
#define MQTT_CALLBACK_SIGN void (*mqttCallback)(String)
#endif

#ifdef HomeHub_DEBUG
#define HomeHub_DEBUG_PRINT(...) do {HomeHub_DEBUG_PORT.print("[HomeHub] : "); HomeHub_DEBUG_PORT.printf( __VA_ARGS__ );HomeHub_DEBUG_PORT.println("");} while (0)
#else
#define HomeHub_DEBUG_PRINT(...)
#endif

#define ROM_ADDRESS 0x57
#define RELAY_MAX_NUMBER 10

typedef struct{
    bool state;
    unsigned long value;
    bool change = false;
}RELAY;

typedef struct{
    bool state;
    unsigned long value;
    bool change = false;
}FAN;

typedef struct{
    const char* type;
    unsigned long value;
    bool change = false;
}SENSOR;

typedef struct{
    const char* NAME;
    unsigned int RELAY_NUMBER;
    unsigned int FAN_NUMBER;
    unsigned int SENSOR_NUMBER;
    RELAY relay[10];
    FAN fan[10];
    SENSOR sensor[10];
}SLAVE;


//const char HTTP_EN[] PROGMEM = "</div></body></html>";

class HomeHub{
	public:
		HomeHub();
        //Used for setting up MQTT callback functions
        typedef void (*callback_with_arg_t)(void*);
        typedef std::function<void(void)> callback_function_t;
		//Public functions are defined here
		void asynctasks();
        //Data Management Fucntions
        void rom_write_page(unsigned int eeaddresspage, byte* data, byte length);
		void rom_write(unsigned int eeaddress, byte data);
		byte rom_read(unsigned int eeaddress);
        //Wifi AP Setup Functions
        bool initiate_wifi_setup();
        bool end_wifi_setup();
        //Wifi Functions
        bool saved_wifi_connect();
        bool manual_wifi_connect(const char* wifi, const char* pass);
        void saved_wifi_dump();
        void save_wifi_data(String ssid,String password);
        //Mqtt Functions
        bool initiate_mqtt();
        void mqttcallback(char* topic, byte* payload, unsigned int length);
        HomeHub& setCallback(MQTT_CALLBACK_SIGN);
        void publish_mqtt(String message);
        void publish_mqtt(const char* topic, String message);
        void update_device();

	private:
        //std::unique_ptr<MDNSResponder> mdns;
        //std::unique_ptr<WiFiServer> server;
		MDNSResponder* mdns;
        WiFiServer* server;
        Ticker* ticker;
    
        //Slave struct variable
        SLAVE slave;
    
		//Wifi Variables
        String _ssid_string = "TNM" + String(ESP.getChipId());
        String _wifi_data = "";
        String _esid = "";
        String _epass = "";
        int _wifi_data_memspace = 0;
        bool _saved_wifi_present_flag = false;
    
        //Mqtt Variables
        String error = "000";
        String LastCommand = "";
        String Device_Id_As_Subscription_Topic = (String)ESP.getChipId()+"ESP";
        String Device_Id_As_Publish_Topic = (String)ESP.getChipId()+"/{2S}";
        char Device_Id_In_Char_As_Subscription_Topic[12];
        char Device_Id_In_Char_As_Publish_Topic[22];
        unsigned long previous_millis = 0;
        const char* mqtt_server = "m12.cloudmqtt.com";
        const char* mqtt_clientname;
        const char* mqtt_serverid = "wbynzcri";
        const char* mqtt_serverpass = "uOIqIxMgf3Dl";
        const int mqtt_port = 12233;
        MQTT_CALLBACK_SIGN;
    
        //Device Update
        const String _host_update = "http://us-central1-sense-smart.cloudfunctions.net/update";
        const String _firmware_version = "3";
        bool _boot_check_update_flag = true;
        
    
        //Slave Handling Variables
        unsigned int _SLAVE_DATA_PORT_counter = 0;
        bool _receiving_json = false;
        bool _received_json = false;
        bool _initiate_AP = false;
        String _SLAVE_DATA_PORT_command = "";
        String _slave_command_buffer = "";
    
        //Time Sensor Variables
        int _timesensor_date,_timesensor_month,_timesensor_year,_timesensor_week,_timesensor_hour,_timesensor_minute,_timesensor_second,_timesensor_temp,_timesensor_error_code;
        bool _timesensor_h12 = false;
        bool _timesensor_century = false;
        bool _timesensor_pmam;
        boolean _timesensor_error;
        boolean _timesensor_set = 1;
        byte _timesensor_memspace[3] = {NULL,260,261};
    
        //async Task control variables
        bool _wifi_setup_webhandler_flag = false;
        bool _mqtt_webhandler_flag = false;
        bool _check_update_flag = false;
        bool _slave_handshake_flag = false;
    
        //millis variables for async tasks
        unsigned long _slave_handshake_millis = 0.0;

		//Private Functions 
		void slave_handler();
        void slave_sync_handler();
        bool slave_handshake_handler();
        void slave_command_processor(const char* command);
        void start_server();
        bool stop_server();
        bool initiate_ap();
        bool end_ap();
        int wifi_setup_webhandler();
        int normal_webhandler();
        bool retrieve_wifi_data();
        bool test_wifi();
        String scan_networks();
        void timesensor_handler();
};

#endif
