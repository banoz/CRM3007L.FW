#ifndef __WDTI_H__
#define __WDTI_H__

// Example Code
/*
#include "WDT.h"
#include "OB38R16T1.h"

void main(void)             //Main Function Start
{
    WDTI_Init();            //Call WDT Initial Subroutine
    while(1);
}
*/

void WDTI_Init(void);
void WDTI_CountClear(void);
void WDTI_Disable(void);

#endif
