#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/debug.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"
#include "inc/hw_gpio.h"
#include "driverlib/rom.h"

//Constants for Min and Max
//Values
#define PWM_FREQUENCY 55
#define MN_Bright 100 //10% 
#define MX_Bright 900 //90%

int main(void)
{
    volatile uint32_t ui32Load;
    volatile uint32_t ui32PWMClock;
    volatile uint16_t R,G,B;
    //ui8Adjust = 83;
    R = G = B = MN_Bright;

    //Setup clock 40MHz
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
    ROM_SysCtlPWMClockSet(SYSCTL_PWMDIV_64);  //40MHz/64 = 625KHz

    //PWM Configure, PORTF with Peripherals, PWM1
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    //PF1 set as PWM Out
    ROM_GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
    ROM_GPIOPinConfigure(GPIO_PF1_M1PWM5);//R
    ROM_GPIOPinConfigure(GPIO_PF2_M1PWM6);//G
    ROM_GPIOPinConfigure(GPIO_PF3_M1PWM7);//B

  

    //Count loaded based PWM ƒ
    ui32PWMClock = SysCtlClockGet() / 64;
    ui32Load = (ui32PWMClock / PWM_FREQUENCY) - 1;
    PWMGenConfigure(PWM1_BASE, PWM_GEN_2, PWM_GEN_MODE_DOWN);
    PWMGenConfigure(PWM1_BASE, PWM_GEN_3, PWM_GEN_MODE_DOWN);
    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_3, ui32Load);
    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_2, ui32Load);

    //3 LEDs PWM values to be loaded
    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, R * ui32Load / 1000);
    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, G * ui32Load / 1000);
    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, B * ui32Load / 1000);
    ROM_PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT|PWM_OUT_6_BIT|PWM_OUT_7_BIT, true);
    ROM_PWMGenEnable(PWM1_BASE, PWM_GEN_2);
    ROM_PWMGenEnable(PWM1_BASE, PWM_GEN_3);



    while(1)
    {


        for(; R <= MX_Bright; R++)
        {
            for(; G <= MX_Bright; G++)
            {
                for(; B <= MX_Bright; B++)
                {
                    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, B * ui32Load / 1000);
                    ROM_SysCtlDelay(12000);

                }//end
                ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, G * ui32Load / 1000);
                ROM_SysCtlDelay(12000);
            }
            ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, R * ui32Load / 1000);
            ROM_SysCtlDelay(12000);
        }

        for(; R >= MN_Bright; R--)
        {
            for(; B >= MN_Bright && R == MX_Bright; B--)
            {
                for(; G >= MN_Bright && B == MX_Bright &&  R == MX_Bright; G--)
                {
                    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, G * ui32Load / 1000);
                    ROM_SysCtlDelay(12000);
                }//end
                ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, B * ui32Load / 1000);
                ROM_SysCtlDelay(12000);
            }//end 2nd for
            ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, R * ui32Load / 1000);
            ROM_SysCtlDelay(12000);

        }//1st for


    }//end while(1)

}//Main