/**
 * Set up and start the Wifi.  Parameters are the WiFi network SSID and
 * password.
 */
void wifi_setup(char *ssid, char *password);

/**
 * WiFi event callback.
 */
void wifi_handle_event_cb(System_Event_t *evt);

