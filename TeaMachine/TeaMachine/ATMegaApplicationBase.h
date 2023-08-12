//
//	ATMegaApplicationBase.h
//
//	Created: 05/02/2017
//	Author: Nick
//	Base class for ATMega328 embedded applications
//

#ifndef __ATMEGAAPPLICATIONBASE_H__
#define __ATMEGAAPPLICATIONBASE_H__

#include "Arduino.h"
#include "System.h"

// Forward declarations
class System;

class ATMegaApplicationBase
{
// Member functions
public:
	ATMegaApplicationBase();
	~ATMegaApplicationBase();
	
	virtual bool Initialise();
	virtual void Process() = 0;
	
public:
	System *GetSystem();

protected:

	// Pointer to one and only instance of System class
	System *m_pTheSystem;

private:
	ATMegaApplicationBase( const ATMegaApplicationBase &c );
	ATMegaApplicationBase& operator=( const ATMegaApplicationBase &c );
};

#endif //__ATMEGAAPPLICATIONBASE_H__
