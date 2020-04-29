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

#include "base/Boundary.h"
#include "base/Config.h"
#include "graph/SkeletonGraph.h"
#include "optimization/Voronoi.h"

//------------------------------------------------------------------------------
//	Defining data types
//------------------------------------------------------------------------------

//----------------------------------------------------------------------
//	Defining macros
//----------------------------------------------------------------------

class EnergyBase : public Common {
private:
	
	string _workerName;    // name of the worker
	
	unsigned int _paramVoronoiFreq;
	double _paramRatioPosition;
	double _paramRatioVoronoi;

protected:
	
	Boundary *_boundary;
	
	vector< Seed > _seedVec;               // seeds of the voronoi diagram
	Voronoi _voronoi;               // geometric voronoi diagram
	Polygon2 _contour;               // boundary of voronoi diagram
	
	void _clear( void );

public:
	
	EnergyBase( void );                     // default constructor
	EnergyBase( const EnergyBase &obj );   // Copy constructor
	virtual ~EnergyBase();                  // Destructor

//------------------------------------------------------------------------------
//  Reference to members
//------------------------------------------------------------------------------
	const Boundary &boundary( void ) const { return *_boundary; }
	
	Boundary &boundary( void ) { return *_boundary; }
	
	const unsigned int &nVertices( void ) const { return _boundary->nVertices(); }
	
	const unsigned int &nEdges( void ) const { return _boundary->nEdges(); }
	
	const Polygon2 &contour( void ) const { return _contour; }
	
	Polygon2 &contour( void ) { return _contour; }
	
	const Voronoi &voronoi( void ) const { return _voronoi; }
	
	Voronoi &voronoi( void ) { return _voronoi; }
	
	const string &workerName( void ) const { return _workerName; }
	
	string &workerName( void ) { return _workerName; }

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
