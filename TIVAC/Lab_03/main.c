//The use of the < > restricts the search path to only the specified path. Using the " "
//causes the search to start in the project directory. For includes like the two standard ones,
//you want to assure that you’re accessing the original, standard files … not one’s that may
//have been modified.

/*A good programming habit is to interleave your peripheral enable statements as follows:
Enable ADC
Enable GPIO
Config ADC
Config GPIO
This interleaving will prevent any possible system faults without incorporating software
delays.
*/

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"

//This changes starting color
uint8_t ui8PinData=2;   //Task 02 b. Red

int main(void)
{
    {
        SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
        GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
    }

    //Original code
    while(1)
    {
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, ui8PinData);
        SysCtlDelay(2000000);
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0x00);
        SysCtlDelay(2000000);
        if(ui8PinData==8) {ui8PinData=2;} else {ui8PinData=ui8PinData*2;}
    }



    /*Task 02 a
    while(1)
        {
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 |GPIO_PIN_2|GPIO_PIN_3, ui8PinData);
            SysCtlDelay(5666666);
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 |GPIO_PIN_2|GPIO_PIN_3, 0x00);
            SysCtlDelay(5666666);
            //First pass !8 -> 4*2 = 8 G, BG
            //Second pass is 8 -> 2 R, BGR
            if(ui8PinData==8) {ui8PinData=2;} else {ui8PinData=ui8PinData*2;}
        }
    */


    /*Task 02 b
    while(1)
    {
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 |GPIO_PIN_2|GPIO_PIN_3, ui8PinData); //Red
        SysCtlDelay(5666666);
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 |GPIO_PIN_2|GPIO_PIN_3, 0x08); //Green
        SysCtlDelay(5666666);
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 |GPIO_PIN_2|GPIO_PIN_3, 0x04); //Blue
        SysCtlDelay(5666666);
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 |GPIO_PIN_2|GPIO_PIN_3, 0x0A); //RG
        SysCtlDelay(5666666);
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 |GPIO_PIN_2|GPIO_PIN_3, 0x06); //RB
        SysCtlDelay(5666666);
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 |GPIO_PIN_2|GPIO_PIN_3, 0x0C); //GB
        SysCtlDelay(5666666);
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 |GPIO_PIN_2|GPIO_PIN_3, 0x0E); //RGB
        SysCtlDelay(5666666);
    }
    */

}//end main
