//
// Boiler.cpp
//
// Created: 28/03/2017
// Author: Nick
//

#include "TeaMachineApp.h"
#include "OneWire.h"
#include "DallasTemperature.h"
#include "Boiler.h"

//
//	Default constructor
//
Boiler::Boiler()
{
	m_pOneWire = NULL;
	m_pSensors = NULL;
} //Masher

//
//	Default constructor
//
Boiler::Boiler(TeaMachine *pTeaMachine) : TeaMachineComponent( pTeaMachine )
{
	m_pOneWire = NULL;
	m_pSensors = NULL;
} //Boiler

//
//	Default destructor
//
Boiler::~Boiler()
{
} //~Boiler

bool Boiler::Initialise()
{
	// Initialise One-Wire functions
	m_pOneWire = new OneWire(ONE_WIRE_BUS);
	m_pSensors = new DallasTemperature(m_pOneWire);
	m_pSensors->begin();
	
	return true;
}

bool Boiler::EnableHeater(bool bSwitchOn)
{
	if( bSwitchOn )
	{
		m_pSystem->SetOutputRegisterBit(OUTREG_BOILERCTRL);	
		MonitorTemperature();	
	}
	else
	{
		m_pSystem->ClearOutputRegisterBit(OUTREG_BOILERCTRL);
	}
		
	return true;	
}

void Boiler::MonitorTemperature()
{
		// call sensors.requestTemperatures() to issue a global temperature
		// request to all devices on the bus
		//Serial.print(" Requesting boiler temperature...");
		//		  sensors.requestTemperatures(); // Send the command to get temperatures
		m_pSensors->requestTemperatures(); // Send the command to get temperatures
		//Serial.println("DONE");

		Serial.print("Boiler temperature is: ");
		//		  Serial.print(sensors.getTempCByIndex(0)); // Why "byIndex"?
		Serial.print(m_pSensors->getTempCByIndex(0)); // Why "byIndex"?
        Serial.print("\n");
		// You can have more than one IC on the same bus.
		// 0 refers to the first IC on the wire
		//delay(1000);
}

bool Boiler::EnableExitValve(bool bOpen)
{
	if( bOpen )
	{
		m_pSystem->SetOutputRegisterBit(OUTREG_VALVECTRL);
	}
	else
	{
		m_pSystem->ClearOutputRegisterBit(OUTREG_VALVECTRL);
	}
	
	return true;
}

float Boiler::GetWaterTemperature()
{
    m_pSensors->requestTemperatures();
	return m_pSensors->getTempCByIndex(0);
}

bool Boiler::Test()
{   
	while(true)
	{
		// Fetch action switch setting
		if((m_pSystem->ReadInputRegisterData() & INREGMASK_ACTIONSWITCH) != 0)
		{
			// Switch boiler on
			EnableHeater(true);
				
			while((m_pSystem->ReadInputRegisterData() & INREGMASK_ACTIONSWITCH) != 0);
		}
		else
		{
			// Switch boiler off
			EnableHeater(false);
						
			while((m_pSystem->ReadInputRegisterData() & INREGMASK_ACTIONSWITCH) == 0);
		}
	} // while
	
	return true;
}

bool Boiler::TestExitValve()
{
	while(true)
	{
		// Fetch action switch setting
		if((m_pSystem->ReadInputRegisterData() & INREGMASK_ACTIONSWITCH) != 0)
		{
			// Open valve
			EnableExitValve(true);
			
			while((m_pSystem->ReadInputRegisterData() & INREGMASK_ACTIONSWITCH) != 0);
		}
		else
		{
			// Close valve
			EnableExitValve(false);
			
			while((m_pSystem->ReadInputRegisterData() & INREGMASK_ACTIONSWITCH) == 0);
		}
	} // while
	
	return true;
}