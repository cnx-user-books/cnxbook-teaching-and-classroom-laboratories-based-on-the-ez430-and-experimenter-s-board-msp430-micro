/**************************************************************************/
/*       Lab9b - Flash memory programming with the CPU executing the      */
/*               code in the RAM                                          */
/*                                                                        */
/*                                                                        */
/* MSP430 Teaching ROM                                                    */
/* Produced by: MSP430@UBI Group - www.msp430.ubi.pt                      */
/*                                                                        */
/* Exercise: Using the MSP-EXP430FG4618 Development Tool implement        */
/*           a flash memory programming with the CPU executing the        */
/*           code in RAM.                                                 */
/*                                                                        */
/*           This procedure requires special attention during the project */
/*           construction. The application begins copying the routines    */
/*           from flash to RAM.                                           */
/*           Directive MEMORY: Device's memory configuration. Identifies  */
/*            the memory ranges that are physically present on the device */
/*           Directive SECTIONS: controls how the sections are built and  */
/*            reserved.                                                   */
/*                                                                        */
/* Instructions:                                                          */
/*                                                                        */
/* This lab requires to configure:                                        */
/*  - Several flash storage management routines                           */
/*  - Check the state of the flag Wait                                    */
/*  - Flash write routine                                                 */
/*                                                                        */
/* The execution time of the different operations can be obtained with    */
/* an oscilloscope connected on pin 2 of the Header 4 or analyzing the    */
/* state of the LED (digital output P2.1)                                 */
/*                                                                        */
/*                                                                        */
/*                                        Copyright by Texas Instruments  */
/**************************************************************************/

#include <msp430xG46x.h>
#include "string.h"

// Define where the code is located in both Flash and RAM
#define flash_start_add 0x3100              // Flash code starting address
#define flash_code_size 0x0200              // Function segment size to be copied
#define RAM_start_add   0x1100              // RAM code starting address

#define infomation_seg_A 0x1080             // Information Segment A
#define infomation_seg_B 0x1000             // Information Segment B
#define seg_length 0x0080                   // Segment length


unsigned char i;

//*************************************************************************
// Erase segment  
//*************************************************************************
#pragma CODE_SECTION(erase_segment,".FLASHCODE")
void erase_segment(int adress)
{
  int *Flash_ptr;                           // Flash pointer

  Flash_ptr = (int *)adress;                // Initialize Flash pointer
  
  while(FCTL3&BUSY);                        // Check BUSY flag
  
  FCTL1 = FWKEY + ERASE;                    // Set Erase bit
  FCTL3 = FWKEY;                            // Clear Lock bit
  
  *Flash_ptr = 0;                           // Dummy write to erase Flash segment
  
  while(FCTL3&BUSY);                        // Check BUSY flag
  
  FCTL3 = FWKEY + LOCK;                     // Set LOCK bit
}
//*************************************************************************
// Write char in flash
//*************************************************************************
#pragma CODE_SECTION(write_char_flash,".FLASHCODE")
void write_char_flash(int adress, char value)
{
  char *Flash_ptr;                          // Flash pointer

  Flash_ptr = (char *)adress;               // Initialize Flash pointer
  
  while(FCTL3&BUSY);                        // Check BUSY flag
  
  FCTL3 = FWKEY;                            // Clear Lock bit
  FCTL1 = FWKEY + WRT;                      // Set WRT bit for write operation
  
  *Flash_ptr = value;                       // Write value to flash
  
  while(FCTL3&BUSY);                        // Check BUSY flag
  
  FCTL1 = FWKEY;                            // Clear WRT bit
  FCTL3 = FWKEY + LOCK;                     // Set LOCK bit
}
//************************************************************************
// Write integer in flash
//************************************************************************
#pragma CODE_SECTION(write_int_flash,".FLASHCODE")
void write_int_flash(int adress, int value)
{
  int *Flash_ptr;                           // Flash pointer

  Flash_ptr = (int *)adress;                // Initialize Flash pointer
  
  while(FCTL3&BUSY);                        // Check BUSY flag
  
  FCTL3 = FWKEY;                            // Clear Lock bit
  FCTL1 = FWKEY + WRT;                      // Set WRT bit for write operation
  
  
  *Flash_ptr = value;                       // Write value to flash
  
  while(FCTL3&BUSY);                        // Check BUSY flag
  
  FCTL1 = FWKEY;                            // Clear WRT bit
  FCTL3 = FWKEY + LOCK;                     // Set LOCK bit
}
//***********************************************************************
// Copy data from one segment to another
//***********************************************************************
#pragma CODE_SECTION(copy_seg_flash,".FLASHCODE")
void copy_seg_flash(int adress_source, int adress_destiny)
{
  int *Flash_ptr_A;                         // Segment Information A pointer
  int *Flash_ptr_B;                         // Segment Information B pointer
  unsigned char i;

  Flash_ptr_A = (int *)adress_source;       // Initialize Information A pointer
  Flash_ptr_B = (int *)adress_destiny;      // Initialize Information B pointer
  
  while(FCTL3&BUSY);                        // Check BUSY flag
  
  FCTL1 = FWKEY + ERASE;                    // Set Erase bit
  FCTL3 = FWKEY;                            // Clear Lock bit
  
  *Flash_ptr_B = 0;                         // Dummy write to erase Flash segment D
  while(FCTL3&BUSY);                        // Check BUSY flag
  
  FCTL1 = FWKEY + WRT;                      // Set WRT bit for write operation

  for (i=0; i<seg_length; i = i + 2)
  {
  	while(FCTL3&BUSY);                      // Check BUSY flag
    *Flash_ptr_B++ = *Flash_ptr_A++;        // copy value segment C to segment D
  }

  while(FCTL3&BUSY);                        // Check BUSY flag
  
  FCTL1 = FWKEY;                            // Clear WRT bit
  FCTL3 = FWKEY + LOCK;                     // Set LOCK bit
}
//***********************************************************************
//Copy function from Flash to RAM
//***********************************************************************
void copy_flash_to_RAM(void)
{
  unsigned char *flash_start_ptr;                       // Pointer to flash
  unsigned char *RAM_start_ptr;                         // Pointer to RAM

  flash_start_ptr = (unsigned char *)flash_start_add;   // Initialize pointers
  RAM_start_ptr = (unsigned char *)RAM_start_add;
  
  // Copy flash function to RAM 
  memcpy(RAM_start_ptr,flash_start_ptr,flash_code_size);
}
//***********************************************************************
// main
//***********************************************************************
void main(void)
{
  WDTCTL = WDTPW + WDTHOLD;                             // Stop WatchDog timer
  
  copy_flash_to_RAM();                                  // call function to copy from flash to RAM

  FCTL2 = FWKEY + FSSEL0 + FN1;                         // MCLK/3 for Flash Timing Generator
  
  erase_segment(infomation_seg_A);                      // Erase flash Information A and B
  erase_segment(infomation_seg_B);
  
  for(i = 0; i< seg_length; i++)                        // write information A with byte data
  {
  	write_char_flash(infomation_seg_A + i,i);
  }
  
  for(i = 0; i< seg_length; i = i + 2)                  // write information B with word data
  {
  	write_int_flash(infomation_seg_B + i,i);
  }

  erase_segment(infomation_seg_B);                      // Erase again Information B
  copy_seg_flash(infomation_seg_A, infomation_seg_B);	// copy contents from Information A to Information B
  

  while(1)                                              // Repeat forever
  {
    _NOP();                                             // SET BREAKPOINT HERE
  }
}
