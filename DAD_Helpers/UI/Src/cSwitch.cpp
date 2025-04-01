//====================================================================================
// cSwitch.h
// Switch debouncing and state tracking class with configurable timing parameters
// Features:
// - Configurable debounce interval
// - Minimum/maximum period detection
// - Press duration tracking
// - Exponential Moving Average (EMA) for period calculation
//====================================================================================
#include "cSwitch.h"

namespace DadUI {

// -----------------------------------------------------------------------------
// Initializes switch parameters and resets internal state
void cSwitch::Init(GPIO_TypeDef* pPort, uint16_t Pin,
                  uint32_t UpdateInterval, uint32_t MinPeriod,
                  uint32_t MaxPeriod, uint32_t AbordMaxPeriod)
{
    // Configure hardware interface
    m_GPIO_Port = pPort;
    m_GPIO_Pin = Pin;

    // Set timing parameters
    m_UpdateInterval = UpdateInterval;
    m_MinPeriod = MinPeriod;
    m_MaxPeriod = MaxPeriod;
    m_AbordMaxPeriod = AbordMaxPeriod;

    // Initialize state variables
    m_Stop = 1;  // Start in stopped state
    m_SwitchState = 0;
    m_PressDuration = 0;
    m_CtPress = 0;
    m_CurrentPeriod = 0;
    m_AvgPeriod = 0;
    m_PeriodUpdateCount = 0;
    m_DebouncePeriod = -1;
}

// -----------------------------------------------------------------------------
// Processes switch input with debouncing and timing analysis
// Implements:
// - Contact bounce filtering
// - Minimum/maximum period validation
// - Exponential Moving Average (EMA) period calculation
// - Automatic timeout handling
void cSwitch::Debounce()
{
    // Increment period counters
    m_CurrentPeriod++;
    if(m_SwitchState == 1) {
        m_PressDuration++;  // Track press duration
    }

    // Handle timeout condition
    if (m_CurrentPeriod > m_AbordMaxPeriod) {
        m_PeriodUpdateCount = 0;
        m_AvgPeriod = 0;
        m_Stop = 1;  // Enter stopped state
    }

    // Read physical switch state (active low configuration)
    bool isSwitchPressed = (HAL_GPIO_ReadPin(m_GPIO_Port, m_GPIO_Pin) == GPIO_PIN_RESET);

    if (isSwitchPressed) {
        // Detect new press events
        if (m_SwitchState == 0) {
            m_SwitchState = 1;  // Set pressed state
            m_PressDuration = 0;
            m_CtPress++;  // Increment press counter

            // Handle first press after timeout
            if (m_Stop == 1) {
                m_Stop = 0;  // Exit stopped state
                m_CurrentPeriod = 0;
            }

            // Validate and process period between presses
            if ((m_CurrentPeriod > m_MinPeriod) && (m_CurrentPeriod < m_MaxPeriod)) {
                // Calculate new EMA period with smoothing factor
                float newAvgPeriod = (float)m_CurrentPeriod;
                if (m_PeriodUpdateCount > 0) {
                	const float ALPHA = 0.2f; // EMA smoothing factor (between 0 and 1)
                	newAvgPeriod = ALPHA * newAvgPeriod + (1.0f - ALPHA) * m_AvgPeriod;

                	// Limit variations to ±15%
                	float minAllowed = m_AvgPeriod * 0.85f;
                	float maxAllowed = m_AvgPeriod * 1.15f;
                	if (newAvgPeriod < minAllowed) newAvgPeriod = minAllowed;
                	if (newAvgPeriod > maxAllowed) newAvgPeriod = maxAllowed;
                }
                // Update tracking variables
                m_AvgPeriod = newAvgPeriod;
                m_PeriodUpdateCount++;
                // Reset current period after processing
                m_CurrentPeriod = 0;
            }
        }
    } else if(m_SwitchState == 1){
    	// Handle switch release with debounce delay
    	if((m_DebouncePeriod == -1)){
        	m_DebouncePeriod = m_UpdateInterval;
    	}else{
        	m_DebouncePeriod--;
        	if(m_DebouncePeriod <= 0){
        		m_SwitchState = 0;
        		m_DebouncePeriod = -1;
        	}
    	}
    }
}

} // namespace DadUI
