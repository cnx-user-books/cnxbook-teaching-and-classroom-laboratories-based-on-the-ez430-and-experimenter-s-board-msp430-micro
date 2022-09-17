/**************************************************************************/
/*              Lab10c2 - Echo test using the I2C mode (Slave)            */
/*                                                                        */
/*                                                                        */
/*                                                                        */
/* MSP430 Teaching ROM                                                    */
/* Produced by: MSP430@UBI Group - www.msp430.ubi.pt                      */
/*                                                                        */
/* Exercise: Using the MSP-EXP430FG4618 Development Tool establish a      */
/*           connection between two MSP430's via the I2C bus. The I2C     */
/*           Slave communicates with I2C Master using the USI. Slave data */
/*           is sent and increments from 0x00 with each transmitted byte  */
/*           which is verified by the Master. This is the Slave code.     */
/*           LED off for address or data Ack; LED on for address or       */
/*           data NAck.                                                   */
/*                                                                        */
/*           ACLK = n/a, MCLK = SMCLK = Calibrated 1MHz                   */
/*                                                                        */
/*                                                                        */
/*                                                                        */
/*                  Slave                      Master                     */
/*               MSP430F20x3                MSP430FG4618                  */
/*             -----------------          -----------------               */
/*         /|\|              XIN|-    /|\|              XIN|-             */
/*          | |                 |      | |                 |              */
/*          --|RST          XOUT|-     --|RST          XOUT|-             */
/*            |                 |        |                 |              */
/*      LED <-|P1.0             |        |                 |              */
/*            |                 |        |                 |              */
/*            |         SDA/P1.7|------->|P3.1/UCB0SDA     |              */
/*            |         SCL/P1.6|<-------|P3.2/UCB0SCL     |              */
/*                                                                        */
/*                                                                        */
/*                                                                        */
/*                                                                        */
/* Instructions:                                                          */
/*                                                                        */
/* This lab requires to configure the I2C module of the MSP430FG4618 as   */
/* master and the MSP430F2013 as slave, both I2C mode:                    */
/*  - Configure the USI control registers (slave)                         */
/*  - Configure USI bit counter Register (slave)                          */
/*                                                                        */
/*                                                                        */
/*                                                                        */
/*                                        Copyright by Texas Instruments  */
/**************************************************************************/




/***************************************************************************
 *                        INCLUDES
 **************************************************************************/
#include  <msp430x20x3.h>
/***************************************************************************
*                          MACROS
 **************************************************************************/
#define I2C_IDLE            0x00
#define I2C_RX_ADDRESS      0x02
#define I2C_PROCESS_ADRESS  0x04
#define I2C_RX              0x06
#define I2C_TX              0x08
#define I2C_RECEIVE_ACK     0x0A

/***************************************************************************
 *                        CONSTANTS
 **************************************************************************/


/***************************************************************************
 *                        TYPEDEFS
 **************************************************************************/


/***************************************************************************
 *                        GLOBAL VARIABLES
 **************************************************************************/

char SlaveData = 0;                     // Variable for transmitted data
char SlaveAddr = 0x90;                  // Address is 0x48<<1 for R/W
unsigned char I2CState;                 // State variable

/***************************************************************************
 *                        LOCAL FUNCTIONS
 **************************************************************************/



/***************************************************************************
 * @fn      I2CSetUp
 *
 * @brief   Initialize I2C device at USI unit
 *
 * @param   None
 *
 * @return  None
 **************************************************************************/
void I2CSetUp(void)
{
  
 // USICTL0 = USIPE6+USIPE7+USISWRST;    // Port & USI mode setup
 // USICTL1 = USII2C+USIIE+USISTTIE;     // Enable I2C mode & USI interrupts
 // USICKCTL = USICKPL;                  // Setup clock polarity
 // USICNT |= USIIFGCC;                  // Disable automatic clear control
 // USICTL0 &= ~USISWRST;                // Enable USI
 // USICTL1 &= ~USIIFG;                  // Clear pending flag
  
  //1) -> initialization/re-configuration process <-BEGIN
  USICTL0 |= USISWRST;//Set UCSWRST -- needed for re-configuration process
  //1) END
  
  //2) -> Initialize all USCI registers Set <- BEGIN
  // CONTROL REGISTERS
  //USICTL0 -> Control Register 0
  //  7   |  6   |  5   |  4   |   3  | 2   |  1  |    0   | 
  //------------------------------------------------------------------------
  //USIPE7|USIPE6|USIPE5|USILSB|USIMST|USIGE|USIOE|USISWRST|
  //USIPE7    (USI SDI/SDA port enable) = 1b  ->  USI function enabled
  //USIPE6    (USI SDO/SCL port enable) = 1b  ->  USI function enabled
  //USIPE5    (USI SCLK port enable)    = 0b  ->  SCLK disable
  //USILSB    (LSB first)               = 0b  ->  MSB first
  //USIMST    (Master)                  = 0b  ->  Slave mode
  //USIGE     (Output latch control)    = 0b  ->  Output latch enable depends on shift clock
  //USIOE     (Serial data output enable)     = 0b  ->  Output enabled
  //USISWRST  (USI software reset)      = 1b  ->  Software reset
  
  USICTL0 = 0XC1;
  //------------------------------------------------------------------------
  //------------------------------------------------------------------------
  //|   7    |  6   |   5    |  4  | 3   |  2   |    1    |  0   |
  //|------------------------------------------------------------|
  //|USICKPH |USII2C|USISTTIE|USIIE|USIAL|USISTP|USISTTIFG|USIIFG|
  //|------------------------------------------------------------|
  //USICKPH   (Clock phase select)               = 0b  ->  Data is changed on the first SCLK edge and captured on the following edge.
  //USII2C    (I2C mode enable)                  = 1b  ->  I2C mode enabled
  //USISTTIE  (START condition interrupt-enable) = 1b  ->  Interrupt on START condition enabled
  //USIIE     (USI counter interrupt enable)     = 1b  ->  Interrupt enabled
  //USIAL     (Arbitration lost)                 = 0b  ->  Not used
  //USISTP    (STOP condition received)          = 0b  ->  Not used
  //USISTTIFG (START condition interrupt flag)   = 0b  ->  Not used
  //USIIFG    (USI counter interrupt flag)       = 0b  ->  No interrupt pending
  USICTL1 = 0x70;                    
  //------------------------------------------------------------------------
  //------------------------------------------------------------------------
  //USICKCTL, USI Clock Control Register
  //   7-5  |   4-2   |   1   |   0    |
  //------------------------------------
  //USIDIVx |USISSELx |USICKPL|USISWCLK|
  //------------------------------------
  //USIDIVx (Clock divider)   = 000b -> Divide by 1
  //USISSELx (Clock source)   = 000b -> SCLK 
  //USICKPL (Clock polarity)  = 1b   -> Inactive state is high
  //USISWCLK (Software clock) = 0b (not used)
  USICKCTL = 0x02;                  
  //------------------------------------------------------------------------
  //------------------------------------------------------------------------
  //USICNT, USI Bit Counter Register
  //     7   |   6   |     5    |  4-0  |
  //------------------------------------
  //USISCLREL| USI16B |USIIFGCC |USICNTx|
  //------------------------------------
  //USISCLREL (SCL release)                     = 0b -> SCL line is held low if USIIFG is set
  //USI16B (16-bit shift register enable)       = 0b -> 8-bit shift register mode
  //USIIFGCC (USI interrupt flag clear control) = 1b -> USIIFG is not cleared automatically
  //USICNTx (USI bit count)                     = 00000b (not relevant at this moment)
  USICNT |= 0x20;                  
  //2) END 
  
  //3) Configure ports <-BEGIN
  //   
  //3) END  
  
  //4) Clear UCSWRST via software -> BEGIN                      
  USICTL0 &= ~USISWRST;                    // **Initialize USI state machine**
  //4) ->END
  
  //5) Interrupts -> BEGIN
  USICTL1 &= ~USIIFG;                  // Clear pending flag 
  //5) -> END
}


/**************************************************************************************************
 * @fn      P1SetUp
 *
 * @brief   Initialize P1 for I2C and LED3 operation
 *
 * @param   None
 *
 * @return  None
 **************************************************************************************************/
void P1SetUp()
{
   
  P1OUT = 0xC0;                        // P1.6 & P1.7 Pullups
  P1REN |= 0xC0;                       // P1.6 & P1.7 Pullups
  P1DIR = 0xFF;                        // Unused pins as outputs
}


/**************************************************************************************************
 * @fn      SystemInit
 *
 * @brief   Initialize the system clock to operate at 1MHz from DCO
 *
 * @param   None
 *
 * @return  None
 **************************************************************************************************/
void SystemInit()
{
  WDTCTL = WDTPW + WDTHOLD;            // Stop watchdog

  BCSCTL1 = CALBC1_1MHZ;               // Set DCO
  DCOCTL = CALDCO_1MHZ;
}


/**************************************************************************************************
 * @fn      main
 *
 * @brief   First function called after startup.
 *
 * @param   None
 *
 * @return  None
 **************************************************************************************************/
void main(void)
{
  //System initialization
  SystemInit();

  //Port1 initialization
  P1SetUp();
  
  //USI module in I2C mode initialization 
  I2CSetUp();
  
  I2CState = I2C_IDLE;                // init I2C slave state machine
  SlaveData = 0;                      // init slave data
  
  _EINT();                            // enable interrupts

  while(1)
  {
    LPM0;                             // CPU off, await USI interrupt
  }
}

/**************************************************************************************************
 * @fn      USI_I2C_ISR
 *
 * @brief     USI interrupt service routine
 *
 * @param   None
 *
 * @return  None
 **************************************************************************************************/
#pragma vector = USI_VECTOR
__interrupt void USI_I2C_ISR (void)
{
  if (USICTL1 & USISTTIFG)             // Start condition?
  {
    P1OUT |= 0x01;                     // LED on: Sequence start
    I2CState = I2C_RX_ADDRESS;                    
  }

  switch(__even_in_range(I2CState,12))
    {
      case I2C_IDLE: 
              break;

      case I2C_RX_ADDRESS: //receive Address
              USICNT = (USICNT & 0xE0) + 0x08;         // Bit counter = 8, RX Address
              USICTL1 &= ~USISTTIFG;                   // Clear start flag
              I2CState = I2C_PROCESS_ADRESS;           // Go to next state: check address
              break;

      case I2C_PROCESS_ADRESS:                         // Process Address and send (N)Ack
              if (USISRL & 0x01)                       // If read command ...
                SlaveAddr++;                           // Save R/W bit
              USICTL0 |= USIOE;                        // SDA = output
              if (USISRL == SlaveAddr)                 // Address match?
              {
                USISRL = 0x00;                         // Send Ack
                P1OUT &= ~0x01;                        // LED off
                I2CState = I2C_TX;                     // Go to next state: TX data
              }
              else
              {
                USISRL = 0xFF;                         // Send NAck
                P1OUT |= 0x01;                         // LED on: error
                I2CState = I2C_RX;                     // Go to next state: prep for next Start
              }
              USICNT |= 0x01;                          // Bit counter = 1, send (N)Ack bit
              break;

      case I2C_RX: // Prep for Start condition
              USICTL0 &= ~USIOE;                       // SDA = input
              SlaveAddr = 0x90;                        // Reset slave address
              I2CState = I2C_IDLE;                     // Reset state machine
              break;

      case I2C_TX: // Send Data byte
              USICTL0 |= USIOE;                        // SDA = output
              USISRL = SlaveData;                      // Send data byte
              USICNT |=  0x08;                         // Bit counter = 8, TX data
              I2CState = I2C_RECEIVE_ACK;              // Go to next state: receive (N)Ack
              break;

      case I2C_RECEIVE_ACK:// Receive Data (N)Ack
              USICTL0 &= ~USIOE;                       // SDA = input
              USICNT |= 0x01;                          // Bit counter = 1, receive (N)Ack
              I2CState = 12;                           // Go to next state: check (N)Ack
              break;

      case 12:// Process Data Ack/NAck
              if (USISRL & 0x01)                       // If Nack received...
              {
                P1OUT |= 0x01;                         // LED on: error
              }
              else                                     // Ack received
              {
                P1OUT &= ~0x01;                        // LED off
              }
              SlaveData++;                             // Increment Slave data 
              // Prep for Start condition
              USICTL0 &= ~USIOE;                       // SDA = input
              SlaveAddr = 0x90;                        // Reset slave address
              I2CState = I2C_IDLE;                     // Reset state machine
              break;
    }

  USICTL1 &= ~USIIFG;                                  // Clear pending flag
}
