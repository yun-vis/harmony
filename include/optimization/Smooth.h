//==============================================================================
// Smooth.h
//  : header file for the optimization
//
//==============================================================================

#ifndef _Smooth_H        // begining of header file
#define _Smooth_H        // notifying that this file is included

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

//------------------------------------------------------------------------------
//	Defining data types
//------------------------------------------------------------------------------
#define SMOOTH_CONFLICT
#define SMOOTH_BOUNDARY

//----------------------------------------------------------------------
//	Defining macros
//----------------------------------------------------------------------

class Smooth : public Common
{
private:

    Boundary        * _boundary;
    Eigen::VectorXd _var;           // x
    Eigen::VectorXd _output;        // b
    Eigen::MatrixXd _coef;          // A
    double          _half_width;    // window_width
    double          _half_height;   // window_height

    unsigned int    _nVars;
    unsigned int    _nConstrs;
    double          _w_angle, _w_position;
    double          _w_contextlength;
    double          _w_boundary, _w_crossing;
    double          _w_labelangle;
    double          _d_Alpha;
    double          _d_Beta;

protected:

    void            _setVars        ( unsigned int & nRows );
    void            _setConstraints ( unsigned int & nRows );
    void            _initVars       ( void );
    void            _initCoefs      ( void );
    void            _initOutputs    ( void );
    void            _updateCoefs    ( void );
    void            _updateOutputs  ( void );
    virtual void    _init           ( Boundary * __boundary, double __width, double __height );

public:

    Smooth();                     // default constructor
    Smooth( const Smooth & obj ); // Copy constructor
    virtual ~Smooth();            // Destructor

//------------------------------------------------------------------------------
//  Reference to members
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Specific functions
//------------------------------------------------------------------------------
    double LeastSquare( unsigned int iter );
    double ConjugateGradient( unsigned int iter );

//------------------------------------------------------------------------------
//      Initialization functions
//------------------------------------------------------------------------------
    void prepare( Boundary * __boundary, double __width, double __height ) {
        _init( __boundary, __width, __height );
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
    friend ostream & operator << ( ostream & stream, const Smooth & obj );
                                // Output
    friend istream & operator >> ( istream & stream, Smooth & obj );
                                // Input

    virtual const char * className( void ) const { return "Smooth"; }
                                // Class name
};

#endif // _Smooth_H

// end of header file
// Do not add any stuff under this line.
