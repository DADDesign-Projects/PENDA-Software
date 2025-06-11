#pragma once
//****************************************************************************
// Performance/Load Monitor
//
// File: cMonitor.h
// Copyright (c) 2025 Dad Design.
//****************************************************************************
#include "main.h"
#include <cstdint>
#include <limits>

namespace DadMisc {
//****************************************************************************
// Class cMonitor: encapsulates execution time monitoring routines
//****************************************************************************
class cMonitor {

public:
	// -----------------------------------------------------------------------
	// Constructor
    cMonitor();

    // -----------------------------------------------------------------------
	// Initialize
    void Init();

    // -----------------------------------------------------------------------
    // DWT initialization (static, done once)
    static void initDWT();

    // -----------------------------------------------------------------------
    // Start monitoring (to be called at the beginning of the function to monitor)
    inline void startMonitoring() {
        if (!m_monitoring_active) {
            uint32_t current_cycles = DWT->CYCCNT;

            // Compute call period
            if (m_last_call_cycles != 0 && m_call_count > 0) {
                uint32_t period = current_cycles - m_last_call_cycles;
                m_total_period_cycles += period;

                if (period < m_min_period_cycles) {
                    m_min_period_cycles = period;
                }
                if (period > m_max_period_cycles) {
                    m_max_period_cycles = period;
                }
            }

            m_last_call_cycles = current_cycles;
            m_start_cycles = current_cycles;
            m_monitoring_active = true;
        }
    }

    // -----------------------------------------------------------------------
    // Stop monitoring (to be called at the end of the function to monitor)
    inline void stopMonitoring() {
        if (m_monitoring_active) {
            uint32_t execution_cycles = DWT->CYCCNT - m_start_cycles;

            // Update execution statistics
            m_total_execution_cycles += execution_cycles;
            m_call_count++;

            if (execution_cycles < m_min_execution_cycles) {
                m_min_execution_cycles = execution_cycles;
            }
            if (execution_cycles > m_max_execution_cycles) {
                m_max_execution_cycles = execution_cycles;
            }

            m_monitoring_active = false;
        }
    }

    // -----------------------------------------------------------------------
    // Reset statistics
    void reset();

    // -----------------------------------------------------------------------
    // Getters for execution time in microseconds
    float getAverageExecutionTime_us() const;
    float getMinExecutionTime_us() const;
    float getMaxExecutionTime_us() const;

    // -----------------------------------------------------------------------
    // Getters for call frequency in Hertz
    float getAverageFrequency_Hz() const;
    float getMinFrequency_Hz() const;
    float getMaxFrequency_Hz() const;

    // -----------------------------------------------------------------------
    // CPU load estimation
    float getCPULoad_percent() const;

    // -----------------------------------------------------------------------
    // Getters for raw data
    inline uint32_t getCallCount() const { return m_call_count; }
    uint32_t getAverageExecutionCycles() const;
    uint32_t getMinExecutionCycles() const;
    inline uint32_t getMaxExecutionCycles() const {
        return m_max_execution_cycles;
    }

private:
    // Execution time statistics
    volatile uint32_t m_call_count;
    volatile uint32_t m_total_execution_cycles;
    volatile uint32_t m_min_execution_cycles;
    volatile uint32_t m_max_execution_cycles;
    volatile uint32_t m_start_cycles;

    // Frequency statistics
    volatile uint32_t m_last_call_cycles;
    volatile uint32_t m_total_period_cycles;
    volatile uint32_t m_min_period_cycles;
    volatile uint32_t m_max_period_cycles;

    // Configuration
    uint32_t 		m_cpu_frequency;
    bool 			m_monitoring_active;

    static bool 	m_dwt_initialized;
};

}// DadMisc
