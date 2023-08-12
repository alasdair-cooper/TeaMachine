//
//	ProcessManagerBase.cpp
//
//	Created: 12/05/2017
//	Author: Nick
//

#include "ProcessManagerBase.h"
#include "TeaMachineApp.h"
#include "TeaMachineController.h"
#include "Drink.h"
#include "WString.h"

//
//	Class constructor
//
ProcessManagerBase::ProcessManagerBase()
{
    ClearMembers();
    m_pTeaMachine = NULL;
    m_pController = NULL;
	m_pDrink = NULL;
} //ProcessManagerBase

//
//	Class constructor
//
ProcessManagerBase::ProcessManagerBase(TeaMachineController *pController)
{
    ClearMembers();
    m_pController = pController;
    m_pTeaMachine = m_pController->GetTeaMachine();
	m_pDrink = NULL;
} //ProcessManagerBase

//
//	Class destructor
//
ProcessManagerBase::~ProcessManagerBase()
{
} //~ProcessManagerBase

//
//  ClearMembers
//
//  Initialise class members
//
void ProcessManagerBase::ClearMembers()
{
    m_lTimeout = 0;
    m_lStartTime = 0;
    m_lErrorCode = PROC_OK;
    m_LastResponse = ProcessComplete;
}

//
//	Initialise()
//
//  Initialise the process manager
//
bool ProcessManagerBase::Initialise()
{
    ClearMembers();
    m_LastResponse = ProcessComplete;
	return true;
}

//
//	Continue()
//
//  Initiate the next step of the process
//
ProcessManagerResponse ProcessManagerBase::Continue()
{
    m_LastResponse = ProcessComplete;
	return m_LastResponse;
}

//
//	Test()
//
//  Perform a complete cycle test
//
bool ProcessManagerBase::Test()
{
	return true;
}

//
//	SetTimeout()
//
//  Set the timeout period in milliseconds
//
void ProcessManagerBase::SetTimeout( unsigned long lTimeout )
{
    m_lTimeout = lTimeout;
    m_lStartTime = millis();
    m_lApplicableTimeoutErrorCode = PROC_TIMEOUT;
}

//
//  Set the timeout period in milliseconds
//
void ProcessManagerBase::SetTimeout( unsigned long lTimeout, long lApplicableErrorCode )
{
    m_lTimeout = lTimeout;
    m_lStartTime = millis();
    m_lApplicableTimeoutErrorCode = lApplicableErrorCode;
}

//
//  ClearTimeout()
//
//  Clear the timeout
//
void ProcessManagerBase::ClearTimeout()
{
    m_lTimeout = 0;
    m_lStartTime = millis();
    m_lApplicableTimeoutErrorCode = PROC_TIMEOUT;
}

//
//	CheckTimeoutExpired()
//
//  Returns true if the timeout period has expired
//
bool ProcessManagerBase::CheckTimeoutExpired()
{
    bool bExpired = ((m_lStartTime + m_lTimeout) < millis()) && (m_lTimeout > 0);
    
    SetErrorCode( bExpired ? m_lApplicableTimeoutErrorCode : PROC_OK );
    
	return bExpired;
}

//
//	SetErrorCode()
//
//  Set process error code
//
void ProcessManagerBase::SetErrorCode(long lErrorCode)
{
	m_lErrorCode = lErrorCode;
}

//
//	LogProcessEvent()
//
//  Log an event connected with a process step
//
void ProcessManagerBase::LogProcessEvent(String strText)
{
	if( m_pDrink != NULL )
	{
		Serial.print("Drink #");
		Serial.print(m_pDrink->GetID());
		Serial.print(": ");
	}

	Serial.print(strText);
}
