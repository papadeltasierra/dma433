#include "ets_sys.h"
#include "osapi.h"
#include "os_type.h"
#include "user_interface.h"
#include "mem.h"
#include "wifi.h"
#include "config.h"
#include "msg.h"
#include "syslog.h"

void ICACHE_FLASH_ATTR wifi_setup(char *ssid, char *password)
{
	struct station_config *config = (struct station_config *)os_zalloc(sizeof(struct station_config));

	/* need to set opmode before you set config */
	wifi_set_event_handler_cb(wifi_handle_event_cb);
	wifi_set_opmode(STATION_MODE);

	os_sprintf(config->ssid, ssid);
	os_sprintf(config->password, password);

	/* need to sure that you are in station mode first,
	 * otherwise it will be failed. */
	wifi_station_set_config(config);
	os_free(config);
	syslog(SMSG_WIFI_INIT);
}

void ICACHE_FLASH_ATTR wifi_handle_event_cb(System_Event_t *evt)
{
	os_printf("event %x\n", evt->event);
	switch (evt->event) {

	case EVENT_STAMODE_CONNECTED:
		syslog(
            SMSG_WIFI_CONNECT,
		    evt->event_info.connected.ssid,
		    evt->event_info.connected.channel);
		break;

	case EVENT_STAMODE_DISCONNECTED:
		syslog(
		    SMSG_WIFI_DISCONNECT,
		    evt->event_info.disconnected.ssid,
		    evt->event_info.disconnected.reason);
		syslog_stop();
		break;

	case EVENT_STAMODE_AUTHMODE_CHANGE:
		/**
		 * os_printf("WiFi: mode: %d -> %d\n",
	     *		evt->event_info.auth_change.old_mode,
         *		evt->event_info.auth_change.new_mode);
         */
		syslog(SMSG_WIFI_EVENT, evt->event);
		break;

	case EVENT_STAMODE_GOT_IP:
		syslog_start();
		syslog(
			SMSG_WIFI_IP,
			IP2STR(&evt->event_info.got_ip.ip),
		    IP2STR(&evt->event_info.got_ip.mask),
		    IP2STR(&evt->event_info.got_ip.gw));
		break;

	case EVENT_SOFTAPMODE_STACONNECTED:
		/**
		 * os_printf("WiFi: station: " MACSTR "join, AID = %d\n",
		 * 		MAC2STR(evt->event_info.sta_connected.mac),
		 * 		evt->event_info.sta_connected.aid);
		 */
		syslog(SMSG_WIFI_EVENT, evt->event);
		break;

	case EVENT_SOFTAPMODE_STADISCONNECTED:
		/**
		 * os_printf("WiFi: station: " MACSTR "leave, AID = %d\n",
         * 				MAC2STR(evt->event_info.sta_disconnected.mac),
         *				evt->event_info.sta_disconnected.aid);/
         */
		syslog(SMSG_WIFI_EVENT, evt->event);
		break;

	default:
		syslog(SMSG_WIFI_EVENT, evt->event);
		break;
	}
}
