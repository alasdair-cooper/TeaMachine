//
//	Display.h
//
//	Created: 24/02/2017
//	Author: Nick
//

#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include "TeaMachineComponent.h"

#define FLASH_INTERVAL_ON		700			// Time on during a flash (mS)
#define FLASH_INTERVAL_OFF		350			// Time off during a flash (mS)

typedef enum
{    
    // Numeric displays
    IndicatorCupNumber,
    IndicatorMilkLevel,
    IndicatorSugarLevel,
    IndicatorStrengthLevel,
}
IndicatorNumeric;

typedef enum
{
    // Alphanumeric displays
    IndicatorDrinkName1,
    IndicatorDrinkName2,
    IndicatorDrinkName3,
    IndicatorDrinkName4
}
IndicatorAlpha;

typedef enum
{
    // Alphanumeric bit map portion
    AlphaHigh,
    AlphaLow
}
AlphaSegmentCodePortion;

typedef enum
{
    IndicatorLED_StatusGreen,
    IndicatorLED_StatusRed,
    IndicatorLED_GoButton,
    IndicatorLED_StopButton
} 
IndicatorLED;

class Display : public TeaMachineComponent
{
public:
	Display();
	Display(TeaMachine *pTeaMachine);
	~Display();
    
    bool Initialise();                               // Initialise the display
	bool Test();					                 // Test the display
    
	void Set( int nStation, int nValue, bool bFlash = false );  // Set the cup number in a station indicator
	void Set( IndicatorNumeric indicator, int nValue );	 // Set a numeric indicator
	void Set( IndicatorAlpha indicator, char cValue );    // Set an alphanumeric indicator
    void Set( IndicatorLED, bool bState );           // Switch an LED indicator on or off
    
    void Clear();                                    // Clear the display 
    void Update();						             // Update the display
   
    unsigned long DoFlash();							 // Flash indicators on or off as required
 		
private:
	Display( const Display &c );
	Display& operator=( const Display &c );
    
    void ClearMembers();
    void WriteDisplayData();                            // Shift out display data
    
    word GetSegmentCode( unsigned int nValue, bool bFlashing = false );     // Returns bit map for a given digit
    word GetSegmentCode( char cValue, AlphaSegmentCodePortion portion );    // Returns bit map for a given character
    word GetSegmentCode( unsigned int nValue, bool bFlashing, bool bLamp ); // Returns bit map for a given digit combined with LED on/off bit
   	
    unsigned int m_nCupNumber;                          // Currently displayed cup number
    unsigned int m_nMilkLevel;                          // Currently displayed milk level
    unsigned int m_nSugarLevel;                         // Currently displayed sugar level
    unsigned int m_nStrengthLevel;                      // Currently displayed strength level
    char    m_szDrinkName[DISPLAY_DRINKNAMELENGTH+1];   // Currently displayed drink name
    unsigned int m_nStationIndicator[STATIONS+1];
    bool    m_bGoButtonLight;                           // State of GO button light
    bool    m_bStopButtonLight;                         // State of STOP button light
    bool    m_bStatusRed;                               // Red status LED
    bool    m_bStatusGreen;                             // Green status LED
    bool    m_bNeedsUpdating;                           // True if display is out of date
	bool	m_bFlashState[STATIONS+1];					// True if corresponding station indicator is flashing
	bool	m_bFlashingIndicatorsAreOn;					// True if flashing digits are showing
}; //Display

#endif //__DISPLAY_H__
