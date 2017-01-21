/**
 * Syslog parameters.
 */
#define CFG_APP_NAME		"Dma433"
#define CFG_SYSLOG_SS_ID	"dma433@1"

/**
 * WiFi network configuration.
 */
#define CFG_WIFI_SSID  		"5u93ca1i7r69"
#define CFG_WIFI_PWD  		"5nipp094rr6M"

/**
 * Watchdog timer interval.
 */
#define CFG_WDOG_INTERVAL	1000*60*20

/**
 * Timezones.
 */
#define TZ_UTC				0
#define TZ_GMT				0
#define TZ_EST				-5

/**
 * NTP servers to use and the timezone.
 */
#define CFG_NTP_SERVER_0	"3.uk.pool.ntp.org"
#define CFG_NTP_SERVER_1    "2.uk.pool.ntp.org"
#define CFG_NTP_SERVER_2    "1.uk.pool.ntp.org"

#define CFG_NTP_TIMEZONE	TZ_UTC

/**
 * syslog server address and port.
 */
#if 0
#define CFG_SYSLOG_SERVER	"192.168.1.5"
#define CFG_SYSLOG_PORT		514
#else
#define CFG_SYSLOG_SERVER	"logs4.papertrailapp.com"
#define CFG_SYSLOG_PORT		34135
#endif

/**
 * GPIO configuration.
 */
#define CFG_GPIO_PERIPH 	PERIPHS_IO_MUX_U0RXD_U
#define CFG_GPIO_FUNC       FUNC_GPIO3
#define CFG_GPIO_NUM        3
#define CFG_GPIO_PIN        GPIO_ID_PIN(CFG_GPIO_NUM)

/**
 * Maximum interval before we MUST send an HTTP request.
 */
#define CFG_HTTP_INTERVAL 	5*60*1000*1000
#define CFG_HTTP_SITE       "http://weatherstation.wunderground.com"
#define CFG_HTTP_PAGE		"weatherstation/updateweatherstation.php"
#define CFG_HTTP_ID		    "ID=IENFIELD17&PASSWORD=xanadu"
#define CFG_HTTP_DATA		"action=updateraw&dateutc=now&tempf=%d.%d&humidity=%d"
#define CFG_HTTP_URL_FMT	CFG_HTTP_SITE "/" CFG_HTTP_PAGE "?" CFG_HTTP_ID "&" CFG_HTTP_DATA

