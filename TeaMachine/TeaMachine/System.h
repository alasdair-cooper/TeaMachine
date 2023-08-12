//
//	System.h
//
//	Created: 05/02/2017
//	Author: Nick
//	Utility class for hardware in the tea machine system
//

#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include "HardwareDefs.h"

class System
{
//	Member functions
public:
	System();
	~System();
	
	void Initialise();
	
	// I/O register access
	void LatchInputRegisterData();
	byte ReadInputRegisterByte();
	word ReadInputRegisterData();
    word ReadInputRegisterData( byte *pControlsLow, byte *pControlsHigh );
	void WriteOutputRegisterData(word wDataHigh, word wDataLow);
	void ClearOutputRegisterBit(unsigned int nBitNumber);
	void SetOutputRegisterBit(unsigned int nBitNumber);
    void SetOutRegisterBitGroup(unsigned int nBitGroupLSB, int newValue);
	int ReadSystemMode();
	bool SystemModeHasChanged();
	void WaitForActionSwitchChange();
	
private:
	System( const System &c );
	System& operator=( const System &c );

	word wOutputRegisterCurrentValueLow;
	word wOutputRegisterCurrentValueHigh;
	
	int m_nSystemMode;							// Last read system mode value
};

#endif
