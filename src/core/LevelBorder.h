//==============================================================================
// LevelBorder.h
//  : header file for the LevelBorder network
//
//==============================================================================

#ifndef _LevelBorder_H        // beginning of header file
#define _LevelBorder_H        // notifying that this file is included

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

#include "core/Grid2.h"
#include "core/Contour2.h"
#include "core/RegionBase.h"
#include "core/LevelBase.h"
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

class LevelBorder : public LevelBase {

private:
	
	ForceGraph _skeletonForceGraph;
	RegionBase _regionBase;
	
protected:
	
	void _init(
			//double *__widthPtr, double *__heightPtr,
	            //double *__veCoveragePtr,
	            SkeletonGraph &__skeletonGraph );
	
	void _clear( void );
	
	void _normalizeSkeleton( void );
	
	void _normalizeRegionBase( void );
	
	void _decomposeSkeleton( void );

public:
	
	LevelBorder();                              // default constructor
	LevelBorder( const LevelBorder &obj );      // Copy constructor
	virtual ~LevelBorder();                     // Destructor

//------------------------------------------------------------------------------
//	Reference to members
//------------------------------------------------------------------------------
	const ForceGraph &skeletonForceGraph( void ) const { return _skeletonForceGraph; }
	
	ForceGraph &skeletonForceGraph( void ) { return _skeletonForceGraph; }
	
	const RegionBase &regionBase( void ) const { return _regionBase; }
	
	RegionBase &regionBase( void ) { return _regionBase; }
	
//------------------------------------------------------------------------------
//  Specific functions
//------------------------------------------------------------------------------
	void buildBoundaryGraph( void ) override;

	void updatePolygonComplex( void ) override;
	
	void prepareForce( map< unsigned int, Polygon2 > *polygonComplexPtr = NULL ) override;

//------------------------------------------------------------------------------
//  File I/O
//------------------------------------------------------------------------------
	void init( SkeletonGraph &__skeletonGraph ) {
		_init( __skeletonGraph );
	}
	
	void clear( void ) { _clear(); }

//------------------------------------------------------------------------------
//      I/O
//------------------------------------------------------------------------------
	friend ostream &operator<<( ostream &stream, const LevelBorder &obj );
	
	// Output
	friend istream &operator>>( istream &stream, LevelBorder &obj );
	// Input
	
	virtual const char *className( void ) const { return "LevelBorder"; }
	// Class name
};

#endif // _LevelBorder_H

// end of header file
// Do not add any stuff under this line.

