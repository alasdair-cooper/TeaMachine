//
//	Display.cpp
//
//	Created: 24/02/2017
//	Author: Nick
//

#include "TeaMachineApp.h"
#include "Display.h"
#include "System.h"

//
//	Default constructor
//
Display::Display()
{
} //Display

//
//	Default constructor
//
Display::Display(TeaMachine *pTeaMachine) : TeaMachineComponent( pTeaMachine )
{
    ClearMembers();
} //Display

//
//	Default destructor
//
Display::~Display()
{
    ClearMembers();
} //~Display

//
//  ClearMembers
//
void Display::ClearMembers()
{
    m_nCupNumber = BLANK;
    m_nMilkLevel = BLANK;
    m_nSugarLevel = BLANK;
    m_nStrengthLevel = BLANK;
    memset(m_szDrinkName, ' ', sizeof(m_szDrinkName) );
    m_bGoButtonLight = false;
    m_bStopButtonLight = false;
    m_bStatusGreen = false;
    m_bStatusRed = false;
    m_bNeedsUpdating = true;
	m_bFlashingIndicatorsAreOn = false;
	
    for( int nStation = 0; nStation <= STATIONS; nStation++ )
    {
	    m_nStationIndicator[nStation] = BLANK;
	    m_bFlashState[nStation] = false;
    } // for
}

//
//	Test
//
//	Perform hardware tests
//
bool Display::Test()
{
    char szTestSequence[] = "ACDEFIKLNORSTW";
    byte userControlsLower = 0;
    byte userControlsUpper = 0;
    m_bGoButtonLight = true;
    m_bStopButtonLight = true;
    m_bStatusGreen = true;
    m_bStatusRed = false;
    
    while(true)
    {
        for( m_nCupNumber = 0; m_nCupNumber < 10; m_nCupNumber++ )
        {
            m_szDrinkName[0] = szTestSequence[m_nCupNumber];
            m_szDrinkName[1] = szTestSequence[(m_nCupNumber+1) % strlen(szTestSequence)];
            m_szDrinkName[2] = szTestSequence[(m_nCupNumber+2) % strlen(szTestSequence)];
            m_szDrinkName[3] = szTestSequence[(m_nCupNumber+3) % strlen(szTestSequence)];
            m_nMilkLevel = (10 - m_nCupNumber) % 10;
            m_nSugarLevel = (m_nCupNumber + 1) % 10;
            m_nStrengthLevel = (9 - m_nCupNumber) % 10;
            m_nStationIndicator[9] = (8 - m_nCupNumber) % 10;
            m_nStationIndicator[8] = (2 + m_nCupNumber) % 10;
            m_nStationIndicator[7] = (7 - m_nCupNumber) % 10;
            m_nStationIndicator[6] = (3 + m_nCupNumber) % 10;
            m_nStationIndicator[5] = (6 - m_nCupNumber) % 10;
            m_nStationIndicator[4] = (4 + m_nCupNumber) % 10;
            m_nStationIndicator[3] = (5 - m_nCupNumber) % 10;
            m_nStationIndicator[2] = (5 + m_nCupNumber) % 10;
            m_nStationIndicator[1] = (4 - m_nCupNumber) % 10;
            m_bGoButtonLight = !m_bGoButtonLight;
            m_bStopButtonLight = !m_bStopButtonLight;
            m_bStatusGreen = !m_bStatusGreen;
            m_bStatusRed = !m_bStatusRed;
            
            WriteDisplayData();
            delay(1000);
            m_pSystem->ReadInputRegisterData( &userControlsLower, &userControlsUpper );
            
            Serial.print(userControlsUpper, HEX);
            Serial.print(",");
            Serial.print(userControlsLower, HEX);
            Serial.print("\n");
            
            if( (userControlsLower & 0x01)==0 )
            {
                Serial.println( "Cup UP" );
            }
            if( (userControlsLower & 0x02)==0 )
            {
                Serial.println( "Cup DOWN" );
            }
            if( !(userControlsLower & 0x04) )
            {
                Serial.println( "Milk UP" );
            }
            if( !(userControlsLower & 0x08) )
            {
                Serial.println( "Milk DOWN" );
            }
            if( !(userControlsLower & 0x10) )
            {
                Serial.println( "Sugar UP" );
            }
            if( !(userControlsLower & 0x20) )
            {
                Serial.println( "Sugar DOWN" );
            }
            if( !(userControlsLower & 0x40) )
            {
                Serial.println( "Strength UP" );
            }
            if( !(userControlsLower & 0x80) )
            {
                Serial.println( "Strength DOWN" );
            }
            if( !(userControlsUpper & 0x01) )
            {
                Serial.println( "Drink UP" );
            }
            if( !(userControlsUpper & 0x02) )
            {
                Serial.println( "Drink DOWN" );
            }
            if( !(userControlsUpper & 0x04) )
            {
                Serial.println( "GO" );
            }
            if( !(userControlsUpper & 0x08) )
            {
                Serial.println( "CANCEL" );
            }
         } // for            
    }     
    	
	return true;
}

//
//	Set
//
//	Set a station indicator
//
void Display::Set( int nStation, int nValue, bool bFlash )
{
    m_nStationIndicator[nStation] = nValue;
	m_bFlashState[nStation] = bFlash;
    m_bNeedsUpdating = true;
}

//
//	Set
//
//	Set a numeric indicator
//
void Display::Set( IndicatorNumeric indicator, int nValue )
{
    switch( indicator )
    {
        case IndicatorCupNumber:
        m_nCupNumber = nValue;
        break;
        
        case IndicatorSugarLevel:
        m_nSugarLevel = nValue;
        break;
        
        case IndicatorMilkLevel:
        m_nMilkLevel = nValue;
        break;
        
        case IndicatorStrengthLevel:
        m_nStrengthLevel = nValue;
        break;
    } // switch
    
    m_bNeedsUpdating = true;
}

//
//	Set
//
//	Set an alphanumeric indicator
//
void Display::Set( IndicatorAlpha indicator, char cValue )
{
    switch( indicator )
    {
        case IndicatorDrinkName1:
        m_szDrinkName[0] = cValue;
        break;
        
        case IndicatorDrinkName2:
        m_szDrinkName[1] = cValue;
        break;
        
        case IndicatorDrinkName3:
        m_szDrinkName[2] = cValue;
        break;
        
        case IndicatorDrinkName4:
        m_szDrinkName[3] = cValue;
        break;
    } // switch
    
    m_bNeedsUpdating = true;
}

//
//  Set the state of an LED indicator
//
void Display::Set( IndicatorLED indicator, bool bState )
{
    m_bNeedsUpdating = true;
    
    switch( indicator )
    {
        case IndicatorLED_StatusGreen:
        m_bStatusGreen = bState;
        m_bStatusRed = false;
        break;
        
        case IndicatorLED_StatusRed:
        m_bStatusRed = bState;
        m_bStatusGreen = false;
        break;
        
        case IndicatorLED_GoButton:
        m_bGoButtonLight = bState;
        break;
        
        case IndicatorLED_StopButton:
        m_bStopButtonLight = bState;
        break;
        
        default:
        m_bNeedsUpdating = false;
        break;        
    } // switch
}

//
//  WriteDisplayData
//
//  Shift out display data
//
void Display::WriteDisplayData()
{
   digitalWrite(DISPLAYREG_LATCH, LOW);
   shiftOut(DISPLAYREG_DATA, DISPLAYREG_CLOCK, MSBFIRST, GetSegmentCode(m_nStationIndicator[1], m_bFlashState[1]));
   shiftOut(DISPLAYREG_DATA, DISPLAYREG_CLOCK, MSBFIRST, GetSegmentCode(m_nStationIndicator[2], m_bFlashState[2]));
   shiftOut(DISPLAYREG_DATA, DISPLAYREG_CLOCK, MSBFIRST, GetSegmentCode(m_nStationIndicator[3], m_bFlashState[3]));
   shiftOut(DISPLAYREG_DATA, DISPLAYREG_CLOCK, MSBFIRST, GetSegmentCode(m_nStationIndicator[4], m_bFlashState[4]));
   shiftOut(DISPLAYREG_DATA, DISPLAYREG_CLOCK, MSBFIRST, GetSegmentCode(m_nStationIndicator[5], m_bFlashState[5]));
   shiftOut(DISPLAYREG_DATA, DISPLAYREG_CLOCK, MSBFIRST, GetSegmentCode(m_nStationIndicator[6], m_bFlashState[6]));
   shiftOut(DISPLAYREG_DATA, DISPLAYREG_CLOCK, MSBFIRST, GetSegmentCode(m_nStationIndicator[7], m_bFlashState[7]));
   shiftOut(DISPLAYREG_DATA, DISPLAYREG_CLOCK, MSBFIRST, GetSegmentCode(m_nStationIndicator[8], m_bFlashState[8]));
   shiftOut(DISPLAYREG_DATA, DISPLAYREG_CLOCK, MSBFIRST, GetSegmentCode(m_nStationIndicator[9], m_bFlashState[9]));
   shiftOut(DISPLAYREG_DATA, DISPLAYREG_CLOCK, MSBFIRST, GetSegmentCode(m_nStrengthLevel, false, m_bStatusGreen));
   shiftOut(DISPLAYREG_DATA, DISPLAYREG_CLOCK, MSBFIRST, GetSegmentCode(m_nSugarLevel, false, m_bStatusRed));
   shiftOut(DISPLAYREG_DATA, DISPLAYREG_CLOCK, MSBFIRST, GetSegmentCode(m_nMilkLevel, false, !m_bGoButtonLight));
   shiftOut(DISPLAYREG_DATA, DISPLAYREG_CLOCK, MSBFIRST, GetSegmentCode(m_szDrinkName[3], AlphaLow));
   shiftOut(DISPLAYREG_DATA, DISPLAYREG_CLOCK, MSBFIRST, GetSegmentCode(m_szDrinkName[3], AlphaHigh));
   shiftOut(DISPLAYREG_DATA, DISPLAYREG_CLOCK, MSBFIRST, GetSegmentCode(m_szDrinkName[2], AlphaLow));
   shiftOut(DISPLAYREG_DATA, DISPLAYREG_CLOCK, MSBFIRST, GetSegmentCode(m_szDrinkName[2], AlphaHigh));
   shiftOut(DISPLAYREG_DATA, DISPLAYREG_CLOCK, MSBFIRST, GetSegmentCode(m_szDrinkName[1], AlphaLow));
   shiftOut(DISPLAYREG_DATA, DISPLAYREG_CLOCK, MSBFIRST, GetSegmentCode(m_szDrinkName[1], AlphaHigh));
   shiftOut(DISPLAYREG_DATA, DISPLAYREG_CLOCK, MSBFIRST, GetSegmentCode(m_szDrinkName[0], AlphaLow));
   shiftOut(DISPLAYREG_DATA, DISPLAYREG_CLOCK, MSBFIRST, GetSegmentCode(m_szDrinkName[0], AlphaHigh));
   shiftOut(DISPLAYREG_DATA, DISPLAYREG_CLOCK, MSBFIRST, GetSegmentCode(m_nCupNumber, false, !m_bStopButtonLight));
   digitalWrite(DISPLAYREG_LATCH, HIGH);
   m_bNeedsUpdating = false;
}

//
//  Initialise
//
//  Initialise the display
//
bool Display::Initialise()
{
	pinMode(DISPLAYREG_CLOCK, OUTPUT);
	pinMode(DISPLAYREG_LATCH, OUTPUT);
	pinMode(DISPLAYREG_DATA, OUTPUT);
    
    ClearMembers();
    
    Update();
    
    return true;
}

//
//  GetSegmentCode
//
//  Returns a binary representation of the segments requiring illumination for a given digit.
//  LSB is set according to LED indicator on/off value.
//
word Display::GetSegmentCode( unsigned int nValue, bool bFlashing, bool bLamp )
{
    word underlyingSegmentCode = GetSegmentCode(nValue, bFlashing);
    
    return bLamp ? (underlyingSegmentCode | 1) : (underlyingSegmentCode & 0xfe);
}

//
//  GetSegmentCode
//
//  Returns a binary representation of the segments requiring illumination for a given digit
//
word Display::GetSegmentCode( unsigned int nValue, bool bFlashing )
{
    word segmentCode = SEGMENTCODE_0;
    switch( nValue )
    {
        case BLANK:
        segmentCode = SEGMENTCODE_BLANK;
        break;
        
        case 0:
        segmentCode = SEGMENTCODE_0;
        break;
        
        case 1:
        segmentCode = SEGMENTCODE_1;
        break;
        
        case 2:
        segmentCode = SEGMENTCODE_2;
        break;
        
        case 3:
        segmentCode = SEGMENTCODE_3;
        break;
        
        case 4:
        segmentCode = SEGMENTCODE_4;
        break;
        
        case 5:
        segmentCode = SEGMENTCODE_5;
        break;
        
        case 6:
        segmentCode = SEGMENTCODE_6;
        break;
        
        case 7:
        segmentCode = SEGMENTCODE_7;
        break;
        
        case 8:
        segmentCode = SEGMENTCODE_8;
        break;
        
        case 9:
        segmentCode = SEGMENTCODE_9;
        break;
        
        case 10:
        segmentCode = SEGMENTCODE_A;
        break;
        
        case 11:
        segmentCode = SEGMENTCODE_B;
        break;
        
        case 12:
        segmentCode = SEGMENTCODE_C;
        break;
        
        case 13:
        segmentCode = SEGMENTCODE_D;
        break;
        
        case 14:
        segmentCode = SEGMENTCODE_E;
        break;
        
        case 15:
        segmentCode = SEGMENTCODE_F;
        break;
        
        case ERROR:
        segmentCode = SEGMENTCODE_ERROR;    // Displays 'E'
        break;
                
        case UNEXPECTED:
        segmentCode = SEGMENTCODE_UNEXPECTED;    // Displays '-'
        break;
    } // switch
    
    return bFlashing ? (m_bFlashingIndicatorsAreOn ? segmentCode : BLANK) : segmentCode;
}

//
//  GetSegmentCode
//
//  Returns a binary representation of the segments requiring illumination for a given character
//
word Display::GetSegmentCode( char cValue, AlphaSegmentCodePortion portion )
{
    word segmentCode = SEGMENTCODE_BLANK;
    
    switch( cValue )
    {
        case 'A':
        segmentCode = (portion == AlphaHigh) ? SEGMENTCODEHI_A : SEGMENTCODELO_A;
        break;
        
        case 'C':
        segmentCode = (portion == AlphaHigh) ? SEGMENTCODEHI_C : SEGMENTCODELO_C;
        break;
        
        case 'D':
        segmentCode = (portion == AlphaHigh) ? SEGMENTCODEHI_D : SEGMENTCODELO_D;
        break;
        
        case 'E':
        segmentCode = (portion == AlphaHigh) ? SEGMENTCODEHI_E : SEGMENTCODELO_E;
        break;
        
        case 'F':
        segmentCode = (portion == AlphaHigh) ? SEGMENTCODEHI_F : SEGMENTCODELO_F;
        break;
        
        case 'I':
        segmentCode = (portion == AlphaHigh) ? SEGMENTCODEHI_I : SEGMENTCODELO_I;
        break;
        
        case 'K':
        segmentCode = (portion == AlphaHigh) ? SEGMENTCODEHI_K : SEGMENTCODELO_K;
        break;
        
        case 'L':
        segmentCode = (portion == AlphaHigh) ? SEGMENTCODEHI_L : SEGMENTCODELO_L;
        break;
        
        case 'N':
        segmentCode = (portion == AlphaHigh) ? SEGMENTCODEHI_N : SEGMENTCODELO_N;
        break;
        
        case 'O':
        segmentCode = (portion == AlphaHigh) ? SEGMENTCODEHI_O : SEGMENTCODELO_O;
        break;
        
        case 'R':
        segmentCode = (portion == AlphaHigh) ? SEGMENTCODEHI_R : SEGMENTCODELO_R;
        break;
        
        case 'S':
        segmentCode = (portion == AlphaHigh) ? SEGMENTCODEHI_S : SEGMENTCODELO_S;
        break;
        
        case 'T':
        segmentCode = (portion == AlphaHigh) ? SEGMENTCODEHI_T : SEGMENTCODELO_T;
        break;
        
        case 'W':
        segmentCode = (portion == AlphaHigh) ? SEGMENTCODEHI_W : SEGMENTCODELO_W;
        break;
        
        case ' ':
        segmentCode = (portion == AlphaHigh) ? SEGMENTCODE_BLANK : SEGMENTCODE_BLANK;
        break;
    }

    return segmentCode;
}

//
//  Update
//
//  Send current set of display values to the display output register
//
void Display::Update()
{
    if( m_bNeedsUpdating )
    {
        WriteDisplayData();
        m_bNeedsUpdating = false;
    }
}

//
//  Clear
//
//  Clear the display
//
void Display::Clear()
{
	ClearMembers();
}

//
//  DoFlash
//
//  Flash indicators on or off as appropriate
//  Return time that next flash action is required
//
unsigned long Display::DoFlash()
{
	// Determine whether flashing digits should be on or off
	m_bFlashingIndicatorsAreOn = !m_bFlashingIndicatorsAreOn;
	
	// Show or blank the digit as appropriate
	m_bNeedsUpdating = true;
	Update();
	
	// Return time that next flash action is required
	return millis() + (m_bFlashingIndicatorsAreOn ? FLASH_INTERVAL_ON : FLASH_INTERVAL_OFF);
}
