/**************************************************************************/
/*                Lab 3C - Buzzer tone generator                          */
/*                                                                        */
/*                                                                        */
/*                                                                        */
/* MSP430 Teaching ROM                                                    */
/* Produced by: MSP430@UBI Group - www.msp430.ubi.pt                      */
/*                                                                        */
/* Exercise: Using the MSP-EXP430FG4618 Development Tool and the          */
/*           MSP430FG4618 device implement a Buzzer tone generator        */
/*                                                                        */
/* Instructions:                                                          */
/* LED1 and LDE2 toogle each time a diferente tune is generated.          */
/*                                                                        */
/* SW1 or SW2 are used to control buzzer volume                           */
/*                                                                        */
/*                                                                        */
/*  Complete the code below:                                              */
/*   - Configure FLL+                                                     */
/*   - Configure TBCCR0                                                   */
/*   - Configure Timer_B                                                  */
/*   - Configure Basic Timer1                                             */
/*   - Configure P1 (buttons) and P2 (LEDs)                               */
/*                                                                        */
/*                                                                        */
/*                                                                        */
/*                                        Copyright by Texas Instruments  */
/**************************************************************************/

#include <msp430xG46x.h>

#define notas 10

#define SI0 15895
#define DO 15258
#define RE 13620
#define MI 12077
#define FA 11405
#define SOL 10159
#define LA 9106
#define SI 7963
#define DO2 7629
#define MUTE 0

//******************************************************************
// Global data
//******************************************************************
unsigned int scale[notas] =  {SI0, DO, RE, MI, FA, SOL, LA, SI, DO2, MUTE};
unsigned int space[notas] =  {RE, MI, DO2, DO, SOL, MUTE, MUTE, MUTE, MUTE, MUTE};
unsigned int index_notas = 0;
char volume = 2;
//******************************************************************
// Port1 Interrupt Service Rotine
//******************************************************************
#pragma vector=PORT1_VECTOR
__interrupt void PORT1_ISR (void)
{	
  if (P1IFG & 0x01) volume = volume - 1;
  if (P1IFG & 0x02) volume = volume + 1;
  
  if (volume == 6) volume = 5;
  if (volume == 0) volume = 1;
  
   TBCCR4 = space[index_notas]/volume;
   
   P1IFG = 0x00;
}
//*****************************************************************
// Basic Timer Interrupt Service Routine. Run with 1 sec period
//*****************************************************************
#pragma vector=BASICTIMER_VECTOR
__interrupt void basic_timer_ISR(void)
{
     P2OUT^=0x06;                            // Toogle LED1 and LED2	
     
     TBCCR0 = space[index_notas];	     // get next note
     TBCCR4 = space[index_notas]/volume;     // set duty-cicle
     
     index_notas++;			     // manage note point
     if (index_notas == notas)
     index_notas = 0; 
}
//******************************************************************
// Main routine
//******************************************************************
void main (void)
{  
//Stop WatchDog
  WDTCTL = WDTPW | WDTHOLD;                 // Stop WDT	
	
//FLL+ configuration 		                // ACLK - 32.768 kHz 
  FLL_CTL0 |= DCOPLUS + XCAP18PF;           // DCO+ set, freq = xtal x D x N+1
  SCFI0 |= FN_4;                            // x2 DCO freq, 8MHz nominal DCO
  SCFQCTL = 121;                            // (121+1) x 32768 x 2 = 7.99 MHz
   
// TimerB configuration
  TBCCR0 = space[0];			           // load first tone from sequency
  TBR = 0;			                       // reset TBR 
  TBCTL = TBSSEL_2 | CNTL_0 | TBCLGRP_0 |MC_1 | ID_0;           // SMCLK, continuous mode 

  TBCCTL4 = OUTMOD_3; 		               // CCR4 interrupt enabled
  TBCCR4 = space[0]/2;
  
// Basic Timer 1 Configuration
  BTCTL = BTDIV | BT_fCLK2_DIV128;          // (ACLK/256)/128
  IE2 |= BTIE;                              // Enable BT interrupt

     
// SW1 and SW2 configuration (Port1)
  P1SEL = 0x00;                             // P1.0 and P1.2 digital I/O
  P1DIR = 0x00;                             // P1.0 and P1.2 inputs
  P1IFG = 0x00;                             // clear P1 flags
  P1IE = 0xFF;                              // Enable port interrupts
 

// LED1 and LED2 configuration (Port2)
  P2DIR |= 0x06;                            // P2.2 and P2.1 as digital output
  P2OUT = 0x04;                             // LED1 on and LED2 off
  
// Buzzer port configuration (Port3)
  P3SEL |= 0x20;                            // P3.5 as special function
  P3DIR |= 0x20;                            // P3.5 as digital output 
  
// Interrupts enabled
  _BIS_SR(GIE);
}
