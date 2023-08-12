//
//	Extractor.cpp
//
//	Created: 08/02/2017
//	Author: Nick
//

#include "TeaMachineApp.h"
#include "Extractor.h"

//
//	Default constructor
//
Extractor::Extractor()
{
} //Extractor

//
//	Default constructor
//
Extractor::Extractor(TeaMachine *pTeaMachine) : TeaMachineComponent( pTeaMachine )
{
} //Extractor

//
//	Default destructor
//
Extractor::~Extractor()
{
} //~Extractor

//
//	Raise()
//
//	Raise the extractor
//
bool Extractor::Raise()
{
	m_pSystem->SetOutputRegisterBit(OUTREG_EXTRACTOR_UPDOWN);
	m_pSystem->SetOutputRegisterBit(OUTREG_EXTRACTOR_EN);
	
	return true;
}

//
//	Lower()
//
//	Lower the extractor
//
bool Extractor::Lower()
{
    m_pSystem->ClearOutputRegisterBit(OUTREG_EXTRACTOR_UPDOWN);
    m_pSystem->SetOutputRegisterBit(OUTREG_EXTRACTOR_EN);
    
    return true;
}

//
//	Stop()
//
//	Switch off the extractor motor
//
void Extractor::Stop()
{
    m_pSystem->ClearOutputRegisterBit(OUTREG_EXTRACTOR_EN);
}

//
//	IsRaised()
//
//	Returns true if the extractor is currently in the raised position
//
bool Extractor::IsRaised()
{
	word wInputRegister = m_pSystem->ReadInputRegisterData();
	return (wInputRegister & INREGMASK_EXTRACTOR_HILIMITSW) != 0;
}

//
//	IsLowered()
//
//	Returns true if the extractor is currently in the lowered position
//
bool Extractor::IsLowered()
{
	word wInputRegister = m_pSystem->ReadInputRegisterData();
	return (wInputRegister & INREGMASK_EXTRACTOR_LOLIMITSW) != 0;
}

//
//	Test
//
//	Perform hardware tests
//
bool Extractor::Test()
{
	while(true)
	{
		// Fetch action switch setting
		if((m_pSystem->ReadInputRegisterData() & INREGMASK_ACTIONSWITCH) != 0)
		{
			/*
if(IsLowered())
{
	Serial.println("Extractor is LOWERED");
}
else
{
	Serial.println("Extractor is NOT LOWERED");
}
delay(1000);
*/
			if(!IsRaised())
			{
				// Raise the extractor
				Raise();
			}
			
			//while((m_pSystem->ReadInputRegisterData() & INREGMASK_ACTIONSWITCH) != 0);
		}
		else
		{
			/*
if(IsLowered())
{
	Serial.println("Extractor is LOWERED");
}
else
{
	Serial.println("Extractor is NOT LOWERED");
}
delay(1000);
*/
			
			if(!IsLowered())
			{
				// Lower the extractor
				Lower();
			}
			
			//while((m_pSystem->ReadInputRegisterData() & INREGMASK_ACTIONSWITCH) == 0);
		}
	} // while
	
	return true;
}
/*
//
//	Test
//
//	Perform hardware tests
//
bool Extractor::Test()
{
	bool bLowering = false;
	
	Serial.println("Test started");
	
	// Check initial state
	word wInputRegister = m_pSystem->ReadInputRegisterData();
	if((wInputRegister & INREGMASK_EXTRACTOR_HILIMITSW) == 0)
	{
		Serial.println("Raising extractor to initial position");
		// Initially, take extractor to the high limit
		m_pSystem->SetOutputRegisterBit(OUTREG_EXTRACTOR_UPDOWN);
		m_pSystem->SetOutputRegisterBit(OUTREG_EXTRACTOR_EN);
	}
	
	// Wait for a limit switch to be hit (i.e. wait for raise to complete)
	wInputRegister = m_pSystem->ReadInputRegisterData();
	while((wInputRegister & INREGMASK_EXTRACTOR_HILIMITSW) == 0)
	{
		wInputRegister = m_pSystem->ReadInputRegisterData();
		delay(100);
	}
	Serial.println("Extractor is in raised position");

	long lStartTime = 0;
	long lRaiseDelay = 0;
	for( int cycle = 0; cycle < 5; cycle++ )
	{
		wInputRegister = m_pSystem->ReadInputRegisterData();
		if( ((wInputRegister & INREGMASK_EXTRACTOR_HILIMITSW) != 0) && !bLowering )
		{
			// Extractor has been raised and is at upper limit, so attempt to lower it
			m_pSystem->ClearOutputRegisterBit(OUTREG_EXTRACTOR_UPDOWN);
			m_pSystem->SetOutputRegisterBit(OUTREG_EXTRACTOR_EN);
			bLowering = true;
			Serial.println("Commanded extractor to LOWER");
		}
		else
		{
			if( ((wInputRegister & INREGMASK_EXTRACTOR_LOLIMITSW) != 0) && bLowering )
			{
				// Extractor has been lowered and is at lower limit, so attempt to raise it
				m_pSystem->SetOutputRegisterBit(OUTREG_EXTRACTOR_UPDOWN);
				m_pSystem->SetOutputRegisterBit(OUTREG_EXTRACTOR_EN);
				bLowering = false;
				Serial.println("Commanded extractor to RAISE");
				
				// Remember start time for this raise
				lStartTime = millis();
			}
		}
		
		// Wait for a one of the limit switches to be hit
		bool bHitUpperLimit = false;
		bool bHitLowerLimit = false;
		if( bLowering)
		{
			Serial.println("Lowering - Waiting to hit limit");
		}
		else
		{
			Serial.println("Raising - Waiting to hit limit");
		}
		do
		{
			// Check limit switches
			wInputRegister = m_pSystem->ReadInputRegisterData();
			bHitUpperLimit = ((wInputRegister & INREGMASK_EXTRACTOR_HILIMITSW) != 0);
			bHitLowerLimit = ((wInputRegister & INREGMASK_EXTRACTOR_LOLIMITSW) != 0);
		}
		while( (bLowering && !bHitLowerLimit) || (!bLowering && !bHitUpperLimit));
		
		// Report limit switch states
		if( bHitLowerLimit )
		{
			Serial.println("Lowering - Lower limit HIT");
		}
		if( bHitUpperLimit )
		{
			Serial.println("Raising - Upper limit HIT");
			if(lRaiseDelay == 0)
			{
				// Remember time needed to fully raise extractor
				lRaiseDelay = millis() - lStartTime;
				Serial.print(lRaiseDelay);
				Serial.println("ms required to fully raise extractor");
			}
		}
			
		Serial.print("End of full cycle ");
		Serial.println(cycle);
	} // for

	if(lRaiseDelay > 0)
	{
		delay(2000);
	
		// Raise extractor to high cup position
		Serial.println("Positioning extractor at bag removal point");
		m_pSystem->SetOutputRegisterBit(OUTREG_EXTRACTOR_UPDOWN);
		m_pSystem->SetOutputRegisterBit(OUTREG_EXTRACTOR_EN);
		Serial.println("Commanded extractor to RAISE");
		long lEndTime = millis() + (lRaiseDelay/2);
		
		// Wait for extractor to reach desired  position
		while(millis() < lEndTime);
		
		// Switch off motor
		m_pSystem->ClearOutputRegisterBit(OUTREG_EXTRACTOR_EN);
		Serial.println("Extractor is positioned at bag removal point");
	}
	
	// Wait a bit
	delay(3000);
	
	// Take extractor to high position
	wInputRegister = m_pSystem->ReadInputRegisterData();
	if((wInputRegister & INREGMASK_EXTRACTOR_HILIMITSW) == 0)
	{
		m_pSystem->SetOutputRegisterBit(OUTREG_EXTRACTOR_UPDOWN);
		m_pSystem->SetOutputRegisterBit(OUTREG_EXTRACTOR_EN);
	}

	// Wait forever
	while(true);
			
	return true;
}

*/