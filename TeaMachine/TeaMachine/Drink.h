//
//	Drink.h
//
//	Created: 16/12/2017
//	Author: Nick
//

#ifndef __DRINK_H__
#define __DRINK_H__

#include "HardwareDefs.h"

#define MAX_SUGAR       3       // Maximum sugar level
#define MAX_MILK        3       // Maximum milk level
#define MAX_STRENGTH    3       // Maximum strength level]

#define DRINK_TYPES     2       // Number of different drink types

typedef enum
{
    Tea,
    Water
}
DrinkType;

typedef enum
{
    Empty,
    BrewStageComplete,
    MilkStageComplete,
    ReadyToDrink,
    Invalid             // Cup placed in incorrect position
}
DrinkState;

// Structure used to store initial values for new drinks
typedef struct
{
    DrinkType           type;
    char                name[DISPLAY_DRINKNAMELENGTH+1];
    unsigned int        milk;
    unsigned int        sugar;
    unsigned int        strength;
} DrinkData;

class Drink
{
private:
    DrinkType   m_Type;                     // Drink type
    DrinkState  m_State;                    // Process state of drink
    char        m_szName[DISPLAY_DRINKNAMELENGTH+1]; // Name of drink
    int         m_nMilk;                    // Milk level
    int         m_nSugar;                   // Number of sugars
    int         m_nStrength;                // Strength level
    
    unsigned int m_nDrinkID;                // Drink identifier
    
    bool        m_bSelectionIsConfirmed;    // Initially false, true once user confirms required drink
    bool        m_bCupIsPresent;            // True if cup is present on the conveyor

    int         m_nTypeIndex;               // Index of drink in static drink array
        
public:
	Drink();
    Drink( unsigned int nDrinkID );
	~Drink();
    
    DrinkType   GetType() { return m_Type; }
    char        *GetName() { return m_szName; }
    void        SetNextType();
    void        SetPreviousType();
    
    int         GetMilk() { return m_nMilk; }
    void        IncrementMilk() { m_nMilk = (m_nMilk == MAX_MILK) ? MAX_MILK : m_nMilk + 1; }
    void        DecrementMilk()  { m_nMilk = (m_nMilk == 0) ? 0 : m_nMilk - 1; }
    
    int         GetSugar() { return m_nSugar; }
    void        IncrementSugar() { m_nSugar = (m_nSugar == MAX_SUGAR) ? MAX_SUGAR : m_nSugar + 1; }
    void        DecrementSugar()  { m_nSugar = (m_nSugar == 0) ? 0 : m_nSugar - 1; }
    
    int         GetStrength() { return m_nStrength; }
    void        IncrementStrength() { m_nStrength = (m_nStrength == MAX_STRENGTH) ? MAX_STRENGTH : m_nStrength + 1; }
    void        DecrementStrength()  { m_nStrength = (m_nStrength == 0) ? 0 : m_nStrength - 1; }
    
    bool        CupIsPresent() { return m_bCupIsPresent; }
    void        SetSelectionConfirmed( bool bConfirmed ) { m_bSelectionIsConfirmed = bConfirmed; }
    bool        SelectionIsConfirmed() { return m_bSelectionIsConfirmed; }

    int         GetID() { return m_nDrinkID; }
    
    DrinkState  GetState() { return m_State; }
    void        SetState( DrinkState state ){ m_State = state; }
            
private:
	Drink( const Drink &c );
	Drink& operator=( const Drink &c );
    
    void InitialiseDrink();
	
	static DrinkData defaultDrinkDefinitions[DRINK_TYPES];			// Static array of default drink definitions

}; //Drink

#endif //__DRINK_H__
