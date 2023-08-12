//
//	CupConveyor.h
//
//	Created: 24/02/2017
//	Author: Nick
//

#ifndef __CUPCONVEYOR_H__
#define __CUPCONVEYOR_H__

#include "TeaMachineComponent.h"

class CupConveyor : public TeaMachineComponent
{
public:
	CupConveyor();
	CupConveyor(TeaMachine *pTeaMachine);
	~CupConveyor();

	bool BeginAdvance();			// Start advancing the conveyor by one position
	bool IsStationary();			// True if conveyor motor has stopped
	bool Test();					// Test the cup conveyor
	
	void Stop();					// Stop the conveyor motor

private:
	CupConveyor( const CupConveyor &c );
	CupConveyor& operator=( const CupConveyor &c );
}; //CupConveyor

#endif //__CUPCONVEYOR_H__
