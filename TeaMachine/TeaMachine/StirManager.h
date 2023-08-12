//
//	StirManager.h
//
//	Created: 12/05/2017
//	Author: Nick
//

#ifndef __StirManager_H__
#define __StirManager_H__

#include "ProcessManagerBase.h"

// Forward declarations
class TeaMachine;
class Stirrer;
class SugarDispenser;

typedef enum
{
    // Reset sequence
    StirBeginReset,
	StirResetCheckComplete,
    
    // Process states
    StirBeginProcess,
	StirDispenseSugar,
	StirWaitSugarDispensed,
	StirWaitForAccess,
	StirDeployedCheckComplete,
	StirActionCheckComplete,
    StirRaiseCheckComplete,
}
StirState;

class StirManager : public ProcessManagerBase
{
public:
	StirManager();
	StirManager(TeaMachineController *pController);
	~StirManager();
	
	bool Initialise();						            // Initialise the conveyor manager
	bool Test();							            // Perform a complete cycle test
    ProcessManagerResponse BeginReset();                // Initiate reset sequence
    ProcessManagerResponse BeginProcess();              // Initiate Stir process
	ProcessManagerResponse Continue();		            // Initiate the next step of the process
	void Stop();	    	                            // Terminate the Stir process
    void SetDrink( Drink *pDrink ) {m_pDrink = pDrink; }; // Set the current drink

private:
	StirManager( const StirManager &c );
	StirManager& operator=( const StirManager &c );
    
    void ClearMembers();                                // Initialise class members
    
	ProcessManagerResponse AdvanceStateMachine();		// Initiate the next step of the process
        
    void SetNextState(StirState nextState);             // Set the next state in sequence
    
    StirState               m_State;
    Drink                   *m_pDrink;                  // Drink currently in process
	Stirrer					*m_pStirrer;				// Pointer to stirrer device	
	SugarDispenser			*m_pSugarDispenser;			// Pointer to sugar dispenser
	unsigned long			m_lStirCompletionTime;		// Time at which stir will terminate
	int						m_nSugarDispensed;			// Amount of sugar dispensed so far
}; //StirManager

#endif //__StirManager_H__
