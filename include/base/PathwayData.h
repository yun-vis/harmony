//==============================================================================
// PathwayData.h
//  : header file for the map
//
//==============================================================================

#ifndef _PATHWAYDATA_H        // begining of header file
#define _PATHWAYDATA_H        // notifying that this file is included

//----------------------------------------------------------------------
//  Including header files
//----------------------------------------------------------------------
#include "base/Pathway.h"

class PathwayData {
protected:
	
	Pathway *_pathwayPtr;

public:
	
	PathwayData();                              // default constructor
	PathwayData( const PathwayData &obj );      // Copy constructor
	virtual ~PathwayData();                     // Destructor

//------------------------------------------------------------------------------
//  Specific functions
//------------------------------------------------------------------------------
	void setPathwayData( Pathway *pathway );
	//, double &width, double &height );

//------------------------------------------------------------------------------
//      I/O
//------------------------------------------------------------------------------
	friend ostream &operator<<( ostream &stream, const PathwayData &obj );
	
	// Output
	friend istream &operator>>( istream &stream, PathwayData &obj );
	// Input
	
	virtual const char *className( void ) const { return "PathwayData"; }
	// Class name
};


#endif // _PATHWAYDATA_H

// end of header file
// Do not add any stuff under this line.
