/**************************************************************************/
/*             Lab5C - Temperature data logger using SD16_A               */
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
/*                MSP430F20x3                                             */
/*             -----------------                                          */
/*         /|\|              XIN|-                                        */
/*          | |                 |                                         */
/*          --|RST          XOUT|-                                        */
/*            |                 |                                         */
/*            |A6  (Temp)   P1.0|-->LED                                   */
/*                                                                        */
/*  Uncalibrated temperature measured from device to device will vary with*/
/*  slope and offset - please see datasheet.                              */
/*                                                                        */
/* Instructions:                                                          */
/*                                                                        */
/* At each timer interrupt the LED connected to P1.0 will blink.          */
/* The SD16_A uses the internal 1.2V Vref. Software sets SD16SC to start  */
/* sample and conversion - SD16SC automatically cleared at EOC.           */
/* SD16_A uses SMCLK at 1 MHz. In Mainloop MSP430 waits in LPM3 to save   */
/* power until SD16_A conversion complete, SD16_ISR will force exit from  */
/* LPM3 in Mainloop on reti.                                              */
/* Temperature in oC stored in flash info memory segments B and C.        */
/*                                                                        */
/*  Configure:                                                            */
/*  - DCO to 1 MHz: MCLK = SMCLK = ~1 MHz                                 */
/*  - Basic Clock module+ control registers: ACLK = 1,5 kHz               */
/*  - SD16_A control registers;                                           */
/*  - Timer_A control registers.                                          */
/*                                        Copyright by Texas Instruments  */
/**************************************************************************/

#include  <msp430x20x3.h>

#define InfoMemoB 0x1040                    // Info memory Segment B
#define InfoMemoC 0x1080                    // Info memory Segment C

unsigned int counter;                       // count WDT interrupts
unsigned int min;                           // minutes counter
unsigned int memo_ptr;                      // memory counter

//*************************************************************************/
// Erase segment  
//*************************************************************************/
void erase_segment(int adress)
{
  int *Flash_ptr;                           // Flash pointer

  Flash_ptr = (int *)adress;                // Initialize Flash pointer
  FCTL1 = FWKEY + ERASE;                    // Set Erase bit
  FCTL3 = FWKEY;                            // Clear Lock bit
  
  *Flash_ptr = 0;                           // Dummy write to erase Flash segment
  
  FCTL3 = FWKEY + LOCK;                     // Set LOCK bit
}
//*************************************************************************/
// Write integer in flash
//*************************************************************************/
void write_int_flash(int adress, int value)
{
  int *Flash_ptr;                           // Flash pointer

  Flash_ptr = (int *)adress;                // Initialize Flash pointer
  FCTL3 = FWKEY;                            // Clear Lock bit
  FCTL1 = FWKEY + WRT;                      // Set WRT bit for write operation
  
  *Flash_ptr = value;                       // Write value to flash
  
  FCTL1 = FWKEY;                            // Clear WRT bit
  FCTL3 = FWKEY + LOCK;                     // Set LOCK bit
}
//*************************************************************************/
// SD16_A Interrupt Service Routine
//*************************************************************************/
#pragma vector=SD16_VECTOR
__interrupt void SD16ISR(void)
{
  unsigned int temperature;
  if (min <= 60)
  {	
    temperature = (SD16MEM0-0x8000)/84 - 232;
    write_int_flash(memo_ptr,temperature);
    memo_ptr += 2;
  } 
  else
  {
  	_NOP();
  } 
}
//*************************************************************************/
// Timer_A Interrupt Service Routine
//*************************************************************************/
#pragma vector=TIMERA0_VECTOR
__interrupt void TimerA0_ISR (void)
{	
  counter++;
  P1OUT ^= 0x01;                            // LED toogle	
 
  if (counter == 60)
   {
   	min++;
   	counter = 0;
    SD16CCTL0 |= SD16SC;                    // Start SD16 conversion
   } 
}
//**************************************************************************/
// Main routine
//**************************************************************************/
void main(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 // Stop watchdog timer
  
  if (CALBC1_1MHZ == 0xFF || CALDCO_1MHZ == 0xFF)                                     
  {  
    while(1);                               // If calibration constants erased
                                            // do not load, trap CPU!!
  }
   
  DCOCTL = CALDCO_1MHZ;                     // Set DCO to 1 MHz (MCLK = SMCLK = 1 MHz)
  BCSCTL1 = DIVA_3;                         // ACLK = 1.5 kHz
  BCSCTL3 = LFXT1S_2;                       // Set VLOCLK (12 kHz)
  
  FCTL2 = FWKEY + FSSEL0 + FN1;             // MCLK/3 for Flash Timing Generator

  P1DIR |= 0x01;                            // P1.0 to output direction
  
  SD16CTL = SD16REFON + SD16SSEL_1;         // 1.2V ref, SMCLK
  SD16INCTL0 = SD16INCH_6;                  // A6+/-
  SD16CCTL0 = SD16SNGL + SD16IE;            // Single conv, interrupt
  
  TAR = 0;                                  // Clean Timer A counter
  CCTL0 = CCIE;                             // CCR0 interrupt enabled
  CCR0 = 1500;                              // this count corresponds to 1 sec
  TACTL = TASSEL_1 | MC_1 | ID_0;           // ACLK, up mode to CCR0

  counter = 0;
  min = 0;
  erase_segment(InfoMemoB);
  erase_segment(InfoMemoC);
  
  memo_ptr = InfoMemoB;
  
  _BIS_SR(LPM3_bits + GIE);                 // Enter LPM3 with interrupt
}
