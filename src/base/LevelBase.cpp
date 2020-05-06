//==============================================================================
// LevelBase.cc
//  : program file for the metro network
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

#include "base/LevelBase.h"

//------------------------------------------------------------------------------
//	Private functions
//------------------------------------------------------------------------------
//  LevelBase::_init -- initialization
//
//  Inputs
//      string
//
//  Outputs
//  double
//
//
void LevelBase::_init( void ) {
	
	_content_widthPtr   = NULL;
	_content_heightPtr  = NULL;
	//_veCoveragePtr      = NULL;
}


//
//  LevelBase::_clear --    clear the current LevelBase information
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void LevelBase::_clear( void ) {
    _octilinearBoundaryVec.clear();
}

//------------------------------------------------------------------------------
//	Protected functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------
//
//  LevelBase::LevelBase -- default constructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
LevelBase::LevelBase( void ) {

    _octilinearBoundaryVec.clear();
}

//
//  LevelBase::LevelBase -- copy constructor
//
//  Inputs
//  obj : object of this class
//
//  Outputs
//  none
//
LevelBase::LevelBase( const LevelBase &obj ) {
}


//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------
//
//  LevelBase::LevelBase --    destructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
LevelBase::~LevelBase( void ) {
}

// end of header file
// Do not add any stuff under this line.
