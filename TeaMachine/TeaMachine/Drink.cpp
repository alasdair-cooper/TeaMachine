//
//	Drink.cpp
//
//	Created: 16/12/2017
//	Author: Nick
//

#include <string.h>
#include "Drink.h"

// Static default drink data
DrinkData Drink::defaultDrinkDefinitions[DRINK_TYPES] = {
	// Type     Name        Milk    Sugar   Strength
	{  Tea,     "TEA",      2,      0,      2 },
	{  Water,   "WAT",      0,      0,      0 }
};

//
//  Class constructor
//
Drink::Drink()
{
    m_nTypeIndex = 0;
    m_nDrinkID = 0;
    InitialiseDrink();
} //Drink

//
//  Class constructor
//
Drink::Drink( unsigned int nID )
{
    m_nTypeIndex = 0;
    m_nDrinkID = nID;
    InitialiseDrink();
} //Drink

//
//  Class destructor
//
Drink::~Drink()
{
} //~Drink

//
//  InitialiseDrink
//
//  Initialise members from the static initialisation array
//
void Drink::InitialiseDrink()
{
    m_Type = defaultDrinkDefinitions[m_nTypeIndex].type;
    strcpy(m_szName, defaultDrinkDefinitions[m_nTypeIndex].name);
    m_nSugar = defaultDrinkDefinitions[m_nTypeIndex].sugar;
    m_nMilk = defaultDrinkDefinitions[m_nTypeIndex].milk;
    m_nStrength = defaultDrinkDefinitions[m_nTypeIndex].strength;
    m_bCupIsPresent = true;
    m_bSelectionIsConfirmed = false;
    m_State = Empty;
}

//
//  SetNextType
//
//  Set the next type in the list of available drink types
//
void Drink::SetNextType()
{
    m_nTypeIndex = (m_nTypeIndex < DRINK_TYPES-1) ? m_nTypeIndex + 1 : m_nTypeIndex;
    InitialiseDrink();
}

//
//  SetPreviousType
//
//  Set the previous type in the list of available drink types
//
void Drink::SetPreviousType()
{
    m_nTypeIndex = (m_nTypeIndex > 0) ? m_nTypeIndex - 1 : 0;
    InitialiseDrink();
}