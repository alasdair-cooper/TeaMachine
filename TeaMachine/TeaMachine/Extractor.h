//
//	Extractor.h
//
//	Created: 08/02/2017
//	Author: Nick
//

#ifndef __EXTRACTOR_H__
#define __EXTRACTOR_H__

#include "TeaMachineComponent.h"

class Extractor : public TeaMachineComponent
{
public:
	Extractor();
	Extractor(TeaMachine *pTeaMachine);
	~Extractor();
	
	bool Test();
	bool Raise();
	bool Lower();
	bool IsRaised();
	bool IsLowered();
    void Stop();
	
protected:
private:
	Extractor( const Extractor &c );
	Extractor& operator=( const Extractor &c );
}; //Extractor

#endif //__EXTRACTOR_H__
