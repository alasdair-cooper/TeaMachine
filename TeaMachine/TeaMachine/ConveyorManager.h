//
//	ConveyorManager.h
//
//	Created: 12/05/2017
//	Author: Nick
//

#ifndef __CONVEYORMANAGER_H__
#define __CONVEYORMANAGER_H__

#include "ProcessManagerBase.h"

// Forward declarations
class TeaMachine;
class CupConveyor;

typedef enum
{
    // Reset sequence
    ResetSynchroniseConveyor,
    
    // Conveyor advance sequence
    ConveyorBeginAdvance,
	ConveyorAdvancing,
    ConveyorAdvanceComplete,
}
ConveyorState;

class ConveyorManager : public ProcessManagerBase
{
public:
	ConveyorManager();
	ConveyorManager(TeaMachineController *pController);
	~ConveyorManager();
	
	bool Initialise();						            // Initialise the conveyor manager
	bool Test();							            // Perform a complete cycle test
    ProcessManagerResponse BeginReset();                // Initiate reset sequence
    ProcessManagerResponse BeginProcess();              // Initiate conveyor advance sequence
	ProcessManagerResponse Continue();		            // Initiate the next step of the process
	void Stop();			                            // Terminate the conveyor advance process

private:
	ConveyorManager( const ConveyorManager &c );
	ConveyorManager& operator=( const ConveyorManager &c );
    
    void ClearMembers();                                // Initialise class members
    
	ProcessManagerResponse AdvanceStateMachine();		// Initiate the next step of the process
        
    void SetNextState(ConveyorState nextState);         // Set the next state in sequence
		
	// Pointers to tea machine components
	CupConveyor	        *m_pCupConveyor;
    
    ConveyorState       m_State;
}; //ConveyorManager

#endif //__CONVEYORMANAGER_H__
