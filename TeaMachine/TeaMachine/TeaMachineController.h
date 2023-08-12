//
//	TeaMachineController.h
//
//	Created: 15/12/2017
//	Author: Nick
//

#ifndef __TEAMACHINECONTROLLER_H__
#define __TEAMACHINECONTROLLER_H__

#include "Drink.h"

// Forward declarations
class TeaMachine;
class UIManager;
class BrewManager;
class MilkManager;
class StirManager;
class ConveyorManager;
class ProcessManagerBase;

typedef enum
{
    ControllerMode_Load = -2,
    ControllerMode_InService = -1,
	ControllerMode_InServiceWithLogging = 0,
    ControllerMode_TestUserInterface = 1,
    ControllerMode_TestBrewManager = 2,
    ControllerMode_TestMilkManager = 3,
    ControllerMode_TestStirManager = 4,
    ControllerMode_TestConveyorManager = 5,
}
TeaMachineControllerMode;

class TeaMachineController
{
public:
    // Creation/destruction
	TeaMachineController();
	TeaMachineController(TeaMachine *pTeaMachine);
	~TeaMachineController();
    
    bool Start( TeaMachineControllerMode mode );           // Start the tea machine

    TeaMachine *GetTeaMachine();
    
    // Notifications
    void NotifyConveyorAdvanceComplete();                   // The cup conveyor has advanced by one position
    Drink *NotifyNewDrinkRequest( unsigned int nStation );  // A new drink has been requested
    void NotifyCupRemoval( unsigned int nStation );         // A cup has been removed

	// Cup identification
    Drink *GetDrinkToTheRight( Drink *pDrink );             // Returns drink to the right of the one specified
    Drink *GetDrinkToTheLeft( Drink *pDrink );              // Returns drink to the left of the one specified
	Drink *GetDrinkByStation( int nStation );				// Returns drink at specified station
	Drink *GetNextDrink();									// Get any valid drink (prioritising unconfirmed drinks)
    int GetDrinkLocation( int nDrinkID );                   // Returns station number of given drink
	
	// Milk/stire collision zone sychronisation
	bool LockCollisionZone( ProcessManagerBase *pRequestor ); // Acquire a lock on the collision zone
	void ReleaseCollisionZone( ProcessManagerBase *pOwner ); // Release the lock on the collision zone

private:
	TeaMachineController( const TeaMachineController &c );
	TeaMachineController& operator=( const TeaMachineController &c );
    
    // Initialisation    
    bool Initialise();
    void ClearMembers();
    
    // Drink management
    unsigned int AllocateDrinkID();
    void ReleaseDrinkID( unsigned int nDrinkID );
    bool ActiveDrinkIsPresent();                            // Determine whether a cup needing service is present
	bool DrinkStateAtStation( int nStation, DrinkState state ); // Returns true if a confirmed drink is present at a given station having a specified state
	bool DrinkUnconfirmedAtStation( int nStation );			// Returns true if an unconfirmed drink is present at a given station
	bool ReceptionCupsReadyToAdvance();						// Returns true if cups at reception are OK to advance

    // Service implementation
    void Service();                                         // Main service loop
    bool ResetHardware();                                   // Restore hardware to known state
    bool TestUserInterface();                               // Test the user interface functions
    
	// Error handling
	void HandleError( TeaMachineErrorContext context, unsigned long lErrorCode );
   
    bool PreparationStationsAreInactive();                  // Determine whether preparation stations are inactive so conveyor may advance

    UIManager       *m_pUIManager;                          // Manager object for display and user controls
    BrewManager     *m_pBrewManager;                        // Manager object for tea brewing process
    MilkManager     *m_pMilkManager;                        // Manager object for milk supply process
    StirManager     *m_pStirManager;                        // Manager object for stirring process
    ConveyorManager *m_pConveyorManager;                    // Manager object for cup conveyor movement
    
    TeaMachine      *m_pTeaMachine;                         // Pointer to one and only TeaMachine object
    
    Drink           *m_drinks[STATIONS+1];                  // Drink array indexed by station location
    bool            m_bDrinkIDAllocated[STATIONS+1];        // Array used to record allocated drink IDs
	
	ProcessManagerBase	*m_pCollisionZoneOwner;				// Pointer to manager object having access to collision zone
}; //TeaMachineController

#endif //__TEAMACHINECONTROLLER_H__
