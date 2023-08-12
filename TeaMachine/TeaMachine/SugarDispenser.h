//
//	SugarDispenser.h
//
//	Created: 24/02/2017
//	Author: Nick
//

#ifndef __SUGARDISPENSER_H__
#define __SUGARDISPENSER_H__

#include "TeaMachineComponent.h"

class SugarDispenser : public TeaMachineComponent
{
public:
	SugarDispenser();
	SugarDispenser(TeaMachine *pTeaMachine);
	~SugarDispenser();

	bool Dispense();				// Perform a dispense cycle
	bool IsComplete();				// True if dispense cycle has completed
	bool Test();					// Test the teabag conveyor
	
	void Stop();					// Stop the dispenser motor
	
private:
	SugarDispenser( const SugarDispenser &c );
	SugarDispenser& operator=( const SugarDispenser &c );
}; //SugarDispenser

#endif //__SUGARDISPENSER_H__
