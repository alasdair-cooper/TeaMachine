//
//	CupConveyor.cpp
//
//	Created: 24/02/2017 13:48:59
//	Author: Nick
//

#include "TeaMachineApp.h"
#include "CupConveyor.h"

//
//	Default constructor
//
CupConveyor::CupConveyor()
{
} //CupConveyor

//
//	Default constructor
//
CupConveyor::CupConveyor(TeaMachine *pTeaMachine) : TeaMachineComponent( pTeaMachine )
{
} //CupConveyor

//
//	Default destructor
//
CupConveyor::~CupConveyor()
{
} //~CupConveyor

//
//	Test
//
//	Perform hardware tests
//
bool CupConveyor::Test()
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
bool CupConveyor::BeginAdvance()
{
	m_pSystem->SetOutputRegisterBit(OUTREG_CUPCON_ADVANCE);
	delay(500);
	m_pSystem->ClearOutputRegisterBit(OUTREG_CUPCON_ADVANCE);
	
	return true;
}

//
//	IsStationary
//
//	Returns true if the conveyor motor has stopped
//
bool CupConveyor::IsStationary()
{
	word wInputRegister = m_pSystem->ReadInputRegisterData();

	return ((wInputRegister & INREGMASK_CUPCON_STATIONARY) != 0);
}

//
//	Stop
//
//	Stop the conveyor
//
void CupConveyor::Stop()
{
	// No software control of this function is possible at present
}
