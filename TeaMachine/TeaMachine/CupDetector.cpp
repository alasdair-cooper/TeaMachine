//
//	CupDetector.cpp
//
//	Created: 04/11/2017
//	Author: Nick
//

#include "TeaMachineApp.h"
#include "CupDetector.h"
#include "System.h"
#include "NewPing.h"
#include "Display.h"

//
//	Default constructor
//
CupDetector::CupDetector()
{
} //CupDetector

//
//	Default constructor
//
CupDetector::CupDetector(TeaMachine *pTeaMachine) : TeaMachineComponent( pTeaMachine )
{
    ClearMembers();
} //CupDetector

//
//	Default destructor
//
CupDetector::~CupDetector()
{
    ClearMembers();
} //~CupDetector

//
//  ClearMembers
//
void CupDetector::ClearMembers()
{
    for( int nStation = 0; nStation <= STATIONS; nStation++ )
    {
        m_bStationStates[nStation] = false;
        m_StationEvents[nStation] = StationEventNoChange;
   }
}

//
//	Test
//
//	Perform hardware tests
//
bool CupDetector::Test()
{
    ClearMembers();
    Display testDisplay;
    testDisplay.Initialise();

	while( true )
	{
		Update();
		bool bCupFound = false;
        String strStationStates;
		for( int nStation = 1; nStation <= STATIONS; nStation++ )
		{
           if( m_bStationStates[nStation] )
            {
	            // Cup is present
	            bCupFound = true;
	            testDisplay.Set( nStation, nStation );
	            strStationStates = "X" + strStationStates;
            }
            else
            {
		        // Cup is not present
		        testDisplay.Set( nStation, BLANK );
		        strStationStates = "-" + strStationStates;
            }
		} // for
		testDisplay.Set( IndicatorLED_StatusGreen, bCupFound );
		testDisplay.Update();
		Serial.println(strStationStates);
		delay(1000);
	} // while
          
	return true;
}

//
//	Update
//
//	Scan all stations and store the results
//
void CupDetector::Update()
{
	// Order in which stations are scanned to minimise interference between detectors
	static int scanSequence[STATIONS] =	{ 1, 4, 7, 2, 5, 8, 3, 6, 9 };
		
    NewPing sonar(CUPDETECT_TRIGGER, CUPDETECT_ECHO, CUPDETECT_MAXDISTANCE);
        
    for( int nSequence = 0; nSequence < STATIONS; nSequence++ )
    {
		int nStation = scanSequence[nSequence];
        // Select range sensor
        m_pSystem->SetOutRegisterBitGroup( OUTREG_CUPDETECT_S0, nStation-1 );
            
        // Required delay between pings
        delay(PING_DELAY);
            
        // Send ping and obtain distance in cm
        unsigned int distance = sonar.ping() / US_ROUNDTRIP_CM;
            
        m_StationEvents[nStation] = StationEventNoChange;
        
        if( distance > 0 )
        {
            if( !m_bStationStates[nStation] )
            {
                // Cup is now present
                m_bStationStates[nStation] = true;
                m_StationEvents[nStation] = StationEventCupAdded;
            }
        }
        else
        {
            if( m_bStationStates[nStation] )
            {
                // Cup is no longer present
                m_bStationStates[nStation] = false;
                m_StationEvents[nStation] = StationEventCupRemoved;
            }
        }
    } // for
}

//
//	IsPresent
//
//	Determine whether or not a cup was detected at a given station during the last scan
//
bool CupDetector::IsPresent( int nStation )
{
    return m_bStationStates[ nStation ];
}

//
//	CheckCupIsPresent
//
//	Check station immediately and return true if cup is found
//
bool CupDetector::CheckCupIsPresent( int nStation )
{
    NewPing sonar(CUPDETECT_TRIGGER, CUPDETECT_ECHO, CUPDETECT_MAXDISTANCE);
    
    // Select range sensor
    m_pSystem->SetOutRegisterBitGroup( OUTREG_CUPDETECT_S0, nStation-1 );
    
    // Send ping and obtain distance in cm
    unsigned int distance = sonar.ping() / US_ROUNDTRIP_CM;
    
    m_bStationStates[nStation] = (distance > 0 );
    
    return m_bStationStates[nStation];
}

//
//  Initialise
//
//  Initialise the CupDetector
//
bool CupDetector::Initialise()
{
	pinMode(CUPDETECT_ECHO, INPUT);
	pinMode(CUPDETECT_TRIGGER, OUTPUT);
    
    ClearMembers();
        
    return true;
}

//
//  NotifyStationAdvance()
//
//  Called when the station positions advance by one (as a result
//  of the conveyor moving by one position). This allows the Cup Detector to
//	maintain correct station states and recognise subsequent station events correctly.
//
void CupDetector::NotifyStationAdvance()
{
    // Shift the station states by one position
    m_bStationStates[ST_COLLECTION_A] = m_bStationStates[ST_COLLECTION_B];
    m_bStationStates[ST_COLLECTION_B] = m_bStationStates[ST_COLLECTION_C];
    m_bStationStates[ST_COLLECTION_C] = m_bStationStates[ST_STIR];
    m_bStationStates[ST_STIR] = m_bStationStates[ST_MILK];
    m_bStationStates[ST_MILK] = m_bStationStates[ST_BREW];
    m_bStationStates[ST_BREW] = m_bStationStates[ST_RECEPTION_A];
    m_bStationStates[ST_RECEPTION_A] = m_bStationStates[ST_RECEPTION_B];
    m_bStationStates[ST_RECEPTION_B] = m_bStationStates[ST_RECEPTION_C];
    m_bStationStates[ST_RECEPTION_C] = false;
  
	// Clear station events
    for( int nStation = 0; nStation <= STATIONS; nStation++ )
    {
	    m_StationEvents[nStation] = StationEventNoChange;
    }
}
