//
//	StirManager.cpp
//
//	Created: 12/05/2017
//	Author: Nick
//

#include "TeaMachineApp.h"
#include "StirManager.h"
#include "Drink.h"
#include "Stirrer.h"
#include "SugarDispenser.h"
#include "TeaMachineController.h"

//
//	Class constructor
//
StirManager::StirManager()
{
    ClearMembers();
} //StirManager

//
//	Class constructor
//
StirManager::StirManager(TeaMachineController *pController) : ProcessManagerBase( pController )
{
    ClearMembers();
} //StirManager

//
//	Class destructor
//
StirManager::~StirManager()
{
} //~StirManager

//
//	ClearMembers()
//
//  Initialise class members
//
void StirManager::ClearMembers()
{
    m_State = StirBeginReset;
	m_lStirCompletionTime = 0;
	m_pStirrer = NULL;
	m_pSugarDispenser = NULL;
	m_nSugarDispensed = 0;
}

//
//	SetNextState()
//
//  Set the next state in the process sequence
//
void StirManager::SetNextState( StirState nextState )
{
	m_State = nextState;
}

//
//	Initialise()
//
//  Initialise the Stir manager
//
bool StirManager::Initialise()
{
    // Call the base class
    ProcessManagerBase::Initialise();
     
    // Create the stirrer object
    m_pStirrer = new Stirrer( m_pTeaMachine );
    m_pStirrer->Initialise();
     
    // Create the sugar dispenser object
    m_pSugarDispenser = new SugarDispenser( m_pTeaMachine );
    m_pSugarDispenser->Initialise();
 
    return true;
}

//
//  BeginReset()
//
//  Initiate the reset sequence
//
ProcessManagerResponse StirManager::BeginReset()
{
	// Set initial state of reset sequence
	SetNextState( StirBeginReset );
	
	SetResponse( ProcessIncomplete );
	
	return ProcessIncomplete;
}

//
//  BeginProcess()
//
//  Initiate the conveyor advance sequence
//
ProcessManagerResponse StirManager::BeginProcess()
{
	if( m_pDrink != NULL )
	{
		m_nSugarDispensed = 0;
	
		// Set initial state of conveyor advance sequence
		SetNextState( StirBeginProcess );
	
		SetResponse( ProcessIncomplete );
	}
	
	return ProcessIncomplete;
}

//
//	Continue()
//
//  Initiate the next step of the process
//
ProcessManagerResponse StirManager::Continue()
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
        
        SetResponse(response);
    }
    
    return response;
}    

//
//  Stop()
//
//  Stop all devices
//
void StirManager::Stop()
{
}

//
//	AdvanceStateMachine()
//
//  Initiate the next step of the process
//
ProcessManagerResponse StirManager::AdvanceStateMachine()
{
    ProcessManagerResponse response = ProcessIncomplete;

    switch( m_State )
    {
        // Reset step: Raise stirrer
        case StirBeginReset:
			if( !m_pStirrer->IsRaised() )
			{
				LogProcessEvent("State: StirBeginReset\n");
				m_pStirrer->Raise();
			}
			
			// Set next state and timeout
			SetNextState(StirResetCheckComplete);
			SetTimeout(STIR_TIMEOUT_RAISE, STIR_ERR_TIMEOUT_RAISE);
            break;
			
		case StirResetCheckComplete:
			if( m_pStirrer->IsRaised() && m_pSugarDispenser->IsComplete() )
			{
				LogProcessEvent("State: StirResetCheckComplete\n");
				// Process step complete - reset has finished
				ClearTimeout();
				response = ProcessComplete;
			}
			break;
                            
        // Process step:
        case StirBeginProcess:
			if( m_pStirrer->IsRaised() && m_pSugarDispenser->IsComplete() )
			{
				LogProcessEvent("State: StirBeginProcess\n");
				
				String strSugarText = "Dispensing ";
				strSugarText += m_pDrink->GetSugar();
				strSugarText += " sugars\n";
				LogProcessEvent(strSugarText);

				if( m_pDrink->GetSugar() > 0 )
				{
					// Sugar required
					SetNextState(StirDispenseSugar);
				}
				else
				{
					// No sugar required
					if( m_pDrink->GetMilk() )
					{
						// Milk was added, so just stir
						SetTimeout(STIR_TIMEOUT_ZONEACCESS, STIR_ERR_TIMEOUT_ZONEACCESS);
						SetNextState(StirWaitForAccess);
					}
					else
					{
						// No sugar or milk, so drink is ready
						SetNextState(StirRaiseCheckComplete);
					}
				}
				ClearTimeout();
			}
			
		case StirDispenseSugar:
			if( (m_nSugarDispensed < m_pDrink->GetSugar()) && m_pSugarDispenser->IsComplete() )
			{
				// Sugar is required and the dispenser is ready, first check that a cup is still present
				if( m_pController->GetDrinkLocation(m_pDrink->GetID()) == ST_STIR )
				{
					// Dispense sugar
					LogProcessEvent("State: StirBeginDispenseSugar\n");
					m_pSugarDispenser->Dispense();
				
					// Set next state and timeout
					SetNextState(StirWaitSugarDispensed);
					SetTimeout(SUGAR_TIMEOUT_FULLCYCLE, SUGAR_ERR_TIMEOUT_FULLCYCLE);
				}
				else
				{
					// Cup has been removed, so abandon this drink
					ClearTimeout();
 					m_pDrink->SetState( Invalid );
 					response = ProcessComplete;
				}
			}
			
		case StirWaitSugarDispensed:
			if( m_pSugarDispenser->IsComplete() )
			{
				LogProcessEvent("State: StirWaitSugarDispensed\n");
				
				m_nSugarDispensed++;
				
				String strDispText = "Dispensed sugar #";
				strDispText += m_nSugarDispensed;
				strDispText += "\n";
				LogProcessEvent(strDispText);
				
				if( m_nSugarDispensed < m_pDrink->GetSugar() )
				{
					// Still sugar to dispense
					SetNextState(StirDispenseSugar);
					ClearTimeout();
				}
				else
				{
					// No more sugar need, so just do the stir - must wait for access to collision zone
					SetTimeout(STIR_TIMEOUT_ZONEACCESS, STIR_ERR_TIMEOUT_ZONEACCESS);
					SetNextState(StirWaitForAccess);
				}
			}
			break;
        
		// Process step		
		case StirWaitForAccess:
			if( m_pController->LockCollisionZone( this ))
			{
				LogProcessEvent("State: StirWaitForAccess\n");
				
				// Stirrer has access to collision zone, so lower it
				m_pStirrer->Lower();
				SetTimeout(STIR_TIMEOUT_LOWER, STIR_ERR_TIMEOUT_LOWER);
				SetNextState(StirDeployedCheckComplete);				
			}
			
        // Process step:
        case StirDeployedCheckComplete:	
			if( m_pStirrer->IsLowered() )
			{
				LogProcessEvent("State: StirDeployedCheckComplete\n");
				m_lStirCompletionTime = millis() + STIR_PERIOD;
				
				// Set next state and timeout
				SetNextState(StirActionCheckComplete);
				ClearTimeout();
			}					
			break;	
		
		case StirActionCheckComplete:
			if( millis() > m_lStirCompletionTime )
			{
				LogProcessEvent("State: StirActionCheckComplete\n");
				m_pStirrer->Raise();
				
				// Set next state and timeout
				SetNextState(StirRaiseCheckComplete);
				SetTimeout(STIR_TIMEOUT_RAISE, STIR_ERR_TIMEOUT_RAISE);
			}
			break;
			
		case StirRaiseCheckComplete:
			if( m_pStirrer->IsRaised() )
			{
				// Stirrer raised so process is complete
				LogProcessEvent("State: StirRaiseCheckComplete\n");
				
				// Relinquish the lock on the collision zone
				m_pController->ReleaseCollisionZone(this);
				
				ClearTimeout();
  				m_pDrink->SetState( ReadyToDrink );
  				response = ProcessComplete;
			}
			break;
    } // switch            
            
	return response;
}

//
//	Test()
//
//  Perform a complete cycle test of the Stir process
//
bool StirManager::Test()
{	
    // Wait for action switch high
	while((m_pTeaMachine->GetSystem()->ReadInputRegisterData() & INREGMASK_ACTIONSWITCH) == 0);
    
	// Set the hardware to its initial state
	ProcessManagerResponse response = BeginReset();
	
	while( response == ProcessIncomplete )
	{
    	response = Continue();
	} // while
	
	m_pDrink = new Drink();
	m_pDrink->IncrementSugar();
	m_pDrink->IncrementSugar();
	
	response = BeginProcess();
	
	while( response == ProcessIncomplete )
	{
		response = Continue();
	} // while
	
	return (response == ProcessComplete);
}
