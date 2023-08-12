//
//	CupDetector.h
//
//	Created: 04/11/2017
//	Author: Nick
//

#ifndef __CUPDETECTOR_H__
#define __CUPDETECTOR_H__

#include "TeaMachineComponent.h"

#define PING_DELAY		30	// Delay between pings in mS

typedef enum
{
    StationEventNoChange,
    StationEventCupAdded,
    StationEventCupRemoved    
}
StationEvent;

class CupDetector : public TeaMachineComponent
{
public:
	CupDetector();
	CupDetector(TeaMachine *pTeaMachine);
	~CupDetector();
    
    bool Initialise();                               // Initialise the cup detector
 	bool Test();					                 // Test the cup detector
   
    void Update();                                   // Scan all stations
    bool IsPresent( int nStation );                  // Returns true if cup was detected
    StationEvent GetChange( int nStation ) { return m_StationEvents[nStation]; }
    bool CheckCupIsPresent( int nStation );          // Check station immediately and return true if cup is found  

    void NotifyStationAdvance();                     // Called when stations advance by one position
	
private:
	CupDetector( const CupDetector &c );
	CupDetector& operator=( const CupDetector &c );
    
	// Initialise class members
    void ClearMembers();
    
    // Array indicating presence or absence of a cup at each station
    bool m_bStationStates[STATIONS+1];
    
    // Array indicating the addition or removal of a cup since the last scan
    StationEvent m_StationEvents[STATIONS+1];
};

#endif //__CUPDETECTOR_H__
