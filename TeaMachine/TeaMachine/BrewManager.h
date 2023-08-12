//
//	BrewManager.h
//
//	Created: 12/05/2017
//	Author: Nick
//

#ifndef __BREWMANAGER_H__
#define __BREWMANAGER_H__

#include "ProcessManagerBase.h"

// Forward declarations
class TeaMachine;
class Boiler;
class BagConveyor;
class BagDisposal;
class CupLevelProbe;
class Extractor;
class Masher;

typedef enum
{
    // Reset sequence
    BrewResetRaiseMasher,
    BrewResetRaiseExtractor,
    BrewResetComplete,
    
    // Brew sequence
    BrewDispenseLowerExtractor,
    BrewDispenseTeabag,
	BrewAwaitBoil,
    BrewBoilWater,
    BrewReleaseWater,
    BrewRaiseProbe,
    BrewMashTeabag,
    BrewRaiseMasher,
    BrewLowerMasherAgain,
    BrewRaiseExtractor,
    BrewComplete,
}
BrewState;

class BrewManager : public ProcessManagerBase
{
public:
	BrewManager();
	BrewManager(TeaMachineController *pController);
	~BrewManager();
	
	bool Initialise();						            // Initialise the brew manager
	bool Test();							            // Perform a complete cycle test
    ProcessManagerResponse BeginReset();                // Initiate reset sequence
    ProcessManagerResponse BeginProcess();              // Initiate brew sequence
	ProcessManagerResponse Continue();		            // Initiate the next step of the process
	void Stop();										// Stop all devices

private:
	BrewManager( const BrewManager &c );
	BrewManager& operator=( const BrewManager &c );
    
    void ClearMembers();                                // Initialise class members
    
	ProcessManagerResponse AdvanceStateMachine();		// Initiate the next step of the process
    
    void SetNextState(BrewState nextState);             // Set the next state in sequence
		
	// Pointers to tea machine components
	Boiler			*m_pBoiler;
	BagConveyor		*m_pBagConveyor;
	CupLevelProbe	*m_pCupLevelProbe;
	Extractor		*m_pExtractor;
	Masher			*m_pMasher;
    
    BrewState       m_State;
    long            m_lTimeRaiseMustStop;               // Time a partial raise must begin lowering (mS)
    long            m_lMashCount;                       // Number of mashes so far
    long            m_lMashesRequired;                  // Number of mashes needed for strength level
    long            m_lLastTemperatureSent;             // Time that last temperature measurement was sent to host
    float           m_fInitialWaterTemp;                // Water temperature at start of brew process
	bool			m_bDrinkAbandoned;					// Set if user removes cup during brew process
}; //BrewManager

#endif //__BREWMANAGER_H__
