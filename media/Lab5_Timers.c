/**************************************************************************/
/*                Lab 4C - Frequency measurement                          */
/*                                                                        */
/*                                                                        */
/*                                                                        */
/* MSP430 Teaching ROM                                                    */
/* Produced by: MSP430@UBI Group - www.msp430.ubi.pt                      */
/*                                                                        */
/* Exercise: Using the MSP-EXP430FG4618 Development Tool and the          */
/*           MSP430FG4618 device perform a Frequency measurement          */
/*                                                                        */
/* Instructions:                                                          */
/*                                                                        */
/*  Complete the code below:                                              */
/*   - Configure Basic Timer1                                             */
/*   - Configure Timer_A                                                  */
/*   - Configure Timer_B                                                  */
/*   - Configure P1, P2 and P3                                            */
/*                                                                        */
/*                                                                        */
/*                                                                        */
/*                                        Copyright by Texas Instruments  */
/**************************************************************************/


#include <msp430xG46x.h>

#include <LCD_defs.h>

//******************************************************************
// Global data
//******************************************************************
unsigned char flag, capture, counter, tick;
unsigned int T, T1, T2, freq;
//******************************************************************
// Write frequency (Hz) in LCD 
//******************************************************************
LCD_freq()
{
  unsigned int freq1, freq2, freq3, freq4;
  
  freq = 7995392/T;
  	
  freq1 = freq / 1000;
  freq2 = freq % 1000;
	
  switch (freq1)				// switch to write frequency first digit
	{
		case 1:P4_A1;
		       break;
		case 2:P4_A2;
			   break;
		case 3:P4_A3;
			   break;
		case 4:P4_A4;
			   break;
		case 5:P4_A5;
		       break;
		case 6:P4_A6;
		       break;
		case 7:P4_A7;
		       break;
		case 8:P4_A8;
		       break;
		case 9:P4_A9;
		       break;
		case 0:P4_A0;
		       break;
	}

 freq3 = freq2 % 100;
 freq2 = freq2 / 100;
	
 switch (freq2)				    // switch to write seconds frequency digit
	{
		case 1: P3_A1;
		        break;
		case 2: P3_A2;
		        break;
		case 3: P3_A3;
		        break;
		case 4: P3_A4;
		        break;
		case 5: P3_A5;
		        break;
		case 6: P3_A6;
		        break;
		case 7: P3_A7;
		        break;
		case 8: P3_A8;
		        break;
		case 9: P3_A9;
		        break;
		case 0: P3_A0;
	}	
	
 freq4 = freq3 % 10;
 freq3 = freq3 / 10;
 	
 switch (freq3)				    // switch to write seconds frequency digit
	{
		case 1: P2_A1;
		        break;
		case 2: P2_A2;
		        break;
		case 3: P2_A3;
		        break;
		case 4: P2_A4;
		        break;
		case 5: P2_A5;
		        break;
		case 6: P2_A6;
		        break;
		case 7: P2_A7;
		        break;
		case 8: P2_A8;
		        break;
		case 9: P2_A9;
		        break;
		case 0: P2_A0;
	}	
	 
 switch (freq4)				    // switch to write seconds frequency digit
	{
		case 1: P1_A1;
		        break;
		case 2: P1_A2;
		        break;
		case 3: P1_A3;
		        break;
		case 4: P1_A4;
		        break;
		case 5: P1_A5;
		        break;
		case 6: P1_A6;
		        break;
		case 7: P1_A7;
		        break;
		case 8: P1_A8;
		        break;
		case 9: P1_A9;
		        break;
		case 0: P1_A0;
	}
}
//******************************************************************
// LCD clean routine
//******************************************************************
void LCD_all_off(void)
{
 LCDM2 = 0x00;
 LCDM3 = 0x00;
 LCDM4 = 0x00;
 LCDM5 = 0x00;
 LCDM6 = 0x00;
 LCDM7 = 0x00;
 LCDM8 = 0x00;
 LCDM9 = 0x00;
 LCDM10 = 0x00;
 LCDM11 = 0x00;
 LCDM12 = 0x00;
 LCDM13 = 0x00;
}  
//******************************************************************
// Timer A Interrupt Service Routine
//******************************************************************
#pragma vector=TIMERA1_VECTOR
__interrupt void TimerA1_ISR (void)
{
 switch (TAIV)
 {
   case TAIV_TACCR1: 
    if (capture == 0){
      T1 = TACCR1;
      flag = 1;
      capture = 1;
    }
    else {
      if (flag == 1) {
      T2 = TACCR1;
      if (T2 > T1)
        T = T2-T1; 
      }
    else{
      TAR = 0;
      }
    capture = 0;
    flag = 0; 					
 } 
                     break;
   
   case TAIV_TACCR2:
                     break;

   case TAIV_TAIFG:
                     tick++;
                     if (tick == 60){
                     	LCD_freq();
                     	tick = 0;
                     }
                     if (flag == 1)flag = 0;
                     
                     break;                     
  
   default:
                     break;  
 }
}	
//*****************************************************************
// Basic Timer Interrupt Service Routine. Run with 1 sec period
//*****************************************************************
#pragma vector=BASICTIMER_VECTOR
__interrupt void basic_timer_ISR(void)
{
	unsigned int read_data;            // read data from file , frequency in kHz
	
     P2OUT^=0x06;		           // toogle LED1 and LED2
	 counter++;

	 if (counter == 5){
       counter = 0;
	   read_data = 200;
	   TBCCR0 = 7995392/read_data;
	   TBCCR4 = TBCCR0/2;
	 }
}
//******************************************************************
// Main routine
//******************************************************************
void main (void)
{  
//Stop WatchDog
  WDTCTL = WDTPW | WDTHOLD;                 // Stop WDT
	
//FLL+ configuration 		            // ACLK - 32.768 kHz 
  FLL_CTL0 |= DCOPLUS + XCAP18PF;           // DCO+ set, freq = xtal x D x N+1
  SCFI0 |= FN_4;                            // x2 DCO freq, 8MHz nominal DCO
  SCFQCTL = 121;                            // (121+1) x 32768 x 2 = 7.99 MHz
   
  // Ports COM0-COM1-COM2-COM3 configuration
  P5DIR |= 0x1E;                            // Ports P5.2, P5.3 and P5.4 as outputs
  P5SEL |= 0x1E;                           // Ports P5.2, P5.3 and P5.4 as special function (COM1, COM2 and COM3)
  
  // S0-S21 ports configuration
   LCDAPCTL0 = LCDS24 | LCDS20 | LCDS16 | LCDS12 | LCDS8 | LCDS4; 
   
  // LCDA configuration
  LCDACTL = LCDFREQ_192 | LCD4MUX | LCDSON | LCDON;   // (ACLK = 32768)/192, 4-mux LCD, LCD_A on, Segments on
  LCDAVCTL0 = LCDCPEN;                                // Charge pump enable
  LCDAVCTL1 = VLCD_3_44;                              // VLCD = 3,44 V
  
  LCD_all_off();
  
// Basic Timer 1 Configuration
  BTCTL = BTDIV | BT_fCLK2_DIV128;          // (ACLK/256)/128
  IE2 |= BTIE;                              // Enable BT interrupt with 1 sec period

// TimerB configuration
  TBCCR0 = 39977;  			                // Output 200 Hz signal with 50% duty cicle 
  TBR = 0;				                    // reset TBR

  TBCCTL4 = OUTMOD_3;                       // CCR4 output mode 3 (set/reset)
  TBCCR4 = TBCCR0/2;
  
  TBCTL = TBSSEL_2 | CNTL_0 | TBCLGRP_0 |MC_1 | ID_0;           // SMCLK, continuous mode 

// TimerA configuration
   TACTL = TASSEL_2 |MC_2 | ID_0 | TAIE;    // SMCLK, continuous mode up to 0xffff   
   TACCTL1 = CM1 | CCIS_0 | CAP | CCIE;     // Capture on rising edge, Cap mode, cap/com interrupt enable
                                            // TACCR1 input signal selected

// TA1 (TACCR1) configuration (Port1)
  P1SEL = 0x04;                             // P1.2 as special function (TA1)
  P1DIR = 0x00;                             // P1.2 as input

// LED1 and LED2 configuration (Port2)
  P2DIR = 0x06;                             // P2.2 and P2.1 as digital output
  P2OUT = 0x04;                             // LED1 on and LED2 off 
  
// TB4 configuration (Port3)
  P3SEL = 0x20;                             // P3.5 as special function (TB4)
  P3DIR = 0x20;                             // P3.5 as output
  
// global data initialition
 counter = 0;
 tick = 0;  
  
// Interrupts enabled
  _BIS_SR(GIE);                             // all interrupts enable
  
  for(;;);
}
