/**
 * Set up SNTP.  Parameters are the three SNTP servers to try and the timezone
 * the the SNTP functions are to assume.
 */
void sntp_setup(char *server0, char *server1, char *server2, uint32_t timezone);
char * ICACHE_FLASH_ATTR sntp_get_syslog_time(void);
