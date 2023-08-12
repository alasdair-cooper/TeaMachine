//
//	UIManager.h
//
//	Created: 12/05/2017
//	Author: Nick
//

#ifndef __UIMANAGER_H__
#define __UIMANAGER_H__

#include "ProcessManagerBase.h"

// Forward declarations
class TeaMachine;
class Display;
class CupDetector;
class UserControls;

// Standard display text
#define TXT_TEST		"TEST"
#define TXT_LOAD		"LOAD"
#define TXT_FAIL		"FAIL"

typedef enum
{
    // Reset sequence
    UIReset,
    UIResetComplete,
       
    // Process states
    UIScanStations,
    UICheckStationScanResults,
    UIScanControls,
    UICheckControlScanResults
}
UIState;

class UIManager : public ProcessManagerBase
{
public:
	UIManager();
	UIManager(TeaMachineController *pController);
	~UIManager();
	
	bool Initialise();						            // Initialise the UI manager
    ProcessManagerResponse BeginReset();                // Initiate reset sequence
    ProcessManagerResponse BeginProcess();              // Initiate UI management sequence
	ProcessManagerResponse Continue();		            // Initiate the next step of the process
	void Stop();										// Stop all devices
    
    void NotifyStationAdvanceBegins();                  // Called when stations begin advancing by one position
    void NotifyStationAdvanceComplete();                // Called when station advance completes
    void SetCurrentDrink( Drink *pDrink );              // Set the drink that the user currently views
	void SetMessage( char *pText, int nMilkIndicator, int nSugarIndicator, int nStrengthIndicator, bool bStatusOK = false, bool bStatusFail = false );	// Display a user message

private:
	UIManager( const UIManager &c );
	UIManager& operator=( const UIManager &c );
    
    void ClearMembers();                                // Initialise class members
    
	ProcessManagerResponse AdvanceStateMachine();		// Initiate the next step of the process
        
    void SetNextState(UIState nextState);               // Set the next state in sequence

    void HandleCupPlacement( int nStation );            // Called when a new cup is detected
    void HandleCupRemoval( int nStation );              // Called when a cup is removed
    void HandleUserControlEvents();                     // Handle changes to user controls
	void ClearCurrentDrinkDisplay();					// Clear current drink display information
	        
	// Pointers to tea machine components
	Display	            *m_pDisplay;
    CupDetector         *m_pCupDetector;
    UserControls        *m_pUserControls;
    
    UIState             m_State;
    
    Drink               *m_pCurrentDrink;               // Currently selected drink    
	bool				m_bStationAdvanceInProgress;	// True if conveyor is moving cups along by one position
	bool				m_bResultsAwaitChecking;		// True if the results of a cup scan have yet to be analysed
    unsigned long       m_lDebounceCheckTime;			// Time that user controls debounce check can take place
    unsigned long       m_lNextFlashActionTime;			// Time that next flash on/off will take place
}; //UIManager

#endif //__UIMANAGER_H__
