//*****************************************************************************
//
// spi_master.c - Example demonstrating how to configure SSI0 in SPI master
// mode.
//
// Copyright (c) 2010-2011 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
//
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
//
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
//
// This is part of revision 8034 of the Stellaris Firmware Development Package.
//
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"
#include "driverlib/rom.h"

// header files that enable UART interrupts
#include "inc/hw_ints.h"
#include "driverlib/uart.h"



#include "driverlib/uart.h"
#include "driverlib/timer.h"

#include "inc/hw_ssi.h"
#include "driverlib/ssi.h"

#include "utils/uartstdio.h"
#include <ctype.h>
#include "inc/hw_timer.h"

#include "Nokia5110.h"

#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif

//*****************************************************************************
//
//! \addtogroup ssi_examples_list
//! <h1>SPI Master (spi_master)</h1>
//!
//! This example shows how to configure the SSI0 as SPI Master.  The code will
//! send three characters on the master Tx then polls the receive FIFO until
//! 3 characters are received on the master Rx.
//!
//! This example uses the following peripherals and I/O signals.  You must
//! review these and change as needed for your own board:
//! - SSI0 peripheral
//! - GPIO Port A peripheral (for SSI0 pins)
//! - SSI0CLK - PA2
//! - SSI0Fss - PA3
//! - SSI0Rx  - PA4
//! - SSI0Tx  - PA5
//!
//! The following UART signals are configured only for displaying console
//! messages for this example.  These are not required for operation of SSI0.
//! - UART0 peripheral
//! - GPIO Port A peripheral (for UART0 pins)
//! - UART0RX - PA0
//! - UART0TX - PA1
//!
//! This example uses the following interrupt handlers.  To use this example
//! in your own application you must add these interrupt handlers to your
//! vector table.
//! - None.
//!
//
//*****************************************************************************

#define WREN    0x06
#define EWSR    0x50
#define WRSR    0x01
#define BYTEPROM    0x02
#define CHIPERASE   0x60
#define READ    0x03
#define HREAD   0x0B
#define RDSR    0x05

#define NUM_SSI_DATA 5
#define NUM_SSI_Tmp  4



void   InitConsole(void)
{
    //SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTStdioConfig(0, 115200, 16000000);
    //LED SETUP as part of UART
    //SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); //Enable GPIO port for LED
    //GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
    //IntMasterEnable(); //Enable interrupts
    //IntEnable(INT_UART0); //Enable the UART interrupt
    //UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);


}//end InitConsole

void   InitADC(void)
{
    // ADC SETUP
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    ADCHardwareOversampleConfigure(ADC0_BASE, 32);

    ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_TS);
    ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_TS);
    ADCSequenceStepConfigure(ADC0_BASE, 1, 2, ADC_CTL_TS);
    ADCSequenceStepConfigure(ADC0_BASE, 1, 3, ADC_CTL_TS|ADC_CTL_IE|ADC_CTL_END);
    ADCSequenceEnable(ADC0_BASE, 1);
}//end InitADC



//*****************************************************************************
//
// Configure SSI1 in master Freescale (SPI) mode.
//
//*****************************************************************************
int  main(void)
{
    uint32_t ulDataTx[NUM_SSI_DATA];
    uint32_t ulDataRx[NUM_SSI_DATA];
    uint32_t ulindex;

    uint8_t  ASCIIChar;
    uint32_t ui32ADC0Value[4];
    volatile uint32_t ui32TempAvg;
    volatile uint32_t ui32TempValueC;
    volatile uint32_t ui32TempValueF;
    uint32_t i = 0;
    uint8_t T_str[4];

    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    //SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);

    InitConsole();
    InitADC();
    UARTprintf("SSI ->\n");
    UARTprintf("Mode: SPI\n");
    UARTprintf("Data: 8-bit\n\n");
    //ulDataRx[0]=1;
    //
    // The SSI0 peripheral and port D must be enabled for use.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    // Configure the pin muxing for SSI1 functions on port D.
    GPIOPinConfigure(GPIO_PA2_SSI0CLK);
    GPIOPinConfigure(GPIO_PA3_SSI0FSS);
    GPIOPinConfigure(GPIO_PA4_SSI0RX);
    GPIOPinConfigure(GPIO_PA5_SSI0TX);
    //GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_1);//sherry

    GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_5 | GPIO_PIN_4 | GPIO_PIN_3 | GPIO_PIN_2);    //sherry    GPIO_PIN_1 |
    //GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_1, GPIO_PIN_1);    //

    //
    // Configure and enable the SSI port for SPI master mode.  Use SSI1,
    // system clock supply, idle clock level low and active low clock in
    // freescale SPI mode, master mode, 1MHz SSI frequency, and 8-bit data.
    // For SPI mode, you can set the polarity of the SSI clock when the SSI
    // unit is idle.  You can also configure what clock edge you want to
    // capture data on.  Please reference the datasheet for more information on
    // the different SPI modes.
    //
    //SSIClockSourceSet(SSI0_BASE,SSI_CLOCK_SYSTEM);
    SSIConfigSetExpClk(SSI0_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0,
                       SSI_MODE_MASTER, 1000000, 8);

    SSIEnable(SSI0_BASE);

    // Timer 1 Interrupt setup
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
    TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);
    uint32_t ui32Period = SysCtlClockGet() / 2;

    // clock division by two to get 0.5 second delay
    TimerLoadSet(TIMER1_BASE, TIMER_A, ui32Period-1);
    IntEnable(INT_TIMER1A);
    TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    TimerEnable(TIMER1_BASE, TIMER_A);


    //Initialize LCD Display 
    SysTick_Init();
    startSSI0();
    initialize_screen(BACKLIGHT_ON,SSI0);
    
    set_buttons_up_down();
    unsigned char menu_elements[12][25];
    menu_elements[0][0]='1';
    menu_elements[0][1]=0x00;
    menu_elements[1][0]='2';
    menu_elements[1][1]=0x00;
    menu_elements[2][0]='3';
    menu_elements[2][1]=0x00;
    menu_elements[3][0]='4';
    menu_elements[3][1]=0x00;
    menu_elements[4][0]='5';
    menu_elements[4][1]=0x00;
    menu_elements[5][0]='6';
    menu_elements[5][1]=0x00;
    menu_elements[6][0]='7';
    menu_elements[6][1]=0x00;
    menu_elements[7][0]='8';
    menu_elements[7][1]=0x00;
    menu_elements[8][0]='9';
    menu_elements[8][1]=0x00;
    menu_elements[9][0]='1';
    menu_elements[9][1]='0';
    menu_elements[9][2]=0x00;
    menu_elements[10][0]='1';
    menu_elements[10][1]='1';
    menu_elements[10][2]=0x00;
    menu_elements[11][0]='1';
    menu_elements[11][1]='2';
    menu_elements[11][2]=0x00;
    set_menu(menu_elements);
    clear_screen(SSI0);
    screen_write("CPE\n403!",ALIGN_CENTRE_CENTRE,SSI0);
    SysTick_Wait50ms(50);    
    
    
    //
    // Read any residual data from the SSI port.  This makes sure the receive
    // FIFOs are empty, so we don't read any unwanted junk.  This is done here
    // because the SPI SSI mode is full-duplex, which allows you to send and
    // receive at the same time.  The SSIDataGetNonBlocking function returns
    // "true" when data was returned, and "false" when no data was returned.
    // The "non-blocking" function checks if there is any data in the receive
    // FIFO and does not "hang" if there isn't.
    //
    while(1)
    {

    }//end while(1)


}//end main(void)

void Timer1AIntHandler(void)
{
     //Clear Timer Interrupt
    TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);

    //moved everything inside while loop from main
    uint32_t ui32ADC0Value[4];
    volatile uint32_t ui32TempAvg;
    volatile uint32_t ui32TempValueC;
    volatile uint32_t ui32TempValueF;

    uint32_t ulDataTx[NUM_SSI_DATA];
    //uint32_t ulDataRx[NUM_SSI_DATA];
    char ulDataRx[NUM_SSI_DATA];
    uint8_t ulindex;
    uint8_t i =0; 
    uint8_t T_str[5];

    int j;
    int max=11,current_pos=0;

   


    ADCIntClear(ADC0_BASE, 1);
    ADCProcessorTrigger(ADC0_BASE, 1);

    while(!ADCIntStatus(ADC0_BASE, 1, false))
    {
    }

    //Sets values into temp variables 
    ADCSequenceDataGet(ADC0_BASE, 1, ui32ADC0Value);
    ui32TempAvg = (ui32ADC0Value[0] + ui32ADC0Value[1] + ui32ADC0Value[2] + ui32ADC0Value[3] + 2)/4;
    ui32TempValueC = (1475 - ((2475 * ui32TempAvg)) / 4096)/10;
    ui32TempValueF = ((ui32TempValueC * 9) + 160) / 5;

    //Convert to string (ui32TempValueF != 0)
    i = 0;
    while(i != 5){
        ulDataTx[i++] = (ui32TempValueF%10) + '0';
        if(i == 2) {ulDataTx[i++] = '.';}
        ui32TempValueF /= 10;
    }


    //ulindex = i;
    UARTprintf("Sending Temp: ");

     while(SSIDataGetNonBlocking(SSI0_BASE, &ulDataRx[0]));

    for(ulindex = 0; ulindex < NUM_SSI_DATA; ulindex++) //for 1
    {
        // Display the data that SSI is transferring.
        UARTprintf("%c", ulDataTx[ulindex]);
        // Send the data using the "blocking" put function. This function
        // will wait until there is room in the send FIFO before returning.
        // This allows you to assure that all the data you send makes it into
        // the send FIFO.
        //ulDataRx[ulindex] &= 0x00FF;
        SSIDataPut(SSI0_BASE, ulDataTx[ulindex]);
    }//end for 1
    UARTprintf("\n\n");    

    // Wait until SSI0 is done transferring all the data in the transmit FIFO.
    while(SSIBusy(SSI0_BASE))
    {
    }

    //SysCtlDelay(10);
    // Display indication that the SSI is receiving data.
    UARTprintf("Temp Received: ");
    // Receive 3 bytes of data.

    for(ulindex = 0; ulindex < NUM_SSI_DATA; ulindex++)
    {
        // Receive the data using the "blocking" Get function. This function
        // will wait until there is data in the receive FIFO before returning.
        SSIDataGet(SSI0_BASE, &ulDataRx[ulindex]);
        // Since we are using 8-bit data, mask off the MSB.
        ulDataRx[ulindex] &= 0x0FFF;
        // Display the data that SSI0 received.
        UARTprintf("%c", ulDataRx[ulindex]);
        clear_screen(SSI0);
        screen_write("\nThe Temp is:\n",ALIGN_CENTRE_TOP,SSI0);
        screen_write(ulDataRx, ALIGN_CENTRE_CENTRE, SSI0);
        screen_write("F°", ALIGN_RIGHT_CENTRE, SSI0);
        
    }
     UARTprintf("\n");
    //screen_write("\n", ALIGN_CENTRE_CENTRE, SSI0);
/*
    clear_screen(SSI0);
        char data=GPIO_PORTA_DATA_R&0x03;
        if((data==0x01) && current_pos<max)
        {
            current_pos++;
        }
        else
        {
            if((data==0x02) && current_pos>0)
            {
                current_pos--;
            }
        }
        show_menu(current_pos,SSI0);
        SysTick_Wait50ms(20);
*/
    //clear_screen(SSI0);
    //screen_write("\nThe Temp is:\n",ALIGN_CENTRE_CENTRE,SSI0);
    //screen_write("CPE \nleaving\n TimerInt!",ALIGN_LEFT_CENTRE,SSI0);
    //screen_write(ulDataRx, ALIGN_CENTRE_CENTRE, SSI0);
    //clear_screen(SSI0);
    SysTick_Wait50ms(30);

}//end Timer1AintHandler


// The delay parameter is in units of the 16 MHz core clock.
void SysTick_Wait(unsigned long delay){
  NVIC_ST_RELOAD_R = delay-1;  // number of counts to wait
  NVIC_ST_CURRENT_R = 0;       // any value written to CURRENT clears
  while((NVIC_ST_CTRL_R&0x00010000)==0){ // wait for count flag
  }
}


void SysTick_Wait50ms(unsigned long delay){
  unsigned long i;
  for(i=0; i<delay; i++){
    SysTick_Wait(800000);  // wait 50ms
  }
}


void SysTick_Init(void){
  NVIC_ST_CTRL_R = 0;               // disable SysTick during setup
  NVIC_ST_CTRL_R = 0x00000005;      // enable SysTick with core clock
}

/*
void UART_Int_Handler(void)
{
    uint32_t ui32UARTStat;

    ui32UARTStat = UARTIntStatus(UART0_BASE, true);

    UARTIntClear(UART0_BASE, ui32UARTStat); //Int Clear

    uint8_t  ASCIIChar;
    uint32_t ui32ADC0Value[4];
    volatile uint32_t ui32TempAvg;
    volatile uint32_t ui32TempValueC;
    volatile uint32_t ui32TempValueF;
    uint32_t i = 0;
    uint8_t T_str[10];

    while(UARTCharsAvail(UART0_BASE))
    {
        ASCIIChar = UARTCharGetNonBlocking(UART0_BASE);


        UARTprintf("Please Type G&g to turn green on&off\n");
        UARTprintf("B&b to Turn blue on&off\n");
        UARTprintf("R&r to turn red on&off\n\n");
        UARTprintf("T | t to turn see the Temp\n\n");


        if(ASCIIChar == 'G') GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3);

        if(ASCIIChar == 'B') GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);

        if(ASCIIChar == 'R') GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_PIN_1);

        if(ASCIIChar == 'g') GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, ~GPIO_PIN_3);

        if(ASCIIChar == 'b') GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, ~GPIO_PIN_2);

        if(ASCIIChar == 'r') GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, ~GPIO_PIN_1);

        UARTCharPut(UART0_BASE, ' ');


        if(ASCIIChar == 'T' | ASCIIChar == 't')
        {
            UARTCharPut(UART0_BASE, ' ');
            ADCIntClear(ADC0_BASE, 1);
            ADCProcessorTrigger(ADC0_BASE, 1);
            while(!ADCIntStatus(ADC0_BASE, 1, false));
            ADCSequenceDataGet(ADC0_BASE, 1, ui32ADC0Value);
            ui32TempAvg = (ui32ADC0Value[0] + ui32ADC0Value[1] + ui32ADC0Value[2] + ui32ADC0Value[3] + 2)/4;
            ui32TempValueC = (1475 - ((2475 * ui32TempAvg)) / 4096) / 10;
            ui32TempValueF = ((ui32TempValueC * 9) + 160) / 5;
            // Output temperature via UART: "<temp> "
            i = 0;
            while(ui32TempValueF != 0){
                T_str[i++] = (ui32TempValueF%10) + '0';
                ui32TempValueF /= 10;
            }

            for(; i > 0; i--)
                UARTCharPut(UART0_BASE, T_str[i-1]);

            UARTCharPut(UART0_BASE, ' ');


        }//end if
        UARTCharPutNonBlocking(UART0_BASE, ASCIIChar);//Feed Back


    }//end while

}//End UART_Handler
*/