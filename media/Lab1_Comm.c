/**************************************************************************/
/*       Lab10a - Echo test using the USCI module in UART mode            */
/*                                                                        */
/*                                                                        */
/*                                                                        */
/* MSP430 Teaching ROM                                                    */
/* Produced by: MSP430@UBI Group - www.msp430.ubi.pt                      */
/*                                                                        */
/* Exercise: Using the MSP-EXP430FG4618 Development Tool and the          */
/*           MSP430FG4618 device use the USCI module in UART mode         */
/*           connected to a PC’s I/O console. When the connection is      */
/*           established, the characters sequence writed at the console   */
/*           through the keyboard will be visualized on the console.      */
/*                                                                        */
/* Instructions:                                                          */
/*                                                                        */
/* This lab requires to configure the USCI module in UART mode:           */
/*  - Configure USCI control registers                                    */
/*  - Configure baud rate generation                                      */
/*  - Configure ports                                                     */
/*  - Enable Rx interrupts                                                */
/*  - Configure FLL+ at 8 MHz                                             */
/*                                                                        */
/* Write to the console and verify the visualization of the characters    */
/*                                                                        */
/*                                                                        */
/*                                        Copyright by Texas Instruments  */
/**************************************************************************/



/***************************************************************************
 *                        INCLUDES
 **************************************************************************/

#include <msp430xG46x.h>

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


/***************************************************************************
 * @fn      UARTSetup
 *
 * @brief   Initialize UART device at USCI unit
 *
 * @param   None
 *
 * @return  None
 **************************************************************************/
void UARTSetup(void)
{
  //1) -> initialization/re-configuration process <-BEGIN
  UCA0CTL1 |= UCSWRST;//Set UCSWRST -- needed for re-configuration process
  //1) END
  
  //2) -> Initialize all USCI registersSet <- BEGIN
  // CONTROL REGISTERS
  //UCA0CTL0 -> Control Register 0
  //  7  |  6  |  5  |  4   |  3  |  2-1  |  0   | 
  //------------------------------------------------------------------------
  //UCPEN|UCPAR|UCMSB|UC7BIT|UCSPB|UCMODEx|UCSYNC|
  //UCPEN (Parity)                    = 0b  ->  Parity disabled
  //UCPAR (Parity select)             = 0b  ->  Odd parity "Not importante"
  //UCMSB (MSB first select)          = 0b  ->  LSB first
  //UC7BIT (Character length)         = 0b  ->  8-bit data
  //UCSPB (Stop bit select)           = 0b  ->  One stop bit
  //UCMODEx (USCI mode)               = 00b ->  UART Mode
  //UCSYNC (Synchronous mode enable)  = 0b  ->  Asynchronous mode
  UCA0CTL0 = 0x00;
  //------------------------------------------------------------------------
  //------------------------------------------------------------------------
  //UCA0CTL1 -> Control Register 1
  //   6-7  |   5   |   4   |   3  |   2    |   1   |    0  |
  //---------------------------------------------------------
  //UCSSELx |UCRXEIE|UCBRKIE|UCDORM|UCTXADDR|UCTXBRK|UCSWRST|
  //---------------------------------------------------------
  //UCSSELx (USCI clock source select)= 10b ->  SMCLK
  //UCRXEIE (interrupt-enable)        = 0b  ->  Erroneous characters rejected
  //UCBRKIE (interrupt-enable)        = 0b  ->  Received break characters set
  //UCDORM (sleep mode)               = 0b  ->  Not dormant
  //UCTXADDR (Transmit address)       = 0b  ->  Next frame transmitted is data
  //UCTXBRK (Transmit break)          = 0b  ->  Next frame transmitted is not a break
  //UCSWRST (Software reset)          = 1b  ->  normaly Set by a PUC
  UCA0CTL1 = 0x81;                     
  //------------------------------------------------------------------------
  // BAUD RATE GENERATION
  // Prescaler = 8MHz/(16 x 9600 ) = 52 = 0x34
  // UCA0BR1 = 0x00 & UCA0BR0 = 0x34
  // UCBRS0 = 0x00
  //------------------------------------------------------------------------
  UCA0BR0 = 0x34;                           // 9600 from 8MHz -> SMCLK  
  UCA0BR1 = 0x00;
  //UCAxMCTL Modulation Control Register
  // 7-4  | 3-1  |  0   |
  //UCBRFx|UCBRSx|UCOS16|
  //UCBRFx (First modulation stage)   = 0001b -> from table 19-4
  //UCBRSx (Second modulation stage)  = 000b  -> from table 19-4
  //UCOS16 (Oversampling mode)        = 1b    -> Enabled
  UCA0MCTL = 0x11;
  //------------------------------------------------------------------------
  //2) END 
  
  //3) Configure ports <-BEGIN
  P2SEL |= 0x30;//P2.4,P2.5 = USCI_A0 TXD/RXD
  //P2DIR |= 0x10;
 
  //3) END  
  
  //4) Clear UCSWRST via software -> BEGIN                      
  UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
  //4) ->END
  
  //5) Enable interrupts -> BEGIN
  IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt
  //5) -> END
}

/****************************************************************************
 * @fn      CLKInit
 *
 * @brief   Initialize the system clock
 *
 * @param   None
 *
 * @return  None
 ****************************************************************************/
void CLKInit(void)
{
  /* disable watchdog timer */
  WDTCTL = WDTPW | WDTHOLD;
  
/*----------------------------------------------------------------------------
   *  Configure MCU clock.
   *
   *  F = D * (N + 1) * faclk  = 7.9954 x 10e6
   *
   *  N = 121
   *  D = 2
   *  facklk = 32768
   *
*----------------------------------------------------------------------------*/
  SCFQCTL   = 121;                  /* N=121 */
  SCFI0     = FLLD_2 | FN_4;        /* set D=2, range=1.3-12.1MHz */
  FLL_CTL0  = DCOPLUS | XCAP14PF;   /* enable divider, set osc capacitance = ~8pF */
}  

/*****************************************************************************
 * @fn      WriteChar
 *
 * @brief   write a single char at RS232 USCII channel
 *
 * @param   ch - the character to write
 *
 * @return  None
 ****************************************************************************/
void WriteChar(char ch)
{
 
    while (!(IFG2 & UCA0TXIFG));     // USART1 TX buffer ready?
    UCA0TXBUF  = ch;                 // Output character
  
}

/*****************************************************************************
 * @fn      main
 *
 * @brief   First function called after startup.
 *
 * @param   None
 *
 * @return  None
 ****************************************************************************/
void main(void)
{
  //Initialize system clock
  CLKInit();
  
  //Initialize comuuninication interface
  UARTSetup();
  
  // Allow interrupts
  _BIS_SR(GIE);  
  
  
  while(1){}// No Return from here
}

/*****************************************************************************
 * @fn          USCI_UART_0_RX_RSI
 *
 * @brief       - 
 *
 * @param       none
 *
 * @return      none
 *****************************************************************************
 */
#pragma vector = USCIAB0RX_VECTOR
__interrupt void USCI_UART_0_RX_RSI(void)
{
  char ch;
  ch = UCA0RXBUF;
  if (ch == 13) //if equal to carriage return (CR)
  {
    WriteChar('\n'); //new line
    WriteChar('\r'); //position at begin
  }else
    WriteChar(ch); // write the characer
}
