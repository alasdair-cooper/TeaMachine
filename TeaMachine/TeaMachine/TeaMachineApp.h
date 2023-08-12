//
//	TeaMachineApp.h
//
//	Created: 05/02/2017
//	Author: Nick
//

#ifndef __TEAMACHINE_H__
#define __TEAMACHINE_H__

#include "Arduino.h"
#include "ATMegaApplicationBase.h"

typedef enum
{
    ErrorContext_Reset,
    ErrorContext_Service
}
TeaMachineErrorContext;

class TeaMachine : public ATMegaApplicationBase
{
	
// Member functions
public:
	TeaMachine();
	~TeaMachine();
	
	bool Initialise();					// Application initialisation
	void Process();						// Main process loop

	bool ExecuteGroup1Tests();			// Run device tests
	void ExecuteGroup2Tests();			// Run process tests
	bool ExecuteLoadSequence();			// Allow operator to renew consumables

private:
	TeaMachine( const TeaMachine &c );
	TeaMachine& operator=( const TeaMachine &c );
	
	unsigned int	m_nSystemMode;		// System operating mode
	unsigned int	m_nTestCode;		// Operator-selected test identifier
};

#endif
