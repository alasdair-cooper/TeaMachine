//
//	CupLevelProbe.cpp
//
//	Created: 24/02/2017
//	Author: Nick
//

#include "TeaMachineApp.h"
#include "CupLevelProbe.h"

//
//	Default constructor
//
CupLevelProbe::CupLevelProbe()
{
} //CupLevelProbe

//
//	Default constructor
//
CupLevelProbe::CupLevelProbe(TeaMachine *pTeaMachine) : TeaMachineComponent( pTeaMachine )
{
} //CupLevelProbe

//
//	Default constructor
//
CupLevelProbe::~CupLevelProbe()
{
} //~CupLevelProbe

//
//	Initialise
//
//	Initialise the device
//
bool CupLevelProbe::Initialise()
{
}

//
//	Test
//
//	Perform hardware tests
//
bool CupLevelProbe::Test()
{
	while(true)
	{
		// Fetch mode
		if((m_pSystem->ReadInputRegisterData() & INREGMASK_ACTIONSWITCH) != 0)
		{
			// Raise the probe arm
			Raise();
			
			while((m_pSystem->ReadInputRegisterData() & INREGMASK_ACTIONSWITCH) != 0);
		}
		else
		{
			// Lower the probe arm
			Lower();
			
			bool bIsFull = !CupIsFull();
			while((m_pSystem->ReadInputRegisterData() & INREGMASK_ACTIONSWITCH) == 0)
			{
				if(CupIsFull())
				{
					// Cup is full
					if(!bIsFull)
					{
						bIsFull = true;
						Serial.print("Cup is FULL\n");
					}
				}	
				else	
				{
					// Cup is not full
					if(bIsFull)
					{
						bIsFull = false;
						Serial.print("Cup is NOT FULL\n");
					}
				}		
			}
		}
        if(IsRaised())
        {
            Serial.print("Probe is RAISED\n");
        }
        else
        {
            Serial.print("Probe is LOWERED\n");
        }
        delay(1000);
	} // while
	
	return true;
}

//
//	Raise
//
//	Raise the probe arm
//
bool CupLevelProbe::Raise()
{
    m_pSystem->ClearOutputRegisterBit(OUTREG_CUPPROBE_UPDOWN);
    m_pSystem->SetOutputRegisterBit(OUTREG_CUPPROBE_EN);
	
	return true;
}

//
//	Lower
//
//	Lower the probe arm
//
bool CupLevelProbe::Lower()
{
	m_pSystem->SetOutputRegisterBit(OUTREG_CUPPROBE_UPDOWN);
	m_pSystem->SetOutputRegisterBit(OUTREG_CUPPROBE_EN);
	
	return true;
}

//
//	Stop
//
//	Stop the motor
//
void CupLevelProbe::Stop()
{
	m_pSystem->ClearOutputRegisterBit(OUTREG_CUPPROBE_EN);
}

//
// IsRaised
//
// Determine whether or not probe arm is raised
//
bool CupLevelProbe::IsRaised()
{
	word wInputRegister = m_pSystem->ReadInputRegisterData();
	return (wInputRegister & INREGMASK_CUPPROBE_RETRACTED) != 0;   
}    

//
//	CupIsFull
//
//	Returns true if the cup level float switch indicates a full cup
//
bool CupLevelProbe::CupIsFull()
{
	word wInputRegister = m_pSystem->ReadInputRegisterData();

	return ((wInputRegister & INREGMASK_CUPLEVEL_HILIMIT) != 0);
}

