//==============================================================================
// RegionData.cpp
//  : program file for the map
//
//------------------------------------------------------------------------------
//
//              Date: Mon Dec 29 04:28:26 2016
//
//==============================================================================

//------------------------------------------------------------------------------
//      Including Header Files
//------------------------------------------------------------------------------

#include "base/RegionData.h"

//------------------------------------------------------------------------------
//      Public functions
//------------------------------------------------------------------------------
//
//  RegionData::RegionData -- default constructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
RegionData::RegionData( void )
{
}

//
//  RegionData::RegionData -- copy constructor
//
//  Inputs
//  obj : object of this class
//
//  Outputs
//  none
//
RegionData::RegionData( const RegionData & obj )
{
}


//------------------------------------------------------------------------------
//      Destructor
//------------------------------------------------------------------------------
//
//  RegionData::~RegionData --    destructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
RegionData::~RegionData( void )
{
}

//------------------------------------------------------------------------------
//  Specific functions
//------------------------------------------------------------------------------
//
//  RegionData::init --    initialization
//
//  Inputs
//  pathname: name of the main path
//
//  Outputs
//  node
//
//void RegionData::setRegionData( Pathway *pathway )
//{
//}

//------------------------------------------------------------------------------
//      I/O
//------------------------------------------------------------------------------
//
//  operator << --      output
//
//  Inputs
//      stream  : output stream
//      obj     : object of this class
//
//  Outputs
//      output stream
//
ostream & operator << ( ostream & stream, const RegionData & obj )
{
	stream << "-- regiondata --" << endl;

    return stream;
}

//
//  operator >> --      input
//
//  Inputs
//      stream  : input stream
//      obj     : object of this class
//
//  Outputs
//      input stream
//
istream & operator >> ( istream & stream, RegionData & obj )
{
    // do nothing
    return stream;
}

// end of header file
// Do not add any stuff under this line.
