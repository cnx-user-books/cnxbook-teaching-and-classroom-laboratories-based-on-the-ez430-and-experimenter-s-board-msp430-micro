/**************************************************************************/
/*              Lab10b1 - Echo test using the SPI mode (Master)           */
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
/* ACLK = 32.768kHz, MCLK = SMCLK = default DCO ~1048k, BRCLK = SMCLK/2   */
/* BF = Slave Busy Flag                                                   */
/*                                                                        */
/*                             MASTER                                     */
/*                          MSP430FG4618                                  */
/*                       -----------------                                */
/*                   /|\|              XIN|-                              */
/*       SLAVE        | |                 |  32kHz xtal                   */
/*                    --|RST          XOUT|-                              */
/*                      |                 |                               */
/*   SLAVE BF      |--->|P3.0/BF          |                               */
/*   SLAVE Data In |<---|P3.1/UCB0SIMO    |                               */
/*   Slave Data Out|--->|P3.2/UCB0SOMI    |                               */
/*   Slave Clock In|<---|P3.3/UCB0CLK     |                               */
/*                      |                 |                               */
/*                                                                        */
/* Instructions:                                                          */
/*                                                                        */
/* This lab requires to configure the USCI module of MSP430FG4618         */
/* as master in SPI mode:                                                 */
/*  - Configure the control registers USCI_B (master)                     */
/*  - Configure data rate USCI_B (master)                                 */
/*  - Configure ports USCI_B (master)                                     */
/*  - Enable Rx interrupts                                                */
/*  - Configure FLL+ at 8 MHz                                             */
/*                                                                        */
/*                                                                        */
/*                                                                        */
/*                                        Copyright by Texas Instruments  */
/**************************************************************************/



/***************************************************************************
 *                         INCLUDES
 **************************************************************************/
#include  "msp430xG46x.h"


/***************************************************************************
*                           MACROS
 **************************************************************************/


#define LED_ON_STATE    0x01
#define LED_OFF_STATE   0x02
#define LED_NUL_STATE   0x00
                
#define LED_ON          0x01
#define LED_OFF         0x00

/***************************************************************************
 *                        CONSTANTS
 **************************************************************************/


/***************************************************************************
 *                         TYPEDEFS
 **************************************************************************/


/***************************************************************************
 *                    GLOBAL VARIABLES
 **************************************************************************/


/***************************************************************************
 *                     LOCAL FUNCTIONS
 **************************************************************************/


/***************************************************************************
 * @fn      SPISetup
 *
 * @brief   Initialize SPI at USCI unit
 *
 * @param   None
 *
 * @return  None
 ***************************************************************************/
void SPISetup(void)
{
  //1) -> initialization/re-configuration process <-BEGIN
  UCB0CTL1 |= UCSWRST;//Set UCSWRST -- needed for re-configuration process
  //1) END
  
  //2) -> Initialize all USCI registers Set <- BEGIN
  // CONTROL REGISTERS
  //UCB0CTL0 -> Control Register 0
  //  7   |  6  |  5  |  4   |  3  |  2-1   |  0   | 
  //-------------------------------------------------------------------------
  //UCCKPH|UCCKPL|UCMSB|UC7BIT|UCMST|UCMODEx|UCSYNC|
  //UCCKPH (Clock phase)              = 0b  ->  Data is changed on the first UCLK edge and captured on the following edge.
  //UCCKPL (Clock polarity)           = 0b  ->  The inactive state is low
  //UCMSB (MSB first select)          = 1b  ->  MSB first
  //UC7BIT (Character length)         = 0b  ->  8-bit data
  //UCMST (Master mode)               = 1b  ->  Master mode
  //UCMODEx (USCI mode)               = 00b ->  3-Pin SPI
  //UCSYNC (Synchronous mode enable)  = 1b  ->  Synchronous mode
  UCB0CTL0 = 0x29;
  //-------------------------------------------------------------------------
  //-------------------------------------------------------------------------
  //UCA0CTL1 -> Control Register 1
  //   6-7  |   5   |   4   |   3  |   2    |   1   |    0  |
  //---------------------------------------------------------
  //UCSSELx |                  Unused               |UCSWRST|
  //---------------------------------------------------------
  //UCSSELx (USCI clock source select)= 10b ->  SMCLK
  //UCSWRST (Software reset)          = 1b  ->  normally set by a PUC
  UCB0CTL1 = 0x81;                     
  //-------------------------------------------------------------------------
  // DATA RATE
  // Data rate = SMCLK/2 ~= 500kHz
  // UCA0BR1 = 0x00 & UCA0BR0 = 0x02
  //-------------------------------------------------------------------------
  UCB0BR0 = 0x02;                           
  UCB0BR1 = 0x00;
  //-------------------------------------------------------------------------
  //2) END 
  
  //3) Configure ports <-BEGIN
  P3SEL |= 0x0E; // P3.1,P3.2,P3.3 option select
 
  //3) END  
  
  //4) Clear UCSWRST via software -> BEGIN                      
  UCB0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
  //4) ->END
  
  //5) Enable interrupts -> BEGIN
  // Not used
  //5) -> END
}


/*****************************************************************************
 * @fn      SPIGetState
 *
 * @brief   Reads the state of the slave unit
 *
 * @param   None
 *
 * @return  Slave unit state
 *****************************************************************************/
unsigned char SPIGetState(void)
{
    while((P3IN & 0x01));             // Verifies busy flag
    IFG2 &= ~UCB0RXIFG;    
    UCB0TXBUF = LED_NUL_STATE;        // Dummy write to start SPI
    while (!(IFG2 & UCB0RXIFG));      // USCI_B0 TX buffer ready?
    return UCB0RXBUF;                  
}

/******************************************************************************
 * @fn      SPISetState
 *
 * @brief   Actualizes the slave unit state
 *
 * @param   State - new state of the slave unit
 *
 * @return  None
 ******************************************************************************/
void SPISetState(unsigned char State)
{
   while(P3IN & 0x01);                // Verifies busy flag
   IFG2 &= ~UCB0RXIFG;  
   UCB0TXBUF = State;                 //  write new state
   while (!(IFG2 & UCB0RXIFG));       // USCI_B0 TX buffer ready?
}


/*******************************************************************************
 * @fn      BasicTimerCfg
 *
 * @brief   Initialize the Basic Timer1 to wake up every 2s
 *
 * @param   None
 *
 * @return  None
 *******************************************************************************/
void BasicTimerCfg(void)
{
  IE2 |= BTIE;                          // Enable BT interrupt
  
  BTCTL = BTDIV+BTIP2+BTIP1+BTIP0;      // 2 sec Interrupt
}

/*******************************************************************************
 * @fn      SystemInit
 *
 * @brief   Initialize the system clock to operate at 1048kHz from DCO
 *
 * @param   None
 *
 * @return  None
 *******************************************************************************/
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

/********************************************************************************
 * @fn      main
 *
 * @brief   First function called after startup.
 *
 * @param   None
 *
 * @return  None
 *******************************************************************************/
void main(void)
{
  //System initialization 
  SystemInit();

  //Basic Timer1 initialization
  BasicTimerCfg();
 
  //USCI modulee in SPI mode configuration
  SPISetup();
  
  //main TASK
  while(1)
  {
    _BIS_SR(LPM3_bits + GIE);           // Enter LPM3 w/ interrupts

    switch (SPIGetState())
    {
    case  LED_OFF_STATE :
          SPISetState(LED_ON_STATE);
          break;
    case  LED_ON_STATE  :
          SPISetState(LED_OFF_STATE);
          break;
    default             :
          break;
    }   
  }
}

/*******************************************************************************
 * @fn      BTimerISR
 *
 * @brief   Basic Timer1 Interrupt Service Routine (ISR).
 *
 * @param   None
 *
 * @return  None
 *******************************************************************************/
#pragma vector=BASICTIMER_VECTOR
__interrupt void BTimerISR(void)
{
    _BIC_SR_IRQ(LPM3_bits);             // Exit from LPM3 on RETI
}
