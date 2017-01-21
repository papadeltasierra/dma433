#include "ets_sys.h"
#include "osapi.h"
#include "os_type.h"
#include "user_interface.h"
#include "mem.h"
#include "espconn.h"
#include "stdarg.h"
#include "config.h"
#include "logging.h"
#define DEFINE_VARS
#include "syslog.h"

/**
 * Some online syslog servers cannot handle structured data and the
 * recommended way to handle this is to append it as part of the message.
 */
#define SYSLOG_STRUCTURED_IN_MSG

#define SYSLOG_IP_LEN   16
#define SYSLOG_BUF_SIZE   256
#define SYSLOG_STACK_DEPTH 8
#define SYSLOG_DUMMY_IP   "0.0.0.0"
#define SYSLOG_VERSION      1
#define SYSLOG_MAX_HOSTNAME 256

// Parameters provided by the application.
char *syslog_hostname = NULL;
const char *syslog_app_name;
const char **syslog_procs;
const SYSLOG_MSG *syslog_msgs;

char *syslog_ip_address = NULL;
char *syslog_stack[SYSLOG_STACK_DEPTH];
uint16 syslog_length[SYSLOG_STACK_DEPTH] = { 0 };

/**
 * The syslog code reads from the 'head' of the stack.
 * Callers write to the 'tail' of the stack.
 * The stack contains 'depth' number of entries.
 */
int syslog_stack_head = 0;
int syslog_stack_tail = 0;
int syslog_stack_depth = 0;
int syslog_buf_len = 0;
bool syslog_sending = FALSE;
struct espconn *syslog_conn = NULL;
esp_udp *syslog_udp = NULL;

/**
 * The syslog is 'inactive' when we believe that there is no IP connectivity.
 */
bool syslog_inactive = TRUE;
ip_addr_t syslog_addr;
struct espconn syslog_espconn;

static void syslog_sendto();
static void syslog_sendto_callback(void *arg);

int syslog_write_index()
{
	int write_index = syslog_stack_tail;

	/**
	 * If the stack is full, we continue overwriting the last entry so that
	 * the first (depth-1) entries reflect what went wrong when the stack
	 * blocked.
	 */
	if (syslog_stack_depth < SYSLOG_STACK_DEPTH)
	{
		syslog_stack_depth++;
		if (syslog_stack_depth < SYSLOG_STACK_DEPTH)
		{
			syslog_stack_tail++;
			if (syslog_stack_tail >= SYSLOG_STACK_DEPTH)
			{
				/**
				 * Writing is wrapping to the start of the array.
				 */
				syslog_stack_tail = 0;
			}
		}
	}

	CONSOLE("wi: %d, %d, %d, %d", write_index, syslog_stack_depth, syslog_stack_tail, SYSLOG_STACK_DEPTH);
	return(write_index);
}


static void ICACHE_FLASH_ATTR syslog_dns_callback(const char * hostname, ip_addr_t * addr, void * arg)
{
	sint8 rc;

	/**
	 * Store the target IP address.
	 */
	if (addr != NULL)
	{
	    // // CONSOLE("syslog: Hostname: %s, IP address: " IPSTR, hostname, IP2STR(addr));
        os_memcpy(syslog_conn->proto.udp->remote_ip, addr, 4);
	    // // CONSOLE("syslog: Hostname: %s, IP address: " IPSTR, hostname, IP2STR(addr));
	    CONSOLE("syslog: local IP address:port = " IPSTR ":%d", IP2STR(syslog_conn->proto.udp->local_ip), syslog_conn->proto.udp->local_port);
	    CONSOLE("syslog: remote IP address:port = " IPSTR ":%d", IP2STR(syslog_conn->proto.udp->remote_ip), syslog_conn->proto.udp->remote_port);
        syslog_inactive = FALSE;
        rc = espconn_create(syslog_conn);
        if (rc == 0)
        {
      	    rc = espconn_regist_sentcb(syslog_conn, syslog_sendto_callback);
        }
        if (rc != 0)
        {
        	// CONSOLE("syslog: create UDP connection failed: %d", (int)rc);
        }
        syslog_sendto();
	}
	else
	{
		// CONSOLE("syslog: gethostbyname() for '%s' failed!", hostname);
	}
}

static void ICACHE_FLASH_ATTR syslog_gethostbyname()
{
	// // CONSOLE("syslog: IN gethostbyname()");
	err_t error = espconn_gethostbyname(&syslog_espconn,
                                     syslog_hostname, &syslog_addr, syslog_dns_callback);
	if (error == ESPCONN_OK) {
                 // Already in the local names table (or hostname was an IP address), execute the callback ourselves.
                 syslog_dns_callback(syslog_hostname, &syslog_addr, NULL);
	}
	else if (error != ESPCONN_INPROGRESS) {
		// CONSOLE("Failed with error: %d", error);
	}
	// // CONSOLE("syslog: OUT gethostbyname()");
}

/**
 * Called when the outer code believes that IP connectivity has been achieved.
 */
void ICACHE_FLASH_ATTR syslog_start(void)
{
	struct ip_info info;

	// // CONSOLE("syslog: IN: start()");

	if (wifi_get_ip_info(0x00, &info))
	{
		os_sprintf(syslog_ip_address, IPSTR, IP2STR(&info.ip));
		os_memcpy(syslog_conn->proto.udp->local_ip, &info.ip, 4);

		/**
		 * We can now resolve the hostname (perhaps again!).
		 */
		syslog_gethostbyname();
	}
	// // CONSOLE("syslog: OUT: start()");
}

/**
 * Called when the outer code believes that IP connectivity is lost.
 */
void ICACHE_FLASH_ATTR syslog_stop(void)
{
  syslog_inactive = TRUE;
  os_sprintf(syslog_ip_address, SYSLOG_DUMMY_IP);
}

/**
 * Try to send the next buffered syslog entry.
 */
static void syslog_sendto()
{
	sint16 rc = 0;
	uint8 *buffer;
	uint16 length;

	if (syslog_sending || syslog_inactive)
	{
		/**
		 * Already sending or no IP so buffer this.
		 */
		CONSOLE("inactive %d, sending %d", syslog_sending, syslog_inactive);
	}
	else
	{
		CONSOLE("syslog: %d, %d, %d", rc, syslog_stack_head, syslog_stack_tail);
		while ((rc == 0) &&
		       (syslog_stack_head != syslog_stack_tail))
		{
			// Recursion!!
			buffer = syslog_stack[syslog_stack_head];
			length = syslog_length[syslog_stack_head];
			syslog_stack_head++;
			syslog_stack_depth--;
			if (syslog_stack_head >= SYSLOG_STACK_DEPTH)
			{
				syslog_stack_head = 0;
			}
			rc = espconn_sendto(
					syslog_conn, buffer, length);
			if (rc == ESPCONN_INPROGRESS)
			{
				/**
				 * Asynchronous send.
				 */
				syslog_sending = TRUE;
			}
			else if (rc != 0)
			{
				CONSOLE("Error: syslog, sendto failed: %d", rc);
			}
			CONSOLE("syslog2: %d, %d, %d", rc, syslog_stack_head, syslog_stack_tail);
		}
		CONSOLE("Done sending");
	}
}

static void syslog_sendto_callback(void *arg)
{
	struct espconn *connection = (struct espconn *)arg;
	// // // CONSOLE("Debug 15");

	if ((connection->type == ESPCONN_UDP) &&
		(connection->proto.udp->remote_port == syslog_conn->proto.udp->remote_port) &&
		((uint32)connection->proto.udp->remote_ip == (uint32)syslog_conn->proto.udp->remote_ip))
	{
		syslog_sending = FALSE;
		syslog_sendto();
	}
}

void ICACHE_FLASH_ATTR syslog_setup(
    char *hostname, int port, const char *app_name, const char **procs, const SYSLOG_MSG *msgs)
{
	char *buffer = NULL;
	int ii;
  sint16 rc;
  syslog_ip_address = (char *)os_zalloc(SYSLOG_IP_LEN);
  buffer = (char *)os_zalloc(SYSLOG_BUF_SIZE * SYSLOG_STACK_DEPTH);
  for (ii = 0; ii < SYSLOG_STACK_DEPTH; ii++)
  {
	  syslog_stack[ii] = buffer;
	  buffer += SYSLOG_BUF_SIZE;
  }

  syslog_conn = (struct espconn *)os_zalloc(sizeof(struct espconn));
  syslog_udp = (esp_udp *)os_zalloc(sizeof(esp_udp));
  syslog_hostname = (char *)os_zalloc(SYSLOG_MAX_HOSTNAME);

  syslog_app_name = app_name;
  syslog_procs = procs;
  syslog_msgs = msgs;
  os_strncpy(syslog_hostname, hostname, SYSLOG_MAX_HOSTNAME);
  os_strcpy(syslog_ip_address, SYSLOG_DUMMY_IP);

  syslog_conn->type = ESPCONN_UDP;
  syslog_conn->proto.udp = syslog_udp;
  syslog_conn->proto.udp->local_port = espconn_port();
  syslog_conn->proto.udp->remote_port = port;
  // // CONSOLE("SYSLOG - setup: %s:%d, rc: %d", hostname, port, rc);
}

/**
 * Create a SYSLOG message.  Refer to RFC5424.
 *
 * This function has been adapted for use by this application and is not
 * intended to be a general purpose function.
 */
void ICACHE_FLASH_ATTR syslog(int msg_id, ...)
{
  char *buffer;
  size_t new_buf_len;
  int written;
  int total_written;
  int space_left;
  int write_index = -1;
  sint16 rc;
  const SYSLOG_MSG *msg = &syslog_msgs[msg_id];

  /**
   * Format is:
   *
   * - "<" Priority ">"
   * - Version <space>
   * - Timestamp <space>
   * - Hostname <space>
   * - Application name <space>
   * - Process ID <space>
   * - Msg ID <space>
   * - Structured-data <optional>
   * - Message.
   *
   * At this time, most info is being ignored.
   */

  va_list argp;
  va_start(argp, msg_id);

  // CONSOLE("Debug: IN: Syslog: %d\n", msg_id);
  write_index = syslog_write_index();
  buffer = syslog_stack[write_index];

  space_left = SYSLOG_BUF_SIZE;
  total_written = 0;

  written = ets_snprintf(buffer, space_left,
        "<%d>%d %s %s %s %d %d",
    (msg->prival | LOG_LOCAL0),
	SYSLOG_VERSION,
	sntp_get_syslog_time(),
    syslog_ip_address,
    syslog_app_name,
    msg->proc_id,
    msg_id);
  space_left -= written;
  total_written += written;

#ifndef SYSLOG_STRUCTURED_IN_MSG
  if (msg->parms != NULL)
  {
    written = ets_snprintf(
        &buffer[total_written], space_left, " [%s ", syslog_app_name);
    space_left -=written;
    total_written += written;

    written = ets_vsnprintf(
        &buffer[total_written], space_left, msg->parms, argp);
    space_left -=written;
    total_written += written;

    written = ets_snprintf(&buffer[total_written], space_left, "]");
    space_left -=written;
    total_written += written;
  }
  else
#endif
  	  {
	  	  // NILLVALUE (lone '-')
	  	  written = ets_snprintf(&buffer[total_written], space_left, " -");
	      space_left -=written;
	      total_written += written;
  }

  if (msg->msg != NULL)
  {
    written = ets_snprintf(
        &buffer[total_written], space_left, " ");
    space_left -=written;
    total_written += written;

    written = ets_snprintf(
        &buffer[total_written], space_left, msg->msg);
    space_left -=written;
    total_written += written;
  }

#ifdef SYSLOG_STRUCTURED_IN_MSG
  /**
   * Append any structured data as part of the message.
   */
  if (msg->parms != NULL)
  {
    written = ets_snprintf(
        &buffer[total_written], space_left, " [%s ", syslog_app_name);
    space_left -=written;
    total_written += written;

    written = ets_vsnprintf(
        &buffer[total_written], space_left, msg->parms, argp);
    space_left -=written;
    total_written += written;

    written = ets_snprintf(&buffer[total_written], space_left, "]");
    space_left -=written;
    total_written += written;
  }
#endif

  syslog_length[write_index] = total_written;
  CONSOLE(buffer);
  // CONSOLE("Debug: MID: Syslog: %d\n", msg_id);
  syslog_sendto();
  // CONSOLE("Debug: OUT: Syslog: %d\n", msg_id);
  va_end(argp);
}
