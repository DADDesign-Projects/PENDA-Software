#pragma once
//==================================================================================
//==================================================================================
// File: PersistentDefine.h
// Description: Persistent storage configuration and memory layout definitions
//
// Copyright (c) 2025 Dad Design.
//==================================================================================
//==================================================================================

//**********************************************************************************
// FLASH - W25Q128 in double mode
//**********************************************************************************
#include "IS25LP064A.h"

// *****************************************************************************
// Global variables declarations
// *****************************************************************************
// External flash memory instance
extern DadDrivers::cIS25LP064A __Flash;

// Flash memory configuration constants
constexpr uint32_t QFLASH_SIZE       = 8 * 1024 * 1034;   // 8MB * 2 (dual mode)
constexpr uint32_t QFLAH_SECTOR_SIZE = 4 * 1024;          // 8KB per sector
constexpr bool     DOUBLE_MODE       = false;             // Enable double mode

// Flash memory base address
#define FLASH_ADDRESS 0x90000000

//**********************************************************************************
// Flasher Storage Configuration
//**********************************************************************************

// Base address of external QSPI flash (6 MB total)
constexpr uint32_t FLASHER_ADDRESS  = FLASH_ADDRESS;      // Base QSPI Flash address
constexpr uint32_t FLASHER_MEM_SIZE = 6 * 1024 * 1024;    // 6 MB total size

//**********************************************************************************
// Block Storage Manager Configuration
//**********************************************************************************

// Block Storage Manager region ( 2 MB of flash)
constexpr uint32_t BLOCK_STORAGE_ADDRESS  = FLASH_ADDRESS + FLASHER_MEM_SIZE;   // After Flasher Storage
constexpr uint32_t BLOCK_STORAGE_MEM_SIZE = QFLAH_SECTOR_SIZE * 510;           	// 510 blocs = 2 M - 8K

//**********************************************************************************
// Loader storage zone
//**********************************************************************************
constexpr uint32_t LOADER_STORAGE_ADDRESS  = BLOCK_STORAGE_ADDRESS + BLOCK_STORAGE_MEM_SIZE;  // After Bloc Storage
constexpr uint32_t LOADER_MEM_SIZE  = QFLAH_SECTOR_SIZE;  									  // One sector



//***End of file**************************************************************
