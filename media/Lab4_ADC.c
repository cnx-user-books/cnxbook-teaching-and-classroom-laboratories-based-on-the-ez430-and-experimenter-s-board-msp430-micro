/**************************************************************************/
/*          Lab5D - Voltage signal comparison using Comparator_A          */
/*                                                                        */
/*                                                                        */
/*                                                                        */
/* MSP430 Teaching ROM                                                    */
/* Produced by: MSP430@UBI Group - www.msp430.ubi.pt                      */
/*                                                                        */
/* Exercise: Using the eZ430-F2013 Development Tool use SD16_A ADC to     */
/*           perform a single temperature sample on channel A6 (on-chip   */
/*           temperature sensor each minute during 1 hour.                */
/*                                                                        */
/*                MSP430xG461x                                            */
/*             -----------------                                          */
/*         /|\|                 |                                         */
/*          | |                 |                                         */
/*          --|RST           CA0|-Vcompare<---|                           */
/*            |                 |             |                           */
/*            |      (DAC0) P6.6|-->----------|                           */
/*            |     (CAOUT) P2.6|-->oscilloscope                          */
/*            |             P2.1|-->LED                                   */
/*                                                                        */
/*                                                                        */
/* Instructions:                                                          */
/*                                                                        */
/* This lab analyses the Comparator_A operation. A voltage is applied to  */
/* one of the Comparator’s inputs, generated optionally by the DAC12 or   */
/* by other external source. Whenever the external voltage value is       */
/* higher than the comparison value internally generated, it is generated */
/* an interrupt that switches the LED state.                              */
/*                                                                        */
/*  Configure:                                                            */
/*  - Comparator_A control registers                                      */
/*                                                                        */
/*                                        Copyright by Texas Instruments  */
/**************************************************************************/


#include <msp430xG46x.h>

unsigned char flag;
//*************************************************************************
// Comp_A interrupt service routine -- toggles LED
//*************************************************************************
#pragma vector=COMPARATORA_VECTOR
__interrupt void Comp_A_ISR (void)
{
  CACTL1 ^= CAIES;                          // Toggles interrupt edge
  P2OUT ^= 0x02;                            // Toggle P2.1
}
//*************************************************************************
// ISR to TACCRO from Timer A 
//*************************************************************************
#pragma vector=TIMERA0_VECTOR
__interrupt void TimerA0_ISR (void)
{	
	DAC12_0DAT++; 
	
	if (DAC12_0DAT == 0xfff )
	    DAC12_0DAT = 0;
	    
	if (flag == 1)		                    // if flag active exit LPM0
	 { 
	  flag = 0;
	  LPM0_EXIT;
	 }  
}
//*************************************************************************
// main
//*************************************************************************
void main (void)
{  
//Stop WatchDog
  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT	
  FLL_CTL0 |= XCAP14PF;                     // Configure load caps
 
// DAC configuration
  DAC12_0DAT = 0x00;                        // DAC_0 output 0V

  ADC12CTL0 = REF2_5V + REFON;              // Internal 2.5V ref on
  
  TAR = 0;									// TAR reset
  TACCR0 = 13600;                           // Delay to allow Ref to settle
  TACCTL0 |= CCIE;                          // Compare-mode interrupt
  TACTL = TACLR + MC_1 + TASSEL_2;          // up mode, SMCLK
  flag = 1;
  _BIS_SR(LPM0_bits + GIE);			        // Enter LPM0, enable interrupts  
  
  DAC12_0CTL = DAC12IR | DAC12AMP_5 | DAC12ENC; // DAC_0 -> P6.6
  												// DAC reference Vref
  												// 12 bits resolution
  												// Imediate load
  												// DAC full scale output
  												// Medium speed/current
  												// Straight binary
  												// Not grouped 

// Timer A configuration
  TAR = 0;									    // TAR reset
  TACCTL0 = CCIE;                               // CCR0 interrupt enabled
  TACCR0 = 32;			    		            // 1 msec counting period
  TACTL = TASSEL_1 | MC_1 | ID_0;               // ACLK, up mode   
  
// DAC0 outout port configuration (P6.6)
  P6DIR &= ~0x40;
  P6SEL |= 0x40;

// Comparator setup
  CACTL1 = CAON + CAREF_2 + CARSEL;             // Enable comp, ref = 0.5*Vcc
  CACTL2 = P2CA0;                               // Pin to CA0

  P2DIR |= 0x042;                               // P2.1 and P2.6 output direction
  P2SEL |= 0x040;                               // P2.1 = LED1 and P2.6 = CAOUT 

  CACTL1 |= CAIE;                               // Setup interrupt for Comparator  
  
// Interrupts enabled and enter in LPM3
  _BIS_SR(LPM3_bits + GIE);
}
