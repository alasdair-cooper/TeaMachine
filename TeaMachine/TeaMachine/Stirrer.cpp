//
//	Stirrer.cpp
//
//	Created: 02/03/2017
//	Author: Nick
//

#include "TeaMachineApp.h"
#include "Stirrer.h"

//
//	Default constructor
//
Stirrer::Stirrer()
{
} //Stirrer

//
//	Default constructor
//
Stirrer::Stirrer(TeaMachine *pTeaMachine) : TeaMachineComponent( pTeaMachine )
{
} //Stirrer

//
//	Default destructor
//
Stirrer::~Stirrer()
{
} //~Stirrer

//
//	Raise()
//
//	Raise the Stirrer
//
bool Stirrer::Raise()
{
	m_pSystem->ClearOutputRegisterBit(OUTREG_STIRRER_UPDOWN);
	m_pSystem->SetOutputRegisterBit(OUTREG_STIRRER_EN);
    
	return true;
}

//
//	Lower()
//
//	Lower the Stirrer
//
bool Stirrer::Lower()
{
	m_pSystem->SetOutputRegisterBit(OUTREG_STIRRER_UPDOWN);
	m_pSystem->SetOutputRegisterBit(OUTREG_STIRRER_EN);
    
	return true;
}

//
//	Stop()
//
//	Switch off the Stirrer motor
//
void Stirrer::Stop()
{
    m_pSystem->ClearOutputRegisterBit(OUTREG_STIRRER_EN);
}

//
//	IsRaised()
//
//	Returns true if the Stirrer is currently in the raised position
//
bool Stirrer::IsRaised()
{
	word wInputRegister = m_pSystem->ReadInputRegisterData();
	return (wInputRegister & INREGMASK_STIRRER_HILIMITSW) != 0;
}

//
//	IsLowered()
//
//	Returns true if the Stirrer is currently in the lowered position
//
bool Stirrer::IsLowered()
{
	return digitalRead(STIRRER_LOWERED);
}

//
//	Test
//
//	Perform hardware tests
//
bool Stirrer::Test()
{
	while(true)
	{
		// Fetch mode
		if((m_pSystem->ReadInputRegisterData() & INREGMASK_ACTIONSWITCH) != 0)
		{
			if(IsLowered())
			{
				Serial.print("RAISE command\n");
				
				// Raise the Stirrer
				Raise();
			}
		
			while((m_pSystem->ReadInputRegisterData() & INREGMASK_ACTIONSWITCH) != 0);
		}
		else
		{
			if(IsRaised())
			{
				Serial.print("LOWER command\n");
				
				// Lower the Stirrer
				Lower();
			}
			
			while((m_pSystem->ReadInputRegisterData() & INREGMASK_ACTIONSWITCH) == 0);
		}		
	} // while
			
	return true;
}