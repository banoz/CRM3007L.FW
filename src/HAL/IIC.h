#ifndef __IIC_SLAVE_H__
#define __IIC_SLAVE_H__

// Example Code
/*
#include "IIC.h"
#include "OB38R16T1.h"

void main(void)
{
    IIC_init_slave();

    while(1)
    {

    }//while(1)
}//end of main

*/

void IIC_init_slave(void);
void IIC_get_stats(unsigned int *rx_count, unsigned int *tx_count, unsigned int *nack_count);

#endif
