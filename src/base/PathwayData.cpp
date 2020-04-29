//==============================================================================
// PathwayData.cpp
//  : program file for the map
//
//------------------------------------------------------------------------------
//
//              Date: Mon Dec 29 04:28:26 2018
//
//==============================================================================

//------------------------------------------------------------------------------
//      Including Header Files
//------------------------------------------------------------------------------

#include "base/PathwayData.h"

//------------------------------------------------------------------------------
//      Public functions
//------------------------------------------------------------------------------
//
//  PathwayData::PathwayData -- default constructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
PathwayData::PathwayData( void )
{
}

//
//  PathwayData::PathwayData -- copy constructor
//
//  Inputs
//  obj : object of this class
//
//  Outputs
//  none
//
PathwayData::PathwayData( const PathwayData & obj )
{
}


//------------------------------------------------------------------------------
//      Destructor
//------------------------------------------------------------------------------
//
//  PathwayData::~PathwayData --    destructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
PathwayData::~PathwayData( void )
{
}

//------------------------------------------------------------------------------
//  Specific functions
//------------------------------------------------------------------------------
//
//  PathwayData::setPathwayData --    set pathway data
//
//  Inputs
//  pathway: pointer of the pathway
//
//  Outputs
//  node
//
void PathwayData::setPathwayData( Pathway *pathway, double &width, double &height )
{
    _pathwayPtr            = pathway;
    *_pathwayPtr->width()  = width;
    *_pathwayPtr->height() = height;
}

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
ostream & operator << ( ostream & stream, const PathwayData & obj )
{
	stream << "-- pathwaydata --" << endl;

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
istream & operator >> ( istream & stream, PathwayData & obj )
{
    // do nothing
    return stream;
}

// end of header file
// Do not add any stuff under this line.
