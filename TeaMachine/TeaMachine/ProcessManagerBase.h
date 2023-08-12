//
//	ProcessManagerBase.h
//
//	Created: 12/05/2017
//	Author: Nick
//

#ifndef __PROCESSMANAGERBASE_H__
#define __PROCESSMANAGERBASE_H__

#include "ProcessDefs.h"
//#include "WString.h"

// Forward declarations
class TeaMachineController;
class TeaMachine;
class Drink;
class String;

typedef enum
{
	ProcessIncomplete,
	ProcessFailed,
	ProcessComplete
}
ProcessManagerResponse;

class ProcessManagerBase
{
public:
	// Construction/destruction
	ProcessManagerBase();
	ProcessManagerBase(TeaMachineController *pController);
	~ProcessManagerBase();

	// Returns process code for most recent error
   	unsigned long GetLastError() { return m_lErrorCode; }
    
    // Returns response to last Begin/Advance call
    ProcessManagerResponse GetLastResponse() { return m_LastResponse; };
		
	// Set drink currently being prepared           
    void SetDrink( Drink *pDrink ) { m_pDrink = pDrink; };
   
    // Log a process event
    void LogProcessEvent(String strText);

    // Pure virtual functions
    virtual ProcessManagerResponse BeginReset() = 0;    // Initiate reset sequence
    virtual ProcessManagerResponse BeginProcess() = 0;  // Initiate process sequence
    virtual ProcessManagerResponse Continue() = 0;      // Advance the state machine
    virtual void Stop() = 0;							// Stop all devices
	
protected:
	// Overrides
	virtual bool Initialise();						// Initialise the process manager
	virtual bool Test();							// Test the process manager
    
	// Timeout control
	void SetTimeout(unsigned long lTimeout);		// Set timeout in mS
	void SetTimeout(unsigned long lTimeout, long lApplicableErrorCode );
    void ClearTimeout();                            // Remove the timeout
	bool CheckTimeoutExpired();						// Check timeout
	void SetErrorCode(long lErrorCode);	            // Set process error code

    void SetResponse( ProcessManagerResponse response ) { m_LastResponse = response; }

protected:
    TeaMachine              *m_pTeaMachine;
    TeaMachineController    *m_pController;
    Drink                   *m_pDrink;              // Drink currently in process
	
private:
	ProcessManagerBase( const ProcessManagerBase &c );
	ProcessManagerBase& operator=( const ProcessManagerBase &c );
    
    void ClearMembers();
	
	unsigned long m_lTimeout;						// Current timeout in mS
	unsigned long m_lStartTime;						// Time count when timeout was set
	unsigned long m_lErrorCode;						// Process code for most recent error
	unsigned long m_lApplicableTimeoutErrorCode;    // Process code in the event of a timeout
    ProcessManagerResponse m_LastResponse;          // Last response to a Begin or Advance call
}; //ProcessManagerBase

#endif //__PROCESSMANAGERBASE_H__
