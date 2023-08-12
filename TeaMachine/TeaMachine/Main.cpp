//
//	Main.cpp
//
#include <Arduino.h>
#include "TeaMachineApp.h"

TeaMachine gApplication;
bool g_bInitialised;	

void setup() 
{
	g_bInitialised = gApplication.Initialise();
}

void loop() 
{
	while(g_bInitialised)
	{
		gApplication.Process();
	}
}

