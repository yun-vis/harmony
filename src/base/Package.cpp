//==============================================================================
// Package.cc
//  : program file for the region data
//
//------------------------------------------------------------------------------
//
//              Date: Thu Feb 21 04:28:26 2019
//
//==============================================================================

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------

#include "base/Package.h"


//------------------------------------------------------------------------------
//	Protected functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------
//
//  Package::init -- initialize
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Package::_init( void )
{
}

//
//  Package::clear --    clear the current Package information
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Package::_clear( void )
{
    // Bone::_clear();
}


//
//  Package::Package -- default constructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
Package::Package( void )
{
    _smoothPtr = new Smooth;
    _octilinearVecPtr = new vector< Octilinear >;
}

//
//  Package::Package -- copy constructor
//
//  Inputs
//  obj : object of this class
//
//  Outputs
//  none
//
Package::Package( const Package & obj )
{
}


//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------
//
//  Package::~Package --    destructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
Package::~Package( void )
{
}


// end of header file
// Do not add any stuff under this line.
