//
//	ATMegaApplicationBase.cpp
//
//	Created: 05/02/2017
//	Author: Nick
//	Base class for ATMega328 embedded applications
//

#include "ATMegaApplicationBase.h"
//#include "System.h"

//
//	Default constructor
//
ATMegaApplicationBase::ATMegaApplicationBase()
{
	m_pTheSystem = NULL;
}

//
//	Default destructor
//
ATMegaApplicationBase::~ATMegaApplicationBase()
{
	delete m_pTheSystem;
}

System *ATMegaApplicationBase::GetSystem()
{
	if( m_pTheSystem == NULL )
	{
		m_pTheSystem = new System();
	}
	
	return m_pTheSystem;
}
//
//	Initialise system
//
bool ATMegaApplicationBase::Initialise()
{
	return true;
}


