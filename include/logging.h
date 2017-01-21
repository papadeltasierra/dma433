/**
 * Logging function - apparently 'log' is a reserved name!
 */
void console(char *fmt, ...);

#define CONSOLE(FMT, args...) ets_uart_printf(FMT,  ##args); ets_uart_printf("\n")
