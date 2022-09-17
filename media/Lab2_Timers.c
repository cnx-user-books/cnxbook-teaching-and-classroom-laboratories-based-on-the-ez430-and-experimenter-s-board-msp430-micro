/**************************************************************************/
/*            Lab 3A_2 - Real Time Clock with Basic Timer 1               */
/*                                                                        */
/*                                                                        */
/*                                                                        */
/* MSP430 Teaching ROM                                                    */
/* Produced by: MSP430@UBI Group - www.msp430.ubi.pt                      */
/*                                                                        */
/* Exercise: Using the MSP-EXP430FG4618 Development Tool and the          */
/*           MSP430FG4618 device implement a Real Time Clock with         */
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
/* SW1 or SW2 are used to select LCD view: calendar or clock              */
/*                                                                        */
/*  Complete the code below:                                              */
/*   - Configure RTC                                                      */
/*   - Configure Basic Timer1                                             */
/*   - Configure P1 (buttons)                                             */
/*                                                                        */
/*                                                                        */
/*                                                                        */
/*                                        Copyright by Texas Instruments  */
/**************************************************************************/


#include  <msp430xG46x.h>

#include  <LCD_defs.h>

//******************************************************************
// Global data
//******************************************************************
unsigned char page;
//******************************************************************
// Write year decade
//******************************************************************
LCD_year()
{
	switch (RTCYEARL & 0xF0)			// switch to write year first digit
	{
		case 0x10: P7_A1;
	               break;
		case 0x20: P7_A2;
		           break;
		case 0x30: P7_A3;
		           break;
		case 0x40: P7_A4;
		           break;
		case 0x50: P7_A5;
		           break;
		case 0x60: P7_A6;
		           break;
		case 0x70: P7_A7;
		           break;
		case 0x80: P7_A8;
		           break;
		case 0x90: P7_A9;
		           break;
		case 0x00: P7_A0;
		           break;
	}
    switch (RTCYEARL & 0x0F)				// switch to write year second digit
	{
		case 0x01: P6_A1;
		           break;
		case 0x02: P6_A2;
		           break;
		case 0x03: P6_A3;
		           break;
		case 0x04: P6_A4;
		           break;
		case 0x05: P6_A5;
		           break;
		case 0x06: P6_A6;
		           break;
		case 0x07: P6_A7;
		           break;
		case 0x08: P6_A8;
		           break;
		case 0x09: P6_A9;
		           break;
		case 0x00: P6_A0;
		           break;
	}	
}
//******************************************************************
// Write month in LCD
//******************************************************************
LCD_month()
{
	switch (RTCMON & 0xF0)				// switch to write month first digit
	{
		case 0x10: P5_A1;
			       break;
		case 0x00: P5_A0;
		           break;
	}
   switch (RTCMON & 0x0F)				// switch to write month second digit
	{
		case 0x01: P4_A1;
		           break;
		case 0x02: P4_A2;
		           break;
		case 0x03: P4_A3;
		           break;
		case 0x04: P4_A4;
		           break;
		case 0x05: P4_A5;
		           break;
		case 0x06: P4_A6;
		           break;
		case 0x07: P4_A7;
		           break;
		case 0x08: P4_A8;
		           break;
		case 0x09: P4_A9;
		           break;
		case 0x00: P4_A0;
		           break;
	}	
}
//******************************************************************
// Write day in LCD
//******************************************************************
LCD_day()
{
	switch (RTCDAY & 0xF0)				// switch to write day first digit
	{
		case 0x10: P3_A1;
		           break;
		case 0x20: P3_A2;
			       break;
		case 0x30: P3_A3;
			       break;	
		case 0x00: P3_A0;
		           break;
	}
   switch (RTCDAY & 0x0F)				// switch to write day second digit
	{
		case 0x01: P2_A1;
		           break;
		case 0x02: P2_A2;
		           break;
		case 0x03: P2_A3;
		           break;
		case 0x04: P2_A4;
		           break;
		case 0x05: P2_A5;
		           break;
		case 0x06: P2_A6;
		           break;
		case 0x07: P2_A7;
		           break;
		case 0x08: P2_A8;
		           break;
		case 0x09: P2_A9;
		           break;
		case 0x00: P2_A0;
	}	
}
//******************************************************************
// Write Hour in LCD
//******************************************************************
LCD_hour()
{
	switch (RTCHOUR & 0xF0)				// switch to write hour first digit
	{
		case 0x10: P7_A1;
				   break;
		case 0x20: P7_A2;
		           break;
		case 0x00: P7_A0;
		           break;
	}
    switch (RTCHOUR & 0x0F)				// switch to write hour second digit
	{
		case 0x01: P6_A1;
		           break;
		case 0x02: P6_A2;
		           break;
		case 0x03: P6_A3;
		           break;
		case 0x04: P6_A4;
		           break;
		case 0x05: P6_A5;
		           break;
		case 0x06: P6_A6;
		           break;
		case 0x07: P6_A7;
		           break;
		case 0x08: P6_A8;
		           break;
		case 0x09: P6_A9;
		           break;
		case 0x00: P6_A0;
		           break;
	}	
}
//******************************************************************
// Write minutes in LCD
//******************************************************************
LCD_min()
{

	switch (RTCMIN & 0xF0)				// switch to write minutes first digit
	{
		case 0x10: P5_A1;
				   break;
		case 0x20: P5_A2;
		           break;
		case 0x30: P5_A3;
		           break;
		case 0x40: P5_A4;
		           break;
		case 0x50: P5_A5;
		           break;
		case 0x00: P5_A0;
		           break;
	}
  switch (RTCMIN & 0x0F)				// switch to write minutes second digit
	{
		case 0x01: P4_A1;
		           break;
		case 0x02: P4_A2;
		           break;
		case 0x03: P4_A3;
		           break;
		case 0x04: P4_A4;
		           break;
		case 0x05: P4_A5;
		           break;
		case 0x06: P4_A6;
		           break;
		case 0x07: P4_A7;
		           break;
		case 0x08: P4_A8;
		           break;
		case 0x09: P4_A9;
		           break;
		case 0x00: P4_A0;
		           break;
	}	
}
//******************************************************************
// Write seconds in LCD
//******************************************************************
LCD_sec()
{
	switch (RTCSEC & 0xF0)				// switch to write seconds first digit
	{
		case 0x10: P3_A1;
		           break;
		case 0x20: P3_A2;
			       break;
		case 0x30: P3_A3;
			       break;
		case 0x40: P3_A4;
			       break;
		case 0x50: P3_A5;
		           break;
		case 0x00: P3_A0;
		           break;
	}
switch (RTCSEC & 0x0F)				         // switch to write seconds second digit
	{
		case 0x01: P2_A1;
		           break;
		case 0x02: P2_A2;
		           break;
		case 0x03: P2_A3;
		           break;
		case 0x04: P2_A4;
		           break;
		case 0x05: P2_A5;
		           break;
		case 0x06: P2_A6;
		           break;
		case 0x07: P2_A7;
		           break;
		case 0x08: P2_A8;
		           break;
		case 0x09: P2_A9;
		           break;
		case 0x00: P2_A0;
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
//******************************************************************
// Port1 interrupt service routine: toogle LCD page
//******************************************************************
#pragma vector=PORT1_VECTOR
__interrupt void PORT1_ISR (void)
{	
   if (page == 0) page = 1;			// toggle LCD page 
   else page = 0;
   
   P1IFG &= ~0x03;				// clean Interrupt flag
}
//*****************************************************************
//  Basic Timer interrupt service routine: refresh LCD with 0.5 sec
//*****************************************************************
#pragma vector=BASICTIMER_VECTOR
__interrupt void basic_timer_ISR(void)
{
P2OUT |=0x02;				    // LED1 turn on
P2OUT ^=0x04;				    // LED2 toogle
 
 if (page == 0)				    // Real time clock
    { 
      LCD_sec();
      LCD_min();
      LCD_hour();    	
    }  
 else					    // calendar
    {
      LCD_year();
      LCD_month();
      LCD_day();    	
    } 

 if (RTCSEC & 0x01)			    // toogle clock dots
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
  WDTCTL = WDTPW | WDTHOLD;              // Stop WDT
  FLL_CTL0 |= XCAP18PF;                  // Set load cap for 32k xtal
  
  // LCD COM0-COM1-COM2-COM3 configuration
  P5DIR |= 0x1E;                           // Ports P5.2, P5.3 and P5.4 as outputs
  P5SEL |= 0x1E;                           // Ports P5.2, P5.3 and P5.4 as special function (COM1, COM2 and COM3)
  
  // LCD_A S0-S21 configuration
  LCDAPCTL0 = LCDS24 | LCDS20 | LCDS16 | LCDS12 | LCDS8 | LCDS4; 
  
  // LCD_A configuration
  LCDACTL = LCDFREQ_192 | LCD4MUX | LCDSON | LCDON;   // (ACLK = 32768)/192, 4-mux LCD, LCD_A on, Segments on
  LCDAVCTL0 = LCDCPEN;                                // Charge pump enable
  LCDAVCTL1 = VLCD_3_44;                              // VLCD = 3,44 V 
  
  // clean LCD
  LCD_all_off();
  
  // RTC configuration
  RTCCTL = RTCBCD | RTCHOLD | RTCMODE_3;
                                        // BCD mode, RTC e BT disable
                                        // Interrupt disable, 
   
  // Init Clock
  RTCSEC =  0x00;                       // Set Seconds in BCD format
  RTCMIN =  0x59;                       // Set Minutes in BCD format
  RTCHOUR = 0x01;                       // Set Hours in BCD format
  
  // Init Calendar
  RTCDOW =  0x06;                       // Set Day-of-Week  in BCD format
  RTCDAY =  0x09;                       // Set Day in BCD format
  RTCMON =  0x08;                       // Set Month in BCD format
  RTCYEAR = 0x2008;                     // Set Year in BCD format

  RTCCTL &= ~RTCHOLD;                   // Enable RTC
  
  // Basic Timer 1 Configuration
  BTCTL = BT_fCLK2_DIV64;               // (ACLK/256)/64 
  IE2 |= BTIE;                          // Enable BT interrupt with 0.5 periode
  
  // LED1 e LED2 configuration
  P2DIR = 0x06;                         // P2.2 and P2.1 as digital output
  P2OUT = 0x04;                         // LED1 on and LED2 off
  
  // LCD init
  LCD_sec();
  LCD_min();
  LCD_hour();
  page = 0; 
  
  // SW1 and SW2 configuration 
  P1SEL &= ~0x03;                       // P1.0 and P1.1 I/O ports
  P1DIR &= ~0x03;                       // P1.0 and P1.1 digital inputs
  P1IFG  = 0x00;                        // Clear P1 flags
  P1IES &= ~0x03;                       // high-to-low transition interrupts
  P1IE |= 0x03;                         // enable port interrupts 

  __bis_SR_register(LPM3_bits + GIE);   // Enter LPM3 w/ interrupt
}


