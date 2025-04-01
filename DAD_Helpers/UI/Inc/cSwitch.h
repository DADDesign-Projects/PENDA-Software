#pragma once
//====================================================================================
// cSwitch.h
// Switch debouncing and state tracking class with configurable timing parameters
// Features:
// - Configurable debounce interval
// - Minimum/maximum period detection
// - Press duration tracking
// - Exponential Moving Average (EMA) for period calculation
//====================================================================================
#include "main.h"
#include <cstdint>

namespace DadUI {

// Timing constants derived from audio system parameters
constexpr float UIRT_RATE2 = SAMPLING_RATE / (float)AUDIO_BUFFER_SIZE;  // Conversion factor between samples and time
constexpr uint32_t kUpdateTime = static_cast<uint32_t>(UIRT_RATE2 * 0.02f);   // 20ms default debounce interval
constexpr uint32_t kMinPeriod = static_cast<uint32_t>(UIRT_RATE2 * 0.15f);    // 150ms minimum valid period
constexpr uint32_t kMaxPeriod = static_cast<uint32_t>(UIRT_RATE2 * 1.1f);     // 1.1s maximum valid period
constexpr uint32_t kAbordMaxPeriod = static_cast<uint32_t>(UIRT_RATE2 * 1.5f); // 1.5s timeout period

//***********************************************************************************
// cSwitch - Digital switch debouncer with advanced timing analysis
//***********************************************************************************
class cSwitch
{
  public:
    // -----------------------------------------------------------------------------
    // Constructor/destructor
    cSwitch() {}   // Default constructor
    ~cSwitch() {}  // Default destructor

    // -----------------------------------------------------------------------------
    // Initializes switch with hardware parameters and timing configuration
    // @param pPort          GPIO port handle
    // @param Pin            GPIO pin number
    // @param UpdateInterval Debounce time in samples (1/SAMPLING_RATE units)
    // @param MinPeriod      Minimum valid period between presses (anti-noise)
    // @param MaxPeriod      Maximum valid period between presses
    // @param AbordMaxPeriod Absolute timeout period for resetting tracking
    void Init(GPIO_TypeDef* pPort, uint16_t Pin,
              uint32_t UpdateInterval = kUpdateTime,
              uint32_t MinPeriod = kMinPeriod,
              uint32_t MaxPeriod = kMaxPeriod,
              uint32_t AbordMaxPeriod = kAbordMaxPeriod);

    // -----------------------------------------------------------------------------
    // Processes switch input with debouncing and state tracking
    // Must be called regularly (typically from an interrupt service routine)
    ITCM void Debounce();

    // -----------------------------------------------------------------------------
    // @return Current debounced switch state (0=released, 1=pressed)
    inline uint8_t getState() const {
        return m_SwitchState;
    }

    // -----------------------------------------------------------------------------
    // Gets switch state and calculates press duration in milliseconds
    // @param PressDurationMs Output parameter for press duration
    // @return Current switch state
    inline uint8_t getState(float &PressDurationMs) const {
        PressDurationMs = static_cast<float>(m_PressDuration) / UIRT_RATE2;
        return m_SwitchState;
    }

    // -----------------------------------------------------------------------------
    // @return Count of valid period updates (for reliability metrics)
    inline uint32_t getPeriodUpdateCount() const {
        return m_PeriodUpdateCount;
    }

    // -----------------------------------------------------------------------------
    // @return Total number of detected presses since initialization
    inline uint64_t getPressCount() const {
        return m_CtPress;
    }

    // -----------------------------------------------------------------------------
    // Calculates average press frequency in Hz
    // @return Press frequency (0 if no valid period established)
    inline float getPressPeriod() const {
        return (m_AvgPeriod == 0) ? 0.0f : (m_AvgPeriod / UIRT_RATE2);
    }

  private:
    GPIO_TypeDef* m_GPIO_Port;       // Hardware GPIO port reference
    uint16_t      m_GPIO_Pin;        // Hardware GPIO pin number

    // Timing configuration
    uint32_t      m_UpdateInterval;  // Debounce interval in samples
    int32_t	      m_DebouncePeriod;  // Debounce counter samples
    uint32_t      m_MinPeriod;       // Minimum valid period between presses
    uint32_t      m_MaxPeriod;       // Maximum valid period between presses
    uint32_t      m_AbordMaxPeriod;  // Absolute timeout period
    uint8_t       m_Stop;            // State tracking flag

    // Runtime state variables
    uint8_t       m_SwitchState;     // Current debounced state (0/1)
    uint32_t      m_PressDuration;   // Current press duration in samples
    uint64_t      m_CtPress;         // Total press counter

    // Period analysis variables
    uint32_t      m_CurrentPeriod;   // Time since last valid press
    float         m_AvgPeriod;       // EMA-smoothed period between presses
    uint32_t      m_PeriodUpdateCount; // Valid period update counter
};

} // namespace DadUI
