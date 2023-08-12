//
//	Masher.h
//
//	Created: 08/02/2017
//	Author: Nick
//

#ifndef __MASHER_H__
#define __MASHER_H__

#include "TeaMachineComponent.h"

class Masher : public TeaMachineComponent
{
public:
	Masher();
	Masher(TeaMachine *pTeaMachine);
	~Masher();
	
	bool Test();
	bool Raise();
	bool Lower();
	bool IsRaised();
	bool IsLowered();
    void Stop();
	
protected:
    typedef enum
    {
        masherActionInactive,
        masherActionRaising,
        masherActionLowering
    }
    MasherAction;
    
private:
	Masher( const Masher &c );
	Masher& operator=( const Masher &c );
    MasherAction m_lastAction;
}; //Masher

#endif //__MASHER_H__
