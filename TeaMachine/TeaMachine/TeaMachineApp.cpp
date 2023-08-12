//
//	TeaMachineApp.cpp
//
//	Created: 05/02/2017
//	Author: Nick
//

#include "TeaMachineApp.h"

#include "TeaMachineController.h"
#include "Extractor.h"
#include "BagConveyor.h"
#include "CupConveyor.h"
#include "Masher.h"
#include "boiler.h"
#include "CupLevelProbe.h"
#include "BrewManager.h"
#include "MilkManager.h"
#include "Display.h"
#include "CupDetector.h"
#include "UserControls.h"
#include "MilkDispenser.h"
#include "Stirrer.h"
#include "SugarDispenser.h"

//
//	Default constructor
//
TeaMachine::TeaMachine()
{
	m_nTestCode = SYSMODE_NORMAL;
}

//
//	Default destructor
//
TeaMachine::~TeaMachine()
{
}

//
//	Initialise system
//
bool TeaMachine::Initialise()
{
	Serial.begin(SERIAL_BAUDRATE);
	
	// Initialise system object
	GetSystem()->Initialise();

	// Fetch system mode
	m_nSystemMode = GetSystem()->ReadSystemMode();
	Serial.print("System mode: ");
	Serial.println(m_nSystemMode);
	
	if(m_nSystemMode == SYSMODE_NORMAL)
	{
		// Normal mode: Switch 'Normal' LED on and 'Test' LED off
		GetSystem()->ClearOutputRegisterBit(OUTREG_LED_TESTMODE);
	}
	else
	{
		// Switch 'Test' LED on and 'Normal' LED off
		GetSystem()->SetOutputRegisterBit(OUTREG_LED_TESTMODE);
	}
		
	// Switch off 'Fault' LED
	GetSystem()->SetOutputRegisterBit(OUTREG_LED_FAULT);
	
	return true;
}

//
//	Main process loop
//
void TeaMachine::Process()
{
 	TeaMachineController controller(this);
    
    // Clear LED displays
    Display *pDisplay = new Display(this);
	pDisplay->Initialise();
    delete pDisplay;
    
	switch( m_nSystemMode )
	{
		case SYSMODE_NORMAL:
			controller.Start(ControllerMode_InService);
			break;
		
		case SYSMODE_TESTGROUP1:
			ExecuteGroup1Tests();
			break;
		
		case SYSMODE_TESTGROUP2:
			ExecuteGroup2Tests();
			break;
		
		case SYSMODE_LOAD:
			controller.Start(ControllerMode_Load);
			break;
	} // switch
	
	// Wait until system mode changes
	while( !GetSystem()->SystemModeHasChanged() );
	
	// Wait a while for the new code to settle
	delay( 500 );
	
	m_nSystemMode = GetSystem()->ReadSystemMode();
}

//
//	ExecuteGroup1Tests
//
//	Run device tests (Group 1)
//
bool TeaMachine::ExecuteGroup1Tests()
{
	bool bResult = false;
	Boiler *pBoilerUnderTest = NULL;
	TeaMachineComponent *pComponentUnderTest = NULL;
	
	// Fetch test code
	m_nTestCode = GetSystem()->ReadInputRegisterData() & INREGMASK_MODE;
	
	Serial.print("Test code: ");
	Serial.println(m_nTestCode);
	
	// Display test information
	Display *pDisplay = new Display(this);
	pDisplay->Initialise();
	pDisplay->Set( IndicatorDrinkName1, 'T');
	pDisplay->Set( IndicatorDrinkName2, 'E');
	pDisplay->Set( IndicatorDrinkName3, 'S');
	pDisplay->Set( IndicatorDrinkName4, 'T');
	pDisplay->Set( IndicatorMilkLevel, 1 );
	pDisplay->Set( IndicatorSugarLevel, m_nTestCode );
	pDisplay->Update();
	delete pDisplay;
	
	// Initiate processing according to system operating mode
	switch(m_nTestCode)
	{
		case TESTCODE_EXTRACTOR:
		pComponentUnderTest = new Extractor(this);
		pComponentUnderTest->Initialise();
		bResult = pComponentUnderTest->Test();
		break;
		
		case TESTCODE_BAGCONVEYOR:
		pComponentUnderTest = new BagConveyor(this);
		pComponentUnderTest->Initialise();
		bResult = pComponentUnderTest->Test();
		break;
		
		case TESTCODE_MASHER:
		pComponentUnderTest = new Masher(this);
		pComponentUnderTest->Initialise();
		bResult = pComponentUnderTest->Test();
		break;
		
		case TESTCODE_BOILERCNTRL:
		pComponentUnderTest = new Boiler(this);
		pComponentUnderTest->Initialise();
		bResult = pComponentUnderTest->Test();
		break;
		
		case TESTCODE_WATERVALVE:
		pBoilerUnderTest = new Boiler(this);
		pBoilerUnderTest->Initialise();
		bResult = pBoilerUnderTest->TestExitValve();
		delete pBoilerUnderTest;
		break;
		
		case TESTCODE_CUPCONVEYOR:
		pComponentUnderTest = new CupConveyor(this);
		pComponentUnderTest->Initialise();
		bResult = pComponentUnderTest->Test();
		break;

		case TESTCODE_CUPLEVELARM:
		pComponentUnderTest = new CupLevelProbe(this);
		pComponentUnderTest->Initialise();
		bResult = pComponentUnderTest->Test();
		break;
		
		case TESTCODE_DISPLAY:
		pComponentUnderTest = new Display(this);
		pComponentUnderTest->Initialise();
		bResult = pComponentUnderTest->Test();
		break;
		
		case TESTCODE_CUPDETECTOR:
		pComponentUnderTest = new CupDetector(this);
		pComponentUnderTest->Initialise();
		bResult = pComponentUnderTest->Test();
		break;
		
		case TESTCODE_USERCONTROLS:
		pComponentUnderTest = new UserControls(this);
		pComponentUnderTest->Initialise();
		bResult = pComponentUnderTest->Test();
		break;
		
		case TESTCODE_MILKDISPENSER:
		pComponentUnderTest = new MilkDispenser(this);
		pComponentUnderTest->Initialise();
		bResult = pComponentUnderTest->Test();
		break;
		
		case TESTCODE_STIRRER:
		pComponentUnderTest = new Stirrer(this);
		pComponentUnderTest->Initialise();
		bResult = pComponentUnderTest->Test();
		break;
		
		case TESTCODE_SUGARDISPENSER:
		pComponentUnderTest = new SugarDispenser(this);
		pComponentUnderTest->Initialise();
		bResult = pComponentUnderTest->Test();
		break;
	} // switch
	
	if( pComponentUnderTest != NULL )
	{
		delete pComponentUnderTest;
	}

	return bResult;
}

//
//	ExecuteGroup2Tests
//
//	Run process tests (Group 2)
//
void TeaMachine::ExecuteGroup2Tests()
{
	TeaMachineController controller(this);
	
	// Fetch test code
	m_nTestCode = GetSystem()->ReadInputRegisterData() & INREGMASK_MODE;
	
	Serial.print("Test code: ");
	Serial.println(m_nTestCode);
	
	// Initiate processing according to system operating mode
	switch(m_nTestCode)
	{		
		case TESTCODE_INSERVICE_WITHLOGGING:
		controller.Start(ControllerMode_InServiceWithLogging);
		break;
		
		case TESTCODE_USERINTERFACE:
		controller.Start(ControllerMode_TestUserInterface);
		break;
		
		case TESTCODE_BREWPROCESS:
		controller.Start(ControllerMode_TestBrewManager);
		break;
		
		case TESTCODE_MILKPROCESS:
		controller.Start(ControllerMode_TestMilkManager);
		break;
		
		case TESTCODE_STIRPROCESS:
		controller.Start(ControllerMode_TestStirManager);
		break;
	} // switch
}