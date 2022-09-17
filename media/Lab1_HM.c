/**************************************************************************/
/*            Lab 8a - Multiplication without hardware multiplier         */
/*                                                                        */
/*                                                                        */
/*                                                                        */
/* MSP430 Teaching ROM                                                    */
/* Produced by: MSP430@UBI Group - www.msp430.ubi.pt                      */
/*                                                                        */
/* Exercise: Using the MSP-EXP430FG4618 Development Tool and the          */
/*           MSP430FG4618 device analyse the MSP430’s performance when    */
/*           makes a multiplication operation without the hardware        */
/*           multiplier.                                                  */
/*           The execution time is measured with an oscilloscope.         */
/*                                                                        */
/* Instructions:                                                          */
/*                                                                        */
/* This lab only uses the Port P2.1 connected to LED2 in order to acquire */
/* the execution time of the multiplication operation when it is          */
/* performed by a software routine.                                       */
/*                                                                        */
/* Is used the default configuration of the FLL+.                         */
/*                                                                        */
/*  Analyse with the oscilloscope (probe connected to port P2.1):         */
/*   - Measure the multiply operation execution time                      */
/*                                                                        */
/*                                                                        */
/*                                        Copyright by Texas Instruments  */
/**************************************************************************/

#include <msp430xG46x.h>
          
int a, b, c;

//*************************************************************************
// main
//*************************************************************************
void main (void)
{
//Stop WatchDog
  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
  
// Port P2.1 (LED2) configured to output
  P2DIR |= 0x02;                            // P2.1 as output
  P2OUT &= ~0x02;                           // Clear P2.1  
  
  a = 10;                                   // initialize variable a
  b = 5;                                    // initialize variable b

  P2OUT ^= 0x02;                            // toogle P2.1 to see multiplie operation time  
  
  c = a*b;                                  // Multiplication operation using the software
                                            // multiplication routine (WITHOUT hardware multiplier)
  
  P2OUT ^= 0x02;                            // toogle P2.1
  
 _BIS_SR(LPM4);                             // Enter low power mode 4 (LPM4)
}
