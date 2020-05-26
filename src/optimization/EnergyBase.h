//==============================================================================
// EnergyBase.h
//  : header file for the optimization
//
//==============================================================================

#ifndef _EnergyBase_H        // begining of header file
#define _EnergyBase_H        // notifying that this file is included

//----------------------------------------------------------------------
//  Including header files
//----------------------------------------------------------------------

#include <iostream>
#include <iomanip>
#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/Dense>

using namespace std;

#include "core/Config.h"
#include "graph/SkeletonGraph.h"
#include "optimization/Octilinear.h"
#include "optimization/Voronoi.h"

//------------------------------------------------------------------------------
//	Defining data types
//------------------------------------------------------------------------------

//----------------------------------------------------------------------
//	Defining macros
//----------------------------------------------------------------------

class EnergyBase {

private:
	
	unsigned int _paramVoronoiFreq;
	double _paramRatioPosition;
	double _paramRatioVoronoi;

protected:
	
	unsigned int _id;
	LEVELTYPE *_levelTypePtr;               // force level
	string _configFilePath;                 // config file path
	Coord2 _boxCenter;                      // bounding box center of the simpleContour
	double _width, _height;                 // bounding box of the simpleContour
	
	vector< Seed > _seedVec;                // seeds of the voronoi diagram
	Voronoi _voronoi;                       // geometric voronoi diagram
	Polygon2 *_contourPtr;                  // boundary of voronoi diagram
	
	void _clear( void );

public:
	
	EnergyBase( void );                     // default constructor
	EnergyBase( const EnergyBase &obj );    // Copy constructor
	virtual ~EnergyBase();                  // Destructor

//------------------------------------------------------------------------------
//  Reference to members
//------------------------------------------------------------------------------

	const unsigned int &id( void ) const { return _id; }
	
	unsigned int &id( void ) { return _id; }
	
	const double &width( void ) const { return _width; }
	
	const double &height( void ) const { return _height; }
	
	const Polygon2 *contourPtr( void ) const { return _contourPtr; }
	
	Polygon2 *contourPtr( void ) { return _contourPtr; }
	
	const Voronoi &voronoi( void ) const { return _voronoi; }
	
	Voronoi &voronoi( void ) { return _voronoi; }

//------------------------------------------------------------------------------
//  Specific functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Initialization functions
//------------------------------------------------------------------------------
	virtual void _initSeed( void ) = 0;

//------------------------------------------------------------------------------
//  File I/O
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//      I/O
//------------------------------------------------------------------------------
	friend ostream &operator<<( ostream &stream, const EnergyBase &obj );
	
	// Output
	friend istream &operator>>( istream &stream, EnergyBase &obj );
	// Input
	
	virtual const char *className( void ) const { return "EnergyBase"; }
	// Class name
};

#endif // _EnergyBase_H

// end of header file
// Do not add any stuff under this line.
