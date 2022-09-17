/**************************************************************************/
/*            Lab 3B - Memory Clock with Timer_A                          */
/*                                                                        */
/*                                                                        */
/*                                                                        */
/* MSP430 Teaching ROM                                                    */
/* Produced by: MSP430@UBI Group - www.msp430.ubi.pt                      */
/*                                                                        */
/* Exercise: Using the MSP-EXP430FG4618 Development Tool and the          */
/*           MSP430FG4618 device implement a Memory Clock with            */
/*           Timer_A                                                      */
/*                                                                        */
/* Instructions:                                                          */
/* Timer A count to the value in CCR0. At the end of each count an        */
/* interrupt is generated.                                                */
/*                                                                        */
/* LED1 turn on while executing basic_timer_ISR, it can be used to        */
/* measure ISR executing time.                                            */
/*                                                                        */
/* LED2 toogle each 0,1 second                                            */
/*                                                                        */
/*                                                                        */
/*  Complete the code below:                                              */
/*   - Configure Timer_A                                                  */
/*                                                                        */
/*                                                                        */
/*                                                                        */
/*                                        Copyright by Texas Instruments  */
/**************************************************************************/


#include <msp430xG46x.h>

__interrupt void p1_isr (void);
__interrupt void Timer_A (void);

#include <LCD_defs.h>


unsigned char min, sec;
unsigned int msec;
unsigned int count1, count2, count3;
//******************************************************************
// Write min
//******************************************************************
LCD_min()
{
	unsigned char min1,min2;
	
	min1 = min/10;
	min2 = min%10;
	
	switch (min1)
	{
		case 1: P7_A1;
				break;
		case 2: P7_A2;
		        break;
		case 3: P7_A3;
		        break;
		case 4: P7_A4;
		        break;
		case 5: P7_A5;
		        break;
		case 0: P7_A0;
		        break;
	}
switch (min2)
	{
		case 1: P6_A1;
		        break;
		case 2: P6_A2;
		        break;
		case 3: P6_A3;
		        break;
		case 4: P6_A4;
		        break;
		case 5: P6_A5;
		        break;
		case 6: P6_A6;
		        break;
		case 7: P6_A7;
		        break;
		case 8: P6_A8;
		        break;
		case 9: P6_A9;
		        break;
		case 0: P6_A0;
		        break;
	}	
}
//******************************************************************
// Write sec
//******************************************************************
LCD_sec()
{
	unsigned char sec1,sec2;
	
	sec1 = sec / 10;
	sec2 = sec % 10;
	
	switch (sec1)
	{
		case 1:P5_A1;
		       break;
		case 2:P5_A2;
			   break;
		case 3:P5_A3;
			   break;
		case 4:P5_A4;
			   break;
		case 5:P5_A5;
		       break;
		case 0:P5_A0;
		       break;
	}
switch (sec2)
	{
		case 1: P4_A1;
		        break; 
		case 2: P4_A2;
		        break;
		case 3: P4_A3;
		        break;
		case 4: P4_A4;
		        break;
		case 5: P4_A5;
		        break;
		case 6: P4_A6;
		        break;
		case 7: P4_A7;
		        break;
		case 8: P4_A8;
		        break;
		case 9: P4_A9;
		        break;
		case 0: P4_A0;
	}	
}
//******************************************************************
// Write msec
//******************************************************************
LCD_msec()
{
	switch (msec)
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
		        break;
	}
    P2_A0;
	P1_A0;
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
// Timer A ISR
//******************************************************************
#pragma vector=TIMERA0_VECTOR
__interrupt void TimerA0_ISR (void)
{	
 P2OUT |=0x02;				    // LED1 turn on
 P2OUT ^=0x04;				    // LED2 toogle
 
 msec++;
 LCD_msec();
 
 if (msec == 10)
  {
  	msec = 0;
  	sec++;
  	LCD_sec();
  	if (sec == 60)
  	{
  		sec = 0;
  		min++;
  		LCD_min();
  		if (min == 60)
  		{
  			min = 0;
  		}
  	}
  }	
 
 if (sec & 0x01)			   // toogle clock dots
    { 
      P3_DOT_ON;	
      P5_DOT_ON;
    }  
 else
    {
      P3_DOT_OFF;
      P5_DOT_OFF;
    }
         
 P2OUT &=~0x02;				    // LED1 turn off   
}
//******************************************************************
// Main routnie
//******************************************************************
void main (void)
{  
  WDTCTL = WDTPW | WDTHOLD;                 // Stop WDT
  
  // LCD COM0-COM1-COM2-COM3 configuration
  P5DIR |= 0x1E;                           // Ports P5.2, P5.3 and P5.4 as outputs
  P5SEL |= 0x1E;                           // Ports P5.2, P5.3 and P5.4 as special function (COM1, COM2 and COM3)
  
  // LCD_A S0-S21 configuration
   LCDAPCTL0 = LCDS24 | LCDS20 | LCDS16 | LCDS12 | LCDS8 | LCDS4; 
   
  // LCDA configuration
  LCDACTL = LCDFREQ_192 | LCD4MUX | LCDSON | LCDON;   // (ACLK = 32768)/192, 4-mux LCD, LCD_A on, Segments on
  LCDAVCTL0 = LCDCPEN;                                // Charge pump enable
  LCDAVCTL1 = VLCD_3_44;                              // VLCD = 3,44 V
   
  // Timer A configuration
  TAR = 0;                                  // Clean Timer A counter
  CCTL0 = CCIE;                             // CCR0 interrupt enabled
  CCR0 = 3268;				                // this count correspond to 100 mseg
  TACTL = TASSEL_1 | MC_1 | ID_0;           // ACLK, continuous mode
  
   // LED1 e LED2 configuration  
  P2DIR = 0x06;                             // P2.2 and P2.1 as digital output;
  P2OUT = 0x04;                             // LED1 on and LED2 off 
  
  // init memory clock data	
  min = 0;
  sec = 0;
  msec = 0;
  
  // Clean LCD 
  LCD_all_off(); 
  
  // refresh LCD
  LCD_msec();
  LCD_sec();
  LCD_min();
  
  // active interruptions and enter in LPM3
 _BIS_SR(LPM3_bits + GIE);
}
