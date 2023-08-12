//
//	UserControls.cpp
//
//	Created: 18/12/2017
//	Author: Nick
//
#include "TeaMachineApp.h"
#include "UserControls.h"

//
//	Class constructor
//
UserControls::UserControls()
{
    ClearMembers();
} //UserControls

//
//	Class constructor
//
UserControls::UserControls(TeaMachine *pTeaMachine) : TeaMachineComponent( pTeaMachine )
{
    ClearMembers();
} //UserControls

//
//	Default destructor
//
UserControls::~UserControls()
{
} //~UserControls

//
//  Initialise()
//
//  Initialise the user controls class
//
bool UserControls::Initialise()
{
    ClearMembers();
}

//
//  ClearMembers()
//
//  Initialise member variables
//
void UserControls::ClearMembers()
{
   for( int nControl = 0; nControl < UC_MAXCONTROLS; nControl++ )
   {
       m_bControlStates[nControl] = false;
       m_bSettledControlStates[nControl]= false;
       m_bPreviousSettledControlStates[nControl]= false;
   } // for
}    

//
//  Test()
//
//  Test the user controls
//
bool UserControls::Test()
{
    // Read/debounce initial states
    ReadStates();
    delay(UC_DEBOUNCE_DELAY);
    ReadSettledStates();
    RecordSettledStates();
    
    while( true )
    {
        delay(20);
        ReadStates();
        delay(UC_DEBOUNCE_DELAY);
        ReadSettledStates();
        
        if( CheckSettledEvent( UC_CUP_INC ) == UserControlSwitchedOn )
        {
            // User has toggled Cup increment control
            Serial.println( "Cup UP on" );
        }

        if( CheckSettledEvent( UC_CUP_DEC ) == UserControlSwitchedOn )
        {
            // User has toggled Cup decrement control
            Serial.println( "Cup DOWN on" );
        }

        if( CheckSettledEvent( UC_DRINK_INC ) == UserControlSwitchedOn )
        {
            // User has toggled 'Next drink type' control
            Serial.println( "Drink UP on" );
        }

        if( CheckSettledEvent( UC_DRINK_DEC ) == UserControlSwitchedOn )
        {
            // User has toggled 'Previous drink type' control
            Serial.println( "Drink DOWN on" );
        }
        
        if( CheckSettledEvent( UC_MILK_INC ) == UserControlSwitchedOn )
        {
            // User has toggled 'Increment milk' control
            Serial.println( "Milk UP on" );
        }
        
        if( CheckSettledEvent( UC_MILK_DEC ) == UserControlSwitchedOn )
        {
            // User has toggled 'Decrement milk' control
            Serial.println( "Milk DOWN on" );
        }
        
        if( CheckSettledEvent( UC_SUGAR_INC ) == UserControlSwitchedOn )
        {
            // User has toggled 'Increment sugar' control
            Serial.println( "Sugar UP on" );
        }
        
        if( CheckSettledEvent( UC_SUGAR_DEC ) == UserControlSwitchedOn )
        {
            // User has toggled 'Decrement sugar' control
            Serial.println( "Sugar DOWN on" );
        }
        
        if( CheckSettledEvent( UC_STRENGTH_INC ) == UserControlSwitchedOn )
        {
            // User has toggled 'Increment strength' control
            Serial.println( "Strength UP on" );
        }
        
        if( CheckSettledEvent( UC_STRENGTH_DEC ) == UserControlSwitchedOn )
        {
            // User has toggled 'Decrement strength' control
            Serial.println( "Strength DOWN on" );
        }
        
        if( CheckSettledEvent( UC_GO ) == UserControlSwitchedOn )
        {
            // User has pressed GO button
            Serial.println( "GO on" );
        }
        
        if( CheckSettledEvent( UC_CANCEL ) == UserControlSwitchedOn )
        {
            // User has pressed CANCEL button
            Serial.println( "CANCEL on" );
        }        
    
        if( CheckSettledEvent( UC_CUP_INC ) == UserControlSwitchedOff )
        {
            // User has toggled Cup increment control
            Serial.println( "Cup UP off" );
        }

        if( CheckSettledEvent( UC_CUP_DEC ) == UserControlSwitchedOff )
        {
            // User has toggled Cup decrement control
            Serial.println( "Cup DOWN off" );
        }

        if( CheckSettledEvent( UC_DRINK_INC ) == UserControlSwitchedOff )
        {
            // User has toggled 'Next drink type' control
            Serial.println( "Drink UP off" );
        }

        if( CheckSettledEvent( UC_DRINK_DEC ) == UserControlSwitchedOff )
        {
            // User has toggled 'Previous drink type' control
            Serial.println( "Drink DOWN off" );
        }
    
        if( CheckSettledEvent( UC_MILK_INC ) == UserControlSwitchedOff )
        {
            // User has toggled 'Increment milk' control
            Serial.println( "Milk UP off" );
        }
    
        if( CheckSettledEvent( UC_MILK_DEC ) == UserControlSwitchedOff )
        {
            // User has toggled 'Decrement milk' control
            Serial.println( "Milk DOWN off" );
        }
    
        if( CheckSettledEvent( UC_SUGAR_INC ) == UserControlSwitchedOff )
        {
            // User has toggled 'Increment sugar' control
            Serial.println( "Sugar UP off" );
        }
    
        if( CheckSettledEvent( UC_SUGAR_DEC ) == UserControlSwitchedOff )
        {
            // User has toggled 'Decrement sugar' control
            Serial.println( "Sugar DOWN off" );
        }
    
        if( CheckSettledEvent( UC_STRENGTH_INC ) == UserControlSwitchedOff )
        {
            // User has toggled 'Increment strength' control
            Serial.println( "Strength UP off" );
        }
    
        if( CheckSettledEvent( UC_STRENGTH_DEC ) == UserControlSwitchedOff )
        {
            // User has toggled 'Decrement strength' control
            Serial.println( "Strength DOWN off" );
        }
    
        if( CheckSettledEvent( UC_GO ) == UserControlSwitchedOff )
        {
            // User has pressed GO button
            Serial.println( "GO off" );
        }
    
        if( CheckSettledEvent( UC_CANCEL ) == UserControlSwitchedOff )
        {
            // User has pressed CANCEL button
            Serial.println( "CANCEL off" );
        }

        // Now record current settled state of controls
        RecordSettledStates();
    }
}

//
//  ReadStates
//
//  Read the current states of the user controls
//
void UserControls::ReadStates( )
{
    for( int nControlID = 0; nControlID < UC_MAXCONTROLS; nControlID++ )
    {
        m_bControlStates[nControlID] = GetControlStateDirect( nControlID );
    } // for
}

//
//  ReadSettledStates
//
//  Read the current states of the user controls and compare them with previously read states to see if they have settled
//
bool UserControls::ReadSettledStates( )
{
    bool bSettled = true;
    for( int nControlID = 0; nControlID < UC_MAXCONTROLS; nControlID++ )
    {
        m_bSettledControlStates[nControlID] = GetControlStateDirect( nControlID );
        if(m_bSettledControlStates[nControlID] != m_bControlStates[nControlID] )
        {
            // Controls have not settled
            bSettled = false;
        }
    } // for
    
    return bSettled;
}

//
//  RecordSettledStates
//
//  Store the current settled states as the previous settled states
//
void UserControls::RecordSettledStates( )
{
    for( int nControlID = 0; nControlID < UC_MAXCONTROLS; nControlID++ )
    {
        m_bPreviousSettledControlStates[ nControlID ] = m_bSettledControlStates[ nControlID ];
    } // for
}

//
//  CheckSettledEvent
//
//  Determine the changes between two sets of settled user control states
//
UserControlEvent UserControls::CheckSettledEvent( int nControlID )
{
    UserControlEvent controlEvent = UserControlNoChange;
    if( m_bSettledControlStates[nControlID] != m_bPreviousSettledControlStates[nControlID] )
    {
        controlEvent = m_bSettledControlStates[nControlID] ? UserControlSwitchedOff : UserControlSwitchedOn;
    }
    
    return controlEvent;
}

//
//  GetControlStateDirect
//  
//  Read the state of a given control directly from the input register
//
bool UserControls::GetControlStateDirect( int nControlID )
{
    bool bState = false;
    
    // Read the input register
    byte userControlsLower = 0;
    byte userControlsUpper = 0;
    m_pSystem->ReadInputRegisterData( &userControlsLower, &userControlsUpper );

    switch( nControlID )
    {
        case UC_CUP_INC:
        bState = (userControlsLower & 0x01);
        break;
        
        case UC_CUP_DEC:
        bState = (userControlsLower & 0x02);
        break;
        
        case UC_MILK_INC:
        bState = (userControlsLower & 0x04);
        break;
        
        case UC_MILK_DEC:
        bState = (userControlsLower & 0x08);
        break;
        
        case UC_SUGAR_INC:
        bState = (userControlsLower & 0x10);
        break;
        
        case UC_SUGAR_DEC:
        bState = (userControlsLower & 0x20);
        break;
        
        case UC_STRENGTH_INC:
        bState = (userControlsLower & 0x40);
        break;
        
        case UC_STRENGTH_DEC:
        bState = (userControlsLower & 0x80);
        break;
        
        case UC_DRINK_INC:
        bState = (userControlsUpper & 0x01);
        break;
        
        case UC_DRINK_DEC:
        bState = (userControlsUpper & 0x02);
        break;
        
        case UC_GO:
        bState = (userControlsUpper & 0x04);
        break;
        
        case UC_CANCEL:
        bState = (userControlsUpper & 0x08);
        break;        
    } // switch  
    
    return bState;      
}    


