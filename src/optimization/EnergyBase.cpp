// EnergyBase.cpp
//  : program file for the boundary network
//
//------------------------------------------------------------------------------
//
//              Date: Mon Dec 10 04:28:26 2012
//
//==============================================================================

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------

#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <cstring>
#include <cmath>

using namespace std;

#include "optimization/EnergyBase.h"

//------------------------------------------------------------------------------
//	Protected functions
//------------------------------------------------------------------------------
//
//  EnergyBase::clear --        memory management
//
//  Inputs
//      none
//
//  Outputs
//      none
//
void EnergyBase::_clear( void )
{
}

//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------

//
//  EnergyBase::EnergyBase -- default constructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
EnergyBase::EnergyBase( void )
{
    _boundary                   = new Boundary;
}

//
//  EnergyBase::EnergyBase -- copy constructor
//
//  Inputs
//  obj : object of this class
//
//  Outputs
//  none
//
EnergyBase::EnergyBase( const EnergyBase & obj )
{
}


//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------

//
//  EnergyBase::~EnergyBase --    destructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
EnergyBase::~EnergyBase( void )
{
}

// end of header file
// Do not add any stuff under this line.

