//
//	BagConveyor.h
//
//	Created: 24/02/2017
//	Author: Nick
//

#ifndef __BAGCONVEYOR_H__
#define __BAGCONVEYOR_H__

#include "TeaMachineComponent.h"

class BagConveyor : public TeaMachineComponent
{
public:
	BagConveyor();
	BagConveyor(TeaMachine *pTeaMachine);
	~BagConveyor();

	bool BeginAdvance();			// Start advancing the conveyor by one position
	bool IsStationary();			// True if conveyor motor has stopped
	bool Test();					// Test the teabag conveyor
	
	void Stop();					// Stop the conveyor
	
private:
	BagConveyor( const BagConveyor &c );
	BagConveyor& operator=( const BagConveyor &c );
}; //BagConveyor

#endif //__BAGCONVEYOR_H__
