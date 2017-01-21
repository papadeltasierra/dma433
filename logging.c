#include "ets_sys.h"
#include "osapi.h"
#include "os_type.h"
#include "user_interface.h"
#include "stdarg.h"
#include "logging.h"
#include "syslog.h"


/**
 * Simple logging function.
 *
 * Would be nice the add TCP client logging later and/or formatting of the
 * timestamp.
 */
void ICACHE_FLASH_ATTR console(char *fmt, ...)
{
	uint32 timestamp;

	timestamp = sntp_get_current_timestamp();

	va_list argp;
	va_start(argp, fmt);

	ets_uart_printf("%10.10u:", timestamp);
	ets_uart_printf(fmt, argp);
	// syslog(LOG_DEBUG | LOG_LOCAL0, fmt, argp);

	va_end(argp);
}

