# DMA433

DMA433 is an example of using DMA to generate a stable, time-sensitive, output that can be fed to a 433MHz transmitter.  The example uses it to create a weather station sender signal that causes a weather station to display a slowly incrementing time.

This code is based heavily on the [ESP8266 MP3 Decoder](https://github.com/espressif/ESP8266_MP3_DECODER) but (mis)uses the DMA to create a carefully constructed set of digital outputs that have nothing to do with I2S or audio.  There are lots of comments (except where the original MP3 decoded uses 'black magic') so you should be able to see what I'm doing and then rewrite the code to do what you need it to do.

## DMA Is Complicated So Why Use DMA?
Have you tried sending a stable waveform by just pushing the GPIO outputs high or low?  It simply won't work!  Even on a board as simple as the ESP8266, the CPU is busy doing other stuff, like keeping the WiFi connection up, whilst your program is running and the result is that you might think that...

- set GPIO high
- wait 400us
- set GPIO low

..will give you a 400 microsecond pulse, but the CPU will see that wait and go 'whoopee, I can go and take care of some stuff' and like a teenager, it comes back and gives you control whebver it feels like it.

But DMA is hardware with one purpose, so do what you tell it.  So this example uses DMA to 'fire and forget' a stable waveform without any need for the CPU to be involved.

## UART receive or I2S Data?
Note that the I2S data line being used is also the normal UART (serial port) receive line oo you will need a switch/jumper or simliar to flip hardware function between loading firmware and running your application.

## How Would I Make This Better?
I would investigate whether I could do something similar with the HSPI lines.  These are designed to run at slower clock rates and do not share pins with the Uart.  If you try this and get it working, please share and also drop me a note.
