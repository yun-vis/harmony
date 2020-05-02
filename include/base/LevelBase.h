//==============================================================================
// LevelBase.h
//  : header file for the LevelBase network
//
//==============================================================================

#ifndef _LevelBase_H        // beginning of header file
#define _LevelBase_H        // notifying that this file is included

//----------------------------------------------------------------------
//  Including header files
//----------------------------------------------------------------------

#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <sstream>
#include <cassert>
#include <cstring>
#include <cmath>

using namespace std;

#include "base/Grid2.h"
#include "base/RegionBase.h"
#include "graph/SkeletonGraph.h"
#include "optimization/Force.h"
#include "base/Contour2.h"
#include "graph/ForceGraph.h"
#include "graph/BoundaryGraph.h"
#include "optimization/Octilinear.h"
#include "optimization/Stress.h"

//------------------------------------------------------------------------------
//	Defining data types
//------------------------------------------------------------------------------


//----------------------------------------------------------------------
//	Defining macros
//----------------------------------------------------------------------

class LevelBase {

private:

protected:
	
	double *_content_widthPtr;
	double *_content_heightPtr;
	double *_veCoveragePtr;
	double *_veRatioPtr;
	LEVELTYPE _levelType;
	
	// boundary of the composite graph
	vector< Octilinear * > _octilinearBoundaryVec;
	
	void _init( void );
	
	void _clear( void );

public:
	
	LevelBase();                              // default constructor
	LevelBase( const LevelBase &obj );      // Copy constructor
	virtual ~LevelBase();                     // Destructor

//------------------------------------------------------------------------------
//	Reference to members
//------------------------------------------------------------------------------
	const vector< Octilinear * > &octilinearBoundaryVec( void ) const { return _octilinearBoundaryVec; }
	
	vector< Octilinear * > &octilinearBoundaryVec( void ) { return _octilinearBoundaryVec; }

//------------------------------------------------------------------------------
//  Specific functions
//------------------------------------------------------------------------------
	virtual void buildBoundaryGraph( void ) = 0;
	
	virtual void updatePolygonComplex( void ) = 0;
	
	void prepare( void ){
		buildBoundaryGraph();
	}

//------------------------------------------------------------------------------
//  File I/O
//------------------------------------------------------------------------------
	void init( void ) { _init(); }
	
	void clear( void ) { _clear(); }

//------------------------------------------------------------------------------
//      I/O
//------------------------------------------------------------------------------
	friend ostream &operator<<( ostream &stream, const LevelBase &obj );
	
	// Output
	friend istream &operator>>( istream &stream, LevelBase &obj );
	// Input
	
	virtual const char *className( void ) const { return "LevelBase"; }
	// Class name
};

#endif // _LevelBase_H

// end of header file
// Do not add any stuff under this line.

