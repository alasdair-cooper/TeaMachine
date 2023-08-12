//
//	Masher.cpp
//
//	Created: 08/02/2017
//	Author: Nick
//

#include "TeaMachineApp.h"
#include "Masher.h"

//
//	Default constructor
//
Masher::Masher()
{
    m_lastAction = masherActionInactive;
} //Masher

//
//	Default constructor
//
Masher::Masher(TeaMachine *pTeaMachine) : TeaMachineComponent( pTeaMachine )
{
    m_lastAction = masherActionInactive;
} //Masher

//
//	Default destructor
//
Masher::~Masher()
{
} //~Masher

//
//	Raise()
//
//	Raise the masher
//
bool Masher::Raise()
{
	m_pSystem->SetOutputRegisterBit(OUTREG_MASHER_UPDOWN);
	m_pSystem->SetOutputRegisterBit(OUTREG_MASHER_EN);
	m_lastAction = masherActionRaising;
    
	return true;
}

//
//	Lower()
//
//	Lower the masher
//
bool Masher::Lower()
{
	m_pSystem->ClearOutputRegisterBit(OUTREG_MASHER_UPDOWN);
	m_pSystem->SetOutputRegisterBit(OUTREG_MASHER_EN);
	m_lastAction = masherActionLowering;
    
	return true;
}

//
//	Stop()
//
//	Switch off the masher motor
//
void Masher::Stop()
{
    m_pSystem->ClearOutputRegisterBit(OUTREG_MASHER_EN);
}

//
//	IsRaised()
//
//	Returns true if the masher is currently in the raised position
//
bool Masher::IsRaised()
{
	word wInputRegister = m_pSystem->ReadInputRegisterData();
	return ((wInputRegister & INREGMASK_MASHER_HILIMITSW) != 0) && (m_lastAction != masherActionLowering);
}

//
//	IsLowered()
//
//	Returns true if the masher is currently in the lowered position
//
bool Masher::IsLowered()
{
	word wInputRegister = m_pSystem->ReadInputRegisterData();
	return ((wInputRegister & INREGMASK_MASHER_LOLIMITSW) != 0) && (m_lastAction != masherActionRaising);
}

//
//	Test
//
//	Perform hardware tests
//
bool Masher::Test()
{
	while(true)
	{
		// Fetch mode
		if((m_pSystem->ReadInputRegisterData() & INREGMASK_ACTIONSWITCH) != 0)
		{
			if(!IsRaised())
			{
				// Raise the masher
				Raise();
			}
		
			while((m_pSystem->ReadInputRegisterData() & INREGMASK_ACTIONSWITCH) != 0);
		}
		else
		{
			if(!IsLowered())
			{
				// Lower the masher
				Lower();
			}
			
			while((m_pSystem->ReadInputRegisterData() & INREGMASK_ACTIONSWITCH) == 0);
		}		
	} // while
			
	return true;
}