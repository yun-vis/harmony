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
#include "base/Boundary.h"
#include "base/Config.h"

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

    Boundary        *_boundaryPtr;
    double          _paramKa;
    double          _paramKr;

  protected:

    unsigned int	_step;	

    int			    _width;
    int			    _height;

    // center coordinates
    double		    _center_x, _center_y;

    void		_init	( Boundary * __boundary );
    void		_reset	( void );
    void		_random	( void );
    void        _onestep( void );

    void		_force		( void );
    double		_gap		( void );

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

//------------------------------------------------------------------------------
//	Fundamental functions
//------------------------------------------------------------------------------
    void init( Boundary * __boundary ) { _init( __boundary ); }
    void reset( void )				{ _reset(); }
    void random( void )				{ _random(); }

//------------------------------------------------------------------------------
//	Misc. functions
//------------------------------------------------------------------------------
    void	force	( void )		{ _force(); }

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
