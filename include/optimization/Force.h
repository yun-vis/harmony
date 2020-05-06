//==============================================================================
// Force.h
//	: program file for networks
//
//------------------------------------------------------------------------------
//
//				Date: Sat Jul 30 23:07:36 2016
//
//==============================================================================

#ifndef _Force_H        // beginning of header file
#define _Force_H        // notifying that this file is included

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------
#include <ctime>

using namespace std;

#include <QImage>
#include "base/Polygon2.h"
#include "base/QuardTree.h"
#include "base/Config.h"
#include "optimization/EnergyBase.h"

//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Defining Classes
//------------------------------------------------------------------------------

class Force : public EnergyBase {

private:
	
	ForceGraph *_forceGraphPtr;
	
	QImage *_diagram;               // image for computing GPU base voronoi diagram
	QuardTree _quardTree;
	unsigned int _iteration;        // iteration of the simulation
	double _temperatureDecay;       // decay of the temperature (simulated annealing)
	
	// configuration parameter
	double _paramKa;               // attractive force
	double _paramKr;               // repulsive force
	double _paramKv;               // voronoi force
	double _paramKc;               // k1 force
	double _paramKd;               // k2 force
	double _paramKe;               // k3 force
	double _paramKo;               // overlap repulsive force
	unsigned int _paramForceLoop;        // maximum loop num
	double _paramDegreeOneMagnitude;   // force magnitude for degree one node
	
	double _paramRatioForce;
	double _paramRatioVoronoi;
	double _paramTransformationStep;
	double _paramCellRatio;
	double _paramDisplacementLimit;
	double _paramFinalEpsilon;
	double _paramThetaThreshold;
	double _paramMinTemperature;
	double _paramAlphaTemperature;
	bool _paramEnableTemperature;
	FORCETYPE _paramMode;             // TYPE_FORCE, TYPE_BARNES_HUT

protected:
	
	void _init( ForceGraph *__forceGraphPtr, Polygon2 *__contourPtr,
	            LEVELTYPE *__levelTypePtr, string __configFilePath );
	
	void _clear( void );
	
	void _random( void );
	
	void _preDisplacement( void );

	void _displacement( void );
	
	void _BarnesHut( void );
	
	void _centroidGeometry( void );
	
	double _gap( void );
	
	double _verletIntegreation( void );
	
	bool _inContour( Coord2 &coord );
	
	void _normalizeWeight();

public:

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------
	// default constructor
	Force();
	// copy constructor
	Force( const Force &obj );

//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------
	~Force();            // destructor

//------------------------------------------------------------------------------
//	Referencing to members
//------------------------------------------------------------------------------

	const FORCETYPE &mode( void ) const { return _paramMode; }
	
	FORCETYPE &mode( void ) { return _paramMode; }
	
	const double &finalEpsilon( void ) const { return _paramFinalEpsilon; }
	
	double &finalEpsilon( void ) { return _paramFinalEpsilon; }
	
	const QImage *diagram( void ) const { return _diagram; }
	
	QImage *diagram( void ) { return _diagram; }
	
	const QuardTree &quardTree( void ) const { return _quardTree; }
	
	QuardTree &quardTree( void ) { return _quardTree; }
	
	double gap( void ) { return _gap(); }
	
	double verletIntegreation( void ) { return _verletIntegreation(); }
	
	const unsigned int &paramForceLoop( void ) const { return _paramForceLoop; }
	
	unsigned int &paramForceLoop( void ) { return _paramForceLoop; }

//------------------------------------------------------------------------------
//	Fundamental functions
//------------------------------------------------------------------------------
	void init( ForceGraph *__forceGraphPtr, Polygon2 *__contourPtr,
	           LEVELTYPE *__levelTypePtr, string __configFilePath ) {
		_init( __forceGraphPtr, __contourPtr, __levelTypePtr, __configFilePath );
	}
	
	void _initSeed( void );
	
	void clear( void ) { _clear(); }
	
	void random( void ) { _random(); }

//------------------------------------------------------------------------------
//	Force functions
//------------------------------------------------------------------------------
	void preDisplacement( void ) { _preDisplacement(); }

	void displacement( void ) { _displacement(); }
	
	void centroidGeometry( void ) { _centroidGeometry(); }
	
	void BarnesHut( void ) { _BarnesHut(); }

//------------------------------------------------------------------------------
//	Specific functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Assignment operators
//------------------------------------------------------------------------------
	Force &operator=( const Force &obj );
	// assignment

//------------------------------------------------------------------------------
//	I/O functions
//------------------------------------------------------------------------------
	friend ostream &operator<<( ostream &stream, const Force &obj );
	
	// output
	friend istream &operator>>( istream &stream, Force &obj );
	// input

//------------------------------------------------------------------------------
//	Class name
//------------------------------------------------------------------------------
	virtual const char *className( void ) const { return "Force"; }
	// class name
};


#endif // _Force_H

// end of header file
// Do not add any stuff under this line.
