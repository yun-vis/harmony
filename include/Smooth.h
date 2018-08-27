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
#include <Eigen/Core>
#include <Eigen/Dense>

using namespace std;

#include "Metro.h"

//------------------------------------------------------------------------------
//	Defining data types
//------------------------------------------------------------------------------
#define SMOOTH_CONFLICT
#define SMOOTH_BOUNDARY

//----------------------------------------------------------------------
//	Defining macros
//----------------------------------------------------------------------

class Smooth
{
private:

    Metro         * _metro;
    Eigen::VectorXd _var;           // x
    Eigen::VectorXd _output;        // b
    Eigen::MatrixXd _coef;          // A
    double          _half_width;    // window_width
    double          _half_height;   // window_height

    unsigned int    _nVars;
    unsigned int    _nConstrs;
    double          _w_focuslength, _w_angle, _w_position;
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
    virtual void    _init           ( Metro * __metro, double __width, double __height );

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

//------------------------------------------------------------------------------
//      Initialization functions
//------------------------------------------------------------------------------
    void prepare( Metro * __metro, double __width, double __height ) {
        _init( __metro, __width, __height );
    }

//------------------------------------------------------------------------------
//  File I/O
//------------------------------------------------------------------------------
    void prepare( void );
    void clear( void );
    void retrieve( void );
    double LeastSquare( unsigned int iter );
    double ConjugateGradient( unsigned int iter );

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
