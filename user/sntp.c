#include "ets_sys.h"
#include "osapi.h"
#include "os_type.h"
#include "user_interface.h"
#include "sntp.h"
#include "msg.h"
#include "syslog.h"

/**
 * Set the NTP time servers to be used and the timezone then
 * initialize the SNTP client.
 */
void ICACHE_FLASH_ATTR sntp_setup(char *server0, char *server1, char *server2, uint32_t timezone)
{
	sntp_setservername(0, server0);
	sntp_setservername(1, server1);
	sntp_setservername(2, server2);
	sntp_set_timezone(timezone);
	sntp_init();
	syslog(SMSG_SNTP_INIT);
}

/**
 * The following function uses a log of carnal knowledge of esp8266 library
 * functions, structures etc.  You have been warned!
 *
 * This works fine with SDK 1.5.0.
 */
struct tm
{
  int tm_sec;
  int tm_min;
  int tm_hour;
  int tm_mday;
  int tm_mon;
  int tm_year;
  int tm_wday;
  int tm_yday;
  int tm_isdst;
};
extern struct tm res_buf;
char no_time_yet[] = "-";
char sntp_syslog_time[32];

/**
 * Note that we cannot access the reult buffer as ESP8266 SDK seems to have
 * make changes to the standard LWIP buffer handling.
 */
char * ICACHE_FLASH_ATTR sntp_get_syslog_time(void)
{
	uint32 t;

	t = sntp_get_current_timestamp();
	sntp_localtime(&t);

	os_sprintf(sntp_syslog_time, "%04d-%02d-%02dT%02d:%02d:%02dZ",
			(1900+res_buf.tm_year), (1+res_buf.tm_mon), res_buf.tm_mday,
			res_buf.tm_hour, res_buf.tm_min, res_buf.tm_sec);

	return(sntp_syslog_time);
}
