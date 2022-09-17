/**************************************************************************/
/*              Lab10b2 - Echo test using the SPI mode (Slave)            */
/*                                                                        */
/*                                                                        */
/*                                                                        */
/* MSP430 Teaching ROM                                                    */
/* Produced by: MSP430@UBI Group - www.msp430.ubi.pt                      */
/*                                                                        */
/* Exercise: Using the MSP-EXP430FG4618 Development Tool establish a      */
/*           data exchange between the MSP430FG4618 and MSP430F2013       */
/*           devices using the SPI mode. The MSP430FG4618 uses the USCI   */
/*           module while the MSP430F2013 uses the USI module.            */
/*                                                                        */
/*                                                                        */
/*   MCLK = SMCLK = Cal 1MHz                                              */
/*   BF = Slave Busy Flag                                                 */
/*                                                                        */
/*                                 Slave                                  */
/*                              MSP430F2013                               */
/*                          -----------------                             */
/*                         |              XIN|-                           */
/*                         |                 |                            */
/*                         |             XOUT|-                           */
/*                         |                 |                            */
/*                   LED <-|P1.0             |           Master           */
/*                         |         BF /P1.4|------->|BF                 */
/*                         |         SDI/P1.7|<-------|SIMO               */
/*                         |         SDO/P1.6|------->|SOMI               */
/*                         |        SCLK/P1.5|<-------|SPI CLOCK          */
/*                                                                        */
/* Instructions:                                                          */
/*                                                                        */
/* This lab requires to configure the USI module of MSP430F2013           */
/* as slave in SPI mode:                                                  */
/*  - Configure the control registers USCI_B (master)                     */
/*  - Configure data rate USCI_B (master)                                 */
/*  - Configure ports USCI_B (master)                                     */
/*  - Enable Rx interrupts                                                */
/*  - Configure FLL+ at 8 MHz                                             */
/*                                                                        */
/* Write to the console and verify the visualization of the characters    */
/*                                                                        */
/*                                                                        */
/*                                        Copyright by Texas Instruments  */
/**************************************************************************/



/***************************************************************************
 *                       INCLUDES
 **************************************************************************/

#include "msp430x20x3.h"

/***************************************************************************
*                        MACROS
 **************************************************************************/


#define LED_ON_STATE    0x01
#define LED_OFF_STATE   0x02
#define LED_NUL_STATE   0x00
                
#define LED_ON          0x01
#define LED_OFF         0x00

#define SET_BUSY_FLAG()   P1OUT |= 0x10;
#define RESET_BUSY_FLAG() P1OUT &= ~0x10;

#define SET_LED()       P1OUT |= 0x01;
#define RESET_DED()     P1OUT &= ~0x01;

/**************************************************************************
 *                   CONSTANTS
 *************************************************************************/


/**************************************************************************
 *                   TYPEDEFS
 *************************************************************************/


/**************************************************************************
 *                GLOBAL VARIABLES
 *************************************************************************/


unsigned char LEDState ;
unsigned char NextState;



/**************************************************************************
 *               LOCAL FUNCTIONS
 *************************************************************************/

/**************************************************************************
 * @fn      SPISetup
 *
 * @brief   Initialize SPI  at USI unit
 *
 * @param   None
 *
 * @return  None
 **************************************************************************/
void SPISetup(void)
{
  
  //1) -> initialization/re-configuration process <-BEGIN
  USICTL0 |= USISWRST;//Set UCSWRST -- needed for re-configuration process
  //1) END
  
  //2) -> Initialize all USI registersSet <- BEGIN
  // CONTROL REGISTERS
  //USICTL0 -> Control Register 0
  //  7   |  6   |  5   |  4   |   3  | 2   |  1  |    0   | 
  //-------------------------------------------------------------------------
  //USIPE7|USIPE6|USIPE5|USILSB|USIMST|USIGE|USIOE|USISWRST|
  //USIPE7    (USI SDI/SDA port enable) = 1b  ->  USI function enabled
  //USIPE6    (USI SDO/SCL port enable) = 1b  ->  USI function enabled
  //USIPE5    (USI SCLK port enable)    = 1b  ->  USI function enabled
  //USILSB    (LSB first)               = 0b  ->  MSB first
  //USIMST    (Master)                  = 0b  ->  Slave mode
  //USIGE     (Output latch control)    = 0b  ->  Output latch enable depends on shift clock
  //USIOE     (Serial data output enable)    = 1b  ->  Output enabled
  //USISWRST  (USI software reset)      = 1b  ->  Software reset
  USICTL0 = 0xE3;
  //-------------------------------------------------------------------------
  //-------------------------------------------------------------------------
  //USICTL1 -> Control Register 1
  //|   7    |  6   |   5    |  4  | 3   |  2   |    1    |  0   |
  //|------------------------------------------------------------|
  //|USICKPH |USII2C|USISTTIE|USIIE|USIAL|USISTP|USISTTIFG|USIIFG|
  //|------------------------------------------------------------|
  //USICKPH   (Clock phase select)               = 0b  ->  Data is changed on the first SCLK edge and captured on the following edge.
  //USII2C    (I2C mode enable)                  = 0b  ->  I2C mode disabled
  //USISTTIE  (START condition interrupt-enable) = 0b  ->  Not used
  //USIIE     (USI counter interrupt enable)     = 1b  -> Interrupt enabled
  //USIAL     (Arbitration lost)                 = 0b  ->  Not used
  //USISTP    (STOP condition received)          = 0b  ->  Not used
  //USISTTIFG (START condition interrupt flag)   = 0b  ->  Not used
  //USIIFG    (USI counter interrupt flag)       = 0b  ->  No interrupt pending
  USICTL1 = 0x10;                     
  //-------------------------------------------------------------------------
  //2) END 
  
  //3) Configure ports <-BEGIN
  // not used
 
  //3) END  
  
  //4) Clear UCSWRST via software -> BEGIN                      
  USICTL0 &= ~USISWRST;                     // **Initialize USCI state machine**
  //4) ->END
  
  //5) Enable interrupts -> BEGIN
  // Not in USE
  //5) -> END
}

/****************************************************************************
 * @fn      InitComm
 *
 * @brief   Communication initialization
 *
 * @param   none
 *
 * @return  None
 ****************************************************************************/
void InitComm(void)
{
    
 USICNT = 8;                 // Load bit counter, clears IFG
 USISRL = LEDState;          // set LED state
 RESET_BUSY_FLAG();          // reset busy flag
} 

/*****************************************************************************
 * @fn      LEdInit
 *
 * @brief   LED initialization
 *
 * @param   state - sets the LED's initial state
 *
 * @return  None
 *****************************************************************************/
LEdInit(unsigned char state)
{
  if (state == LED_OFF_STATE)
  {
    RESET_DED();
    LEDState = LED_OFF_STATE;
  }else
  {
    SET_LED();
    LEDState = LED_ON_STATE;
  }  
  P1DIR |= 0x11;                        //  P1.0,4 output
} 


/****************************************************************************
 * @fn      SystemInit
 *
 * @brief   Initialize the system clock to operate at 1MHz from DCO
 *
 * @param   None
 *
 * @return  None
 ****************************************************************************/
void SystemInit()
{
  
  WDTCTL = WDTPW + WDTHOLD;             // Stop watchdog timer

  BCSCTL1 = CALBC1_1MHZ;                // Set DCO
  DCOCTL = CALDCO_1MHZ;
}


/****************************************************************************
 * @fn      main
 *
 * @brief   First function called after startup.
 *
 * @param   None
 *
 * @return  None
 ***************************************************************************/
void main(void)
{
 //System clock initialization
  SystemInit();
  
  //LED state initialization
  LEdInit(LED_OFF_STATE);
  
  //USi module in SPI mode initialization
  SPISetup();
 
  //Communication initialization 
  InitComm();
  while(1)
  {
    _BIS_SR(LPM0_bits + GIE);           // Enter LPM0 w/ interrupt
  
    switch (NextState)
    {
      case 0x00 :
        break;
      default :
        LEDState = NextState;          // new state
        break;
     }    
    if (LEDState == LED_OFF_STATE)
    {
      RESET_DED();
    }else
    {
      SET_LED();
    }  
    USISRL = LEDState; // prepares new communication with new state
    RESET_BUSY_FLAG(); // clears busy flag - ready for new communication
    
  }
}

/**************************************************************************************************
 * @fn      USI_ISR
 *
 * @brief   USI interrupt service routine
 *
 * @param   None
 *
 * @return  None
 **************************************************************************************************/
#pragma vector=USI_VECTOR
__interrupt void USI_ISR(void)
{
  SET_BUSY_FLAG();            // set busy flag - slave is reasy with a new communication
  NextState = USISRL;         // read new command
  USICNT = 8;                 // Load bit counter for next TX
  _BIC_SR_IRQ(LPM4_bits);     // Exit from LPM4 on RETI
}




