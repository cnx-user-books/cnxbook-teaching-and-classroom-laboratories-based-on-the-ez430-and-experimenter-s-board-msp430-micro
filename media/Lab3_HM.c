/**************************************************************************/
/*               Lab 8c - RMS and active power calculation                */
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
/*           It is performed a task operational analysis in which is      */
/*	     calculated the active power and the RMS value of an              */
/*	     electrical system.                                               */
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
/*  Analyse the values of:                                                */
/*   - Active power                                                       */
/*   - RMS current                                                        */
/*   - RMS voltage                                                        */
/*                                                                        */
/*  Analyse with the oscilloscope (probe connected to port P2.1):         */
/*   - Measure the multiply operation execution times                     */
/*                                                                        */
/*                                                                        */
/*                                        Copyright by Texas Instruments  */
/**************************************************************************/

#include <msp430xG46x.h>
#include <math.h>
          
#define samples 200
          
int i [samples];
int u [samples];

unsigned char j;

int I, U, P;

long result;
//*************************************************************************
// main
//*************************************************************************
void main (void)
{
//Stop WatchDog
  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
  
// load data from file
  _NOP();                                   // load current from file
  			
  _NOP();                                   // load voltage from file
  
  
// Port P2.1 configured to output
  P2DIR |= 0x02;                            // P2.1 as output
  P2OUT &= ~0x02;                           // Clear P2.1  

// active power computation  
  P2OUT ^= 0x02;                            // toogle P2.1 to see MACS operation time
  MPYS = i[0];                              // Assign current to first operand (Signed multiply)
  OP2 = u[0];                               // Assign voltage to second operand
 
  for(j = 1; j<samples; j++){               // signed multiply and accumulate (MACS) samples {p = u * i}
    
    MACS = i[j];
    OP2 = u[j];  
  }
  P2OUT ^= 0x02;                            // toogle P2.1
 
  result = RESHI << 8;                      // save RESHI:RESLO int result
  result = result << 8;
  result = result | RESLO;
  P = result / samples;                     // compute active power
  
// RMS current computation
  P2OUT ^= 0x02;                            // toogle P2.1 to see RMS computing time
  MPYS = i[0];                              // Assign current to first operand (Signed multiply)
  OP2 = i[0];                               // Assign current to second operand

  for(j = 1; j<samples; j++){               // signed multiply and accumulate (MACS) samples {i^2 = i * i}
    
    MACS = i[j];
    OP2 = i[j];  
  }
 
  result = RESHI << 8;                      // save RESHI:RESLO int result
  result = result << 8;
  result = result | RESLO;
  I = sqrt(result / samples);               // compute RMS current (using the square root "sqrt" instruction)
  P2OUT ^= 0x02;                            // toogle P2.1
  
// RMS voltage computation
  MPYS = u[0];                              // Assign voltage to first operand (Signed multiply)
  OP2 = u[0];                               // Assign voltage to second operand

  for(j = 1; j<samples; j++){               // signed multiply and accumulate (MACS) samples {u^2 = u * u}
    
    MACS = u[j];
    OP2 = u[j];  
  }
 
  result = RESHI << 8;                      // save RESHI:RESLO int result
  result = result << 8;
  result = result | RESLO;
  P2OUT ^= 0x02;                            // toogle P2.1 to analyse sqrt() executing time
  U = sqrt(result / samples);               // compute RMS current (using the square root "sqrt" instruction)
  P2OUT ^= 0x02;                            // toogle P2.1
  
  _BIS_SR(LPM4);                            // Enter low power mode 4 (LPM4)
}
