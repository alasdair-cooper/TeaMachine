//
//	TeaMachineController.cpp
//
//	Created: 15/12/2017
//	Author: Nick
//

#include "TeaMachineApp.h"
#include "TeaMachineController.h"
#include "UIManager.h"
#include "BrewManager.h"
#include "MilkManager.h"
#include "StirManager.h"
#include "ConveyorManager.h"
#include "Drink.h"

//
//  Class constructor
//
TeaMachineController::TeaMachineController()
{
    ClearMembers();
    m_pTeaMachine = NULL;
} //TeaMachineController

//
//  Class constructor
//
TeaMachineController::TeaMachineController( TeaMachine *pTeaMachine )
{
    ClearMembers();
    m_pTeaMachine = pTeaMachine;
} //TeaMachineController

//
//  Class destructor
//
TeaMachineController::~TeaMachineController()
{
} //~TeaMachineController

//
//  ClearMembers
//
void TeaMachineController::ClearMembers()
{
    m_pUIManager = NULL;
    m_pBrewManager = NULL;
    m_pMilkManager = NULL;
    m_pStirManager = NULL;
    m_pConveyorManager = NULL;
	m_pCollisionZoneOwner = NULL;
    
    // Clear station array and drink number array
    for( int index=0; index < STATIONS+1; index ++)
    {
        m_drinks[index] = NULL;
        m_bDrinkIDAllocated[index] = false;
    }
}

//
//  GetTeaMachine()
//
//  Returns pointer to the one and only TeaMachine object
//
TeaMachine *TeaMachineController::GetTeaMachine()
{
    return m_pTeaMachine;
}

//
//  Start()
//
//  Start the tea machine
//
bool TeaMachineController::Start( TeaMachineControllerMode mode )
{
    bool bResult = false;
 
    // Initialise the tea machine
    Initialise();

    switch( mode )
    {
        // Start tea machine for normal 'in service' operation (no logging)
        case ControllerMode_InService:
			Serial.end();
            Service();
			Serial.begin(SERIAL_BAUDRATE);
            break;
			
        // Start tea machine for normal operation with logging switched on
        case ControllerMode_InServiceWithLogging:
			m_pUIManager->SetMessage( TXT_TEST, 2, ControllerMode_InServiceWithLogging, BLANK );
		    Service();
		    break;
           
        // Test the user interface operation
        case ControllerMode_TestUserInterface:
            TestUserInterface();
            break;
           
        // Test the brew process 
        case ControllerMode_TestBrewManager:          
			m_pUIManager->SetMessage( TXT_TEST, 2, ControllerMode_TestBrewManager, BLANK );
            bResult = m_pBrewManager->Test();
            break;
            
        // Test the milk process
        case ControllerMode_TestMilkManager:
			m_pUIManager->SetMessage( TXT_TEST, 2, ControllerMode_TestMilkManager, BLANK );
            bResult = m_pMilkManager->Test();
            break;
            
        // Test the stir process
        case ControllerMode_TestStirManager:
			m_pUIManager->SetMessage( TXT_TEST, 2, ControllerMode_TestStirManager, BLANK );
            bResult = m_pStirManager->Test();
            break;
          
        // Test the conveyor advance
	    case ControllerMode_TestConveyorManager:
		   break;
         
        // Set load positions
		case ControllerMode_Load:
			bResult = m_pMilkManager->SetLoadPosition();
			m_pUIManager->SetMessage( TXT_LOAD, BLANK, BLANK, BLANK );
		   break;
    } //switch
   
    return bResult;
}

//
//  Initialise()
//
//  Initialise class members and manager objects
//
bool TeaMachineController::Initialise()
{
    // Create and initialise UI manager
    m_pUIManager = new UIManager(this);
    m_pUIManager->Initialise();

    // Create and initialise brew manager
    m_pBrewManager = new BrewManager(this);
    m_pBrewManager->Initialise();

    // Create and initialise Milk manager
    m_pMilkManager = new MilkManager(this);
    m_pMilkManager->Initialise();
  
    // Create and initialise Stir manager
    m_pStirManager = new StirManager(this);
    m_pStirManager->Initialise();

    // Create and initialise conveyor manager
    m_pConveyorManager = new ConveyorManager(this);
    m_pConveyorManager->Initialise();

    return true; 
}

//
//  Service()
//
//  Main service loop
//
void TeaMachineController::Service()
{
    if( ResetHardware() )
    {
        m_pUIManager->BeginProcess();
        
        // Do this loop forever
        while(!m_pTeaMachine->GetSystem()->SystemModeHasChanged())
        {
            // Yield to UI manager
            m_pUIManager->Continue();
            
            switch( m_pConveyorManager->GetLastResponse() )
            {
                case ProcessComplete:
                // Conveyor is not advancing
   //             if( ActiveDrinkIsPresent() )
                if( true )
                {
                    // At least one cup is present on preparation/reception stations and is active (i.e. user has chosen drink)
                    if( (m_drinks[ST_STIR] != NULL) && (m_pStirManager->GetLastResponse() != ProcessIncomplete)  )
                    {
                        // Check drink standing at stir station
                        if( m_drinks[ST_STIR]->CupIsPresent() && m_drinks[ST_STIR]->SelectionIsConfirmed() && (m_drinks[ST_STIR]->GetState() == MilkStageComplete ) )
                        {
Serial.println("Starting stir process");
                            // Stir station cup is ready to begin stir process
                            m_pStirManager->SetDrink(m_drinks[ST_STIR]);
                            if( m_pStirManager->BeginProcess() == ProcessFailed )
                            {
	                            // Handle process failure
	                            HandleError( ErrorContext_Service, m_pStirManager->GetLastError() );
                            }
                        }
                    }
                    
                    if( (m_drinks[ST_MILK] != NULL) && (m_pMilkManager->GetLastResponse() != ProcessIncomplete)  )
                    {
                        // Check drink standing at milk station
                        if( m_drinks[ST_MILK]->CupIsPresent() && m_drinks[ST_MILK]->SelectionIsConfirmed() && (m_drinks[ST_MILK]->GetState() == BrewStageComplete ) )
                        {
 Serial.println("Starting milk process");
                           // Milk station cup is ready to begin milk process
                            m_pMilkManager->SetDrink(m_drinks[ST_MILK]);
                            if( m_pMilkManager->BeginProcess() == ProcessFailed )
							{
								// Handle process failure
								HandleError( ErrorContext_Service, m_pMilkManager->GetLastError() );
							}
                        }
                    }
                    
                    if( ( m_drinks[ST_BREW] != NULL) && (m_pBrewManager->GetLastResponse() != ProcessIncomplete) )
                    {
                        // Check drink standing at brew station
                        if( m_drinks[ST_BREW]->CupIsPresent() && m_drinks[ST_BREW]->SelectionIsConfirmed() && (m_drinks[ST_BREW]->GetState() == Empty) )
                        {
 Serial.println("Starting brew process");
                           // Brew station cup is ready to begin brew process
                            m_pBrewManager->SetDrink(m_drinks[ST_BREW]);
                            if( m_pBrewManager->BeginProcess() == ProcessFailed )
                            {
	                            // Handle process failure
	                            HandleError( ErrorContext_Service, m_pBrewManager->GetLastError() );
                            }
                        }
                    }
                    
                    // Yield to brew process manager
                    if( m_pBrewManager->Continue() == ProcessFailed )
                    {
	                    // Handle process failure
	                    HandleError( ErrorContext_Service, m_pBrewManager->GetLastError() );
                    }
                    
                    // Yield to milk process manager
                    if( m_pMilkManager->Continue() == ProcessFailed )
                    {
	                    // Handle process failure
	                    HandleError( ErrorContext_Service, m_pMilkManager->GetLastError() );
                    }
                    
                    // Yield to stir process manager
                    if( m_pStirManager->Continue() == ProcessFailed )
                    {
	                    // Handle process failure
	                    HandleError( ErrorContext_Service, m_pStirManager->GetLastError() );
                    }
                    
                    if( PreparationStationsAreInactive() )
                    {
                        // Preparation stations are inactive so the conveyor can advance if required
						// The conveyor should advance if any of the following are true:
						//   (a) Any empty cups are at the reception stations without unconfirmed cups to their left
						//	 (b) A brew-complete cup is at the brew station
						//   (c) A milk-complete cup is at the milk station
						//   (d) A ready-to-drink cup is at the stir station
						// Advance cannot take place if:
						//   (a) A cup is present on the far left station
						//   (b) An unconfirmed cup is present at the brew station 
						if((	ReceptionCupsReadyToAdvance()
							||  DrinkStateAtStation(ST_BREW, BrewStageComplete)
							||  DrinkStateAtStation(ST_MILK, MilkStageComplete)
							||  DrinkStateAtStation(ST_STIR, ReadyToDrink) ) && !((GetDrinkByStation(ST_FARLEFT) != NULL) || (DrinkUnconfirmedAtStation(ST_BREW))))
						{
Serial.println("Conveyor advance");
							// Tell UI Manager that conveyor is advancing
							m_pUIManager->NotifyStationAdvanceBegins();
							
							// Initiate the advance
							m_pConveyorManager->BeginProcess();
						}
                    }
					
					// Yield to the conveyor manager
					m_pConveyorManager->Continue();
                }
                break;
                
                case ProcessIncomplete:
                // Conveyor is in the process of advancing, so do nothing else until it completes
                m_pConveyorManager->Continue();
                break;
                
                case ProcessFailed:
                // Conveyor failure
                HandleError( ErrorContext_Service, m_pConveyorManager->GetLastError() );
                break;
            } // switch
        } // while
    }
}

//
//	ReceptionCupsReadyToAdvance
//
//  Returns true if an empty confirmed cup is present in one of the reception stations without having
//  unconfirmed cups to its left (up to and including the brew station)
//
bool TeaMachineController::ReceptionCupsReadyToAdvance()
{
	bool bReadyToAdvance = false;
	
	bReadyToAdvance |= DrinkStateAtStation( ST_RECEPTION_A, Empty ) && !DrinkUnconfirmedAtStation( ST_BREW );
	bReadyToAdvance |= DrinkStateAtStation( ST_RECEPTION_B, Empty ) && !(DrinkUnconfirmedAtStation( ST_BREW ) || DrinkUnconfirmedAtStation( ST_RECEPTION_A )) ;
	bReadyToAdvance |= DrinkStateAtStation( ST_RECEPTION_C, Empty ) && !(DrinkUnconfirmedAtStation( ST_BREW ) || DrinkUnconfirmedAtStation( ST_RECEPTION_A ) || DrinkUnconfirmedAtStation( ST_RECEPTION_B )) ;

	return bReadyToAdvance;
}

//
//	DrinkStateAtStation
//
//  Returns true if a drink is present at a given station having a specified state
//
bool TeaMachineController::DrinkStateAtStation( int nStation, DrinkState state )
{
	Drink *pTheDrink = m_drinks[nStation];
	
	bool bStateCorresponds = false;
	if( pTheDrink != NULL )
	{
		bStateCorresponds = ( pTheDrink->GetState() == state ) && pTheDrink->SelectionIsConfirmed();
	}
	
	return bStateCorresponds;
}

//
//	DrinkUnconfirmedAtStation
//
//  Returns true if a drink is present at a given station and it is unconfirmed
//
bool TeaMachineController::DrinkUnconfirmedAtStation( int nStation )
{
	Drink *pTheDrink = m_drinks[nStation];
	
	bool bPresentAndUnconfirmed = false;
	if( pTheDrink != NULL )
	{
		bPresentAndUnconfirmed = !pTheDrink->SelectionIsConfirmed();
	}
	
	return bPresentAndUnconfirmed;
}

//
//  TestUserInterface()
//
//  Operate UI as if tea machine was in service but without brew functions etc
//
bool TeaMachineController::TestUserInterface()
{
    bool bResult = true;
    ProcessManagerResponse UIManagerResponse = m_pUIManager->BeginReset();
    while( UIManagerResponse == ProcessIncomplete )
    {
        if( UIManagerResponse == ProcessIncomplete )
        {
            UIManagerResponse = m_pUIManager->Continue();
        }
    } // while

	// Initialise conveyor as this may be required in the test
    ProcessManagerResponse conveyorManagerResponse = m_pConveyorManager->BeginReset();
    while( conveyorManagerResponse == ProcessIncomplete )
    {
	    if( conveyorManagerResponse == ProcessIncomplete )
	    {
		    conveyorManagerResponse = m_pConveyorManager->Continue();
	    }
    } // while
	unsigned long nNextAdvance = millis() + CONVEYOR_TEST_PERIOD;
	
	// Begin the processing cycle
    UIManagerResponse = m_pUIManager->BeginProcess();
    if( UIManagerResponse != ProcessFailed )
    {
        // Do this loop forever
        while(true)
        {
            // Yield to UI manager
            m_pUIManager->Continue();
 
			if((m_pTeaMachine->GetSystem()->ReadInputRegisterData() & INREGMASK_ACTIONSWITCH) != 0)
			{
				// Action switch is up, so, every few seconds, advance the conveyor
				if( (millis() > nNextAdvance) && (conveyorManagerResponse == ProcessComplete))
				{
					// Tell UI Manager that conveyor is advancing
					m_pUIManager->NotifyStationAdvanceBegins();
					
					// Initiate advance
					conveyorManagerResponse = m_pConveyorManager->BeginProcess();
					
					// Remember to do another advance
					nNextAdvance = millis() + CONVEYOR_TEST_PERIOD;
				}
			}
			
			// Yield to conveyor manager
			conveyorManagerResponse = m_pConveyorManager->Continue();
        } // while
    }
    else
    {
        HandleError( ErrorContext_Reset, m_pUIManager->GetLastError() );
        bResult = false;
    }
    
    return bResult;
}

//
//  PreparationStationsAreInactive
//
//  Determine whether the preparation stations are inactive
//  i.e. Processes are not incomplete or cups are not present
//
bool TeaMachineController::PreparationStationsAreInactive()
{
    bool bInactive = false;

    return (m_pBrewManager->GetLastResponse() != ProcessIncomplete) && (m_pMilkManager->GetLastResponse() != ProcessIncomplete) && (m_pStirManager->GetLastResponse() != ProcessIncomplete);
}

//
//  ResetHardware()
//
//  Restore hardware to a known state
//
bool TeaMachineController::ResetHardware()
{     
    bool bResult = true;
    
	Serial.print( "Beginning reset sequence\n" );
	
    // Initiate reset sequences
    ProcessManagerResponse brewManagerResponse = m_pBrewManager->BeginReset();
    ProcessManagerResponse milkManagerResponse = m_pMilkManager->BeginReset();
    ProcessManagerResponse stirManagerResponse = m_pStirManager->BeginReset();
    ProcessManagerResponse conveyorManagerResponse = m_pConveyorManager->BeginReset();
    ProcessManagerResponse UIManagerResponse = m_pUIManager->BeginReset();
    
    // Ensure reset sequences complete
    while(     (brewManagerResponse == ProcessIncomplete) 
            || (milkManagerResponse == ProcessIncomplete)
            || (stirManagerResponse == ProcessIncomplete)
            || (conveyorManagerResponse == ProcessIncomplete)
            || (UIManagerResponse == ProcessIncomplete))
    {
        if( brewManagerResponse == ProcessIncomplete )
        {
            brewManagerResponse = m_pBrewManager->Continue();
        }
        if( milkManagerResponse == ProcessIncomplete )
        {
            milkManagerResponse = m_pMilkManager->Continue();
        }
        if( stirManagerResponse == ProcessIncomplete )
        {
            stirManagerResponse = m_pStirManager->Continue();
        }
        if( conveyorManagerResponse == ProcessIncomplete )
        {
            conveyorManagerResponse = m_pConveyorManager->Continue();
        }
        if( UIManagerResponse == ProcessIncomplete )
        {
            UIManagerResponse = m_pUIManager->Continue();
        }
    } // while
 
    // Handle any errors resulting from the reset sequence
    unsigned long lErrorCode = PROC_OK;
    if( brewManagerResponse == ProcessFailed )
    {
        HandleError( ErrorContext_Reset, m_pBrewManager->GetLastError() );
        bResult = false;
    }
    if( milkManagerResponse == ProcessFailed )
    {
        HandleError( ErrorContext_Reset, m_pMilkManager->GetLastError() );
        bResult = false;
    }
    if( stirManagerResponse == ProcessFailed )
    {
        HandleError( ErrorContext_Reset, m_pStirManager->GetLastError() );
        bResult = false;
    }
    if( conveyorManagerResponse == ProcessFailed )
    {
        HandleError( ErrorContext_Reset, m_pConveyorManager->GetLastError() );
        bResult = false;
    }
    if( UIManagerResponse == ProcessFailed )
    {
        HandleError( ErrorContext_Reset, m_pUIManager->GetLastError() );
        bResult = false;
    }
   
    return bResult;
}

//
//  NotifyNewDrinkRequest()
//
//  Notify the controller that a new drink has been requested
//
Drink *TeaMachineController::NotifyNewDrinkRequest( unsigned int nStation )
{
    // If a drink object already exists (it shouldn't) destroy it
    if( m_drinks[nStation] != NULL )
    {
        delete m_drinks[nStation];
    }
    
    // Create new drink
    Drink *pNewDrink = new Drink( AllocateDrinkID() );

    // Record the new drink in the drink array
    m_drinks[nStation] = pNewDrink;

    return pNewDrink;
}

//
//  NotifyCupRemoval()
//
//  Inform the controller that a cup has been removed
//
void TeaMachineController::NotifyCupRemoval( unsigned int nStation )
{
    Drink *pDrink = m_drinks[nStation];
    
    if( pDrink != NULL )
    {
        // There is a cup at this station
        if( pDrink->GetID() == pDrink->GetID() )
        {
            // Cancel the ID and destroy the drink
            m_bDrinkIDAllocated[pDrink->GetID()] = false;
            delete pDrink;
			m_drinks[nStation] = NULL;
			
			// Reset manager objects as necessary (these will reset the hardware in preparation for the next drink)
			switch( nStation )
			{
				case ST_BREW:
					m_pBrewManager->BeginReset();
					break;
					
				case ST_MILK:
					m_pMilkManager->BeginReset();
					break;
				
				case ST_STIR:
					m_pStirManager->BeginReset();
					break;
			} //switch			
        }                
    }                 
}

//
//  AllocateDrinkID()
//
//  Find an unused drink ID and mark it as used
//
unsigned int TeaMachineController::AllocateDrinkID()
{
    unsigned int nThisDrinkID = 0;
    
	do
    {
	    nThisDrinkID++;
    }
    while( (nThisDrinkID <= STATIONS) && m_bDrinkIDAllocated[nThisDrinkID] );
    
    // Mark number as 'in use'
    m_bDrinkIDAllocated[nThisDrinkID] = true;
    
    return nThisDrinkID;
}

//
//  GetDrinkToTheRight()
//
//  Find the drink to the right of the one specified.
//  If given drink is on the far right hand station, return the original drink.
//
Drink *TeaMachineController::GetDrinkToTheRight( Drink *pDrink )
{
    Drink *pDrinkToRight = pDrink;
    Drink *pPreviousDrink = pDrink;
    
	if(pDrink->GetState() != Invalid)
	{
		for( int nStation = ST_FARRIGHT; nStation < ST_FARLEFT; nStation++ )
		{
			if( m_drinks[nStation] != NULL )
			{
				// Found a drink on the conveyor
				if( (m_drinks[nStation]->GetID() == pDrink->GetID()) && (m_drinks[nStation]->GetState() != Invalid) )
				{
					// This drink is the one specified, so remember the drink to its right
					pDrinkToRight = pPreviousDrink;
				}
				pPreviousDrink = m_drinks[nStation];
			}
		} // for
	}
	else
	{
		pDrinkToRight = GetNextDrink();
	}
    
    return pDrinkToRight;
}

//
//  GetDrinkToTheLeft()
//
//  Find the drink to the left of the one specified.
//  If given drink is on the far left hand station, return the original drink.
//
Drink *TeaMachineController::GetDrinkToTheLeft( Drink *pDrink )
{
    Drink *pDrinkToLeft = pDrink;
    Drink *pPreviousDrink = pDrink;
	
	if(pDrink->GetState() != Invalid)
	{	
		for( int nStation = ST_FARLEFT; nStation >= ST_FARRIGHT; nStation-- )
		{
			if( m_drinks[nStation] != NULL )
			{
				// Found a drink on the conveyor
				if( (m_drinks[nStation]->GetID() == pDrink->GetID()) && (m_drinks[nStation]->GetState() != Invalid) )
				{
					// This drink is the one specified, so remember the drink to its left 
					pDrinkToLeft = pPreviousDrink;
				}
				pPreviousDrink = m_drinks[nStation];
			}        
		} // for
	}
	else
	{
		pDrinkToLeft = GetNextDrink();
	}
  
    return pDrinkToLeft;
}
	
//
//  GetDrinkByStation()
//
//  Returns the drink at a given station
//
Drink *TeaMachineController::GetDrinkByStation( int nStation )
{
	return m_drinks[nStation];
}

//
//  GetNextDrink()
//
//  Returns the first valid drink whose selection is not confirmed, or if no unconfirmed drinks
//	are present, any other drink is returned. If no drinks are present then NULL is returned.
//
Drink *TeaMachineController::GetNextDrink()
{
	Drink *pUnconfirmedDrink = NULL;
	Drink *pConfirmedDrink = NULL;
	
	for( int nStation = ST_FARRIGHT; nStation <= ST_FARLEFT; nStation++ )
	{
		if( m_drinks[nStation] != NULL )
		{
			if( m_drinks[nStation]->GetState() != Invalid )
			{
				// A valid drink is present at this station
				if( m_drinks[nStation]->SelectionIsConfirmed() )
				{
					pConfirmedDrink = m_drinks[nStation];
				}
				else
				{
					pUnconfirmedDrink = m_drinks[nStation];
				}
			}			
		}	
	} // for
	
	return (pUnconfirmedDrink == NULL) ? pConfirmedDrink : pUnconfirmedDrink; 
}

//
//  GetDrinkLocation()
//
//  Determine the location of a drink with a given ID
//
int TeaMachineController::GetDrinkLocation( int nDrinkID )
{
    int nLocation = ST_UNKNOWN;
    
    for( int nStation = ST_FARLEFT; nStation >= ST_FARRIGHT; nStation-- )
    {
        if( m_drinks[nStation] != NULL )
        {
            // A drink is present at this station
            if( m_drinks[nStation]->GetID() == nDrinkID )
            {
                // Found the given drink
                nLocation = nStation;
            }                
        }
    } // for
    
    return nLocation;
} 

//
//  ReleaseDrinkID()
//
//  Release a given drink ID so that it may be reused
//
void TeaMachineController::ReleaseDrinkID( unsigned int nDrinkID )
{
    m_bDrinkIDAllocated[nDrinkID] = false;
}

//
//  ActiveDrinkIsPresent()
//
//  Determine whether or not a cup is present on the preparation/reception stations and that the user has chosen the drink
//
bool TeaMachineController::ActiveDrinkIsPresent()
{
    bool bActiveDrinkIsPresent = false;
    
    // Check stir station
    if( m_drinks[ST_STIR] != NULL )
    {
	    if( m_drinks[ST_STIR]->CupIsPresent() && m_drinks[ST_STIR]->SelectionIsConfirmed() )
	    {
		    bActiveDrinkIsPresent = true;
	    }
    }
    
    // Check milk station
    if( m_drinks[ST_MILK] != NULL )
    {
	    if( m_drinks[ST_MILK]->CupIsPresent() && m_drinks[ST_MILK]->SelectionIsConfirmed() )
	    {
		    bActiveDrinkIsPresent = true;
	    }
    }
    
    // Check brew station
    if( m_drinks[ST_BREW] != NULL )
    {
	    if( m_drinks[ST_BREW]->CupIsPresent() && m_drinks[ST_BREW]->SelectionIsConfirmed() )
	    {
		    bActiveDrinkIsPresent = true;
	    }
    }
    
    // Check first reception station
    if( m_drinks[ST_RECEPTION_A] != NULL )
    {
        if( m_drinks[ST_RECEPTION_A]->CupIsPresent() && m_drinks[ST_RECEPTION_A]->SelectionIsConfirmed() )
        {
            bActiveDrinkIsPresent = true;
        }
    }
    
    // Check second reception station
    if( m_drinks[ST_RECEPTION_B] != NULL )
    {
        if( m_drinks[ST_RECEPTION_B]->CupIsPresent() && m_drinks[ST_RECEPTION_B]->SelectionIsConfirmed() )
        {
            bActiveDrinkIsPresent = true;
        }
    }
    
    // Check third reception station
    if( m_drinks[ST_RECEPTION_C] != NULL )
    {
        if( m_drinks[ST_RECEPTION_C]->CupIsPresent() && m_drinks[ST_RECEPTION_C]->SelectionIsConfirmed() )
        {
            bActiveDrinkIsPresent = true;
        }
    }
    
    return bActiveDrinkIsPresent;
}

//
//  NotifyConveyorAdvance()
//
//  Inform controller that conveyor advance has completed
//
void TeaMachineController::NotifyConveyorAdvanceComplete()
{
    if( m_drinks[ST_COLLECTION_A] != NULL )
    {
        // There should not be a drink at Collection station A, but if there is, delete it
        delete m_drinks[ST_COLLECTION_A];
    }
    
    // Move the drinks along by one position
    m_drinks[ST_COLLECTION_A] = m_drinks[ST_COLLECTION_B];
    m_drinks[ST_COLLECTION_B] = m_drinks[ST_COLLECTION_C];
    m_drinks[ST_COLLECTION_C] = m_drinks[ST_STIR];
    m_drinks[ST_STIR] = m_drinks[ST_MILK];
    m_drinks[ST_MILK] = m_drinks[ST_BREW];
    m_drinks[ST_BREW] = m_drinks[ST_RECEPTION_A];
    m_drinks[ST_RECEPTION_A] = m_drinks[ST_RECEPTION_B];
    m_drinks[ST_RECEPTION_B] = m_drinks[ST_RECEPTION_C];
    m_drinks[ST_RECEPTION_C] = NULL;
    
    // Notify UI Manager that station positions have all advanced by one
    m_pUIManager->NotifyStationAdvanceComplete();
}

//
//  HandleError
//
//  Handle a failure condition by displaying error code and context
//
void TeaMachineController::HandleError( TeaMachineErrorContext context, unsigned long lErrorCode )
{
	// Stop all devices
	m_pBrewManager->Stop();
	m_pMilkManager->Stop();
	m_pStirManager->Stop();
	m_pConveyorManager->Stop();
	
	// Illuminate FAULT LED and display failure information
	m_pTeaMachine->GetSystem()->ClearOutputRegisterBit(OUTREG_LED_FAULT);
	m_pUIManager->SetMessage( TXT_FAIL, (int)context, (int)lErrorCode >> 4, (int)lErrorCode & 0x0F, false, true );
	
	// Wait forever (forces operator to reset system)
	while(true);
}

//
//  LockCollisionZone
//
//	Acquire a lock on the zone in which the milk dispenser and the stirrer collide
//
bool TeaMachineController::LockCollisionZone( ProcessManagerBase *pRequestor )
{
	if( m_pCollisionZoneOwner == NULL )
	{
		// Caller has acquired the collision zone
		m_pCollisionZoneOwner = pRequestor;
	}
	
	return (m_pCollisionZoneOwner == pRequestor);
}

//
//  ReleaseCollisionZone
//
//	Release previously acquired lock on the collision zone
//
void TeaMachineController::ReleaseCollisionZone( ProcessManagerBase *pOwner )
{
	if( m_pCollisionZoneOwner == pOwner )
	{
		// Caller is current owner so release lock
		m_pCollisionZoneOwner = NULL;
	}
}