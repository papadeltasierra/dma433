/**
 * Application indices.
 */
#define SMSG_APP_SLOG           0
#define SMSG_APP_WDOG           1
#define SMSG_APP_SNTP           2
#define SMSG_APP_WIFI           3
#define SMSG_APP_HTTP           4
#define SMSG_APP_433            5
#define SMSG_APP_TEMP           6
#define SMSG_APP_LAST           7

#define SMSG_SLOG_INIT 0
#define SMSG_WDOG_INIT 1
#define SMSG_WDOG_HEART 2
#define SMSG_WDOG_OK 3
#define SMSG_WDOG_FAIL 4
#define SMSG_SNTP_INIT 5
#define SMSG_WIFI_INIT 6
#define SMSG_WIFI_CONNECT 7
#define SMSG_WIFI_IP 8
#define SMSG_WIFI_DISCONNECT 9
#define SMSG_WIFI_EVENT 10
#define SMSG_HTTP_SENT 11
#define SMSG_HTTP_OK 12
#define SMSG_HTTP_FAILED 13
#define SMSG_HTTP_DNS_FAILED 14
#define SMSG_TEMP_DATA 15
#define SMSG_TEMP_UNCHANGED 16
#define SMSG_TEMP_CHECKSUM      17
#define SMSG_INVALID            18

#ifdef DEFINE_VARS
const char smsg_app_name[] = CFG_APP_NAME;

const char *smsg_procs[] =
{
  "Syslog",
  "Watchdog",
  "SNTP",
  "WiFi",
  "HTTP",
  "Temp",
  "***"
};

const SYSLOG_MSG smsg_msgs[SMSG_INVALID + 1] = {

// syslog component has been initialized.
  {
      SMSG_APP_SLOG,
      LOG_INFO,
      NULL,
      "Syslog initialized successful",
  },
// watchdog component has been initialized.
  {
      SMSG_APP_WDOG,
      LOG_INFO,
      NULL,
      "Watchdog initialized.",
  },
// watchdog 'I am alive' heartbeat.  Also shows overruns as a way of
// monitoring the health of the system.
  {
      SMSG_APP_SLOG,
      LOG_INFO,
      "Overrun=\"%d\"",
      "Watchdog heartbeat.",
  },
// period watchdog check has passed; application is healthy.
  {
      SMSG_APP_WDOG,
      LOG_DEBUG,
      NULL,
      "Watchdog check passed.",
  },
// periodic watchdog check has failed; system will reboot which is the only
// way that we might get back to some sort of stability.
  {
      SMSG_APP_WDOG,
      LOG_EMERG,
      NULL,
      "Watchdog test failed - rebooting.",
  },
// SNTP component has initialized.
  {
      SMSG_APP_SNTP,
      LOG_INFO,
      NULL,
      "SNTP intialization.",
  },
// WiFi has initialized.
  {
      SMSG_APP_WIFI,
      LOG_INFO,
      NULL,
      "WiFi initialization successful.",
  },
// WiFi has managed to connect to the WiFi network.
  {
      SMSG_APP_WIFI,
      LOG_NOTICE,
      "SSID=\"%s\" Channel=\"%d\"",
      "WiFi connected to network.",
  },
// WiFi has successfully requested and obtained an IP address.
  {
      SMSG_APP_WIFI,
      LOG_NOTICE,
      "IP=\"" IPSTR "\", Mask=\"" IPSTR "\" Gway=\"" IPSTR "\"",
      "WiFi obtained IP address.",
  },
  // WiFi has disconnected from the WiFi network.
    {
        SMSG_APP_WIFI,
        LOG_CRIT,
        "SSID=\"%s\" Reason=\"%d\"",
        "WiFi has failed.",
    },
// WiFi event has occurred.
	  {
	      SMSG_APP_WIFI,
	      LOG_CRIT,
	      "Event=\"%d\"",
	      "WiFi event occurred.",
	  },
// An HTTP request has been sent.
  {
      SMSG_APP_HTTP,
      LOG_DEBUG,
      NULL,
      "HTTP request sent.",
  },
// A 'success' response has been received for the HTTP request.
  {
      SMSG_APP_HTTP,
      LOG_DEBUG,
      "Status-code=\"%d\"",
      "HTTP request successful.",
  },
// The server failed the HTTP request.
  {
      SMSG_APP_HTTP,
      LOG_CRIT,
      "Status-code=\"%d\"",
      "HTTP request failed.",
  },
// The HTTP request failed because the preceeding DNS request could not
// identify the host in the request.
  {
      SMSG_APP_HTTP,
      LOG_CRIT,
      "Hostname=\"%s\" Error-code=\"%d\""
      "DNS request failed.",
  },
// Data looking like a possible temperature has been received.
  {
      SMSG_APP_TEMP,
      LOG_DEBUG,
      "Data=\"%d\" Temp=\"%d.%ddegC\" Humidity=\"%d%%\"",
      "Temp data received.",
  },
// The received temperature data is valid but has not changed and it's not
// time to send a timed-refresh.
  {
      SMSG_APP_TEMP,
      LOG_DEBUG,
      NULL,
      "Temp data unchanged.",
  },
// The data checksum failed; odds are that this was not temperature data at
// all.
  {
      SMSG_APP_TEMP,
      LOG_DEBUG,
      "Received=\"%d\" Calculated=\"%d\"",
      "Temp checksum.",
  },
  {
      SMSG_APP_TEMP,
      LOG_CRIT,
      "",
      "** Invalid **"
  }
};
#endif
