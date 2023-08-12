//
//	TeaMachineComponent.cpp
//
//	Created: 12/05/2017
//	Author: Nick
//

#include "TeaMachineApp.h"
#include "System.h"
#include "TeaMachineComponent.h"

//
//	Class constructor
//
TeaMachineComponent::TeaMachineComponent()
{
	m_pTeaMachine = NULL;
	m_pSystem = NULL;
} //TeaMachineComponent

//
//	Class constructor
//
TeaMachineComponent::TeaMachineComponent(TeaMachine *pTeaMachine)
{
	m_pTeaMachine = pTeaMachine;
	m_pSystem = pTeaMachine->GetSystem();
} //TeaMachineComponent

//
//	Class destructor
//
TeaMachineComponent::~TeaMachineComponent()
{
} //~TeaMachineComponent

//
//	Initialise()
//
//	Initialise an element of the tea machine
//
bool TeaMachineComponent::Initialise()
{
	return true;
} //~TeaMachineComponent

//
//	Test()
//
//	Test an element of the tea machine
//
bool TeaMachineComponent::Test()
{
	return true;
}
