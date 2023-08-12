//
//	CupLevelProbe.h
//
//	Created: 24/02/2017
//	Author: Nick
//

#ifndef __CUPLEVELPROBE_H__
#define __CUPLEVELPROBE_H__

#include "TeaMachineComponent.h"

class CupLevelProbe : public TeaMachineComponent
{
public:
	CupLevelProbe();
	CupLevelProbe(TeaMachine *pTeaMachine);
	~CupLevelProbe();

	bool Initialise();				// Initialise the device
	bool Raise();					// Raise the probe arm
	bool Lower();					// Lower the probe arm
    bool IsRaised();                // True if probe is raised
	bool CupIsFull();				// True if cup is full
	bool Test();					// Test device
	void Stop();					// Stop the motor
	
private:
	CupLevelProbe( const CupLevelProbe &c );
	CupLevelProbe& operator=( const CupLevelProbe &c );
}; //CupLevelProbe

#endif //__CUPLEVELPROBE_H__
