//==============================================================================
// Stress.h
//  : header file for the optimization
//
//==============================================================================

#ifndef _Stress_H        // begining of header file
#define _Stress_H        // notifying that this file is included

//----------------------------------------------------------------------
//  Including header files
//----------------------------------------------------------------------

#include <iostream>
#include <iomanip>
#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/Dense>

using namespace std;

#include "core/Config.h"
#include "graph/ForceGraph.h"
#include "optimization/EnergyBase.h"

//------------------------------------------------------------------------------
//	Defining data types
//------------------------------------------------------------------------------
//#define STRESS_CONFLICT
#define STRESS_BOUNDARY

//----------------------------------------------------------------------
//	Defining macros
//----------------------------------------------------------------------

class Stress : public EnergyBase {
private:
	
	BoundaryGraph _boundaryGraph;
	
	Eigen::VectorXd _var;           // x
	Eigen::VectorXd _output;        // b
	Eigen::MatrixXd _coef;          // A
	double _half_width;    // window_width
	double _half_height;   // window_height
	
	unsigned int _nVars;
	unsigned int _nConstrs;
	double _w_angle, _w_position;
	double _w_contextlength;
	double _w_boundary, _w_crossing;
	double _w_labelangle;
	double _d_Alpha;           // focus edge length
	// double          _d_Beta;         // context region length
	
	OPTTYPE _paramOptType;      // Least square or Conjugate Gradient
	unsigned int _paramVoronoiFreq;
	double _paramRatioPosition;
	double _paramRatioVoronoi;
	
	// Conjugate Gradient
	Eigen::MatrixXd _A;
	Eigen::VectorXd _b, _Ap;
	Eigen::VectorXd _err, _p;

protected:
	
	void _setVars( unsigned int &nRows );
	
	void _setConstraints( unsigned int &nRows );
	
	void _initVars( void );
	
	void _initCoefs( void );
	
	void _initOutputs( void );
	
	void _computeVoronoi( void );
	
	void _initStressSeed( void );
	
	void _updateCoefs( void );
	
	void _updateOutputs( void );
	
	void _initStress( ForceGraph *__forceGraphPtr,
	                  Polygon2 *__contourPtr );
	
	void _clear( void );
	
	void _initSeed( void ) { ; }

public:
	
	Stress( void );               // default constructor
	Stress( const Stress &obj ); // Copy constructor
	virtual ~Stress();            // Destructor

//------------------------------------------------------------------------------
//  Reference to members
//------------------------------------------------------------------------------
	const OPTTYPE &opttype( void ) const { return _paramOptType; }
	
	OPTTYPE &opttype( void ) { return _paramOptType; }
	
	const BoundaryGraph &boundaryGraph( void ) const { return _boundaryGraph; }
	
	BoundaryGraph &boundaryGraph( void ) { return _boundaryGraph; }
	
//------------------------------------------------------------------------------
//  Specific functions
//------------------------------------------------------------------------------
	double LeastSquare( unsigned int iter );
	
	void initConjugateGradient( void );
	
	double ConjugateGradient( unsigned int &iter );
	
	void run( void );

//------------------------------------------------------------------------------
//      Initialization functions
//------------------------------------------------------------------------------
	void prepare( ForceGraph *__forceGraphPtr, Polygon2 *__contourPtr ) {
		_clear();
		_initStress( __forceGraphPtr, __contourPtr );
	}
	
	void clear( void ) {
		_clear();
	}

//------------------------------------------------------------------------------
//  File I/O
//------------------------------------------------------------------------------
	void prepare( void );
	
	void retrieve( void );

//------------------------------------------------------------------------------
//      I/O
//------------------------------------------------------------------------------
	friend ostream &operator<<( ostream &stream, const Stress &obj );
	
	// Output
	friend istream &operator>>( istream &stream, Stress &obj );
	// Input
	
	virtual const char *className( void ) const { return "Stress"; }
	// Class name
};

#endif // _Stress_H

// end of header file
// Do not add any stuff under this line.
