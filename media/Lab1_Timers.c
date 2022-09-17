/**************************************************************************/
/*               Lab 3A_1 - Memory clock with Basic Timer 1               */
/*                                                                        */
/*                                                                        */
/*                                                                        */
/* MSP430 Teaching ROM                                                    */
/* Produced by: MSP430@UBI Group - www.msp430.ubi.pt                      */
/*                                                                        */
/* Exercise: Using the MSP-EXP430FG4618 Development Tool and the          */
/*           MSP430FG4618 device implement a Memory clock with            */
/*           Basic Timer 1                                                */
/*                                                                        */
/* Instructions:                                                          */
/* LCD show time in HOUR : MINUTES : SECONDS format                       */
/*                                                                        */
/* LED1 turn on while executing basic_timer_ISR, it can be used to        */
/* measure ISR executing time.                                            */
/*                                                                        */
/* LED2 toogle each second.                                               */
/*                                                                        */
/*  Complete the code below:                                              */
/*   - Configure Watchdog timer                                           */
/*   - Configure FLL+                                                     */
/*   - Configure Basic Timer1                                             */
/*   - Configure P2                                                       */
/*                                                                        */
/*                                                                        */
/*                                                                        */
/*                                        Copyright by Texas Instruments  */
/**************************************************************************/

#include  <msp430xG46x.h>

#include <LCD_defs.h>

//******************************************************************
// Global data
//******************************************************************
unsigned char hour, min, sec;
//******************************************************************
// Write Hour in LCD
//******************************************************************
LCD_hour()
{
	unsigned char hour1,hour2;
	
	hour1 = hour/10;			// find hour most significant digit 
	hour2 = hour%10;			// find hour less significant digit
	
	switch (hour1)				// switch to write hour first digit
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
		case 6: P7_A6;
		        break;
		case 7: P7_A7;
		        break;
		case 8: P7_A8;
		        break;
		case 9: P7_A9;
		        break;
		case 0: P7_A0;
		        break;
	}
    switch (hour2)				// switch to write hour second digit
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
// Write minutes in LCD
//******************************************************************
LCD_min()
{
	unsigned char min1,min2;
	
	min1 = min/10;			    // find minutes less significant digit
	min2 = min%10;			    // find minutes less significant digit
	
	switch (min1)		            // switch to write minutes first digit
	{
		case 1: P5_A1;
				break;
		case 2: P5_A2;
		        break;
		case 3: P5_A3;
		        break;
		case 4: P5_A4;
		        break;
		case 5: P5_A5;
		        break;
		case 6: P5_A6;
		        break;
		case 7: P5_A7;
		        break;
		case 8: P5_A8;
		        break;
		case 9: P5_A9;
		        break;
		case 0: P5_A0;
		        break;
	}
switch (min2)				    // switch to write minutes second digit
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
		        break;
	}	
}
//******************************************************************
// Write seconds in LCD
//******************************************************************
LCD_sec()
{
	unsigned char sec1,sec2;
	
	sec1 = sec / 10;
	sec2 = sec % 10;
	
	switch (sec1)			     // switch to write seconds first digit
	{
		case 1:P3_A1;
		       break;
		case 2:P3_A2;
			   break;
		case 3:P3_A3;
			   break;
		case 4:P3_A4;
			   break;
		case 5:P3_A5;
		       break;
		case 6:P3_A6;
		       break;
		case 7:P3_A7;
		       break;
		case 8:P3_A8;
		       break;
		case 9:P3_A9;
		       break;
		case 0:P3_A0;
		       break;
	}
switch (sec2)				    // switch to write seconds second digit
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
}
//******************************************************************
//LCD clean. All segments are turnned off
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
//*****************************************************************
// BAsic Timer Interrupt Service Routine. Run with 1 sec period
//*****************************************************************
#pragma vector=BASICTIMER_VECTOR
__interrupt void basic_timer_ISR(void)
{
P2OUT |=0x02;				    // LED1 turn on
P2OUT ^=0x04;				    // LED2 toogle
 
 sec++;					    // increment seconds 
 LCD_sec();				        // refresh seconds field in LCD
 
 if (sec == 60)				    // one minute was pass
  {
    sec = 0;				// reset seconds counter
  	min++;				        // increment minutes
  	LCD_min();			        // refresh minutes field in LCD
  	if (min == 60)			    // one hour was pass
  	 {
  	   min = 0;			    // reset minutes counter
  	   hour++;		            // increment  hours 
  	   LCD_min();			    // refresh hours field in LCD
  	   if (hour == 24)		    // one day was pass
  	   {
  	   	hour = 0;		    // reset hours counter
  	   }
  	 } 
  }	
 
 if (sec & 0x01)			    // toogle clock dots
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
//*****************************************************************
// Main routine
//*****************************************************************
void main(void)
{
  WDTCTL = WDTPW | WDTHOLD;     // Stop WDT
  FLL_CTL0 |= XCAP18PF;         // Set load cap for 32k xtal
  
  // LCD COM0-COM1-COM2-COM3 configuration
  P5DIR |= 0x1E;                           // Ports P5.2, P5.3 and P5.4 as outputs
  P5SEL |= 0x1E;                           // Ports P5.2, P5.3 and P5.4 as special function (COM1, COM2 and COM3)
  
  // LCD_A S0-S21 configuration
   LCDAPCTL0 = LCDS24 | LCDS20 | LCDS16 | LCDS12 | LCDS8 | LCDS4; 
  
  // LCD_A configuration
  LCDACTL = LCDFREQ_192 | LCD4MUX | LCDSON | LCDON;   // (ACLK = 32768)/192, 4-mux LCD, LCD_A on, Segments on
  LCDAVCTL0 = LCDCPEN;                                // Charge pump enable
  LCDAVCTL1 = VLCD_3_44;                              // VLCD = 3,44 V 
  
  // Clean LCD
  LCD_all_off(); 
  
  // Set memory clock variables
  sec = 0;
  min = 0;
  hour = 0;
  
  // Write time in LCD
  LCD_sec();
  LCD_min();
  LCD_hour();
  
  // LED1 e LED2 configuration
  P2DIR |= 0x06;                            // P2.2 and P2.1 as digital output
  P2OUT |= 0x04;                            // LED1 on and LED2 off
  
 // Basic Timer 1 Configuration
  BTCTL = BTDIV | BT_fCLK2_DIV128;          // (ACLK/256)/128 
  IE2 |= BTIE;                              // Enable Basic Timer 1 interrupt

  _BIS_SR(LPM3_bits + GIE);                 // Enter LPM3 with all interrupts enabled
}

