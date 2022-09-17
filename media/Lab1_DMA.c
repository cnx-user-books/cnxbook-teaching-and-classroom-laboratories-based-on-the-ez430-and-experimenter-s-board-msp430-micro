/**************************************************************************/
/*           Lab7a - Data Memory transfer triggered by software           */
/*                                                                        */
/*                                                                        */
/* MSP430 Teaching ROM                                                    */
/* Produced by: MSP430@UBI Group - www.msp430.ubi.pt                      */
/*                                                                        */
/* Exercise: Using the DMA controller included in the MSP-EXP430FG4618    */
/*           Development Tool analysed the data transfer between two      */
/*           regions of memory.                                           */
/*                                                                        */
/* Instructions:                                                          */
/*                                                                        */
/* This lab requires to configure:                                        */
/*  - DMA Source and Destination Addresses Registers                      */
/*  - DMA Size Address Register                                           */
/*  - DMA Control Registers                                               */
/*  - DMA Channel 0 Control Register                                      */
/*                                                                        */
/*                                                                        */
/*                                                                        */
/*                                        Copyright by Texas Instruments  */
/**************************************************************************/


#include "msp430xG46x.h"

int tab_1[32] = {2048, 2447, 2831, 3185, 3495, 3750, 3939, 4056,
                 4095, 4056, 3939, 3750, 3495, 3185, 2831, 2447,
                 2048, 1648, 1264,  910,  600,  345,  156,   39,
                    0,   39,  156,  345,  600,  910, 1264, 1648};
                             
int tab_2[32] = {      0, 0, 0, 0, 0, 0, 0, 0,
                       0, 0, 0, 0, 0, 0, 0, 0,
                       0, 0, 0, 0, 0, 0, 0, 0,
                       0, 0, 0, 0, 0, 0, 0, 0,};
                             

void main(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
  
  P2DIR |= 0x02;                            // P2.1  output
  P2OUT &= ~0x02;                           // P2.1  output low
  
  DMA0SA = (void (*)( )) &tab_1;       	    // Start block address
  DMA0DA = (void (*)( )) &tab_2;            // Destination block address
  
  DMA0SZ = 0x0020;                          // Block size
  
  DMA0CTL = DMADT_0 + DMASRCINCR_3 + DMADSTINCR_3 + DMAEN;
  // Single transfer,
  // DMA source and destination addresses increment,
  // Enable DMA0



  while(1)
  {
    P2OUT ^= 0x02;                          // Toggle P2.1
    DMA0CTL |= DMAREQ;                      // Trigger block transfer
  }
}

