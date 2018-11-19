//==============================================================================
// Force.h
//	: program file for networks
//
//------------------------------------------------------------------------------
//
//				Date: Sat Jul 30 23:07:36 2016
//
//==============================================================================

#ifndef _Force_H		// beginning of header file
#define _Force_H		// notifying that this file is included

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------

#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Boolean_set_operations_2.h>
#include <CGAL/bounding_box.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Polygon_2_algorithms.h>
#include <CGAL/centroid.h>
#include <CGAL/double.h>

#include <boost/lexical_cast.hpp>

#include "graph/ForceGraph.h"
#include "base/Config.h"
#include "base/Polygon2.h"

//------------------------------------------------------------------------------
//	Defining data types
//------------------------------------------------------------------------------
typedef CGAL::Exact_predicates_exact_constructions_kernel K;
typedef CGAL::Polygon_2< K >::Vertex_circulator                      Vertex_circulator;


//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------
#ifndef	DIVISOR
#define DIVISOR		(29)
#endif	// DIVISOR

#ifndef UNIT_STEP
#define UNIT_STEP	(2)
#endif	// UNIT_STEP

#ifndef MAX_LIMIT_STEP
#define MAX_LIMIT_STEP	(90)    //origin->40
#endif	// MAX_LIMIT_STEP

#ifndef STANDARD_LIMIT_STEP
#define STANDARD_LIMIT_STEP	(40)
#endif	// STARNDARD_LIMIT_STEP

#ifndef MIN_LIMIT_STEP
#define MIN_LIMIT_STEP	(10)
#endif	// MIN_LIMIT_STEP

#define	MIN_RATIO	(0.0)

//------------------------------------------------------------------------------
//	Defining Classes
//------------------------------------------------------------------------------

class Force {

  private:

    Coord2          _center;
    Polygon2        _contour;
    ForceGraph     *_graphPtr;

    // parameter configuration
    double          _paramKa;
    double          _paramKr;
    double          _paramRatioForce;
    double          _paramRatioVoronoi;
    double          _paramTransformationStep;
    double          _paramCellRatio;
    double          _paramDisplacementLimit;
    double          _paramFinalEpsilon;

  protected:

    unsigned int	_step;
    FORCETYPE       _mode;  // TYPE_FORCE, TYPE_CENTROID, TYPE_HYBRID
    int			    _width;
    int			    _height;

    // center coordinates
    double		    _center_x, _center_y;

    void		_init	( ForceGraph * __graph, int __width, int __height );
    void		_reset	( void );
    void		_random	( void );

    void		_force		( void );
    void		_centroid	( void );
    double		_gap		( void );

    bool        _inContour( Coord2 &coord );

public:

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------
    Force();			// default constructor
    Force( const Force & obj );
				// copy constructor

//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------
    ~Force();			// destructor

//------------------------------------------------------------------------------
//	Referencing to members
//------------------------------------------------------------------------------
    const unsigned int &	step  ( void )	const	{ return _step; }
    unsigned int &		    step  ( void )		    { return _step; }

    const int &			    width ( void )	const	{ return _width; }
    int &			        width ( void )		    { return _width; }
    const int &			    height( void )	const	{ return _height; }
    int &			        height( void )		    { return _height; }

    const Coord2 &		    center( void )	const	{ return _center; }
    Coord2 &			    center( void )		    { return _center; }

    const Polygon2 &		contour( void )	const	{ return _contour; }
    Polygon2 &			    contour( void )		    { return _contour; }

    const FORCETYPE &		mode( void )	const	{ return _mode; }
    FORCETYPE &			    mode( void )		    { return _mode; }

    const double &			finalEpsilon ( void )	const	{ return _paramFinalEpsilon; }
    double &			    finalEpsilon ( void )		    { return _paramFinalEpsilon; }

    double	gap	( void )	{ return _gap(); }

//------------------------------------------------------------------------------
//	Fundamental functions
//------------------------------------------------------------------------------
    void init( ForceGraph * __graph, int __width, int __height ) { _init( __graph, __width, __height ); }
    void reset( void )				{ _reset(); }
    void random( void )				{ _random(); }
    void setContour( Polygon2 &p );

//------------------------------------------------------------------------------
//	Misc. functions
//------------------------------------------------------------------------------
    void	force	( void )		{ _force(); }
    void	centroid( void )		{ _centroid(); }

//------------------------------------------------------------------------------
//	static functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Assignment operators
//------------------------------------------------------------------------------
    Force & operator = ( const Force & obj );
				// assignment

//------------------------------------------------------------------------------
//	I/O functions
//------------------------------------------------------------------------------
    friend ostream & operator << ( ostream & stream, const Force & obj );
				// output
    friend istream & operator >> ( istream & stream, Force & obj );
				// input

//------------------------------------------------------------------------------
//	Class name
//------------------------------------------------------------------------------
    virtual const char * className( void ) const { return "Force"; }
				// class name
};


#endif // _Force_H

// end of header file
// Do not add any stuff under this line.
