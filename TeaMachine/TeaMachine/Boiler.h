// 
// Boiler.h
//
// Created: 28/03/2017
// Author: Nick
//

#ifndef __BOILER_H__
#define __BOILER_H__

#include "TeaMachineComponent.h"

// Forward declarations
class DallasTemperature;
class OneWire;

class Boiler : public TeaMachineComponent
{
public:
	Boiler();
	Boiler(TeaMachine *pTeaMachine);
	~Boiler();

	bool Test();                    // Test the boiler
	bool TestExitValve();

	bool Initialise();
	bool EnableHeater(bool bSwitchOn);
	bool EnableExitValve(bool bOpen);
	float GetWaterTemperature();
	void MonitorTemperature();

	OneWire *m_pOneWire;
	DallasTemperature *m_pSensors;
	
protected:
private:
	Boiler( const Boiler &c );
	Boiler& operator=( const Boiler &c );
}; //Boiler

#endif //__BOILER_H__
