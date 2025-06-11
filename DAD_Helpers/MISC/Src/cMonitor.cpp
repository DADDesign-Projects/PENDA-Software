//****************************************************************************
// Performance/Load Monitor
//
// File: cMonitor.cpp
// Copyright (c) 2025 Dad Design.
//****************************************************************************
#include "cMonitor.h"


namespace DadMisc {

//****************************************************************************
// Class cMonitor: encapsulates execution time monitoring routines
//****************************************************************************
// -----------------------------------------------------------------------
// Constructor
cMonitor::cMonitor(){
    m_call_count=0;
    m_total_execution_cycles=0;
    m_min_execution_cycles=UINT32_MAX;
    m_max_execution_cycles=0;
    m_start_cycles=0;

    // Frequency statistics
    m_last_call_cycles=0;
    m_total_period_cycles=0;
    m_min_period_cycles=UINT32_MAX;
    m_max_period_cycles=0;

    // Configuration
    m_cpu_frequency=SystemCoreClock;
    m_monitoring_active=false;
}

// -----------------------------------------------------------------------
// Initialize
void cMonitor::Init(){
	m_start_cycles = 0;
	m_last_call_cycles = 0;
	SystemCoreClockUpdate();
	m_cpu_frequency = SystemCoreClock;
	m_monitoring_active=false;
	reset();
	initDWT();
}

// -----------------------------------------------------------------------
// DWT initialization (static, done once)
void cMonitor::initDWT() {
	if (!m_dwt_initialized) {
		CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;   // Enable trace
		DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;              // Enable cycle counter
		DWT->CYCCNT = 0;                                  // Reset counter
		m_dwt_initialized = true;
	}
}

// -----------------------------------------------------------------------
// Reset statistics
void cMonitor::reset() {
	m_call_count = 0;
	m_total_execution_cycles = 0;
	m_min_execution_cycles = UINT32_MAX;
	m_max_execution_cycles = 0;
	m_total_period_cycles = 0;
	m_min_period_cycles = UINT32_MAX;
	m_max_period_cycles = 0;
	m_last_call_cycles = 0;
}

// -----------------------------------------------------------------------
// Getters for execution time in microseconds
float cMonitor::getAverageExecutionTime_us() const {
	if (m_call_count == 0) return 0.0f;
	float avg_cycles = (float)m_total_execution_cycles / m_call_count;
	return (avg_cycles * 1000000.0f) / m_cpu_frequency;
}

float cMonitor::getMinExecutionTime_us() const {
	if (m_min_execution_cycles == UINT32_MAX) return 0.0f;
	return ((float)m_min_execution_cycles * 1000000.0f) / m_cpu_frequency;
}

float cMonitor::getMaxExecutionTime_us() const {
	return ((float)m_max_execution_cycles * 1000000.0f) / m_cpu_frequency;
}

// -----------------------------------------------------------------------
// Getters for call frequency in Hertz
float cMonitor::getAverageFrequency_Hz() const {
	if (m_call_count <= 1) return 0.0f;
	float m_avg_period_cycles = (float)m_total_period_cycles / (m_call_count - 1);
	return (float)m_cpu_frequency / m_avg_period_cycles;
}

float cMonitor::getMinFrequency_Hz() const {
	if (m_max_period_cycles == 0) return 0.0f;
	return (float)m_cpu_frequency / m_max_period_cycles;
}

float cMonitor::getMaxFrequency_Hz() const {
	if (m_min_period_cycles == UINT32_MAX) return 0.0f;
	return (float)m_cpu_frequency / m_min_period_cycles;
}

// -----------------------------------------------------------------------
// CPU load estimation
float cMonitor::getCPULoad_percent() const {
	if (m_call_count <= 1) return 0.0f;
	float avg_exec_time = getAverageExecutionTime_us();
	float avg_frequency = getAverageFrequency_Hz();
	return (avg_exec_time * avg_frequency) / 10000.0f; // Convert to %
}

// -----------------------------------------------------------------------
// Getters for raw data
uint32_t cMonitor::getAverageExecutionCycles() const {
	if (m_call_count == 0) return 0;
	return m_total_execution_cycles / m_call_count;
}
uint32_t cMonitor::getMinExecutionCycles() const {
	return m_min_execution_cycles == UINT32_MAX ? 0 : m_min_execution_cycles;
}


// Static variable definition
bool cMonitor::m_dwt_initialized = false;
}// DadMisc
