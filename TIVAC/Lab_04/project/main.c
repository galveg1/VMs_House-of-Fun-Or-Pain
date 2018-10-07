#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "inc/hw_gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom_map.h"

//Task 2 you're using 2 ints the sw2 int has to disable the timer0 int while it waits or the
//while from main while always take priority over the 1.5 int
void Mytimer1A_Delay(uint32_t ttime);


int main()
{
    uint32_t ui32Period;             //Variable for computation of period
    uint32_t ui32prescaler2 = 2;     //ADDED code varibale to create requested ƒ of 2Hz
    //int32_t ui32prescaler10 = 10;  //10Hz

    //MAIN CODE provided
    SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); //Enable PortF

    // Unlock GPIO F pin 0 (PF0) to use SW2 switch
    //Used in Task 02
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) = 0x1;


    //GPIO used in Taks 01 and 02
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0); //SW2 as input
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3); //Set 1,2,3 as output

    //Added code PIN4->Pg1340
    //Enable PU Resistro used in Task 02
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    //Interrupt enable used in Task 02
    GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_FALLING_EDGE);
    GPIOIntEnable(GPIO_PORTF_BASE, GPIO_PIN_0);
    IntEnable(INT_GPIOF);

    //Given Code used in Task 01 & 02
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);       //Select timer 0
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

    ui32Period = (SysCtlClockGet()/ui32prescaler2)/2;

    TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period-1);
    //Timer Interrupt Enable
    IntEnable(INT_TIMER0A);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    IntMasterEnable();
    //Timer Enable
    TimerEnable(TIMER0_BASE, TIMER_A);


    while(1)
    {

    }//END While

}//END MAIN

void Mytimer1A_Delay(uint32_t ttime1)
{
    // Dis-enable Timer 0 Interrupt
    TimerIntDisable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    //Following code configures Timer1
    //for use in Task 02
    int i = 0;

    SYSCTL_RCGCTIMER_R |= 2;
    TIMER1_CTL_R = 0;
    TIMER1_CFG_R = 0x04;
    TIMER1_TAMR_R = 0X02;
    TIMER1_TAILR_R = 64000-1;
    TIMER1_TAPR_R = 250-1;
    TIMER1_CTL_R |= 0X01;


    for(i=0;i < ttime1;i++)
        while((TIMER1_RIS_R & 0x1) == 0)
            TIMER1_ICR_R = 0x1;

}//end timer1A_delaySec

void Timer0IntHandler(void)
{

    //Clear Int Flag
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);


    if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_2))
    {
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0);
        SysCtlDelay(1250000);//ADDED code to create requested Duty Cycle
        //^ This should be timer1A_delaySec(int a)
    }
    else
    {
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 4);
        SysCtlDelay(5000000);//ADDED code to create requested Duty Cycle
    }
    //END if/else statement

}//end Timer0IntHandler

void Timer1IntHandler(void)
{
    //Clear GPIO Int
    GPIOIntClear(GPIO_PORTF_BASE, GPIO_INT_PIN_0);
    //LED ON
    //Blue LED PIN2
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 4);
    Mytimer1A_Delay(3529412);
    //LED OFF
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0);

    //Re-Enable(INT_TIMER0A);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

}//end Timer1Handler







