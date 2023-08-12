//
//	ConveyorManager.cpp
//
//	Created: 12/05/2017
//	Author: Nick
//

#include "TeaMachineApp.h"
#include "ConveyorManager.h"
#include "CupConveyor.h"
#include "TeaMachineController.h"

//
//	Class constructor
//
ConveyorManager::ConveyorManager()
{
    ClearMembers();
} //ConveyorManager

//
//	Class constructor
//
ConveyorManager::ConveyorManager(TeaMachineController *pController) : ProcessManagerBase( pController )
{
    ClearMembers();
} //ConveyorManager

//
//	Class destructor
//
ConveyorManager::~ConveyorManager()
{
} //~ConveyorManager

//
//	ClearMembers()
//
//  Initialise class members
//
void ConveyorManager::ClearMembers()
{
	m_pCupConveyor = NULL;
    m_State = ResetSynchroniseConveyor;
}

//
//	SetNextState()
//
//  Set the next state in the process sequence
//
void ConveyorManager::SetNextState( ConveyorState nextState )
{
    m_State = nextState;
}

//
//	Initialise()
//
//  Initialise the conveyor manager
//
bool ConveyorManager::Initialise()
{
    // Call the base class
    ProcessManagerBase::Initialise();

    // Create and initialise a cup conveyor object
	m_pCupConveyor = new CupConveyor(m_pTeaMachine);
	
	return m_pCupConveyor->Initialise();
}

//
//  BeginReset()
//
//  Initiate the reset sequence
//
ProcessManagerResponse ConveyorManager::BeginReset()
{
	// Set initial state of reset sequence
	SetNextState(ResetSynchroniseConveyor);
	
	SetResponse(ProcessIncomplete);
	
	return ProcessIncomplete;
}

//
//  BeginProcess()
//
//  Initiate the conveyor advance sequence
//
ProcessManagerResponse ConveyorManager::BeginProcess()
{
	SetTimeout(CONVEYOR_TIMEOUT_ADVANCE, CONVEYOR_ERR_TIMEOUT_ADVANCE);
	
	// Set initial state of conveyor advance sequence
	SetNextState( ConveyorBeginAdvance );
	
	SetResponse(ProcessIncomplete);
	
	return ProcessIncomplete;
}

//
//	Continue()
//
//  Initiate the next step of the process
//
ProcessManagerResponse ConveyorManager::Continue()
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
            Serial.print("TIMEOUT code: ");
            Serial.print(GetLastError());
            Serial.print("\n");
        }
        
        SetResponse(response);
    }
    
    return response;
}    

//
//  Stop()
//
//  Stop all devices
//
void ConveyorManager::Stop()
{
	m_pCupConveyor->Stop();
}

//
//	AdvanceStateMachine()
//
//  Initiate the next step of the process
//
ProcessManagerResponse ConveyorManager::AdvanceStateMachine()
{
    ProcessManagerResponse response = ProcessIncomplete;

    switch( m_State )
    {
        // Reset step
        case ResetSynchroniseConveyor:
            Serial.print("State: ResetSynchroniseConveyor\n");
            if( m_pCupConveyor->IsStationary() )
            {
                // Process step complete - reset has finished
                ClearTimeout();
                response = ProcessComplete;
            }
            
            break;
                    
        // Process step: Request conveyor advance
        case ConveyorBeginAdvance:
            Serial.print("State: ConveyorBeginAdvance\n");
            m_pCupConveyor->BeginAdvance();
            SetTimeout(CONVEYOR_TIMEOUT_ADVANCE, CONVEYOR_ERR_TIMEOUT_ADVANCE);
            SetNextState(ConveyorAdvancing);
            break;
                   
        // Process step: Conveyor advance is in process
        case ConveyorAdvancing:
			if( m_pCupConveyor->IsStationary())
			{
				Serial.print("State: ConveyorAdvanceComplete\n");
				ClearTimeout();
				m_pController->NotifyConveyorAdvanceComplete();
	            SetNextState(ConveyorAdvanceComplete);
			}
			break;
		
        // Process step: Conveyor advance is complete
        case ConveyorAdvanceComplete:
			response = ProcessComplete;
			break;
    } // switch            
            
	return response;
}

//
//	Test()
//
//  Perform a complete cycle test of the conveyor process
//
bool ConveyorManager::Test()
{	
    // Wait for action switch high
	while((m_pTeaMachine->GetSystem()->ReadInputRegisterData() & INREGMASK_ACTIONSWITCH) == 0);
    
	// Set the hardware to its initial state
	ProcessManagerResponse response = BeginReset();
	
	while( response == ProcessIncomplete )
	{
    	response = Continue();
	} // while
    
	return (response == ProcessComplete);
}
