/**************************************************************************/
/*            Lab 8b - Multiplication with hardware multiplier            */
/*                                                                        */
/*                                                                        */
/*                                                                        */
/* MSP430 Teaching ROM                                                    */
/* Produced by: MSP430@UBI Group - www.msp430.ubi.pt                      */
/*                                                                        */
/* Exercise: Using the MSP-EXP430FG4618 Development Tool and the          */
/*           MSP430FG4618 device analyse the MSP430’s performance when    */
/*           makes a multiplication operation with the hardware           */
/*           multiplier.                                                  */
/*           Two different situations are analysed:                       */
/*	     - Using the “*” operator                                         */
/*	     - Accessing the hardware multiplier registers directly.          */
/*                                                                        */
/*           The execution time is measured with an oscilloscope.         */
/*                                                                        */
/* Instructions:                                                          */
/*                                                                        */
/* This lab only uses the Port P2.1 connected to LED2 in order to acquire */
/* the execution time of the multiplication operation when it is          */
/* performed by the hardware multiplier.                                  */
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

// Port P2.1 configured to output
  P2DIR |= 0x02;                            // P2.1 as output
  P2OUT &= ~0x02;                           // Clear P2.1 

// using the * operator
  P2OUT ^= 0x02;                            // toogle P2.1 to see multiplie operation time
  a = 10;                                   // initialize variable a
  b = 5;                                    // initialize variable b

  c = a*b;                                  // Multiplication operation using the hardware
                                            // multiplication routine (WITH hardware multiplier)

  P2OUT ^= 0x02;                            // toogle P2.1 to see multiplie operation time  
 
 _NOP();                                    // take time 
 _NOP();
 _NOP();
 _NOP();
 _NOP();
 _NOP();
 _NOP();
 _NOP();
 
   
// Writing directly in Hardware multiplier registers 
  P2OUT ^= 0x02;                            // toogle P2.1 to see multiplie operation time 
  MPY = a;                                  // Hardware multiplier first operand (Unsigned multiply) init
  OP2 = b;                                  // Hardware multiplier second operand init
  c = RESLO;                                // Assign to variable c the content of the result register RESLO
  P2OUT ^= 0x02;                            // toogle P2.1 to see multiplie operation time
  
  _BIS_SR(LPM4);                            // Enter Low power mode 4 (LPM4)
}
