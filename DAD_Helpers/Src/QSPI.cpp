#pragma GCC optimize ("O0")
//====================================================================================
// QSPI.cpp
//
// Utility for using QSPI Flash memory
// Copyright(c) 2025 Dad Design.
//====================================================================================
#include "QSPI.h"
#include <cstring>

extern Dad::cIS25LPxxx __Flash;

namespace DadQSPI{

//***********************************************************************************
// Class cQSPI_FlasherStorage
// This class manages a simple file system in QSPI flash memory
//***********************************************************************************
// --------------------------------------------------------------------------
// Retrieves pointer to file data in flash memory
// @param pFileName Name of the file to find
// @return Pointer to file data or nullptr if file not found
uint8_t* cQSPI_FlasherStorage::GetFilePtr(const char *pFileName) const {
    for(int8_t Index=0; Index < DIR_FILE_COUNT; Index++){
        if(0 == strcmp(Dir[Index].Name, pFileName)){
            return (uint8_t*) Dir[Index].DataAddress;
        }
    }
    return nullptr;
}
   
// --------------------------------------------------------------------------
// Gets the size of a file in bytes
// @param pFileName Name of the file to check
// @return Size of file in bytes, or 0 if file not found
uint32_t cQSPI_FlasherStorage::GetFileSize(const char* pFileName) const {
    for (int8_t Index = 0; Index < DIR_FILE_COUNT; Index++) {
        if (0 == strcmp(Dir[Index].Name, pFileName)) {
            return Dir[Index].Size;
        }
    }
    return 0;
}

//***********************************************************************************
// class cQSPI_PersistentStorage
// This class manages persistent storage in QSPI flash memory
// It handles saving, loading, and deleting data in flash memory blocks
// Each block contains a header and data area
//

// Base address for the persistent storage area
// Calculated by offsetting QSPI base address by flasher memory size
static sSaveBloc* BASE_BLOC_ADRESSE = (sSaveBloc*)(QSPI_ADRESSE + FLASHER_MEM_SIZE);

// --------------------------------------------------------------------------
// Saves data to flash memory by splitting it into blocks if necessary
// Each block contains a header (sSaveBloc) and data area
// If data is larger than one block, it creates a linked list of blocks
// @param saveNumber Unique identifier for the save
// @param pDataSource Pointer to the data to be saved
// @param Size Size of the data in bytes
// @return true if save successful, false if not enough space or error
//

bool cQSPI_PersistentStorage::Save(uint32_t saveNumber, const void* pDataSource, uint32_t Size) {
    // Deletes a saveNumber from flash memory
    Delete(saveNumber);
    
    // Find first available block
    sSaveBloc* pCurrentBlock = findFreeBlock(BASE_BLOC_ADRESSE);    
    
    // Initialize save block header
    sSaveBloc SaveBloc;
    SaveBloc.m_saveNumber = saveNumber;
    SaveBloc.m_dataSize = Size;
    SaveBloc.m_isValid = HEADER_MAGIC;
    
    // Track remaining data to be written
    uint32_t remainingSize = Size;
    const uint8_t* pData = (uint8_t*)(pDataSource);
    
    // Write data across multiple blocks if needed
    while(pCurrentBlock != nullptr){
        
        // Calculate how much data can fit in current block
        uint32_t blockDataSize;
        if(remainingSize > DATA_SIZE){
            // More data remains than can fit in one block
            blockDataSize = DATA_SIZE;
            SaveBloc.m_pNextBlock = findFreeBlock(pCurrentBlock+1);  // Link to next block
        }else{
            // All remaining data can fit in this block
            blockDataSize = remainingSize;
            SaveBloc.m_pNextBlock = nullptr;  // Last block in chain
        }
        // Copy data to the save block structure
        memcpy(&SaveBloc.m_Data[0], pData, blockDataSize);
        
        // Write the entire block (header + data) to flash
        __Flash.FastWrite((uint8_t*)(&SaveBloc), (uint32_t)(pCurrentBlock), BLOCK_SIZE);
        HAL_Delay(10); // Delay to ensure write completion
        SCB_InvalidateDCache_by_Addr((void *)pCurrentBlock, DATA_SIZE);
                    
        // Update pointers and remaining size for next iteration
        pData += blockDataSize;
        remainingSize -= blockDataSize;
        pCurrentBlock = SaveBloc.m_pNextBlock;
    }

    // Check if all data was written
    if(remainingSize != 0){
        // Not enough space was available, delete partial save
        Delete(saveNumber);
        return false;
    }else{
        return true;
    }
}

// --------------------------------------------------------------------------
// Loads data from flash memory using save number as identifier
// Follows the linked list of blocks to reconstruct the complete data
// @param saveNumber Identifier of the save to load
// @param pBuffer Buffer to store the loaded data
// @param Size Will contain the size of loaded data 0 if load error
//
void cQSPI_PersistentStorage::Load(uint32_t saveNumber, void* pData, uint32_t& Size) {
    Size = 0;
    uint8_t* pBuffer = (uint8_t*)(pData);
    
    // Search through all blocks for matching save number
    sSaveBloc* pSaveBloc = FindFirstBlock(saveNumber);
    uint32_t remainingSize;
    if(pSaveBloc != nullptr) {  
        remainingSize = pSaveBloc->m_dataSize;
    }

    // Read data from all linked blocks
    while (pSaveBloc != nullptr) {
        // Copy data from flash to buffer
        // Direct memory access is possible because flash is memory-mapped
        uint32_t blockDataSize;
        if(remainingSize > DATA_SIZE){
            blockDataSize = DATA_SIZE;
        }else{
            blockDataSize = remainingSize;
        }

        memcpy(pBuffer, &(pSaveBloc->m_Data[0]), blockDataSize);
        
        // Update pointers and remaining size
        pBuffer += blockDataSize;
        Size += blockDataSize;
        remainingSize -= blockDataSize;
        pSaveBloc = pSaveBloc->m_pNextBlock;
    }
}

// --------------------------------------------------------------------------
// Deletes a save from flash memory by erasing all blocks in its chain
// @param saveNumber Identifier of the save to delete
// @return true if deletion successful, false if save not found
//
void cQSPI_PersistentStorage::Delete(uint32_t saveNumber) {
    // Search through all blocks for matching save number
    sSaveBloc* pSaveBloc = FindFirstBlock(saveNumber); 
    // Erase all blocks in the chain
    while (pSaveBloc != nullptr) {
        sSaveBloc* pNextBloc = pSaveBloc->m_pNextBlock;

        __Flash.EraseSector((uint32_t)pSaveBloc);
        HAL_Delay(10);  // Delay to ensure erase completion
        //SCB_InvalidateDCache_by_Addr((void *)pSaveBloc, DATA_SIZE);
        
        pSaveBloc = pNextBloc;  // Move to next block in chain
    }
}

// --------------------------------------------------------------------------   
// Finds a free block in the storage area by looking for erased blocks
// A block is considered free if its valid flag is all 1's (0xFFFFFFFF)
// @return Pointer to free block or nullptr if no free blocks available
//
sSaveBloc* cQSPI_PersistentStorage::findFreeBlock(sSaveBloc* StartBloc) const {
    sSaveBloc* pSaveBloc = StartBloc;  
    for (uint16_t i = 0; i < NUM_BLOCKS_PERSISTENT; i++) {
        
        // Check if block is erased (all bits are 1)
        if (pSaveBloc->m_isValid == INVALID_MARKER){          
            return pSaveBloc;
        }
        pSaveBloc++;
    }
    return nullptr;  // No free blocks found     
}

// --------------------------------------------------------------------------   
// Initialize Blocks
void cQSPI_PersistentStorage::InitializeBlock(){
    sSaveBloc* pSaveBloc = BASE_BLOC_ADRESSE;  
    for (uint16_t i = 0; i < NUM_BLOCKS_PERSISTENT; i++) {
        // Erase current block
        __Flash.EraseSector((uint32_t)pSaveBloc);
        HAL_Delay(5);  // Delay to ensure erase completion
        //SCB_InvalidateDCache_by_Addr((void *)pSaveBloc, DATA_SIZE);

        pSaveBloc++;
    }
}

// --------------------------------------------------------------------------   
// Find first saveNumber block
// @return Pointer to first saveNumber block or nullptr if no free blocks available
sSaveBloc* cQSPI_PersistentStorage::FindFirstBlock(uint32_t saveNumber) const{
    sSaveBloc* pSaveBloc = BASE_BLOC_ADRESSE;  
    for (uint32_t i = 0; i < NUM_BLOCKS_PERSISTENT; i++) {
        // Check if block is erased (all bits are 1)
        if ((pSaveBloc->m_isValid == HEADER_MAGIC) && (pSaveBloc->m_saveNumber == saveNumber)){          
            return pSaveBloc;
        }
        pSaveBloc++;
    }
    return nullptr;  // No free blocks found 
}

} //DadQSPI
