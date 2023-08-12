//
//	BrewManager.cpp
//
//	Created: 12/05/2017
//	Author: Nick
//

#include "TeaMachineApp.h"
#include "BrewManager.h"
#include "Boiler.h"
#include "BagConveyor.h"
#include "CupLevelProbe.h"
#include "Extractor.h"
#include "Masher.h"
#include "Drink.h"
#include "TeaMachineController.h"
#include "WString.h"

//
//	Class constructor
//
BrewManager::BrewManager()
{
    ClearMembers();
} //BrewManager

//
//	Class constructor
//
BrewManager::BrewManager(TeaMachineController *pController) : ProcessManagerBase( pController )
{
    ClearMembers();
} //BrewManager

//
//	Class destructor
//
BrewManager::~BrewManager()
{
} //~BrewManager

//
//	ClearMembers()
//
//  Initialise class members
//
void BrewManager::ClearMembers()
{
	m_pBoiler = NULL;
	m_pBagConveyor = NULL;
	m_pCupLevelProbe = NULL;
	m_pExtractor = NULL;
	m_pMasher = NULL;
	m_State = BrewResetRaiseMasher;
	m_lMashCount = 0;
	m_lMashesRequired = 0;
	m_lTimeRaiseMustStop = 0;
	m_fInitialWaterTemp = 0;
	m_lLastTemperatureSent = 0;
	m_bDrinkAbandoned = false;
}

//
//	SetNextState()
//
//  Set the next state in the process sequence
//
void BrewManager::SetNextState( BrewState nextState )
{
    m_State = nextState;
}

//
//	Initialise()
//
//  Initialise the brew manager to begin a new brewing process
//
bool BrewManager::Initialise()
{
    // Call the base class
    ProcessManagerBase::Initialise();
 
 	// Create instances of all Tea Machine components required by the brewing process
	m_pBoiler = new Boiler(m_pTeaMachine);
	m_pBagConveyor = new BagConveyor(m_pTeaMachine);
	m_pCupLevelProbe = new CupLevelProbe(m_pTeaMachine);
	m_pExtractor = new Extractor(m_pTeaMachine);
	m_pMasher = new Masher(m_pTeaMachine);
	
	// Initialise all components
	m_pBoiler->Initialise();
	m_pBagConveyor->Initialise();
	m_pCupLevelProbe->Initialise();
	m_pExtractor->Initialise();
	m_pMasher->Initialise();

    return true;
}

//
//  BeginReset()
//
//  Initiate the reset sequence
//
ProcessManagerResponse BrewManager::BeginReset()
{
	// Set initial state of reset sequence
	SetNextState(BrewResetRaiseMasher);
	
	SetResponse(ProcessIncomplete);
	
	return ProcessIncomplete;
}

//
//  BeginProcess()
//
//  Initiate the brew sequence
//
ProcessManagerResponse BrewManager::BeginProcess()
{
	// Store current temperature for use later
	m_fInitialWaterTemp = m_pBoiler->GetWaterTemperature();

	// Determine number of mashes required
	m_lMashCount = 0;
	m_lMashesRequired = m_pDrink->GetStrength() * BREW_MASHFACTOR;
	
	m_lLastTemperatureSent = 0;
	
	String strMashText = "Mashes required: ";
	strMashText += m_lMashesRequired;
	strMashText += "\n";
	LogProcessEvent( strMashText );
	
	// Set initial state of brewing sequence
	SetNextState( BrewDispenseLowerExtractor );
	
	SetResponse(ProcessIncomplete);
	
	return ProcessIncomplete;
}

//
//	Continue()
//
//  Initiate the next step of the process
//
ProcessManagerResponse BrewManager::Continue()
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
            // Brewing process has failed, so make sure everything is switched off
            Stop();
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
void BrewManager::Stop()
{
	m_pBoiler->EnableHeater(false);
	m_pBoiler->EnableExitValve(false);
	m_pExtractor->Stop();
	m_pMasher->Stop();
	m_pBagConveyor->Stop();
	m_pCupLevelProbe->Stop();
}

//
//	AdvanceStateMachine()
//
//  Initiate the next step of the process
//
ProcessManagerResponse BrewManager::AdvanceStateMachine()
{
    ProcessManagerResponse response = ProcessIncomplete;

    switch( m_State )
    {
        // Reset step 1: Raise extractor and cup level probe, switch off boiler and close exit vale
        case BrewResetRaiseMasher:
            LogProcessEvent("State: BrewResetRaiseMasher\n");
            m_pCupLevelProbe->Raise();
            m_pBoiler->EnableHeater(false);
            m_pBoiler->EnableExitValve(false);
            m_pMasher->Raise();
          
            // Set next state and timeout
            SetNextState(BrewResetRaiseExtractor);
            SetTimeout(BREW_TIMEOUT_MASHER, BREW_ERR_TIMEOUT_MASHER);
            break;
            
        // Reset step 2: If the masher is raised, raise the extractor 
        case BrewResetRaiseExtractor:
            if(m_pMasher->IsRaised())
            {
                LogProcessEvent("State: BrewResetRaiseExtractor\n");
                m_pExtractor->Raise();
                SetTimeout(BREW_TIMEOUT_EXTRACTOR, BREW_ERR_TIMEOUT_EXTRACTOR);
                SetNextState(BrewResetComplete);
            }                    
            break;
            
        // Reset step 3: If the extractor is raised, reset is complete
        case BrewResetComplete:
            if(m_pExtractor->IsRaised())
            {
                LogProcessEvent("State: BrewResetComplete\n");
				
                // Process step complete - reset has finished
                ClearTimeout();
                response = ProcessComplete;
            }
            break;
        
        // Brew step 1: Lower the extractor and switch on the boiler
        case BrewDispenseLowerExtractor:
            LogProcessEvent("State: BrewDispenseLowerExtractor\n");
			if( m_pDrink->GetType() == Tea )
			{
				m_pExtractor->Lower();
			}
            m_pBoiler->EnableHeater(true);
            SetTimeout(BREW_TIMEOUT_EXTRACTOR, BREW_ERR_TIMEOUT_EXTRACTOR);
            SetNextState(BrewDispenseTeabag);
            break;
        
        // Brew step 2: If extractor is lowered, dispense teabag from bag conveyor    
        case BrewDispenseTeabag:
            if( m_pExtractor->IsLowered() )
            {
                LogProcessEvent("State: BrewDispenseTeabag\n");
				
				// Check cup is still present before dispensing teabag
				if( m_pController->GetDrinkLocation(m_pDrink->GetID()) == ST_BREW )
				{
					m_pBagConveyor->BeginAdvance();
					SetTimeout(BREW_TIMEOUT_CONVEYOR, BREW_ERR_TIMEOUT_CONVEYOR);
					SetNextState(BrewAwaitBoil);
				}
				else
				{
					// Cup is not present, so abandon this drink by marking it as invalid
					m_pDrink->SetState( Invalid );
					m_bDrinkAbandoned = true;
					m_pBoiler->EnableHeater(false);
					m_pExtractor->Raise();
					LogProcessEvent("Cup has been removed - drink abandoned\n");
					SetTimeout(BREW_TIMEOUT_EXTRACTOR, BREW_ERR_TIMEOUT_EXTRACTOR);
					SetNextState(BrewComplete);
				}
            }
            break;
			
        // Brew step 3: If teabag is dispensed, await a rise in water temperature
        case BrewAwaitBoil:
            if( m_pBagConveyor->IsStationary() )
            {
				LogProcessEvent("State: BrewAwaitBoil\n");
				SetTimeout(BREW_TIMEOUT_HEAT, BREW_ERR_TIMEOUT_HEAT);
				SetNextState(BrewReleaseWater);
			}
			break;
        
        // Brew step 4: If water is up to temperature, deploy level probe and open the water valve
        case BrewReleaseWater:
			{
				float waterTemperature = m_pBoiler->GetWaterTemperature();
				if( millis() > (m_lLastTemperatureSent + 5000) )
				{
					// Log water temperature every five seconds
					String strTempText = "Water temperature: ";
					strTempText += waterTemperature;
					strTempText += "\n";
					LogProcessEvent(strTempText);
					
					m_lLastTemperatureSent = millis();
				}
                
				if( (waterTemperature > (m_fInitialWaterTemp + 5)) || (waterTemperature > (BREW_WATERTEMPERATURE_MIN - 5)) )
				{
					// Water temperature is rising, so heater is probably OK
					// Only need to time out if water doesn't boil   
					SetTimeout(BREW_TIMEOUT_BOIL, BREW_ERR_TIMEOUT_BOIL);
				}                    
                
				if( waterTemperature > BREW_WATERTEMPERATURE_MIN )
				{
					LogProcessEvent("State: BrewReleaseWater\n");

					// Water has attained correct temperature for brewing, so switch off heater
					m_pBoiler->EnableHeater(false);

					// Check cup is still present before opening valve
					if( m_pController->GetDrinkLocation(m_pDrink->GetID()) == ST_BREW )
					{
						// Lower the cup-level probe and release water
						m_pCupLevelProbe->Lower();
						delay(1000);    // Avoids big EMI pulse
						m_pBoiler->EnableExitValve(true);
						SetTimeout(BREW_TIMEOUT_CUPFILL, BREW_ERR_TIMEOUT_CUPFILL);
						SetNextState(BrewRaiseProbe);					
					}
					else
					{
						// Cup is not present, so abandon this drink by marking it as invalid
						m_pDrink->SetState( Invalid );
						m_bDrinkAbandoned = true;
						m_pExtractor->Raise();
						LogProcessEvent("Cup has been removed - drink abandoned\n");
						SetTimeout(BREW_TIMEOUT_EXTRACTOR, BREW_ERR_TIMEOUT_EXTRACTOR);
						SetNextState(BrewComplete);
					}
				}
			}
            break;
        
        // Brew step 5: If cup is full, raise probe, close valve and switch off boiler
        case BrewRaiseProbe:
            if( m_pCupLevelProbe->CupIsFull() )
            {
                LogProcessEvent("State: BrewRaiseProbe\n");
                m_pCupLevelProbe->Raise();
                m_pBoiler->EnableExitValve(false);
                SetTimeout(BREW_TIMEOUT_PROBE, BREW_ERR_TIMEOUT_PROBE);
				if( m_pDrink->GetType() == Tea )
				{
					// For tea, go and mash
					SetNextState(BrewMashTeabag);
				}
				else
				{
					// For anything else, process is complete
					SetNextState(BrewComplete);
				}
            }
            break;
        
        // Brew step 5A: If probe is in raised position, lower the masher
        case BrewMashTeabag:
            {
                if(m_pCupLevelProbe->IsRaised())
                {
                    LogProcessEvent("State: BrewMashTeabag\n");
                    m_pMasher->Lower();
                    SetTimeout(BREW_TIMEOUT_MASHER, BREW_ERR_TIMEOUT_MASHER);
                    SetNextState(BrewRaiseMasher);
                }                
            }
            break;
        
        // Brew step 6: If masher is lowered, raise it
        case BrewRaiseMasher:
            if(m_pMasher->IsLowered() )
            {
                LogProcessEvent("State: BrewRaiseMasher\n");
                m_pMasher->Raise();
                m_lMashCount++;
				
				String strMashCycleText = "Mash cycle: ";
				strMashCycleText += m_lMashCount;
				strMashCycleText += "\n";
				LogProcessEvent( strMashCycleText );
				
                SetTimeout(BREW_TIMEOUT_MASHER, BREW_ERR_TIMEOUT_MASHER);
                if( m_lMashCount < m_lMashesRequired )
                {
                   // Keep mashing, using partial mashes
                    m_lTimeRaiseMustStop = millis() + BREW_PARTIALRAISEDELAY;
                    SetNextState(BrewLowerMasherAgain);
                }
                else
                {
                    // Raise the extractor once this mash cycle is over
                    SetNextState(BrewRaiseExtractor);
                }
            }
            break;
        
        // Brew step 6a: If masher is partially raised, lower it again
        case BrewLowerMasherAgain:
            if( millis() > m_lTimeRaiseMustStop )
            {
                LogProcessEvent("State: BrewLowerMasherAgain\n");
                m_pMasher->Lower();
                SetTimeout(BREW_TIMEOUT_MASHER, BREW_ERR_TIMEOUT_MASHER);
                SetNextState(BrewRaiseMasher);
            }
                
            break;
        
        // Brew step 7: If masher is raised, raise the extractor
        case BrewRaiseExtractor:
            if(m_pMasher->IsRaised() )
            {
                LogProcessEvent("State: BrewRaiseExtractor\n");
                m_pExtractor->Raise();
                SetTimeout(BREW_TIMEOUT_EXTRACTOR, BREW_ERR_TIMEOUT_EXTRACTOR);
                SetNextState(BrewComplete);
            }
            break;
        
        // Brew step 8: If teabag recovery is complete, the process has successfully completed
        case BrewComplete:
			if(m_pExtractor->IsRaised())
			{
				LogProcessEvent("State: BrewComplete\n");

				// Brew process is complete
				response = ProcessComplete;
				ClearTimeout();
            
				// The brewing stage of the drink is now complete
				m_pDrink->SetState( m_bDrinkAbandoned ? Invalid : BrewStageComplete );
			}
			break;
    } // switch            
            
	return response;
}

//
//	Test()
//
//  Perform a complete cycle test of the brewing process
//
bool BrewManager::Test()
{	
	// Prepare a default drink
	Drink theDrink;
	SetDrink( &theDrink );
	
    // Wait for action switch high
	while((m_pTeaMachine->GetSystem()->ReadInputRegisterData() & INREGMASK_ACTIONSWITCH) == 0);
    
	// Set the hardware to its initial state
	ProcessManagerResponse response = BeginProcess();
	
	while( response == ProcessIncomplete )
	{
    	response = Continue();
	} // while
    
	return (response == ProcessComplete);
}
