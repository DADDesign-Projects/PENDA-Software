#pragma once
//====================================================================================
// Serialize.h
//
// Description: A utility class for serialization and deserialization of data types, 
// including primitives and strings. Provides methods to push data into a buffer and 
// pull data from it, with support for raw data handling. Includes buffer management 
// features such as clearing and resetting read position.
//
// Copyright(c) 2025 Dad Design.
//====================================================================================
#include <vector>
#include <cstdint>
#include <cstring>
#include <string>
#include "main.h"

namespace DadQSPI {
class cSerialize;

//***********************************************************************************
// Class cSerializedObject
//***********************************************************************************
class cSerializedObject{
public:
    // --------------------------------------------------------------------------
    // Serialize the object
    virtual void Save(DadQSPI::cSerialize &Serializer, uint32_t SerializeID) = 0;

    // --------------------------------------------------------------------------
    // Deserialize the object
    virtual void Restore(DadQSPI::cSerialize &Serializer, uint32_t SerializeID) = 0;

};

//***********************************************************************************
// Class cSerialize
//***********************************************************************************
class cSerialize {
public:
    // --------------------------------------------------------------------------
	// Constructor
	cSerialize(){
	    buffer.clear();
	    readIndex = 0;
	}

    // --------------------------------------------------------------------------
    // Template method for pushing data into the buffer
    template<typename T>
    void Push(const T& value) {
        PushRaw(&value, sizeof(T));
    }
    
    // --------------------------------------------------------------------------
    // Template method for pulling data from the buffer
    template<typename T>
    void Pull(T& value) {
         PullRaw(&value, sizeof(T));
      }

    // --------------------------------------------------------------------------
    // Push raw data into the buffer
    void PushRaw(const void* data, size_t size);
    
    // --------------------------------------------------------------------------
    // Pull raw data from the buffer
    void PullRaw(void* data, size_t size);

    // --------------------------------------------------------------------------
    // Push a string into the buffer
    void Push(const std::string& str);

    // --------------------------------------------------------------------------
    // Pull a string from the buffer
    std::string PullString();

    // --------------------------------------------------------------------------
    // Get the size and content of the buffer
    size_t getBuffer(const uint8_t** outBuffer) const;

    // --------------------------------------------------------------------------
    // Set the buffer with new data
    void setBuffer(const void* data, size_t size);

    // --------------------------------------------------------------------------
    // Clear the buffer
    void clearBuffer() {
        buffer.clear();
        readIndex = 0;
    }

    // --------------------------------------------------------------------------
    // Reset the read position to the beginning of the buffer
    void resetReadIndex() {
        readIndex = 0;
    }

private:
    std::vector<uint8_t> buffer; // Buffer to hold serialized data
    size_t readIndex = 0;        // Current read position in the buffer
};
} //DadQSPI
