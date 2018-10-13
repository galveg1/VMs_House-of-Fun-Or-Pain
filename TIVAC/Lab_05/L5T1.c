#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"
#include "driverlib/gpio.h"
//Internal TI product name for the device family
//Allows access to the proper libraries in ROM
#define TARGET_IS_BLIZZARD_RB1
#include "driverlib/rom.h"

int main(void)
{
    //Using Sequencer 1 FIFO 4 deep
    uint32_t ui32ADC0Value[4];
    //These are declared volatile so each cannot be optimized by the compiler
    //and will be available to the expression or local window at run time

    volatile uint32_t ui32TempAvg;
    volatile uint32_t ui32TempValueC;
    volatile uint32_t ui32TempValueF;

    //Clock set 40MHz
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
    //Enable ADC0
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    //HW over sample configure, 2-64 number of samples possible
    ROM_ADCHardwareOversampleConfigure(ADC0_BASE, 64);

    //ADC Sequencer configure
    ROM_ADCSequenceConfigure(ADC0_BASE, 2, ADC_TRIGGER_PROCESSOR, 0);
    ROM_ADCSequenceStepConfigure(ADC0_BASE, 2, 0, ADC_CTL_TS);
    ROM_ADCSequenceStepConfigure(ADC0_BASE, 2, 1, ADC_CTL_TS);
    ROM_ADCSequenceStepConfigure(ADC0_BASE, 2, 2, ADC_CTL_TS);
    //(ADC_CTL_TS) <-sample the TSensor, (ADC_CTL_IE) <- set int flag when sample is done
    //(ADC_CTL_END) <- last conversion on sequencer 1 Pg 587
    ROM_ADCSequenceStepConfigure(ADC0_BASE,2,3,ADC_CTL_TS|ADC_CTL_IE|ADC_CTL_END);
    ROM_ADCSequenceEnable(ADC0_BASE, 2);

    //LED setup
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);

    while(1)
    {
        //Clear Flag
        ROM_ADCIntClear(ADC0_BASE, 2);
        //SW ADC trigger, note not only way to trigger
        ROM_ADCProcessorTrigger(ADC0_BASE, 2);

        while(!ROM_ADCIntStatus(ADC0_BASE, 2, false))
        {
        }

        //Copy output FIFO to buffer Mem
        ROM_ADCSequenceDataGet(ADC0_BASE, 2, ui32ADC0Value);
        //                                                                                  Here 2 v
        ui32TempAvg = (ui32ADC0Value[0] + ui32ADC0Value[1] + ui32ADC0Value[2] + ui32ADC0Value[3] + 2)/4;
        //TEMP = 147.5 – ((75 * (VREFP – VREFN) * ADCVALUE) / 4096)
        ui32TempValueC = (1475 - ((2475 * ui32TempAvg)) / 4096)/10;//<- /10 give correct ans for VREFP-VREFN
        //F°Conversion
        ui32TempValueF = ((ui32TempValueC * 9) + 160) / 5;

        //Red if(t>72)
            if(ui32TempValueF > 69)
                ROM_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 4);
            else
                ROM_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0);

    }//end while(1)


}//END MAIN

//NOTES:
//.text length was          3a4 932
//.text length second time  2ac 684
//My values are possibly more because of added comments or difference between CCS versions from PDF
//Red 8, Green 2, Blue 4
