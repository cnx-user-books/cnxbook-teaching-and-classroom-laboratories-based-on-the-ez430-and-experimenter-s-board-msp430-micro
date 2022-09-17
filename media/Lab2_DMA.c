/**************************************************************************/
/*                 Lab7b - Sinusoidal signal generator                    */
/*                                                                        */
/*                                                                        */
/* MSP430 Teaching ROM                                                    */
/* Produced by: MSP430@UBI Group - www.msp430.ubi.pt                      */
/*                                                                        */
/* Exercise: Using the DMA controller included in the MSP-EXP430FG4618    */
/*           Development Tool transfer a sinusoidal wave to the DAC12_0's */
/*           output. Timer_A operates in upmode with TACCR1 loading       */
/*           DAC12_0 on rising edge and DAC12_0IFG triggering next DMA1   */
/*           transfer. DAC12_0 uses internal 1.5V reference.              */
/*                                                                        */
/* Instructions:                                                          */
/*                                                                        */
/* This lab requires to configure:                                        */
/*  - DMA source and destination addresses registers                      */
/*  - DMA size address register                                           */
/*  - DMA control registers                                               */
/*  - DMA channel 0 control register                                      */
/*  - DAC12 control register                                              */
/*  - Timer_A control register                                            */
/*                                                                        */
/* The verification of this lab is done observing with an oscilloscope    */
/* the output of the DAC12 Channel 0 available on header 8 pin 6.         */
/*                                                                        */
/*                                        Copyright by Texas Instruments  */
/**************************************************************************/

#include "msp430xG46x.h"

//--------------------------------------------------------------------------
// 12-bit Sine Lookup table with 32 steps
//--------------------------------------------------------------------------

 int Sin_tab[32] = {      2048, 2447, 2831, 3185, 3495, 3750, 3939, 4056,
                          4095, 4056, 3939, 3750, 3495, 3185, 2831, 2447,
                          2048, 1648, 1264,  910,  600,  345,  156,   39,
                             0,   39,  156,  345,  600,  910, 1264, 1648};
void main(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 // Stop watchdog timer
  ADC12CTL0 = REFON;                        // Internal reference
 
 
//--------------------------------------------------------------------------
// DMA controller configuration
//-------------------------------------------------------------------------- 
  DMA0SA = (void (*)( ))&Sin_tab;           // Source block address
  DMA0DA = (void (*)( ))&DAC12_0DAT;        // Destination single address
  
  DMA0SZ = 0x020;                           // Block size
  
  DMACTL0 = DMA0TSEL_5;                     // DAC12IFG trigger
  DMA0CTL = DMADT_4 + DMASRCINCR_3 + DMAEN;
                                            // Repeated single transfer,
                                            // DMA source address increment,
                                            // since DMASRCBYTE = 0, the source address
                                            // increments by two (word-word)
  
//--------------------------------------------------------------------------
// DAC12 configuration
//--------------------------------------------------------------------------
  DAC12_0CTL = DAC12LSEL_2 + DAC12IR + DAC12AMP_5 + DAC12IFG + DAC12ENC;
                                            // Rising edge of Timer_A.OUT1 (TA1),
                                            // DAC12 full-scale output: 1x reference voltage,
                                            // Input and output buffers: Medium freq./current,
                                            // Enable DAC12
 
//--------------------------------------------------------------------------
// Timer_A configuration
//--------------------------------------------------------------------------
  
  TACCTL1 = OUTMOD_3;                       // TACCR1 set/reset
  TACCR1 = 020;                             // TACCR1 PWM Duty Cycle
  TACCR0 = 032-1;                           // Clock period of TACCR0
  TACTL = TASSEL_2 + MC_1;                  // SMCLK, contmode
  __bis_SR_register(LPM0_bits + GIE);       // Enter LPM0
}
