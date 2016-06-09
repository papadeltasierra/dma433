/*
 *  This function demostrates using the DMA based 433 MHz transmitter.
 */

#include "ets_sys.h"
#include "osapi.h"
#include "os_type.h"
#include "user_interface.h"
#include "mem.h"
#include "driver/gpio16.h"
#include "driver/uart.h"

#define CONSOLE(FMT, args...) ets_uart_printf(FMT,  ##args); ets_uart_printf("\n")

/**
 * *** YOU WILL WANT TO CHANGE THIS ***
 *
 * This defines the format of the 32-bit data that is sent to the
 * weather sensor. The format you choose will be specific to your
 * sensor.
 */
#define CFG_433_1_VALUE		400
#define CFG_433_4_VALUE		(4 * CFG_433_1_VALUE)
#define CFG_433_17_VALUE	(17 * CFG_433_1_VALUE)

#define CFG_433_SENDER		0x94000000
#define CFG_433_CHANNEL     0x00000000
#define CFG_433_BATTERY_OK  0x00800000
#define CFG_433_BEEP        0x00400000
#define CFG_433_00200000    0x00200000
#define CFG_433_00100000    0x00100000
#define CFG_433_MANUAL      0x00000000

#define CFG_TEMP_SHIFT      8
#define CFG_TEMP_MASK       0x000FFF00

os_timer_t send_timer = { 0 };

/**
 * Forward prototypes.
 */
static void ICACHE_FLASH_ATTR send_433_temp(sint32 temperature);

/**
 * *** YOUR WEATHER STATION WILL HAVE ITS OWN CHECKSUM ALGORITHM ***
 *
 * Generate the 8-bit checksum that forms the end of the 32-bit
 * data to send to the weather station receiver.
 */
void add_433_checksum(uint32 *data_433)
{
	uint32 check_sum = 0;
	uint32 data = (*data_433);
	int ii;

	for (ii = 0; ii < 40; ii++)
	{
		if (check_sum & 0x80)
		{
			check_sum ^= 0x18;
		}
		check_sum = ((check_sum & 0x80) >> 7) | (check_sum << 1);
		check_sum ^= ((data & 0x80000000) >> 31);
		data <<= 1;
	}
	(*data_433) |= (check_sum & 0x000000FF);
	return;
}

/**
 * Build the 32-bit value that is used to transmit the temperature to
 * the base station and then request that it be sent three times.
 */
static void send_433_temp(sint32 temperature)
{
	uint32 data_433;
	int ii;
	int jj;
	uint32 mask;

	/**
	 * First build the 32-bit value.
	 */
	CONSOLE("Temp: %d", temperature);
	data_433 = 0;
	data_433 |= CFG_433_SENDER;
	data_433 |= CFG_433_CHANNEL;
	data_433 |= CFG_433_BATTERY_OK;
	// data_433 |= CFG_433_BEEP;
	// data_433 |= CFG_433_00200000;
	// data_433 |= CFG_433_00100000;
	data_433 |= CFG_433_MANUAL;
	data_433 |= ((temperature << CFG_TEMP_SHIFT) & CFG_TEMP_MASK);

	add_433_checksum(&data_433);
	CONSOLE("Data: %u", data_433);

	/**
	 * Now build the frame and send this using the new DMA/I2S infrastructure.
	 */
	CONSOLE("InitSignal...");
	i2sInitSignal();
	CONSOLE("BuildSignal...");
	mask = 0x80000000;
	i2sWriteFrame();
	for (jj = 0; jj < 32; jj++)
	{
		if (mask & data_433)
		{
			i2sWriteOne();
		}
		else
		{
			i2sWriteZero();
		}
		mask >>= 1;
	}
	CONSOLE("TermSignal...");
	i2sTermSignal();
	CONSOLE("SendSignal...");
	i2sSendSignal();
	CONSOLE("DMA is sending...");
}

/**
 * Timer driven loop that drives the whole process.  The timer keeps
 * incrementing the temperature by +/- 0.1degC between -12.7 and +12.8.
 *
 * Note that the encoding multiples up temperatures by 10 so 12.3 is
 * represented by 123.
 */
static void send_loop(void *arg)
{
	static sint32 temp = -128;
	static sint32 temp_inc = 1;

	CONSOLE("Send temp: %d", temp);
	send_433_temp(temp);
	temp = temp + temp_inc;
	if ((temp < -127 || temp > 128))
	{
		/**
		 * Reverse direction.
		 */
		temp_inc = -temp_inc;
		temp = temp + 2 * temp_inc;
	}
}

static void send_callback(void *arg)
{
	/**
	 * Sending has finished; log the time it took.
	 */
	uint32 send_time = slc_dbg_get_send_time();
    CONSOLE("Frame send in %dus", send_time);
}

void user_init(void)
{
	/**
	 * First set up the serial port so that we can see output from
	 * logging etc.
	 */
	UARTInit(BIT_RATE_76800);
	CONSOLE("SDK version:%s", system_get_sdk_version());
	CONSOLE("433MHz receiver active");

	CONSOLE("Initialize I2S...");
	i2sInit(send_callback);

    /**
     * Finally launch the timer that sends data every 10 seconds.
     */
	CONSOLE("Start send timer");
    os_timer_disarm(&send_timer);
    os_timer_setfn(&send_timer, (os_timer_func_t *)send_loop, NULL);
    os_timer_arm(&send_timer, 10*1000, TRUE);
}
