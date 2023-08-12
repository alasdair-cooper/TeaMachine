//
//	TeaMachineComponent.h
//
//	Created: 12/05/2017
//	Author: Nick
//

#ifndef __TEAMACHINECOMPONENT_H__
#define __TEAMACHINECOMPONENT_H__

#include "HardwareDefs.h"

class TeaMachine;
class System;

class TeaMachineComponent
{
public:
	TeaMachineComponent();
	TeaMachineComponent(TeaMachine *pTeaMachine);
	~TeaMachineComponent();

public:	
	virtual bool Initialise();
	virtual bool Test();

protected:
	TeaMachine	*m_pTeaMachine;		// Pointer to one and only TeaMachine object
	System		*m_pSystem;			// Pointer to one and only System object

private:
	TeaMachineComponent( const TeaMachineComponent &c );
	TeaMachineComponent& operator=( const TeaMachineComponent &c );
}; //TeaMachineComponent

#endif //__TEAMACHINECOMPONENT_H__
