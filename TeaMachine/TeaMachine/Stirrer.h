//
//	Stirrer.h
//
//	Created: 03/02/2017
//	Author: Nick
//

#ifndef __STIRRER_H__
#define __STIRRER_H__

#include "TeaMachineComponent.h"

class Stirrer : public TeaMachineComponent
{
public:
	Stirrer();
	Stirrer(TeaMachine *pTeaMachine);
	~Stirrer();
	
	bool Test();
	bool Raise();
	bool Lower();
	bool IsRaised();
	bool IsLowered();
    void Stop();
	
private:
	Stirrer( const Stirrer &c );
	Stirrer& operator=( const Stirrer &c );
}; //Stirrer

#endif //__STIRRER_H__
