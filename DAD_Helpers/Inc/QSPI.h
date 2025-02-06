#pragma once
//====================================================================================
// QSPI.h
//
// Utility for using QSPI Flash memory
// Copyright(c) 2025 Dad Design.
//====================================================================================
#include "main.h"
#include <cstdint>
#include "cIS25LPxxx.h"

// External reference to the Daisy hardware interface
namespace DadQSPI{

// Base address of the memory-mapped QSPI flash
#define QSPI_ADRESSE            0x90000000

// Size of one flash page - smallest erasable unit
// 4KB pages are standard for most flash memory devices
#define QSPI_PAGE_SIZE          4096        // 4K per page

// Total size of QSPI flash memory
// 8MB = 8 * 1024 * 1024 bytes
#define QSPI_SIZE               8388608     // Size of the total QSPI flash memory 8M

// Size reserved for the flasher storage
// 6MB = 6 * 1024 * 1024 bytes
// Remaining 2MB will be used for persistent storage
#define FLASHER_MEM_SIZE        6291456     // Size of the QSPI Storage flash memory 6M

//***********************************************************************************
// Class cQSPI_FlasherStorage
// This class manages a simple file system in QSPI flash memory
//***********************************************************************************

// Calculate number of pages available for flasher storage
// 6MB / 4KB = 1536 pages
#define QSPI_PAGE_COUNT     FLASHER_MEM_SIZE/ QSPI_PAGE_SIZE    

// Maximum length for file names in the directory
#define MAX_ENTRY_NAME      40                                      

// Maximum number of files that can be stored in the directory
#define DIR_FILE_COUNT      20                                      

// --------------------------------------------------------------------------
// Directory structure
// Array of file entries containing name, size, and address in flash
typedef struct stFile {
    char     Name[MAX_ENTRY_NAME];   // File name
    uint32_t Size;                   // File size in bytes
    uint32_t DataAddress;            // Address where file data is stored in flash
} Directory[DIR_FILE_COUNT];

// --------------------------------------------------------------------------
// Structure representing the physical layout of QSPI flash memory
// Organizes memory into pages of QSPI_PAGE_SIZE bytes
typedef uint8_t Page[QSPI_PAGE_SIZE];
struct stQSPI {
    Page Data[QSPI_PAGE_COUNT];      // Array of pages containing file data
};

// Verify at compile time that the structure fits within allocated flash space
static_assert(sizeof(stQSPI) <= FLASHER_MEM_SIZE, "Memory used > QSPI Flash memory size");

// ==============================================================================
// Class managing flash storage for files
class cQSPI_FlasherStorage{
public:
    // --------------------------------------------------------------------------
    // Retrieves pointer to file data in flash memory
    // @param pFileName Name of the file to find
    // @return Pointer to file data or nullptr if file not found
    uint8_t* GetFilePtr(const char *pFileName) const;
    
   
    // --------------------------------------------------------------------------
    // Gets the size of a file in bytes
    // @param pFileName Name of the file to check
    // @return Size of file in bytes, or 0 if file not found
    uint32_t GetFileSize(const char* pFileName) const;

protected:
    stFile      Dir[DIR_FILE_COUNT];                    // Directory entries
    uint8_t     Data[FLASHER_MEM_SIZE - sizeof(Directory)];    // Storage area for file data
};

//***********************************************************************************
// class cQSPI_PersistentStorage
// This class manages persistent storage in QSPI flash memory
// It handles saving, loading, and deleting data in flash memory blocks
// Each block contains a header and data area
//

// Flash memory block and storage configuration
// Block size matches the QSPI flash page size for optimal performance
constexpr uint32_t BLOCK_SIZE = QSPI_PAGE_SIZE;

// Size of the save block header, consisting of:
// - 3 uint32_t (saveNumber, dataSize, isValid)
// - 1 pointer (pNextBlock)
constexpr uint32_t HEADER_SIZE  = (3 * sizeof(uint32_t)) + sizeof(void *);

// Available space for data in each block
// Calculated by subtracting header size from total block size
constexpr uint32_t DATA_SIZE = BLOCK_SIZE - HEADER_SIZE;

// Total size available for persistent storage
// Calculated by subtracting flasher memory size from total QSPI size
constexpr uint32_t PERSISTENT_STORAGE_SIZE = QSPI_SIZE - FLASHER_MEM_SIZE;

// Total number of available blocks for persistent storage
constexpr uint32_t NUM_BLOCKS_PERSISTENT = PERSISTENT_STORAGE_SIZE / BLOCK_SIZE;

// Structure defining the layout of a save block in flash memory
struct sSaveBloc {
    uint32_t    m_saveNumber;      // Save identification number
    uint32_t    m_dataSize;        // Size of the data contained in this block
    sSaveBloc*  m_pNextBlock;      // Pointer to next block (for multi-block saves)
    uint32_t    m_isValid;         // Validity marker (0xFFFFFFFF = invalid)
    uint8_t     m_Data[DATA_SIZE]; // Data storage array
};

// Magic number used to mark valid blocks
// Pattern 0xAA55AA55 chosen for its distinctive bit pattern
constexpr uint32_t HEADER_MAGIC = 0xAA55AA55;

// Value indicating an invalid or erased block
// In flash memory, erased state is all bits set to 1
constexpr uint32_t INVALID_MARKER = 0xFFFFFFFF;


class cQSPI_PersistentStorage {   
public:
      
    // --------------------------------------------------------------------------
    // Constructor
    // Initializes the persistent storage system
    cQSPI_PersistentStorage(){}

    // --------------------------------------------------------------------------   
    // Initialize Blocks
    void InitializeBlock();

    // --------------------------------------------------------------------------
    // Saves data to flash memory by splitting it into blocks if necessary
    // Each block contains a header (sSaveBloc) and data area
    // If data is larger than one block, it creates a linked list of blocks
    // @param saveNumber Unique identifier for the save
    // @param pDataSource Pointer to the data to be saved
    // @param Size Size of the data in bytes
    // @return true if save successful, false if not enough space or error
    //
    bool Save(uint32_t saveNumber, const void* pDataSource, uint32_t Size);

    // --------------------------------------------------------------------------
    // Loads data from flash memory using save number as identifier
    // Follows the linked list of blocks to reconstruct the complete data
    // @param saveNumber Identifier of the save to load
    // @param pBuffer Buffer to store the loaded data
    // @param Size Will contain the size of loaded data
    // @return true if load successful, false if save not found
    //
    void Load(uint32_t saveNumber, void* pData, uint32_t& Size);

    // --------------------------------------------------------------------------
    // Deletes a save from flash memory by erasing all blocks in its chain
    // @param saveNumber Identifier of the save to delete
    // @return true if deletion successful, false if save not found
    //
    void Delete(uint32_t saveNumber);
    
protected:

    // --------------------------------------------------------------------------   
    // Finds a free block in the storage area by looking for erased blocks
    // A block is considered free if its valid flag is all 1's (0xFFFFFFFF)
    // @return Pointer to free block or nullptr if no free blocks available
    //
    sSaveBloc* findFreeBlock(sSaveBloc* StartBloc) const;

    // --------------------------------------------------------------------------   
    // Find first saveNumber block
    // @return Pointer to first saveNumber block or nullptr if no free blocks available
    sSaveBloc* FindFirstBlock(uint32_t saveNumber) const;
};
} //Dad
