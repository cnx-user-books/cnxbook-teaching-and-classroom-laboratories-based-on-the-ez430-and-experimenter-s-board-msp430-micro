/**************************************************************************/
/*              Lab10c1 - Echo test using the I2C mode (Master)           */
/*                                                                        */
/*                                                                        */
/*                                                                        */
/* MSP430 Teaching ROM                                                    */
/* Produced by: MSP430@UBI Group - www.msp430.ubi.pt                      */
/*                                                                        */
/* Exercise: Using the MSP-EXP430FG4618 Development Tool establish a      */
/*           connection between two MSP430's via the I2C bus. The master  */
/*           receives one byte from the slave. This is the master code.   */
/*           It receives a single byte as soon as a button connected on   */
/*           P1.0 was pressed.                                            */
/*                                                                        */
/*           ACLK = 32kHz, MCLK = SMCLK = TACLK = BRCLK = 1MHz            */
/*                                                                        */
/*                                                                        */
/*                                                                        */
/*                MSP430F2013                 MSP430xG461x                */
/*                    slave                       master                  */
/*              -----------------           -----------------             */
/*            -|XIN      SDA/P1.7|<------->|P3.1/UCB0SDA  XIN|-           */
/*       32kHz |                 |         |                 | 32kHz      */
/*            -|XOUT             |         |             XOUT|-           */
/*             |         SCL/P1.6|<------->|P3.2/UCB0SCL     |            */
/*             |                 |         |                 |            */
/*                                                                        */
/*                                                                        */
/*                                                                        */
/*                                                                        */
/* Instructions:                                                          */
/*                                                                        */
/* This lab requires to configure the I2C module of the MSP430FG4618 as   */
/* master and the MSP430F2013 as slave, both I2C mode:                    */
/*  - Configure the control registers USCI_B (master)                     */
/*  - Configure data rate USCI_B (master)                                 */
/*  - Configure slave address (master)                                    */
/*  - Configure ports                                                     */
/*  - Enable Rx interrupts                                                */
/*                                                                        */
/*                                                                        */
/*                                                                        */
/*                                        Copyright by Texas Instruments  */
/**************************************************************************/



/***************************************************************************
 *                        INCLUDES
 **************************************************************************/

#include "msp430xG46x.h"

/***************************************************************************
*                          MACROS
 **************************************************************************/


/***************************************************************************
 *                        CONSTANTS
 **************************************************************************/


/***************************************************************************
 *                        TYPEDEFS
 **************************************************************************/


/***************************************************************************
 *                        GLOBAL VARIABLES
 **************************************************************************/

volatile unsigned char RxBuffer;


/***************************************************************************
 *                        LOCAL FUNCTIONS
 **************************************************************************/

/***************************************************************************
 * @fn      I2CSetUp
 *
 * @brief   Initialize I2C device at USCI unit
 *
 * @param   None
 *
 * @return  None
 **************************************************************************/
void I2CSetUp(void)
{
  //1) -> initialization/re-configuration process <-BEGIN
  UCB0CTL1 |= UCSWRST;//Set UCSWRST -- needed for re-configuration process
  //1) END
  
  //2) -> Initialize all USCI registers Set <- BEGIN
  // CONTROL REGISTERS
  //UCB0CTL0 -> Control Register 0
  //  7  |  6    | 5  |  4   |  3  |  2-1  |  0   | 
  //------------------------------------------------------------------------
  //UCA10|UCSLA10|UCMM|Unused|UCMST|UCMODEx|UCSYNC|
  //UCA10 (Own addressing mode)       = 0b -> Own address is a 7-bit address
  //UCSLA10 (Slave addressing)        = 0b ->  Address slave with 7-bit address
  //UCMM (Multi-master)               = 0b ->  Single master
  //Unused 
  //UCMST (Master mode)               = 1b  ->  Master mode
  //UCMODEx (USCI mode)               = 11b ->  I2C Mode
  //UCSYNC (Synchronous mode enable)  = 1b  ->  Synchronous mode
  
  UCB0CTL0 = 0x0F;
  //------------------------------------------------------------------------
  //------------------------------------------------------------------------
  //UCB0CTL1 -> Control Register 1
  //   6-7  |   5   |   4   |   3  |   2    |   1   |    0  |
  //---------------------------------------------------------
  //UCSSELx |Unused|UCTR|UCTXNACK|UCTXSTP|UCTXSTT|UCSWRST|
  //---------------------------------------------------------
  //UCSSELx (USCI clock source select) = 10b ->  SMCLK
  //Unused 
  //UCTR (Transmitter/Receiver) = 0b ->  Receiver
  //UCTXNACK (Transmit a NACK) = 0b ->  Acknowledge normally
  //UCTXSTP (Transmit STOP condition) = 0b ->  No STOP generated
  //UCTXSTT (Transmit START condition) = 0b ->  Do not generate START
  //UCSWRST (Software reset) = 1b  -> Enabled
  UCB0CTL1 = 0x81;                    
  //------------------------------------------------------------------------
  // DATA RATE 
  // data rate -> fSCL = SMCLK/11 = 95.3kHz
  //------------------------------------------------------------------------
  UCB0BR0 = 0x0B; // fSCL = SMCLK/11 = 95.3kHz 
  UCB0BR1 = 0x00;
  
  //------------------------------------------------------------------------
  // SLAVE ADRESS
  //
  UCB0I2CSA = 0x48;                         // Slave Address is 048h
  //------------------------------------------------------------------------
  //2) END 
  
  //3) Configure ports <-BEGIN
  P3SEL |=0x06;  // Assign I2C pins to USCI_B0
 
  //3) END  
  
  //4) Clear UCSWRST via software -> BEGIN                      
  UCB0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
  //4) ->END
  
  //5) Enable interrupts -> BEGIN
  IE2 |= UCB0RXIE;                          // Enable RX interrup 
  //5) -> END
}


/***************************************************************************
 * @fn      P1SetUp
 *
 * @brief   Initialize P1 for switch operation
 *
 * @param   None
 *
 * @return  None
 **************************************************************************/
void P1SetUp()
{
   
  P1IES|=0x01;
  P1IE|=0x01;
}


/**************************************************************************
 * @fn      SystemInit
 *
 * @brief   Initialize the system clock to operate at 1048kHz from DCO
 *
 * @param   None
 *
 * @return  None
 **************************************************************************/
void SystemInit()
{
  unsigned int i;
  
  WDTCTL = WDTPW+WDTHOLD;               // Stop watchdog timer
  FLL_CTL0 |= XCAP14PF;                 // Configure load caps
  
  // Wait for xtal to stabilize
  do
  {
    IFG1 &= ~OFIFG;                     // Clear OSCFault flag
    for (i = 0x47FF; i > 0; i--);       // Time for flag to set
  }
  while ((IFG1 & OFIFG));               // OSCFault flag still set?
}

/**************************************************************************
 * @fn      main
 *
 * @brief   First function called after startup.
 *
 * @param   None
 *
 * @return  None
 **************************************************************************/
void main(void)
{
  //System initialization
  SystemInit();
 
  // Port 1 (switch SW1) configuration
  P1SetUp();
  
  //USCI module in I2C mode configuration
  I2CSetUp();
  
  while (1)
  {
    __bis_SR_register(CPUOFF + GIE);   // Enter LPM0 w/ interrupts
                                       // Remain in LPM0 until all data is received
  }
}


/****************************************************************************
 * @fn      I2CTX_ISR
 *
 * @brief    The USCIB0 data ISR is used to move received data from the I2C slave
 *           to the MSP430 memory (RxBuffer variable).
 *
 * @param   None
 *
 * @return  None
 ***************************************************************************/
#pragma vector = USCIAB0TX_VECTOR
__interrupt void I2CTX_ISR(void)
{
  RxBuffer = UCB0RXBUF;                 // Move RX data to address PRxData
  __bic_SR_register_on_exit(CPUOFF);    // Exit LPM0 <<<<<<PUT BREAKPOINT HERE>>>>>
}

/****************************************************************************
 * @fn      P1_ISR
 *
 * @brief     Pushing button SW1 starts reading I2C data
 *
 * @param   None
 *
 * @return  None
 ***************************************************************************/
#pragma vector=  PORT1_VECTOR
__interrupt void P1_ISR (void)
{
  unsigned int i; 
   i=1500;                            //Delay, button debounce 
   do (i--);
   while (i !=0);
  
  P1IFG=0x00;                         // clear interrupt flags
  UCB0CTL1 |= UCTXSTT;                // I2C start condition
  while(UCTXSTT&UCB0CTL1);            // wait till UCTXSTT bit is reset
 // UCB0CTL1 |= UCTXSTP;                // Generate I2C stop condition
}



