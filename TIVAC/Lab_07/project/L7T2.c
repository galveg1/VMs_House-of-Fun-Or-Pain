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

// header file to manipulate characters
#include <ctype.h>




   int main(void) {

    //Clock SETUP
    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    //UART SETUP
    //enable UART0 & Port A peripherals
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //Tx and Rx pins as UART pins
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //LED SETUP as part of UART
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); //Enable GPIO port for LED
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3); //LEDs PF2

    //Initialize UART parameters: 115200 Baud Rate, 8 data bits, 1 stop bit
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    //Enable UART interrupt
    IntMasterEnable(); //Enable interrupts
    IntEnable(INT_UART0); //Enable the UART interrupt
    UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT); //enable RX and TX interrupts only

    // output "Enter Text: "
    UARTCharPut(UART0_BASE, 'E');
    UARTCharPut(UART0_BASE, 'n');
    UARTCharPut(UART0_BASE, 't');
    UARTCharPut(UART0_BASE, 'e');
    UARTCharPut(UART0_BASE, 'r');
    UARTCharPut(UART0_BASE, ' ');
    UARTCharPut(UART0_BASE, 'C');
    UARTCharPut(UART0_BASE, 'M');
    UARTCharPut(UART0_BASE, 'D');
    UARTCharPut(UART0_BASE, ':');
    UARTCharPut(UART0_BASE, ' ');

    // ADC SETUP
        ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
        ROM_ADCHardwareOversampleConfigure(ADC0_BASE, 32);

        ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);
        ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_TS);
        ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_TS);
        ADCSequenceStepConfigure(ADC0_BASE, 1, 2, ADC_CTL_TS);
        ADCSequenceStepConfigure(ADC0_BASE, 1, 3, ADC_CTL_TS|ADC_CTL_IE|ADC_CTL_END);
        ADCSequenceEnable(ADC0_BASE, 1);



    while (1) // let interrupt handler do the UART echo function
    {
        // if (UARTCharsAvail(UART0_BASE)) UARTCharPut(UART0_BASE, UARTCharGet(UART0_BASE));
    }

}//end Main


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