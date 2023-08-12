//
//	MilkDispenser.cpp
//
//	Created: 27/01/2018
//	Author: Nick
//

#include "TeaMachineApp.h"
#include "MilkDispenser.h"
#include "ProcessDefs.h"

//
//	Default constructor
//
MilkDispenser::MilkDispenser()
{
} //MilkDispenser

//
//	Default constructor
//
MilkDispenser::MilkDispenser(TeaMachine *pTeaMachine) : TeaMachineComponent( pTeaMachine )
{
} //MilkDispenser

//
//	Default destructor
//
MilkDispenser::~MilkDispenser()
{
} //~MilkDispenser

//
//	Initialise
//
//	Initialise the device
//
bool MilkDispenser::Initialise()
{
}

//
//	Raise()
//
//	Raise the milk dispenser
//
bool MilkDispenser::Raise()
{
	m_pSystem->SetOutputRegisterBit(OUTREG_MILKDISP_RAISE);
	m_pSystem->ClearOutputRegisterBit(OUTREG_MILKDISP_RAISE);
	m_pSystem->SetOutputRegisterBit(OUTREG_MILKDISP_EN);
	
	return true;
}

//
//	Dispense()
//
//	Perform a dispense cycle
//
bool MilkDispenser::Dispense()
{
    m_pSystem->SetOutputRegisterBit(OUTREG_MILKDISP_LOWER);
    m_pSystem->ClearOutputRegisterBit(OUTREG_MILKDISP_LOWER);
    m_pSystem->SetOutputRegisterBit(OUTREG_MILKDISP_EN);
    
    return true;
}

//
//	Stop()
//
//	Switch off the MilkDispenser motor
//
void MilkDispenser::Stop()
{
    m_pSystem->ClearOutputRegisterBit(OUTREG_MILKDISP_EN);
}

//
//	IsRaised()
//
//	Returns true if the milk dispenser is currently in the raised position
//
bool MilkDispenser::IsRaised()
{
	word wInputRegister = m_pSystem->ReadInputRegisterData();
	return (wInputRegister & INREGMASK_MILKDISP_HILIMITSW) != 0;
}

//
//	IsRaising()
//
//	Returns true if the milk dispenser is in the process of returning to the raised position
//
bool MilkDispenser::IsRaising()
{
	word wInputRegister = m_pSystem->ReadInputRegisterData();
	return (wInputRegister & INREGMASK_MILKDISP_RAISING) != 0;
}

//
//	SetLoadPosition()
//
//	Move the milk dispenser into the position required to load/unload the bottle
//
bool MilkDispenser::SetLoadPosition()
{
	bool bResult = false;
	
	if( !IsRaised() )
	{
		// Dispenser is not in the raised position, so raise it
		unsigned int lTimeout = millis() + MILK_TIMEOUT_RAISE;
		Raise();
		while( !IsRaised() && (millis() < lTimeout) );
	}
	
	if( IsRaised() )
	{
		// Start moving to the load position
		Dispense();
			
		unsigned int lTimeToStop = millis() + MILK_LOAD_DELAY;
	
		while( millis() < lTimeToStop );
		
		// Stop the movement
		Stop();
		
		bResult = true;
	}
	
	return bResult;
}

//
//	Test
//
//	Perform hardware tests
//
bool MilkDispenser::Test()
{
	Dispense();
	while(true);
	
	return true;
}
