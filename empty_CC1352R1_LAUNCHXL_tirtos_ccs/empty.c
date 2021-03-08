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

// #include <ti/drivers/I2C.h>
// #include <ti/drivers/SPI.h>
// #include <ti/drivers/UART.h>
// #include <ti/drivers/Watchdog.h>

/* Driver configuration */
#include "ti_drivers_config.h"


#define KEY_LENGTH_BYTES 16

/*
 *  ======== mainThread ========
 */
void *mainThread(void *arg0)
{
    /* 1 second delay */
    int32_t delay = 1;
    int i = 0;

    TRNG_Handle trng_handle;
    int_fast16_t rslt = 0;
    CryptoKey entropy_key;
    uint8_t entropy_buf[KEY_LENGTH_BYTES];


    /* Call driver init functions */
    GPIO_init();
    TRNG_init();
    // I2C_init();
    // SPI_init();
    // UART_init();
    // Watchdog_init();

//  True random number generator
    trng_handle = TRNG_open(0, NULL);
    CryptoKeyPlaintext_initBlankKey(&entropy_key, entropy_buf, KEY_LENGTH_BYTES);
    TRNG_generateEntropy(trng_handle, &entropy_key);

    /* Configure the LED pin */
    GPIO_setConfig(CONFIG_GPIO_LED_0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);

    /* Turn on user LED */
    GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_ON);

    for (i = 0; i < KEY_LENGTH_BYTES; i++) {
        rslt += entropy_buf[i];
    }

    srand(rslt);
    TRNG_close(trng_handle);

    while (1) {
        delay = rand() % 10 + 1;
        sleep(delay);
        GPIO_toggle(CONFIG_GPIO_LED_0);
    }
}
