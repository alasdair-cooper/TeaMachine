//
//	Device.cpp
//
//	Created: 05/02/2017
//	Author: Nick
//	Utility class for hardware in the tea machine system
//

#include "ATMegaApplicationBase.h"
#include "System.h"

//
//	Default constructor
//
System::System()
{
	wOutputRegisterCurrentValueLow = 0;
	wOutputRegisterCurrentValueHigh = 0;
	m_nSystemMode = -1;
}

//
//	Default destructor
//
System::~System()
{
}

//
//	Initialise
//
void System::Initialise()
{
	// Initialise I/O ports
	pinMode(LED_BUILTIN, OUTPUT);
	pinMode(INPUTREG_CLOCK, OUTPUT);
	pinMode(INPUTREG_LATCH, OUTPUT);
	pinMode(INPUTREG_DATA, INPUT);
	pinMode(OUTPUTREG_CLOCK, OUTPUT);
	pinMode(OUTPUTREG_LATCH, OUTPUT);
	pinMode(OUTPUTREG_DATA, OUTPUT);
	pinMode(SYSMODE_BIT0, INPUT);
	pinMode(SYSMODE_BIT1, INPUT);
	pinMode(STIRRER_LOWERED, INPUT);
	pinMode(SUGAR_DONE, INPUT);
}

//
//	LatchInputRegisterData
//
//	Load current state of inputs into shift register ready for shifting
//
void System::LatchInputRegisterData()
{	
	// Set the latch pin high to read current state of inputs
	digitalWrite(INPUTREG_LATCH, HIGH);
	
	// Wait a bit
	delayMicroseconds(1);
	
	// Set it low to allow data to be shifted out
	digitalWrite(INPUTREG_LATCH, LOW);
}

//
//  ReadInputRegisterData
//
//	Extract data from shift register
//
word System::ReadInputRegisterData()
{
    LatchInputRegisterData();
    
    byte lowByteMain = ReadInputRegisterByte();
    byte hiByteMain = ReadInputRegisterByte();
    
    return lowByteMain | (hiByteMain << 8);
}

//
//  ReadInputRegisterData
//
//	Extract data from shift register including user control settings
//
word System::ReadInputRegisterData( byte *pControlsLow, byte *pControlsHigh )
{
    LatchInputRegisterData();
    
    byte lowByteMain = ReadInputRegisterByte();
    byte hiByteMain = ReadInputRegisterByte();
    *pControlsLow = ReadInputRegisterByte();
    *pControlsHigh = ReadInputRegisterByte();
    
    return lowByteMain | (hiByteMain << 8);
}

//
//  ReadInputRegisterByte
//
//	Extract byte from shift register
//
byte System::ReadInputRegisterByte()
{
	int nState = 0;;
	byte myDataIn = 0;

	for ( int nBit=7; nBit >= 0 ; nBit-- )
	{
		digitalWrite(INPUTREG_CLOCK, 0);
		delayMicroseconds(2);
		if(digitalRead(INPUTREG_DATA)) 
		{
			nState = 1;
			myDataIn = myDataIn | (1 << nBit);
		}
		else 
		{
			nState = 0;
		}

		digitalWrite(INPUTREG_CLOCK, 1);
	}

	return myDataIn;
}

//
//	WriteOutputRegisterData
//	
void System::WriteOutputRegisterData(word wHighData, word wLowData)
{
   digitalWrite(OUTPUTREG_LATCH, LOW);
   shiftOut(OUTPUTREG_DATA, OUTPUTREG_CLOCK, LSBFIRST, wHighData>>8);
   shiftOut(OUTPUTREG_DATA, OUTPUTREG_CLOCK, LSBFIRST, wHighData);
   shiftOut(OUTPUTREG_DATA, OUTPUTREG_CLOCK, LSBFIRST, wLowData>>8);
   shiftOut(OUTPUTREG_DATA, OUTPUTREG_CLOCK, LSBFIRST, wLowData);
   digitalWrite(OUTPUTREG_LATCH, HIGH);
   wOutputRegisterCurrentValueHigh = wHighData;
   wOutputRegisterCurrentValueLow = wLowData;
}

//
//  ClearOutputRegisterBit
//
//	Clear a specified bit in the output register
//
void System::ClearOutputRegisterBit(unsigned int nBitNumber)
{
    if( nBitNumber > 15 )
    {
        // Clear bit in high word
        wOutputRegisterCurrentValueHigh &= ((1 << (nBitNumber-16)) ^ 0xFFFF);
    }
    else
    {
        // Clear bit in low word
        wOutputRegisterCurrentValueLow &= ((1 << nBitNumber) ^ 0xFFFF);
    }

    WriteOutputRegisterData(wOutputRegisterCurrentValueHigh, wOutputRegisterCurrentValueLow);
}

//
//  SetOutputRegisterBit
//
//  Set a specified bit in the output register
//
void System::SetOutputRegisterBit(unsigned int nBitNumber)
{
    if( nBitNumber > 15 )
    {
        // Set bit in high word
        wOutputRegisterCurrentValueHigh |= 1 << (nBitNumber-16);
    }
    else
    {
        // Set bit in low word
        wOutputRegisterCurrentValueLow |= 1 << nBitNumber;
    }

    WriteOutputRegisterData(wOutputRegisterCurrentValueHigh, wOutputRegisterCurrentValueLow);
}

//
//  SetOutRegisterBitGroup
//
//  Set four bit output register value
//
void System::SetOutRegisterBitGroup(unsigned int nBitGroupLSB, int newValue)
{
    // Create mask for bit group and clear current setting
    word bitGroupMask = (0xF << nBitGroupLSB) ^ 0xFFFF;
    wOutputRegisterCurrentValueLow &= bitGroupMask;
    
    // Place the new setting in the bit group
    wOutputRegisterCurrentValueLow |= newValue << (nBitGroupLSB & 0xF);
    
    // Update the output register
    WriteOutputRegisterData(wOutputRegisterCurrentValueHigh, wOutputRegisterCurrentValueLow);
}

//
//  ReadSystemMode
//
//	Read the two-bit system mode
//
int System::ReadSystemMode()
{
	int nSysMode0 = digitalRead(SYSMODE_BIT0);
	int nSysMode1 = digitalRead(SYSMODE_BIT1);
	
	m_nSystemMode = nSysMode0 + (nSysMode1 * 2);
	
	return m_nSystemMode;
}

//
//  SystemModeHasChanged
//
//	Read the two-bit system mode to see if it has changed
//
bool System::SystemModeHasChanged()
{
	int nSysMode0 = digitalRead(SYSMODE_BIT0);
	int nSysMode1 = digitalRead(SYSMODE_BIT1);
	
	return ((nSysMode0 + (nSysMode1 * 2)) != m_nSystemMode);
}

//
//  WaitForActionSwitchChange
//
//	Wait for the state of the Action switch to change
//
void System::WaitForActionSwitchChange()
{
	// Read the current state
	word wCurrentState = ReadInputRegisterData() & INREGMASK_ACTIONSWITCH;
	
	// Wait for it to change
	while((ReadInputRegisterData() & INREGMASK_ACTIONSWITCH) == wCurrentState);
}
