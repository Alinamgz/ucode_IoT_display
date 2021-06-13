/*
 * Copyright (c) 2015-2019, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== empty.c ========
 */

/* For usleep() */
#include <unistd.h>
#include <stdint.h>
#include <stddef.h>
#include <time.h>
#include <stdlib.h>

/* Driver Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/TRNG.h>
#include <ti/drivers/cryptoutils/cryptokey/CryptoKeyPlaintext.h>

//Display headers
#include <ti/display/Display.h>
#include <ti/display/DisplayUart.h>
#include <ti/display/DisplayExt.h>
#include <ti/display/AnsiColor.h>

/* Driver configuration */
#include "ti_drivers_config.h"


#define KEY_LENGTH_BYTES 16

/*
 *  ======== mainThread ========
 */
void *mainThread(void *arg0)
{
    /* 1 second delay */
    int_fast16_t rslt = 0;
    int i = 0;
    unsigned count = 0;
    unsigned led_value;
    int32_t delay = 1;

    TRNG_Handle trng_handle;

    Display_Handle lcd_handle;
    Display_Handle uart_handle;
    Display_Params display_params;

    char *led_ON = "On";
    char *led_OFF = "OFF";
    uint8_t entropy_buf[KEY_LENGTH_BYTES];
    CryptoKey entropy_key;


    /* Call driver init functions */
    GPIO_init();
    TRNG_init();
    Display_init();


    /* Configure the LED pin */
    GPIO_setConfig(CONFIG_GPIO_LED_0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    /* Turn on user LED */

//-------------------------------- DISPLAY ----------------------------------------------
    Display_Params_init(&display_params);
    display_params.lineClearMode = DISPLAY_CLEAR_BOTH;
    lcd_handle = Display_open(Display_Type_LCD, &display_params);
    uart_handle = Display_open(Display_Type_UART, &display_params);

    if (!lcd_handle || !uart_handle) {
        while(1);
    }

    if (lcd_handle) {
        Display_printf(lcd_handle, 5, 5, "Hello LCD!");
    }
    if(uart_handle) {
        Display_printf(uart_handle, 0, 0, "Hello UART!");
    }

    if (Display_getType(uart_handle) & Display_Type_ANSI) {
        led_ON = ANSI_COLOR(FG_GREEN, ATTR_BOLD) "On" ANSI_COLOR(ATTR_RESET);
        led_OFF = ANSI_COLOR(FG_RED, ATTR_UNDERLINE) "Off" ANSI_COLOR(ATTR_RESET);
    }

//---------------------------------- TRNG----------------------------------------------
//  True random number generator
    trng_handle = TRNG_open(0, NULL);
    if (!trng_handle) {
        while(1);
    }

    GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_ON);

    CryptoKeyPlaintext_initBlankKey(&entropy_key, entropy_buf, KEY_LENGTH_BYTES);
    TRNG_generateEntropy(trng_handle, &entropy_key);

    for (i = 0; i < KEY_LENGTH_BYTES; i++) {
        rslt += entropy_buf[i];
    }

    srand(rslt);
    TRNG_close(trng_handle);

//-------------------------------------------------------------------------------------
    sleep(3);
    Display_clear(lcd_handle);

    while (1) {
        led_value = GPIO_read(CONFIG_GPIO_LED_0);

//      Print to lcd
       Display_printf(lcd_handle, 0, 0, "LED: %s", (led_value == CONFIG_GPIO_LED_ON) ? "On!" : "Off!");

//      Print to uart
       Display_printf(uart_handle, 0, 0, "LED: %s", (led_value == CONFIG_GPIO_LED_ON) ? led_ON : led_OFF);

        if (Display_getType(uart_handle) & Display_Type_ANSI) {
           Display_printf(uart_handle,
                          DisplayUart_SCROLLING,
                          0,
                          "[ %d ] LED: %s",
                          count++,
                          (led_value == CONFIG_GPIO_LED_ON) ? led_ON : led_OFF);
        }

        delay = rand() % 10 + 1;
        sleep(delay);
        GPIO_toggle(CONFIG_GPIO_LED_0);
    }

}
