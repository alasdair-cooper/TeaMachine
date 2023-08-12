//
//	BagConveyor.cpp
//
//	Created: 24/02/2017
//	Author: Nick
//

#include "TeaMachineApp.h"
#include "BagConveyor.h"

//
//	Default constructor
//
BagConveyor::BagConveyor()
{
} //BagConveyor

//
//	Default constructor
//
BagConveyor::BagConveyor(TeaMachine *pTeaMachine) : TeaMachineComponent( pTeaMachine )
{
} //BagConveyor

//
//	Default destructor
//
BagConveyor::~BagConveyor()
{
} //~BagConveyor

//
//	Test
//
//	Perform hardware tests
//
bool BagConveyor::Test()
{
	BeginAdvance();
	
	// Wait forever
	while(true);
	
	return true;
}

//
//	BeginAdvance
//
//	Advance the conveyor
//
bool BagConveyor::BeginAdvance()
{
	m_pSystem->SetOutputRegisterBit(OUTREG_BAGCON_ADVANCE);
	delay(500);
	m_pSystem->ClearOutputRegisterBit(OUTREG_BAGCON_ADVANCE);
	
	return true;
}

//
//	IsStationary
//
//	Returns true if the conveyor motor has stopped
//
bool BagConveyor::IsStationary()
{
	word wInputRegister = m_pSystem->ReadInputRegisterData();

	return ((wInputRegister & INREGMASK_BAGCON_STATIONARY) != 0);
}

//
//	Stop
//
//	Stop the conveyor
//
void BagConveyor::Stop()
{
	// No software control of this function is possible at present
}
