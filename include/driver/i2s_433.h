#ifndef _I2S_FREERTOS_H_
#define _I2S_FREERTOS_H_

/**
 * The interface is that:
 *
 * 1. Call i2sInit() to initialize the system.
 * 2. For each frame you want to send...
 *    2.1 Call i2sInitSignal()
 *    2.2 Call i2sWriteFrame()
 *    2.3 For each data bit call i2sWriteZero()/One()
 *    2.4 Call i2sTermSignal() to complete the data frame
 *    2.5 Call i2sSendSignal().
 * 3. Optionally wait for the 'completed' callback.
 */

/**
 * A definition for a callback that allows the user of this function to know
 * when the DMA transfer has completed.
 */
typedef void (*I2S_SEND_COMPLETE)(void);

void ICACHE_FLASH_ATTR i2sInit(I2S_SEND_COMPLETE);
void ICACHE_FLASH_ATTR i2sSendSignal(void);
void ICACHE_FLASH_ATTR i2sInitSignal();
void ICACHE_FLASH_ATTR i2sTermSignal();
void ICACHE_FLASH_ATTR i2sWriteFrame();
void ICACHE_FLASH_ATTR i2sWriteZero();
void ICACHE_FLASH_ATTR i2sWriteOne();

#endif
