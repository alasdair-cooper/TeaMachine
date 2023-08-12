//
//	MilkManager.h
//
//	Created: 12/05/2017
//	Author: Nick
//

#ifndef __MILKMANAGER_H__
#define __MILKMANAGER_H__

#include "ProcessManagerBase.h"

// Forward declarations
class TeaMachine;
class MilkDispenser;

typedef enum
{
    // Reset sequence
    MilkResetDispenser,
	MilkResetCheckComplete,
    
    // Process states
    MilkBeginDispense,
	MilkInitiateCycle,
	MilkCheckCycleComplete,
	MilkCheckZoneClear
}
MilkState;

class MilkManager : public ProcessManagerBase
{
public:
	MilkManager();
	MilkManager(TeaMachineController *pController);
	~MilkManager();
	
	bool Initialise();						            // Initialise the conveyor manager
	bool Test();							            // Perform a complete cycle test
    ProcessManagerResponse BeginReset();                // Initiate reset sequence
    ProcessManagerResponse BeginProcess();              // Initiate Milk process
	ProcessManagerResponse Continue();		            // Initiate the next step of the process
	void Stop();										// Terminate the Milk process
    	
	bool SetLoadPosition();								// Allow loading of milk bottle
	
private:
	MilkManager( const MilkManager &c );
	MilkManager& operator=( const MilkManager &c );
    
    void ClearMembers();                                // Initialise class members
    
	ProcessManagerResponse AdvanceStateMachine();		// Initiate the next step of the process
        
    void SetNextState(MilkState nextState);             // Set the next state in sequence
    
    MilkState               m_State;
	MilkDispenser			*m_pMilkDispenser;			// Milk dispenser driver object
	
	int						m_nMilkDispensesRequired;		// Number of dispense cycles required
	unsigned long			m_lNextDispenseTime;		// Time after which the next dispense may begin
}; //MilkManager

#endif //__MilkManager_H__
