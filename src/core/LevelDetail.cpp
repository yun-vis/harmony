//==============================================================================
// LevelDetail.cc
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

#include "core/LevelDetail.h"

//------------------------------------------------------------------------------
//	Private functions
//------------------------------------------------------------------------------
//
//  LevelDetail::_clear --    clear the current LevelDetail information
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void LevelDetail::_clear( void ) {
}

//------------------------------------------------------------------------------
//	Protected functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------
//
//  LevelDetail::LevelDetail -- default constructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
LevelDetail::LevelDetail( void ) {


}

//
//  LevelDetail::LevelDetail -- copy constructor
//
//  Inputs
//  obj : object of this class
//
//  Outputs
//  none
//
LevelDetail::LevelDetail( const LevelDetail &obj ) {
}


//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------
//
//  LevelDetail::LevelDetail --    destructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
LevelDetail::~LevelDetail( void ) {
}

void LevelDetail::buildBoundaryGraph( void ) {


}

void LevelDetail::updatePolygonComplex( void ) {


}

void LevelDetail::prepareForce( map< unsigned int, Polygon2 > *polygonComplexPtr ) {
}

// end of header file
// Do not add any stuff under this line.
