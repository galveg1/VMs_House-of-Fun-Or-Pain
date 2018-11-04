#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/fpu.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom.h"


#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


//#define SERIES_LENGTH 100
#define SERIES_LENGTH 1000

float gSeriesData[SERIES_LENGTH];
int32_t i32DataCount = 0;
int main(void)
{
    float fRadians;             //float variable
    FPULazyStackingEnable();    //Enable lazy stacking
    FPUEnable();                //Enable FPU

    //CPU 50MHz
    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
    fRadians = ((2 * M_PI) / SERIES_LENGTH);

    // from 0 - 1000 angles of a circle
    while(i32DataCount < SERIES_LENGTH)
    {
        //gSeriesData[i32DataCount] = sinf(fRadians * i32DataCount);
        // store sine values onto array
        // 1.0*sin(2pi50t) + 0.5*cos(2pi200t)
        gSeriesData[i32DataCount] = (1.0*sin(fRadians*50*i32DataCount) + 0.5*cos(fRadians*200*i32DataCount));


        i32DataCount++;
    }
    while(1)
    {
    }//end while()
}//end main
