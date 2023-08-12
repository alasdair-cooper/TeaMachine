//
//	UserControls.h
//
//	Created: 18/12/2017
//	Author: Nick
//

#ifndef __USERCONTROLS_H__
#define __USERCONTROLS_H__

#include "TeaMachineComponent.h"

typedef enum
{
    UserControlNoChange,
    UserControlSwitchedOn,
    UserControlSwitchedOff
}    
UserControlEvent;

class UserControls : public TeaMachineComponent
{
public:
    // Construction/destruction
	UserControls();
	UserControls(TeaMachine *pTeaMachine);
	~UserControls();

    // Virtual function overrides
    bool Initialise();
    bool Test();
    
    // Read control states
    void ReadStates();
    
    // Read debounced control states
    bool ReadSettledStates();
    
    // Store debounced control states for future reference
    void RecordSettledStates();

    // Determine control events by comparing latest switch states with previous ones
    UserControlEvent CheckSettledEvent( int nControlID );
    
private:
    // Initialisation
	UserControls( const UserControls &c );
	UserControls& operator=( const UserControls &c );
    void ClearMembers();
 
     // Read control state directly from hardware
     bool GetControlStateDirect( int nControlID );
   
    // Array containing on/off state of all user controls
    bool                m_bControlStates[UC_MAXCONTROLS+1];
    bool                m_bSettledControlStates[UC_MAXCONTROLS+1];
    bool                m_bPreviousSettledControlStates[UC_MAXCONTROLS+1];
   
    // Array containing control events detected at last scan
//    UserControlEvent    m_ControlEvents[UC_NUMBEROFCONTROLS+1];
}; //UserControls

#endif //__UserControls_H__
