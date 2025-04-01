//====================================================================================
// cMemory.cpp
// Manages the storage of effect parameters
//====================================================================================
#include "cMemory.h"

// Serialize
extern DadQSPI::cSerialize __SerializeParameter;

namespace DadQSPI {

//****************************************************************************
// cMemory
// Handles saving and restoring serialized objects to/from persistent storage
//****************************************************************************
// ------------------------------------------------------------------------
// Initializes the class
void cMemory::Init(){
	uint32_t LoadSize = 0;
	__PersistentStorage.Load(IDMemStorage, &m_MemoryPersistent, sizeof(m_MemoryPersistent), LoadSize);

	// If storage size does not match expected size, reset memory states
	if(LoadSize != sizeof(m_MemoryPersistent)){
		for (uint8_t Index = 0; Index < NB_MEM_SLOT ; Index++){
			m_MemoryPersistent.m_Save[Index] = 0;
			__PersistentStorage.Delete(IDMemSlot + Index + 1);
		}
		__PersistentStorage.Save(IDMemStorage, &m_MemoryPersistent, sizeof(m_MemoryPersistent));
	}

	// Mark current memory slot as clean
	m_Dirty = 0;
}

// ------------------------------------------------------------------------
// Saves the state of the object into a specified memory slot
void cMemory::Save(uint8_t saveNumber, const uint8_t *pBuffer, uint32_t Size){

	if((pBuffer != nullptr)&&(Size !=0)){
		__PersistentStorage.Save(IDMemSlot + saveNumber, pBuffer, Size);

		m_Dirty = 0;
		m_MemoryPersistent.m_Save[saveNumber] = 1; // Mark as saved
		m_MemoryPersistent.m_ActiveSlot = saveNumber;
		__PersistentStorage.Save(IDMemStorage, &m_MemoryPersistent, sizeof(m_MemoryPersistent));
	}
}

// ------------------------------------------------------------------------
// Restores the state of the object from a specified memory slot
uint32_t cMemory::Restore(uint8_t saveNumber, uint8_t *pBuffer, uint32_t Size){
	uint32_t SizeLoad=0;
	if(m_MemoryPersistent.m_Save[saveNumber] == 1){
		if(pBuffer != nullptr){
			__PersistentStorage.Load(IDMemSlot + saveNumber, pBuffer, Size, SizeLoad);
			m_MemoryPersistent.m_ActiveSlot = saveNumber;
			m_Dirty = 0;
			__PersistentStorage.Save(IDMemStorage, &m_MemoryPersistent, sizeof(m_MemoryPersistent));
		}
	}
    return SizeLoad;
}

// ------------------------------------------------------------------------
// Erase memory slot
void cMemory::Erase(uint8_t saveNumber){
	if(m_MemoryPersistent.m_ActiveSlot == saveNumber){
		return;
	}

	// Delete the saved data in the persistent storage
	__PersistentStorage.Delete(IDMemSlot + saveNumber);

	// Mark the slot as free
	m_MemoryPersistent.m_Save[saveNumber] = 0;

	__PersistentStorage.Save(IDMemStorage, &m_MemoryPersistent, sizeof(m_MemoryPersistent));
}


} // namespace DadQSPI
