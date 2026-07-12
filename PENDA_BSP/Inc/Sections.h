//==================================================================================
//==================================================================================
// File: Sections.h
// Description: Memory section definitions and attributes for specialized memory regions
// 
// Copyright (c) 2025 Dad Design.
//==================================================================================
//==================================================================================

#pragma once

//**********************************************************************************
// Memory Section Definitions
//**********************************************************************************

// SDRAM memory section attribute
#define SDRAM_SECTION __attribute__((section(".SDRAM_Section")))

// RAM section without cache for DMA or critical operations
#define NO_CACHE_RAM __attribute__((section(".RAM_NO_CACHE_Section")))

// QSPI Flash sections for different storage purposes
#define QFLASH_FLASHER	__attribute__((section(".QFLASH_FlasherStorage")))
#define QFLASH_TABSAVE	__attribute__((section(".QFLASH_TabSaveBlock")))
#define QFLASH_LOADER	__attribute__((section(".QFLASH_LoaderInfo")))
#define QFLASH_SECTION	__attribute__((section(".QFLASH_Section")))


// Instruction TCM (Tightly Coupled Memory) for fast code execution
// (Provisional?) halt of ITCM memory mapping for critical sections due to unresolved malfunctions
//#define ITCM __attribute__((section(".moveITCM")))


#define RAM_D1 __attribute__((section(".RAM_D1_Section")))

//***End of file**************************************************************
