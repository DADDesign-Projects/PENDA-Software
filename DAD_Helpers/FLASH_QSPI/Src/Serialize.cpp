//====================================================================================
// Serialize.cpp
//
// Description: A utility class for serialization and deserialization of data types, 
// including primitives and strings. Provides methods to push data into a buffer and 
// pull data from it, with support for raw data handling. Includes buffer management 
// features such as clearing and resetting read position.
//
// Copyright(c) 2025 Dad Design.
//====================================================================================

#include "Serialize.h"
namespace DadQSPI {

//***********************************************************************************
// Class cSerialize
//***********************************************************************************

// --------------------------------------------------------------------------
// Push raw data into the buffer
void cSerialize::PushRaw(const void* data, size_t size) {
    const uint8_t* bytes = static_cast<const uint8_t*>(data);
    buffer.insert(buffer.end(), bytes, bytes + size);
}

// --------------------------------------------------------------------------
// Pull raw data from the buffer
void cSerialize::PullRaw(void* data, size_t size) {
    if (readIndex + size <= buffer.size()) {
        std::memcpy(data, buffer.data() + readIndex, size);
        readIndex += size;
    }
}

// --------------------------------------------------------------------------
// Push a string into the buffer
void cSerialize::Push(const std::string& str) {
    uint32_t length = static_cast<uint32_t>(str.size());
    Push(length); // Push the length of the string first
    PushRaw(str.data(), length); // Push the string data
}

// --------------------------------------------------------------------------
// Pull a string from the buffer
std::string cSerialize::PullString() {
    uint32_t length=0; 
    Pull(length); // Pull the length of the string first
    if (readIndex + length <= buffer.size()) {
        std::string str(length, '\0');
        PullRaw(&str[0], length); // Pull the string data
        return str;
    }
    return std::string();
}

// --------------------------------------------------------------------------
// Get the size and content of the buffer
size_t cSerialize::getBuffer(const uint8_t** outBuffer) const {
    *outBuffer = buffer.data();
    return buffer.size();
}

// --------------------------------------------------------------------------
// Set the buffer with new data
void cSerialize::setBuffer(const void* data, size_t size) {
    const uint8_t* bytes = static_cast<const uint8_t*>(data);
    buffer.assign(bytes, bytes + size);
    readIndex = 0;
}


} //DadQSPI
