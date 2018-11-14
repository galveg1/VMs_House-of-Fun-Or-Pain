/*
 * Copyright (c) 2015-2017, Texas Instruments Incorporated
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
 *  ======== hello.c ========
 */

/* XDC Module Headers */
/* TI-RTOS Header files */
#include <xdc/std.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/drivers/GPIO.h>
#include <ti/sysbios/knl/Clock.h>

/* Example/Board Header files */
#include "Board.h"
void myDelay(int count);
/* Could be anything, like computing primes */
#define FakeBlockingSlowWork() myDelay(12000000)
#define FakeBlockingFastWork() myDelay(2000000)


Task_Struct workTask;
Task_Struct urgentWorkTask;

/* Make sure we have nice 8-byte alignment on the stack to avoid wasting memory */

#pragma DATA_ALIGN(workTaskStack, 8)
#pragma DATA_ALIGN(urgentWorkTaskStack, 8)


#define STACKSIZE 1024

static uint8_t workTaskStack[STACKSIZE];
static uint8_t urgentWorkTaskStack[STACKSIZE];

void doUrgentWork(void)
{
    GPIO_write(Board_GPIO_LED1, Board_GPIO_LED_OFF);
    FakeBlockingFastWork(); /* Pretend to do something useful but time-consuming */
    GPIO_write(Board_GPIO_LED1, Board_GPIO_LED_ON);
}//end doUrgentWork
void doWork(void)
{
    GPIO_write(Board_GPIO_LED0, Board_GPIO_LED_OFF);
    FakeBlockingSlowWork(); /* Pretend to do something useful but time-consuming */
    GPIO_write(Board_GPIO_LED0, Board_GPIO_LED_ON);
}//edn doWork
Void workTaskFunc(UArg arg0, UArg arg1)
{
    while (1) {
        /* Do work */
        doWork();
        /* Wait a while, because doWork should be a periodic thing, not continuous.*/
       // myDelay(2400000);

        Task_sleep(Clock_tickPeriod/2);   /*1000µs/2=500ms*/
    }
}//end workTaskFunc
//ADDED
Void urgentWorkTaskFunc(UArg arg0, UArg arg1)
{

    while (1) {
        /* Do work */
        doUrgentWork();
        /* Wait a while, because doWork should be a periodic thing, not continuous.*/
       // myDelay(2400000);
       Task_sleep(Clock_tickPeriod/4);   /*1000µs/2=500ms*/
    }
}//end workTaskFunc

/** ======== main ========
 *
 */
int main(void)
{
    Board_initGeneral();
    GPIO_init();
    /* Set up the led task */
    Task_Params workTaskParams;
    Task_Params_init(&workTaskParams);
    workTaskParams.stackSize = STACKSIZE;
    workTaskParams.priority = 1;
    workTaskParams.stack = &workTaskStack;
    Task_construct(&workTask, workTaskFunc, &workTaskParams, NULL);

    Task_Params urgentWorkTaskParams;
    Task_Params_init(&urgentWorkTaskParams);
    urgentWorkTaskParams.stackSize = STACKSIZE;
    urgentWorkTaskParams.priority = 3;
    urgentWorkTaskParams.stack = &urgentWorkTaskStack;
    Task_construct(&urgentWorkTask, urgentWorkTaskFunc, &urgentWorkTaskParams, NULL);

    /* Start kernel. */
    BIOS_start();
    return (0);
}
/*
 * ======== myDelay ========
 * Assembly function to delay. Decrements the count until it is zero
 * The exact duration depends on the processor speed.
 */
__asm(" .sect \".text:myDelay\"\n"
        " .clink\n"
        " .thumbfunc myDelay\n"
        " .thumb\n"
        " .global myDelay\n"
        "myDelay:\n"
        " subs r0, #1\n"
        " bne.n myDelay\n"
        " bx lr\n");
