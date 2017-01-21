/**
 * The following from the Linux syslog.h file.
 */
#define LOG_EMERG 0 /* system is unusable */
#define LOG_ALERT 1 /* action must be taken immediately */
#define LOG_CRIT  2 /* critical conditions */
#define LOG_ERR   3 /* error conditions */
#define LOG_WARNING 4 /* warning conditions */
#define LOG_NOTICE  5 /* normal but significant condition */
#define LOG_INFO  6 /* informational */
#define LOG_DEBUG 7 /* debug-level messages */

#define LOG_PRIMASK 0x07  /* mask to extract priority part (internal) */
        /* extract priority */
#define LOG_PRI(p)  ((p) & LOG_PRIMASK)
#define LOG_MAKEPRI(fac, pri) (((fac) << 3) | (pri))

// Structure defining the syslog messages to be created.
typedef struct syslog_msg
{
  int proc_id;
  uint8 prival;
  char *parms;
  char *msg;
} SYSLOG_MSG;

#ifdef SYSLOG_NAMES
#define INTERNAL_NOPRI  0x10  /* the "no priority" priority */
        /* mark "facility" */
#define INTERNAL_MARK LOG_MAKEPRI(LOG_NFACILITIES, 0)
typedef struct _code {
  char  *c_name;
  int c_val;
} CODE;

CODE prioritynames[] = {
  "alert",  LOG_ALERT,
  "crit",   LOG_CRIT,
  "debug",  LOG_DEBUG,
  "emerg",  LOG_EMERG,
  "err",    LOG_ERR,
  "error",  LOG_ERR,    /* DEPRECATED */
  "info",   LOG_INFO,
  "none",   INTERNAL_NOPRI,   /* INTERNAL */
  "notice", LOG_NOTICE,
  "panic",  LOG_EMERG,    /* DEPRECATED */
  "warn",   LOG_WARNING,    /* DEPRECATED */
  "warning",  LOG_WARNING,
  NULL,   -1,
};
#endif

/* facility codes */
#define LOG_KERN  (0<<3)  /* kernel messages */
#define LOG_USER  (1<<3)  /* random user-level messages */
#define LOG_MAIL  (2<<3)  /* mail system */
#define LOG_DAEMON  (3<<3)  /* system daemons */
#define LOG_AUTH  (4<<3)  /* security/authorization messages */
#define LOG_SYSLOG  (5<<3)  /* messages generated internally by syslogd */
#define LOG_LPR   (6<<3)  /* line printer subsystem */
#define LOG_NEWS  (7<<3)  /* network news subsystem */
#define LOG_UUCP  (8<<3)  /* UUCP subsystem */
#define LOG_CRON  (9<<3)  /* clock daemon */
#define LOG_AUTHPRIV  (10<<3) /* security/authorization messages (private) */

  /* other codes through 15 reserved for system use */
#define LOG_LOCAL0  (16<<3) /* reserved for local use */
#define LOG_LOCAL1  (17<<3) /* reserved for local use */
#define LOG_LOCAL2  (18<<3) /* reserved for local use */
#define LOG_LOCAL3  (19<<3) /* reserved for local use */
#define LOG_LOCAL4  (20<<3) /* reserved for local use */
#define LOG_LOCAL5  (21<<3) /* reserved for local use */
#define LOG_LOCAL6  (22<<3) /* reserved for local use */
#define LOG_LOCAL7  (23<<3) /* reserved for local use */

#define LOG_NFACILITIES 24  /* current number of facilities */
#define LOG_FACMASK 0x03f8  /* mask to extract facility part */
        /* facility of pri */
#define LOG_FAC(p)  (((p) & LOG_FACMASK) >> 3)

void syslog_setup(char *hostname, int port, const char* app_name, const char **procs, const SYSLOG_MSG *msgs);
void syslog(int, ...);
