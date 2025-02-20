/*
******************************************************************************
**
**  File        : LinkerScript.ld (debug in RAM dedicated)
**
**  Author      : STM32CubeIDE
**
**  Abstract    : Linker script for STM32H7 series
**                1024Kbytes RAM_EXEC and 544Kbytes RAM
**
**                Set heap size, stack size and stack location according
**                to application requirements.
**
**                Set memory bank area and size if external memory is used.
**
**  Target      : STMicroelectronics STM32
**
**  Distribution: The file is distributed as is, without any warranty
**                of any kind.
**
*****************************************************************************
** @attention
**
** Copyright (c) 2024 STMicroelectronics.
** All rights reserved.
**
** This software is licensed under terms that can be found in the LICENSE file
** in the root directory of this software component.
** If no LICENSE file comes with this software, it is provided AS-IS.
**
****************************************************************************
*/

/* Entry Point */
ENTRY(Reset_Handler)

/* Highest address of the user mode stack */
_estack = ORIGIN(RAM) + LENGTH(RAM);    /* end of RAM */
/* Generate a link error if heap and stack don't fit into RAM */
_Min_Heap_Size = 0x200;      /* required amount of heap  */
_Min_Stack_Size = 0x400; /* required amount of stack */
/* Start user variables */

/* End user variables */

/* Specify the memory areas */
MEMORY
{
/* Start MX regions */
  NO_CACHE_RAM	(xrw)	: ORIGIN = 0x24070800, LENGTH = 62K
  DTCMRAM	(xrw)	: ORIGIN = 0x20000000, LENGTH = 128K
  FLASH	(rx)	: ORIGIN = 0x08000000, LENGTH = 128K
  QFLASH	(rx)	: ORIGIN = 0x90000000, LENGTH = 8192K
  RAM_D3	(xrw)	: ORIGIN = 0x38000000, LENGTH = 64K
  ITCMRAM	(xrw)	: ORIGIN = 0x00000000, LENGTH = 64K
  RAM_D2	(xrw)	: ORIGIN = 0x30000000, LENGTH = 288K
  RAM	(xrw)	: ORIGIN = 0x24000000, LENGTH = 450K
  SDRAM	(xrw)	: ORIGIN = 0xc0000000, LENGTH = 65536K
/* End MX regions */
/*removed by MX default:   RAM_EXEC	(xrw)	: ORIGIN = 0x24000000, LENGTH = 1024K */
}

/* Define output sections */
SECTIONS
{
  
  
  
  /* used by the startup to initialize data */
  _sidata = LOADADDR(.data);

  /* Initialized data sections goes into RAM, load LMA copy after code */
  .data :
  {
    . = ALIGN(4);
    _sdata = .;        /* create a global symbol at data start */
    *(.data)           /* .data sections */
    *(.data*)          /* .data* sections */

    . = ALIGN(4);
    _edata = .;        /* define a global symbol at data end */
  } >RAM 

  /* Uninitialized data section */
  . = ALIGN(4);
  .bss :
  {
    /* This is used by the startup in order to initialize the .bss section */
    _sbss = .;         /* define a global symbol at bss start */
    __bss_start__ = _sbss;
    *(.bss)
    *(.bss*)
    *(COMMON)

    . = ALIGN(4);
    _ebss = .;         /* define a global symbol at bss end */
    __bss_end__ = _ebss;
  } >RAM

  /* User_heap_stack section, used to check that there is enough RAM left */
  ._user_heap_stack :
  {
    . = ALIGN(8);
    PROVIDE ( end = . );
    PROVIDE ( _end = . );
    . = . + _Min_Heap_Size;
    . = . + _Min_Stack_Size;
    . = ALIGN(8);
  } >RAM

  /* Remove information from the standard libraries */
  /DISCARD/ :
  {
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )
  }

  .ARM.attributes 0 : { *(.ARM.attributes) }

 /*  Unitialized DTCMRAM section into "DTCMRAM" DTCMRAM type memory [MMT section] */
 .DTCMRAM_Section :
  {
    . = ALIGN(4);
    KEEP (*(.DTCMRAM_Section))
    . = ALIGN(4);
  } >DTCMRAM

 /*  Unitialized QFLASH section into "QFLASH" QFLASH type memory [MMT section] */
 .QFLASH_Section (NOLOAD):
  {
    . = ALIGN(4);
    KEEP (*(.QFLASH_Section))
    . = ALIGN(4);
  } >QFLASH

 /*  Unitialized RAM_D3 section into "RAM_D3" RAM_D3 type memory [MMT section] */
 .RAM_D3_Section :
  {
    . = ALIGN(4);
    KEEP (*(.RAM_D3_Section))
    . = ALIGN(4);
  } >RAM_D3

 /*  Unitialized ITCMRAM section into "ITCMRAM" ITCMRAM type memory [MMT section] */
 .ITCMRAM_Section :
  {
    . = ALIGN(4);
    KEEP (*(.ITCMRAM_Section))
    . = ALIGN(4);
  } >ITCMRAM

 /*  Unitialized RAM_D2 section into "RAM_D2" RAM_D2 type memory [MMT section] */
 .RAM_D2_Section :
  {
    . = ALIGN(4);
    KEEP (*(.RAM_D2_Section))
    . = ALIGN(4);
  } >RAM_D2

 /*  Unitialized SDRAM section into "SDRAM" SDRAM type memory [MMT section] */
 .SDRAM_Section (NOLOAD) :
  {
    . = ALIGN(4);
    KEEP (*(.SDRAM_Section))
    . = ALIGN(4);
  } >SDRAM
 
 .NO_CACHE_RAM_Section :
  {
    . = ALIGN(4);
    KEEP (*(.NO_CACHE_RAM_Section))
    . = ALIGN(4);
  } >NO_CACHE_RAM
}
