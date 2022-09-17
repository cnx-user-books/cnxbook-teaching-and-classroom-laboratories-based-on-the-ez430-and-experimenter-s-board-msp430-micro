/**************************************************************************/
/*                   Lab5B - Data acquisition using ADC12                 */
/*                                                                        */
/*                                                                        */
/*                                                                        */
/* MSP430 Teaching ROM                                                    */
/* Produced by: MSP430@UBI Group - www.msp430.ubi.pt                      */
/*                                                                        */
/* Exercise: Using the MSP-EXP430FG4618 Development Tool and the          */
/*           MSP430FG4618 device explore the ADC12 and OA modules. The    */
/*           test voltage is generated in the DAC12 module (channel 0)    */
/*           modifying the DAC12_ODAT register's value. The analogue      */
/*           voltage is amplified by the OA module. Afterwards this       */
/*           signal is applied to ADC12 input to be converted. Compare    */
/*           the DAC12_ODAT and the ADC12MEM0 values.                     */
/*                                                                        */
/* Instructions:                                                          */
/* The ADC12 sample-and-hold time is configured to be 64 ADC12CLK cycles. */
/* It uses the internal reference generator voltage, Vref+ = 2,5 V,       */
/* performing a single-channel, single-conversion at channel 1,           */  
/* using as clock source the ADC12OSC/1.                                  */
/*                                                                        */
/* The OA module is configured as Non-inverting PGA with unity gain.      */
/* The Non-inverting input is the DAC0 internal while the output is       */
/* connected to internal/external A1.                                     */
/*                                                                        */
/*  Complete the code below:                                              */
/*   - Configure ADC12 control registers                                  */
/*   - Configure OA control registers                                     */
/*                                                                        */
/*                                                                        */
/*                                        Copyright by Texas Instruments  */
/**************************************************************************/



/*************************************************************************/
/*                                 INCLUDES                              */
/*************************************************************************/
#include  "msp430xG46x.h"


/*************************************************************************/
/*                                  MACROS                               */
/*************************************************************************/

/*************************************************************************/
/*                                CONSTANTS                              */
/*************************************************************************/


/*************************************************************************/
/*                                 TYPEDEFS                              */
/*************************************************************************/


/*************************************************************************/
/*                             GLOBAL VARIABLES                          */
/*************************************************************************/


/*************************************************************************/
/*                             LOCAL FUNCTIONS                           */
/*************************************************************************/

/*************************************************************************/
// @fn      DAC12Init
//
// @brief   Initialize  DAC12
//
// @param   None
//
// @return  None
/*************************************************************************/
void DAC12Init(void)
{
  // DAC12 configuration
  DAC12_0DAT = 0x00;                            // DAC_0 output 0V

  ADC12CTL0 = REF2_5V | REFON;                  // Internal 2.5V ref on
  
  DAC12_0CTL = DAC12IR | DAC12AMP_5 | DAC12ENC; // DAC_0 -> P6.6
  				                // DAC_1 -> P6.7
                                                // DAC reference Vref
                                                // 12 bits resolution
                                                // Immediate load
                                                // DAC full scale output
                                                // Medium speed/current                                             
                                                // Straight binary
                                                // Not grouped 
}

/*************************************************************************/
// @fn      ADC12Init
//
// @brief   Initialize  ADC12
//
// @param   None
//
// @return  None
/*************************************************************************/
void ADC12Init(void)
{
   
 /* Set up the ADC12 to sample a single-channel repeat-conversion */ 
  ADC12CTL0 &= ~ENC;                             //enable configuration 
  //ADC12CTL0 -> Control Register 0
  //15-12| 11-8 | 7 |   6   |  5  |   4   |    3    |     2    | 1 |   0   |
  //----------------------------------------------------------------------------
  //SHT1x|SHT0x |MSC|REF2_5V|REFON|ADC12ON|ADC12OVIE|ADC12TOVIE|ENC|ADC12SC|
  //SHT1x (Sample-and-hold time)                            = 0000b  ->  Not valid
  //SHT0x (Sample-and-hold time)                            = 0010b  ->  64 ADC12CLK cycles
  //MSC (Multiple sample and conversion)                    = 0b  ->  Not valid
  //REF2_5V (Reference generator voltage)                   = 1b  ->  2.5 V
  //REFON (Reference generator on)                          = 1b  ->  Reference on
  //ADC12ON (ADC12 on)                                      = 1b  ->  ADC12 on
  //ADC12OVIE (SADC12MEMx overflow-interrupt enable)        = 0b  ->  disabled
  //ADC12TOVIE (ADC12 conversion-time-overflow int enable)  = 0b  ->  disabled
  //ENC (Enable conversion)                                 = 0b  ->  enable configuration
  //ADC12SC (Start conversion)                              = 1b  ->  Start sample-and-conversion                 
    ADC12CTL0 |= SHT02 | ADC12ON; 
    
 //ADC12CTL1 -> Control Register 1
  //   15-12  |11-10| 9 | 8  |   7-5   |    4-3   |  2-1  |    0    |
  //----------------------------------------------------------------------------
  //CSTARTADDx| SHSx|SHP|ISSH|ADC12DIVx|ADC12SSELx|CONSEQx|ADC12BUSY|
  //CSTARTADDx (Conversion start address.)   = 0000b  ->  ADC12MEM0
  //SHSx (Sample-and-hold source select)     = 00b  ->  ADC12SC bit
  //SHP (Sample-and-hold pulse-mode select)  = 0b  -> SAMPCON signal is sourced from the sample-input signal.
  //ISSH (Invert signal sample-and-hold)     = 0b  ->  not inverted.
  //ADC12DIVx (ADC12 clock divider)          = 000b  ->  /1
  //ADC12SSELx (ADC12 clock source)          = 00b  ->  ADC12OSC
  //CONSEQx (Conversion sequence mode )      = 00b  ->  Single-channel, single-conversion
  //ADC12BUSY (ADC12 busy)                   = xb  ->  read only  
    ADC12CTL1 = CSTARTADD_0 ;                   // Start MEM0, TB1, Rpt Sing.
    
      
 //ADC12MCTLx -> Conversion Memory Control Registers
  //   7  | 6-4 |3-0  | 
  //----------------------------------------------------------------------------
  //  EOS |SREFx|INCHx|
  //EOS (End of sequence)           = 0b  ->  Not Used
  //SREFx (Select reference )       = 001b  ->  VR+ = VREF+ and VR- = AVSS
  //INCHx (Input channel select)    = 0001b  ->  A1
    ADC12MCTL0 = INCH_1 | SREF_1;               
    
    ADC12CTL0 |= ENC;                           // Enable conversions
    ADC12CTL0 |= ADC12SC;                       // Start conversion
    ADC12IE |= BIT0;                            // Interrupt enable 
}    


/*************************************************************************/
// @fn      OPInit
//
// @brief   Initialize  Operational Amplifiers module ( Non Inverting PGA )
//
// @param   None
//
// @return  None
//************************************************************************/
void OPInit()
{
  // Non-inverting PGA */
 
  //OAFBRx (OAx feedback resistor select) = 000b -> G=1
  //                                        001b -> G=1.3
  //                                        010b -> G=2
  //                                        011b -> G=2.7
  //                                        100b -> G=4
  //                                        101b -> G=5.3
  //                                        110b -> G=8
  //                                        111b -> G=16
  
  
  // OA0CTL1, Opamp Control Register 1
  // 7-5| 4-2| 1  |     0    |
  //----------------------------------------------------------------------------
  //OAFBRx|OAFCx|Reserved|OARRIP|
  //OAFBRx (OAx feedback resistor select) = 000b  ->  Tap 0 (G=1)
  //OAFCx (OAx function control)          = 100b  ->  Non-inverting PGA
  //Reserved 
  //OARRIP (OA rail-to-rail input off)    = 0b  ->  OAx input signal range is rail-to-rail
    OA0CTL1 |= OAFC_4 | OAFBR_0;
    
  // OAxCTL0, Opamp Control Register 0
  // 7-6| 5-4| 3-2 | 1  |     0    |
  //----------------------------------------------------------------------------
  //OANx|OAPx|OAPMx|OAPMx|OAADC1|OAADC0|
  //OANx (Inverting input select)     = XXb  ->  not important
  //OAPx (Non-inverting input select) = 10b  ->  DAC0 internal
  //OAPMx (Slew rate select)          = 11b  -> Fast
  //OAADC1 (OA output select)         = 1b  ->  output connected to internal/external A1
  //OAADC0 (OA output select)         = 0b  ->  output not connected to internal A12
    OA0CTL0 |= OAP_2 | OAPM_3 | OAADC1;  
        
}    

//************************************************************************/
// @fn      SystemInit
//
// @brief   Initialize the system clock to operate at 1048kHz from DCO
//
// @param   None
//
// @return  None
//************************************************************************/
void SystemInit()
{
    // Stop the watchdog timer */
    WDTCTL = WDTPW | WDTHOLD; 

    // Configure the FLL to lock at 128x32768Hz => 4.199304MHz */
    SCFI0 = FN_3 | FLLD_4;
    FLL_CTL0 = XCAP18PF;
    SCFQCTL = 32 - 1;
    FLL_CTL0 |= DCOPLUS; 
}

//************************************************************************/
// @fn      main
//
// @brief   First function called after startup.
//
// @param   None
//
// @return  None
//************************************************************************/
void main (void)
{  
  //system clok configuration
  SystemInit();  
  
  //DAC12 configuration
  DAC12Init();

  //ADC12 configuration
  ADC12Init();
  
  //OpAmp configuration
  OPInit();   
  
 // Timer_A configuration
  TACTL = TASSEL_1 + MC_2 + TACLR + TAIE;  // ACLK
                                           // Continuous mode: CCR0 counts up to 0FFFFh
                                           // Timer_A clear
                                           // Timer_A interrupt enable
// Interrupts enabled and enter in LPM3
  _BIS_SR(LPM3_bits + GIE);
}


//************************************************************************/
// @fn      ADC_ISR
//
// @brief   ADC12_ISR reads the obtained value and prepares new conversion
//
// @param   None
//
// @return  None
//************************************************************************/
#pragma vector=ADC12_VECTOR
__interrupt void ADC_ISR(void)
{
   int x;
    x = ADC12MEM0;         // Reads data 
    ADC12CTL0 |= ADC12SC;  // Start new conversion
}

//************************************************************************/
// @fn      TimerA_ISR
//
// @brief   Starts new conversion
//
// @param   None
//
// @return  None
//************************************************************************/
#pragma vector=TIMERA1_VECTOR
__interrupt void TimerA_ISR (void)
{
    ADC12CTL0 &= ~ADC12SC; //start a new ADC12 conversion
    TACTL &= ~TAIFG;
}
