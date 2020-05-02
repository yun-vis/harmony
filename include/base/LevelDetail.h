//==============================================================================
// LevelBorder.h
//  : header file for the LevelBorder network
//
//==============================================================================

#ifndef _LevelDetail_H        // beginning of header file
#define _LevelDetail_H        // notifying that this file is included

//----------------------------------------------------------------------
//  Including header files
//----------------------------------------------------------------------

#include <iostream>
#include <iomanip>
#include <string>
#include <tinyxml.h>
#include <fstream>
#include <sstream>
#include <cassert>
#include <cstring>
#include <cmath>

using namespace std;

#include "base/Grid2.h"
#include "base/Contour2.h"
#include "base/RegionBase.h"
#include "base/LevelBase.h"
#include "graph/SkeletonGraph.h"
#include "graph/ForceGraph.h"
#include "graph/BoundaryGraph.h"
#include "optimization/Force.h"
#include "optimization/Stress.h"
#include "optimization/Octilinear.h"

//------------------------------------------------------------------------------
//	Defining data types
//------------------------------------------------------------------------------

//----------------------------------------------------------------------
//	Defining macros
//----------------------------------------------------------------------

class LevelDetail : public LevelBase {

private:

protected:
	
	void _init( double *__widthPtr, double *__heightPtr, double *__veCoveragePtr );
	
	void _clear( void );
	
public:
	
	LevelDetail();                              // default constructor
	LevelDetail( const LevelDetail &obj );      // Copy constructor
	virtual ~LevelDetail();                     // Destructor

//------------------------------------------------------------------------------
//	Reference to members
//------------------------------------------------------------------------------
	//const RegionBase &regionBase( void ) const { return _regionBase; }
	
	//RegionBase &regionBase( void ) { return _regionBase; }
	
//------------------------------------------------------------------------------
//  Specific functions
//------------------------------------------------------------------------------
	void buildBoundaryGraph( void ) override;

	void updatePolygonComplex( void ) override;
	
	void prepare( void );
	
//------------------------------------------------------------------------------
//  File I/O
//------------------------------------------------------------------------------
	void init( double *__widthPtr, double *__heightPtr, double *__veCoveragePtr ) {
		_init( __widthPtr, __heightPtr, __veCoveragePtr );
	}
	
	void clear( void ) { _clear(); }

//------------------------------------------------------------------------------
//      I/O
//------------------------------------------------------------------------------
	friend ostream &operator<<( ostream &stream, const LevelDetail &obj );
	
	// Output
	friend istream &operator>>( istream &stream, LevelDetail &obj );
	// Input
	
	virtual const char *className( void ) const { return "LevelDetail"; }
	// Class name
};

#endif // _LevelDetail_H

// end of header file
// Do not add any stuff under this line.
