#pragma once
//====================================================================================
// cMemory.h
// Manages the storage of effect parameters
//====================================================================================
#include "main.h"
#include "PendaUI.h"
#include "QSPI.h"
#include "Serialize.h"

// Handles data storage in QSPI flash memory
extern DadQSPI::cQSPI_PersistentStorage __PersistentStorage;

// Number of memory slots
#define NB_MEM_SLOT 10

namespace DadQSPI {

//****************************************************************************
// cMemory
// Handles saving and restoring serialized objects to/from persistent storage
//****************************************************************************
struct sMemoryPersistent{
    uint8_t  m_ActiveSlot;
    uint32_t m_Save[NB_MEM_SLOT];
};

class cMemory{
public:
    // ------------------------------------------------------------------------
    // Constructor & Destructor
    cMemory(){}
    ~cMemory(){}

    // ------------------------------------------------------------------------
    // Initializes the class
    void Init(uint32_t SerializeID);

    // ------------------------------------------------------------------------
    // Saves the state of the object into a specified memory slot
    void Save(uint8_t saveNumber, const uint8_t *pBuffer, uint32_t Size);

    // ------------------------------------------------------------------------
    // Erase memory slot
    void Erase(uint8_t saveNumber);

    // ------------------------------------------------------------------------
    // Checks if a slot has a saved state
    inline uint8_t isSave(uint8_t saveNumber) const{
    	if(saveNumber < NB_MEM_SLOT){
    		return m_MemoryPersistent.m_Save[saveNumber];
    	}else{
    		return 0;
    	}
    }

    // ------------------------------------------------------------------------
    // Restores the state of the object from a specified memory slot
    uint32_t Restore(uint8_t saveNumber, uint8_t *pBuffer, uint32_t Size);

    // ------------------------------------------------------------------------
    // Get the active memory slot
    inline uint8_t getActiveSlot(){
    	return m_MemoryPersistent.m_ActiveSlot;
    }

    // ------------------------------------------------------------------------
    // Get size of the slot
    inline uint32_t getSize(uint8_t saveNumber) const{
    	if(saveNumber < NB_MEM_SLOT){
    		return __PersistentStorage.getSize(m_SerializeID + 1 + saveNumber);
    	}else{
    		return 0;
    	}
    }

protected:
    // ------------------------------------------------------------------------
    // Data member
    sMemoryPersistent 	m_MemoryPersistent;		// Persistent data of class
    uint32_t 			m_SerializeID;			// Serialization ID for saving/restoring state
};
} // namespace DadQSPI
