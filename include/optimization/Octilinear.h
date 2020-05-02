//==============================================================================
// Octilinear.h
//  : header file for the optimization
//
//==============================================================================

#ifndef _Octilinear_H        // begining of header file
#define _Octilinear_H        // notifying that this file is included

//----------------------------------------------------------------------
//  Including header files
//----------------------------------------------------------------------

#include <iostream>
#include <iomanip>
#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/Dense>

using namespace std;

#include "base/Config.h"
#include "graph/BoundaryGraph.h"

//------------------------------------------------------------------------------
//	Defining data types
//------------------------------------------------------------------------------
//#define OCTILINEAR_CONFLICT
//#define OCTILINEAR_BOUNDARY

//------------------------------------------------------------------------------
//	Defining macros
//------------------------------------------------------------------------------

class Octilinear {
private:
	
	OPTTYPE _opttype;
	
	BoundaryGraph _boundary;        // inner + outer boundary
	
	Eigen::VectorXd _var;           // x
	Eigen::VectorXd _output;        // b
	Eigen::MatrixXd _coef;          // A
	double _half_width;    // window_width
	double _half_height;   // window_height
	
	unsigned int _nVars;
	unsigned int _nConstrs;
	double _w_octilinear, _w_position, _w_fixposition;
	double _w_boundary, _w_crossing;
	//double _d_Alpha;
	double _d_Beta;
	vector< double > _theta;         // closest octilinear theta

protected:
	
	void _setVars( unsigned int &nRows );
	
	void _setConstraints( unsigned int &nRows );
	
	void _initCoefs( void );
	
	void _initVars( void );
	
	void _initOutputs( void );
	
	void _updateCoefs( void );
	
	void _updateOutputs( void );
	
	virtual void _init( double __width, double __height );
	
	void _setTargetAngle( void );
	
	void _updateEdgeCurAngle( void );
	
	bool _isOnLine( Coord2 &a, Coord2 &b, Coord2 &c );

public:
	
	Octilinear( void );                     // default constructor
	Octilinear( const Octilinear &obj ); // Copy constructor
	virtual ~Octilinear();            // Destructor

//------------------------------------------------------------------------------
//  Reference to members
//------------------------------------------------------------------------------
	const BoundaryGraph &boundary( void ) const { return _boundary; }
	
	BoundaryGraph &boundary( void ) { return _boundary; }

	const OPTTYPE &opttype( void ) const { return _opttype; }
	
	OPTTYPE &opttype( void ) { return _opttype; }

//------------------------------------------------------------------------------
//  Specific functions
//------------------------------------------------------------------------------
	double LeastSquare( unsigned int iter );
	
	double ConjugateGradient( unsigned int iter );

//------------------------------------------------------------------------------
//      Initialization functions
//------------------------------------------------------------------------------
	void prepare( double __half_width, double __half_height ) {
		
		_init( __half_width, __half_height );
	}

//------------------------------------------------------------------------------
//  File I/O
//------------------------------------------------------------------------------
	void prepare( void );
	
	void clear( void );
	
	void retrieve( void );

//------------------------------------------------------------------------------
//      I/O
//------------------------------------------------------------------------------
	friend ostream &operator<<( ostream &stream, const Octilinear &obj );
	
	// Output
	friend istream &operator>>( istream &stream, Octilinear &obj );
	// Input
	
	virtual const char *className( void ) const { return "Octilinear"; }
	// Class name
};

#endif // _Octilinear_H

// end of header file
// Do not add any stuff under this line.
