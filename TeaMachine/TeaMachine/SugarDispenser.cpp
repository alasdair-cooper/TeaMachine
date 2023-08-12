//
//	SugarDispenser.cpp
//
//	Created: 10/03/2018
//	Author: Nick
//

#include "TeaMachineApp.h"
#include "SugarDispenser.h"

//
//	Default constructor
//
SugarDispenser::SugarDispenser()
{
} //SugarDispenser

//
//	Default constructor
//
SugarDispenser::SugarDispenser(TeaMachine *pTeaMachine) : TeaMachineComponent( pTeaMachine )
{
} //SugarDispenser

//
//	Default destructor
//
SugarDispenser::~SugarDispenser()
{
} //~SugarDispenser

//
//	Test
//
//	Perform hardware tests
//
bool SugarDispenser::Test()
{
	while(true)
	{
		// Wait for action switch to change state
		m_pSystem->WaitForActionSwitchChange();
	
		// Dispense the sugar
		Dispense();
	}
	
	return true;
}

//
//	Dispense
//
//	Perform a dispense cycle
//
bool SugarDispenser::Dispense()
{
	m_pSystem->SetOutputRegisterBit(OUTREG_SUGAR_DISPENSE);
	delay(500);
	m_pSystem->ClearOutputRegisterBit(OUTREG_SUGAR_DISPENSE);
	
	return true;
}

//
//	IsComplete
//
//	Returns true if the dispense cycle has terminated
//
bool SugarDispenser::IsComplete()
{
	return (digitalRead(SUGAR_DONE) != 0);
}

//
//	Stop
//
//	Stop the sugar dispenser motor
//
void SugarDispenser::Stop()
{
	// No software control of this function is possible at present
}
