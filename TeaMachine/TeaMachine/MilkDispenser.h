//
//	MilkDispenser.h
//
//	Created: 27/01/2018
//	Author: Nick
//

#ifndef __MILKDISPENSER_H__
#define __MILKDISPENSER_H__

#include "TeaMachineComponent.h"

class MilkDispenser : public TeaMachineComponent
{
public:
	MilkDispenser();
	MilkDispenser(TeaMachine *pTeaMachine);
	~MilkDispenser();
	
	bool Test();
	bool Initialise();
	bool Raise();
	bool Dispense();
	bool IsRaised();
	bool IsRaising();
    void Stop();
	bool SetLoadPosition();

private:
	MilkDispenser( const MilkDispenser &c );
	MilkDispenser& operator=( const MilkDispenser &c );
}; //MilkDispenser

#endif //__MILKDISPENSER_H__
