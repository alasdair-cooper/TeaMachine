
//
//	UIManager.cpp
//
//	Created: 12/05/2017
//	Author: Nick
//

#include "TeaMachineApp.h"
#include "TeaMachineController.h"
#include "UIManager.h"
#include "Display.h"
#include "CupDetector.h"
#include "UserControls.h"
#include "Drink.h"

//
//	Class constructor
//
UIManager::UIManager()
{
    ClearMembers();
} //UIManager

//
//	Class constructor
//
UIManager::UIManager(TeaMachineController *pController) : ProcessManagerBase( pController )
{
    ClearMembers();
} //UIManager

//
//	Class destructor
//
UIManager::~UIManager()
{
} //~UIManager

//
//	ClearMembers()
//
//  Initialise class members
//
void UIManager::ClearMembers()
{
	m_pDisplay = NULL;
	m_pCupDetector = NULL;
	m_pUserControls = NULL;
    m_pCurrentDrink = NULL;
    m_State = UIReset;
    m_lDebounceCheckTime = 0;
	m_bStationAdvanceInProgress = false;
	m_bResultsAwaitChecking = false;
	m_lNextFlashActionTime = millis();
}

//
//	SetNextState()
//
//  Set the next state in the process sequence
//
void UIManager::SetNextState( UIState nextState )
{
    m_State = nextState;
}

//
//	SetCurrentDrink()
//
//  Set the drink to be displayed and modified by the user
//
void UIManager::SetCurrentDrink( Drink *pDrink )
{
    m_pCurrentDrink = pDrink;
    
    if( (m_pDisplay != NULL))
	{
		if( pDrink != NULL )
		{
			if( pDrink->GetState() != Invalid )
			{
				// Update the display with the new drink details
				m_pDisplay->Set( IndicatorCupNumber, (int)pDrink->GetID() );
				m_pDisplay->Set( IndicatorMilkLevel, (int)pDrink->GetMilk() );
				m_pDisplay->Set( IndicatorSugarLevel, (int)pDrink->GetSugar() );
				m_pDisplay->Set( IndicatorStrengthLevel, (int)pDrink->GetStrength() );
        
				char szName[DISPLAY_DRINKNAMELENGTH+1];
				strcpy( szName, pDrink->GetName() );
				m_pDisplay->Set( IndicatorDrinkName1, szName[0] );
				m_pDisplay->Set( IndicatorDrinkName2, szName[1] );
				m_pDisplay->Set( IndicatorDrinkName3, szName[2] );
				m_pDisplay->Set( IndicatorDrinkName4, szName[3] );
        
				m_pDisplay->Set( IndicatorLED_GoButton, !pDrink->SelectionIsConfirmed() );
				m_pDisplay->Set( IndicatorLED_StopButton, pDrink->SelectionIsConfirmed() && (pDrink->GetState() == Empty) && (m_pController->GetDrinkLocation(pDrink->GetID()) <= ST_RECEPTION_A) );
			}
			else
			{
				ClearCurrentDrinkDisplay();
			}
		}
		else
		{
			ClearCurrentDrinkDisplay();
		}

		m_pDisplay->Update();
	}
}

//
//	ClearCurrentDrinkDisplay
//
//  Clear current drink display information
//
void UIManager::ClearCurrentDrinkDisplay()
{
	// Clear current cup details
	m_pDisplay->Set( IndicatorCupNumber, BLANK );
	m_pDisplay->Set( IndicatorMilkLevel, BLANK );
	m_pDisplay->Set( IndicatorSugarLevel, BLANK );
	m_pDisplay->Set( IndicatorStrengthLevel, BLANK );
	m_pDisplay->Set( IndicatorDrinkName1, ' ' );
	m_pDisplay->Set( IndicatorDrinkName2, ' ' );
	m_pDisplay->Set( IndicatorDrinkName3, ' ' );
	m_pDisplay->Set( IndicatorDrinkName4, ' ' );	
	m_pDisplay->Set( IndicatorLED_GoButton, false );
	m_pDisplay->Set( IndicatorLED_StopButton, false );
}

//
//	Initialise()
//
//  Initialise the UI manager
//
bool UIManager::Initialise()
{
    // Call the base class
    ProcessManagerBase::Initialise();

    // Initialise display
	m_pDisplay = new Display(m_pTeaMachine);
	m_pDisplay->Initialise();

    // Initialise cup detector
    m_pCupDetector = new CupDetector(m_pTeaMachine);
    m_pCupDetector->Initialise();
 
    // Initialise user controls
    m_pUserControls = new UserControls(m_pTeaMachine);
    m_pUserControls->Initialise();
    
    return true;
}

//
//  SetMessage()
//
//  Display a message to the user
//
void UIManager::SetMessage( char *pText, int nMilkIndicator, int nSugarIndicator, int nStrengthIndicator, bool bStatusOK, bool bStatusFail )
{
	int nLength = strlen( pText );
	m_pDisplay->Set( IndicatorDrinkName1, (nLength > 0) ? pText[0] : ' ' );
	m_pDisplay->Set( IndicatorDrinkName2, (nLength > 1) ? pText[1] : ' ' );
	m_pDisplay->Set( IndicatorDrinkName3, (nLength > 2) ? pText[2] : ' ' );
	m_pDisplay->Set( IndicatorDrinkName4, (nLength > 3) ? pText[3] : ' ' );
	m_pDisplay->Set( IndicatorMilkLevel, nMilkIndicator );
	m_pDisplay->Set( IndicatorSugarLevel, nSugarIndicator );
	m_pDisplay->Set( IndicatorStrengthLevel, nStrengthIndicator );
	m_pDisplay->Set( IndicatorCupNumber, BLANK );
	m_pDisplay->Set( IndicatorLED_StatusGreen, bStatusOK );
	m_pDisplay->Set( IndicatorLED_StatusRed, bStatusFail );
	m_pDisplay->Update();
}

//
//  BeginReset()
//
//  Initiate the reset sequence
//
ProcessManagerResponse UIManager::BeginReset()
{
	// Set initial state of reset sequence
	SetNextState( UIReset );
	
	SetResponse( ProcessIncomplete );
	
	return ProcessIncomplete;
}

//
//  BeginProcess()
//
//  Initiate the conveyor advance sequence
//
ProcessManagerResponse UIManager::BeginProcess()
{
	// Illuminate 'Status OK' LED
	m_pDisplay->Set( IndicatorLED_StatusGreen, true );
	m_pDisplay->Update();
	
	// Set initial state of conveyor advance sequence
	SetNextState( UIScanStations );
	
	SetResponse( ProcessIncomplete );
	
	return ProcessIncomplete;
}

//
//	Continue()
//
//  Initiate the next step of the process
//
ProcessManagerResponse UIManager::Continue()
{
    ProcessManagerResponse response = GetLastResponse();
    
	// Implement display flashing as required
	if( (millis() > m_lNextFlashActionTime) && (m_pDisplay != NULL) )
	{
		m_lNextFlashActionTime = m_pDisplay->DoFlash();
	}
	
	// Advance process state machine if necessary
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
void UIManager::Stop()
{
	// Nothing to do for UI
}

//
//	AdvanceStateMachine()
//
//  Initiate the next step of the process
//
ProcessManagerResponse UIManager::AdvanceStateMachine()
{
    ProcessManagerResponse response = ProcessIncomplete;

    switch( m_State )
    {
        // Reset user interface
        case UIReset:
            Serial.print("State: UIReset\n");
            ClearTimeout();
            m_pUserControls->ReadStates();
            SetNextState(UIResetComplete);
            break;
            
        // Obtain settled user control states
        case UIResetComplete:
            Serial.print("State: UIResetComplete\n");
            if( m_pUserControls->ReadSettledStates() )
            {
                m_pUserControls->RecordSettledStates();            
                response = ProcessComplete;
            }
            else
            {
                // Reread user controls until they settle
                SetNextState(UIReset);              
            }
            break;
                             
        // UIScanStations: Scan cup stations
        case UIScanStations:
			if( !m_bStationAdvanceInProgress )
			{
				// Scan cup stations
				m_pCupDetector->Update();
				m_bResultsAwaitChecking = true;
			}
            SetNextState(UICheckStationScanResults);
            break;
        
        // UICheckStationScanResults: Determine whether cups have been added or removed
        case UICheckStationScanResults:
			if( !m_bStationAdvanceInProgress && m_bResultsAwaitChecking )
			{
				// Check results of last scan
				for( int nStation = 1; nStation <=STATIONS; nStation++ )
				{
					if(m_pCupDetector->GetChange(nStation) == StationEventCupAdded)
					{
						// A cup has been placed on the conveyor
						HandleCupPlacement(nStation);
					}
					if(m_pCupDetector->GetChange(nStation) == StationEventCupRemoved)
					{
						// A cup has been removed from the conveyor
						HandleCupRemoval(nStation);
					}
				} // for
				m_bResultsAwaitChecking = false;
			}
        
            SetNextState(UIScanControls);
            break;
            
        // UICheckControls: Examine the user controls
        case UIScanControls:
            // Check state of controls
            m_pUserControls->ReadStates();
            m_lDebounceCheckTime = millis() + UC_DEBOUNCE_DELAY;
           
            SetNextState(UICheckControlScanResults);
            break;
            
        // UICheckControlScanResults: Act on any changes to the user controls
        case UICheckControlScanResults:
            if( millis() > m_lDebounceCheckTime )
            {
                 if( m_pUserControls->ReadSettledStates() )
                {
                    // Check state of controls
                    HandleUserControlEvents();
                
                    SetNextState(UIScanStations);
                }
                else
                {
                    // Keep reading states until they settle
                    SetNextState(UIScanControls);
                }
            }
            
            break;          
    } // switch            
            
	return response;
}

//
//  HandleUserControlEvents()
//
//  Confirm user control states and take appropriate action
//
void UIManager::HandleUserControlEvents()
{
    if( m_pCurrentDrink != NULL )
    {
        if( m_pUserControls->CheckSettledEvent( UC_CUP_INC ) == UserControlSwitchedOn )
        {
            // User has toggled Cup increment control
            SetCurrentDrink( m_pController->GetDrinkToTheRight(m_pCurrentDrink) );
            m_pDisplay->Set( IndicatorCupNumber, (int)m_pCurrentDrink->GetID() );
        }

        if( m_pUserControls->CheckSettledEvent( UC_CUP_DEC ) == UserControlSwitchedOn )
        {
            // User has toggled Cup decrement control
            SetCurrentDrink( m_pController->GetDrinkToTheLeft(m_pCurrentDrink) );
            m_pDisplay->Set( IndicatorCupNumber, (int)m_pCurrentDrink->GetID() );
        }

        if( !m_pCurrentDrink->SelectionIsConfirmed() )
        {
            // Can only make changes to a drink whose selection is not yet confirmed
            if( m_pUserControls->CheckSettledEvent( UC_DRINK_INC ) == UserControlSwitchedOn )
            {
                // User has toggled 'Next drink type' control
                m_pCurrentDrink->SetNextType();
                
                // Update display
                char szName[DISPLAY_DRINKNAMELENGTH+1];
                strcpy( szName, m_pCurrentDrink->GetName() );
                m_pDisplay->Set( IndicatorDrinkName1, szName[0] );
                m_pDisplay->Set( IndicatorDrinkName2, szName[1] );
                m_pDisplay->Set( IndicatorDrinkName3, szName[2] );
                m_pDisplay->Set( IndicatorDrinkName4, szName[3] );
            }

            if( m_pUserControls->CheckSettledEvent( UC_DRINK_DEC ) == UserControlSwitchedOn )
            {
                // User has toggled 'Previous drink type' control
                m_pCurrentDrink->SetPreviousType();
                
                // Update display
                char szName[DISPLAY_DRINKNAMELENGTH+1];
                strcpy( szName, m_pCurrentDrink->GetName() );
                m_pDisplay->Set( IndicatorDrinkName1, szName[0] );
                m_pDisplay->Set( IndicatorDrinkName2, szName[1] );
                m_pDisplay->Set( IndicatorDrinkName3, szName[2] );
                m_pDisplay->Set( IndicatorDrinkName4, szName[3] );
            }
    
            if( m_pUserControls->CheckSettledEvent( UC_MILK_INC ) == UserControlSwitchedOn )
            {
                // User has toggled 'Increment milk' control
                m_pCurrentDrink->IncrementMilk();
                m_pDisplay->Set( IndicatorMilkLevel, (int)m_pCurrentDrink->GetMilk() );
            }
    
            if( m_pUserControls->CheckSettledEvent( UC_MILK_DEC ) == UserControlSwitchedOn )
            {
                // User has toggled 'Decrement milk' control
                m_pCurrentDrink->DecrementMilk();
                m_pDisplay->Set( IndicatorMilkLevel, (int)m_pCurrentDrink->GetMilk() );
            }
    
            if( m_pUserControls->CheckSettledEvent( UC_SUGAR_INC ) == UserControlSwitchedOn )
            {
                // User has toggled 'Increment sugar' control
                m_pCurrentDrink->IncrementSugar();
                m_pDisplay->Set( IndicatorSugarLevel, (int)m_pCurrentDrink->GetSugar() );
            }
    
            if( m_pUserControls->CheckSettledEvent( UC_SUGAR_DEC ) == UserControlSwitchedOn )
            {
                // User has toggled 'Decrement sugar' control
                m_pCurrentDrink->DecrementSugar();
                m_pDisplay->Set( IndicatorSugarLevel, (int)m_pCurrentDrink->GetSugar() );
            }
    
            if( m_pUserControls->CheckSettledEvent( UC_STRENGTH_INC ) == UserControlSwitchedOn )
            {
                // User has toggled 'Increment strength' control
                m_pCurrentDrink->IncrementStrength();
                m_pDisplay->Set( IndicatorStrengthLevel, (int)m_pCurrentDrink->GetStrength() );
            }
    
            if( m_pUserControls->CheckSettledEvent( UC_STRENGTH_DEC ) == UserControlSwitchedOn )
            {
                // User has toggled 'Decrement strength' control
                m_pCurrentDrink->DecrementStrength();
                m_pDisplay->Set( IndicatorStrengthLevel, (int)m_pCurrentDrink->GetStrength() );
            }
    
            if( m_pUserControls->CheckSettledEvent( UC_GO ) == UserControlSwitchedOn )
            {
                // User has pressed GO button
                m_pCurrentDrink->SetSelectionConfirmed( true );
                m_pDisplay->Set( IndicatorLED_GoButton, false );
                m_pDisplay->Set( IndicatorLED_StopButton, true );
				
				// Stop the station indicator flashing for this drink 
				int nStation = m_pController->GetDrinkLocation( m_pCurrentDrink->GetID() );
                m_pDisplay->Set( nStation, m_pCurrentDrink->GetID(), false );
            }
        }            
    
        if( m_pUserControls->CheckSettledEvent( UC_CANCEL ) == UserControlSwitchedOn )
        {
            // User has pressed CANCEL button
            int nStation = m_pController->GetDrinkLocation( m_pCurrentDrink->GetID() );
            if( (nStation==ST_RECEPTION_A) || (nStation==ST_RECEPTION_B) || (nStation==ST_RECEPTION_C) )
            {
                // Cancellation only allowed at reception stations
                m_pCurrentDrink->SetSelectionConfirmed( false );
                m_pDisplay->Set( IndicatorLED_GoButton, true );
                m_pDisplay->Set( IndicatorLED_StopButton, false );
				m_pDisplay->Set( nStation, m_pCurrentDrink->GetID(), true );
            }
        }
        
        // Render changes to display hardware
        m_pDisplay->Update();
    }        

    // Now record current settled state of controls
    m_pUserControls->RecordSettledStates();
}

//
//	HandleCupPlacement()
//
//  A new cup has been detected
//
void UIManager::HandleCupPlacement( int nStation )
{
   Drink *pNewDrink = m_pController->NotifyNewDrinkRequest( nStation );

   if( nStation <= ST_BREW )
   {
       // Cup has been placed in a correct station for a new drink
       pNewDrink->SetState( Empty );
	   
	   // Show new cup ID and make it flash
       m_pDisplay->Set( nStation, pNewDrink->GetID(), true );
       SetCurrentDrink( pNewDrink );
   }
   else
   {
       // Cup has been placed on an incorrect station, so it will be ignored
       pNewDrink->SetState( Invalid );
       m_pDisplay->Set( nStation, UNEXPECTED );	  
   }
   m_pDisplay->Update();
}

//
//	HandleCupRemoval()
//
//  A cup has been removed
//
void UIManager::HandleCupRemoval( int nStation )
{
    m_pController->NotifyCupRemoval( nStation );
    m_pDisplay->Set( nStation, BLANK );

	SetCurrentDrink( m_pController->GetNextDrink() );
}

//
//  NotifyStationAdvanceBegins()
//
//  Called when the station positions begin advancing by one (as a result
//  of the conveyor moving by one position)
//
void UIManager::NotifyStationAdvanceBegins()
{
	m_bStationAdvanceInProgress = true;
}

//
//  NotifyStationAdvanceComplete()
//
//  Called when the station positions have advanced by one (as a result
//  of the conveyor moving by one position)
//
void UIManager::NotifyStationAdvanceComplete()
{
	if( m_pCupDetector != NULL )
	{
		m_pCupDetector->NotifyStationAdvance();
	}
	
	// Update the station indicators
	for( int nStation = ST_FARRIGHT; nStation <= ST_FARLEFT; nStation++ )
	{
		Drink *pDrink = m_pController->GetDrinkByStation( nStation );
		if( pDrink != NULL )
		{
			if( pDrink->GetState() != Invalid )
			{
				// Valid drink is present
				m_pDisplay->Set( nStation, pDrink->GetID(), !pDrink->SelectionIsConfirmed() || (pDrink->GetState()==ReadyToDrink));
			}
			else
			{
				// Invalid cup present
				m_pDisplay->Set( nStation, UNEXPECTED );
				
			}
		}
		else
		{
			// No cup present
			m_pDisplay->Set( nStation, BLANK );				
		}
		m_pDisplay->Update();
	}
	
	if(m_pCurrentDrink != NULL )
	{
		// If current drink has advanced beyond reception stations, cancellation is not possible, so extinguish 'stop' LED
		m_pDisplay->Set( IndicatorLED_StopButton, (m_pController->GetDrinkLocation(m_pCurrentDrink->GetID()) <= ST_RECEPTION_A) && m_pCurrentDrink->SelectionIsConfirmed());
	}
	
	// Station advance has completed, so clear flag
	m_bStationAdvanceInProgress = false;
}