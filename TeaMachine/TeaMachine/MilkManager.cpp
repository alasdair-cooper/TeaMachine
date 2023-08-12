//
//	MilkManager.cpp
//
//	Created: 12/05/2017
//	Author: Nick
//

#include "TeaMachineApp.h"
#include "MilkManager.h"
#include "MilkDispenser.h"
#include "Drink.h"
#include "TeaMachineController.h"

//
//	Class constructor
//
MilkManager::MilkManager()
{
    ClearMembers();
} //MilkManager

//
//	Class constructor
//
MilkManager::MilkManager(TeaMachineController *pController) : ProcessManagerBase( pController )
{
    ClearMembers();
} //MilkManager

//
//	Class destructor
//
MilkManager::~MilkManager()
{
} //~MilkManager

//
//	ClearMembers()
//
//  Initialise class members
//
void MilkManager::ClearMembers()
{
    m_State = MilkResetDispenser;
	m_pMilkDispenser = NULL;
	m_nMilkDispensesRequired = 0;
	m_lNextDispenseTime = 0;
}

//
//	SetNextState()
//
//  Set the next state in the process sequence
//
void MilkManager::SetNextState( MilkState nextState )
{
    m_State = nextState;
}

//
//	Initialise()
//
//  Initialise the UI manager
//
bool MilkManager::Initialise()
{
    // Call the base class
    ProcessManagerBase::Initialise();
    
	// Create the Milk Dispenser object
	m_pMilkDispenser = new MilkDispenser( m_pTeaMachine );
	m_pMilkDispenser->Initialise();

	return true;
}

//
//	Continue()
//
//  Initiate the next step of the process
//
ProcessManagerResponse MilkManager::Continue()
{
    ProcessManagerResponse response = GetLastResponse();
    
    if( (GetLastResponse() != ProcessFailed) && (GetLastResponse() != ProcessComplete) )
    {
        response = ProcessFailed;
        
        // Check timeout
        if( !CheckTimeoutExpired() )
        {
            // Advance to the next state in the process
            response = AdvanceStateMachine();
        }
        else
        {
            // Timeout has expired
			String strTimeoutText = "TIMEOUT code: ";
			strTimeoutText += GetLastError();
			strTimeoutText + "\n";
            LogProcessEvent(strTimeoutText);
        }
        
        if( response == ProcessFailed)
        {
            // Process has failed, so make sure milk dispenser is in the raised position
            m_pMilkDispenser->Raise();
        }
        
        SetResponse(response);
    }
    
    return response;
}    

//
//  BeginReset()
//
//  Initiate the reset sequence
//
ProcessManagerResponse MilkManager::BeginReset()
{
	// Set initial state of reset sequence
	SetNextState( MilkResetDispenser );
	
	SetResponse( ProcessIncomplete );
	
	return ProcessIncomplete;
}

//
//  BeginProcess()
//
//  Initiate the conveyor advance sequence
//
ProcessManagerResponse MilkManager::BeginProcess()
{
	// Initialise milk dispense process
	if( m_pDrink != NULL )
	{
		m_nMilkDispensesRequired = m_pDrink->GetMilk() * DISPENSE_FACTOR;
	}

	String strDispText = "Milk dispenses required: ";
	strDispText += m_nMilkDispensesRequired;
	strDispText += "/n";	
	LogProcessEvent(strDispText);

	m_lNextDispenseTime = millis();
	
	// Set initial state of milk dispensing sequence
	SetNextState( MilkBeginDispense );
	
	SetResponse( ProcessIncomplete );
	
	return ProcessIncomplete;
}

//
//  Stop()
//
//  Stop all devices
//
void MilkManager::Stop()
{
	m_pMilkDispenser->Stop();
}

//
//	AdvanceStateMachine()
//
//  Initiate the next step of the process
//
ProcessManagerResponse MilkManager::AdvanceStateMachine()
{
    ProcessManagerResponse response = ProcessIncomplete;

    switch( m_State )
    {
        // Reset step: Raise milk dispenser
        case MilkResetDispenser:
			if( !m_pMilkDispenser->IsRaised() )
			{
				LogProcessEvent("State: MilkResetDispenser\n");
				m_pMilkDispenser->Raise();
			}
			
			// Set next state and timeout
			SetNextState(MilkResetCheckComplete);
			SetTimeout(MILK_TIMEOUT_RAISE, MILK_ERR_TIMEOUT_RAISE);
			break;
        
       // Reset step: Check reset complete
       case MilkResetCheckComplete:
		   if( m_pMilkDispenser->IsRaised() )
		   {
			   LogProcessEvent("State: MilkResetCheckComplete\n");
			   // Process step complete - reset has finished
			   ClearTimeout();
			   response = ProcessComplete;
		   }
		   break;
       
        // Process step: Initiate dispense cycle
        case MilkBeginDispense:
			if( m_nMilkDispensesRequired > 0 )
			{
				// Request/check access to collision zone
				SetTimeout(MILK_TIMEOUT_ZONEACCESS, MILK_ERR_TIMEOUT_ZONEACCESS);
				if( m_pController->LockCollisionZone(this) )
				{
					LogProcessEvent("State: MilkBeginDispense\n");
					
					// Lock acquired, so proceed to dispense stage
					SetNextState(MilkInitiateCycle);
				}
			}
			else
			{
			    // Dispense is finished - wait for dispenser to clear the collision zone
				SetNextState(MilkCheckZoneClear);
				SetTimeout(MILK_TIMEOUT_FULLCYCLE, MILK_ERR_TIMEOUT_FULLCYCLE);
			}
			break;
			
		case MilkInitiateCycle:
			// Milk is required so, check to see if the dispenser can be lowered yet
			if( millis() > m_lNextDispenseTime )
			{
		        LogProcessEvent("State: MilkInitiateCycle\n");
				// Initiate dispense cycle
				m_pMilkDispenser->Dispense();
				
				// Set next state and timeout
				SetNextState(MilkCheckCycleComplete);
				SetTimeout(MILK_TIMEOUT_FULLCYCLE, MILK_ERR_TIMEOUT_FULLCYCLE);
			}
            break;

        // Process step: Check for completion of dispense cycle
        case MilkCheckCycleComplete:
       		if( m_pMilkDispenser->IsRaising() )
			{
				LogProcessEvent("State: MilkCheckCycleComplete\n");
				m_nMilkDispensesRequired--;
				
				// Calculate time at which next dispense may begin
				m_lNextDispenseTime = millis() + MILK_DISPENSE_INTERVAL;
				
				// Set next state and timeout
				SetNextState(MilkBeginDispense);
				SetTimeout(MILK_TIMEOUT_FULLCYCLE, MILK_ERR_TIMEOUT_FULLCYCLE);
			}
			break;
			
		// Process step: Check that dispenser is clear of collision zone and release lock
		case MilkCheckZoneClear:
			if( m_pMilkDispenser->IsRaised())
			{
				LogProcessEvent("State: MilkCheckZoneClear\n");
				
			    // Process complete
				m_pController->ReleaseCollisionZone(this);
			    ClearTimeout();
			    m_pDrink->SetState( MilkStageComplete );
			    LogProcessEvent("Milk dispense complete\n");
			    response = ProcessComplete;				
			}
			break;
    } // switch            
            
	return response;
}

//
//	Test()
//
//  Perform a complete cycle test of the Milk process
//
bool MilkManager::Test()
{
	// Wait for action switch high
	while((m_pTeaMachine->GetSystem()->ReadInputRegisterData() & INREGMASK_ACTIONSWITCH) == 0);
	
	// Set the hardware to its initial state
	ProcessManagerResponse response = BeginReset();
	
	while( response == ProcessIncomplete )
	{
		response = Continue();
	} // while
	
	m_pDrink = new Drink;
	
	response = BeginProcess();
	
	while( response == ProcessIncomplete )
	{
		response = Continue();
	} // while
	
	return (response == ProcessComplete);
}

//
//	SetLoadPosition()
//
//  Set milk dispenser to load position
//
bool MilkManager::SetLoadPosition()
{
	// Wait for action switch high
//	while((m_pTeaMachine->GetSystem()->ReadInputRegisterData() & INREGMASK_ACTIONSWITCH) == 0);
	
	// Initialise hardware
	ProcessManagerResponse response = BeginReset();
	
	while( response == ProcessIncomplete )
	{
		response = Continue();
	} // while
	
	// Set milk dispenser to load position
	m_pMilkDispenser->SetLoadPosition();
			
	return (response == ProcessComplete);
}
