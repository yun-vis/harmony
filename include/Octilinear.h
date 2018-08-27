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
#include <Eigen/Core>
#include <Eigen/Dense>

using namespace std;

#include "Metro.h"

//------------------------------------------------------------------------------
//	Defining data types
//------------------------------------------------------------------------------
#define OCTILINEAR_CONFLICT
#define OCTILINEAR_BOUNDARY

//----------------------------------------------------------------------
//	Defining macros
//----------------------------------------------------------------------

class Octilinear
{
private:

    Metro             * _metro;
    Eigen::VectorXd     _var;           // x
    Eigen::VectorXd     _output;        // b
    Eigen::MatrixXd     _coef;          // A
    double              _half_width;    // window_width
    double              _half_height;   // window_height

    unsigned int        _nVars;
    unsigned int        _nConstrs;
    double              _w_octilinear, _w_position;
    double              _w_boundary, _w_crossing;
    double              _d_Alpha;
    double              _d_Beta;
    vector< double >    _theta;         // closest octilinear theta

protected:

    void                _setVars        ( unsigned int & nRows );
    void                _setConstraints ( unsigned int & nRows );
    void                _initCoefs      ( void );
    void                _initVars       ( void );
    void                _initOutputs    ( void );
    void                _updateCoefs    ( void );
    void                _updateOutputs  ( void );
    virtual void        _init           ( Metro * __metro, double __width, double __height );
    void                _setTargetAngle( void );
    void                _updateEdgeCurAngle( void );

public:

    Octilinear();                     // default constructor
    Octilinear( const Octilinear & obj ); // Copy constructor
    virtual ~Octilinear();            // Destructor

//------------------------------------------------------------------------------
//  Reference to members
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Specific functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//      Initialization functions
//------------------------------------------------------------------------------
    void prepare( Metro * __metro, double __half_width, double __half_height ) {
        _init( __metro, __half_width, __half_height );
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
    friend ostream & operator << ( ostream & stream, const Octilinear & obj );
                                // Output
    friend istream & operator >> ( istream & stream, Octilinear & obj );
                                // Input

    virtual const char * className( void ) const { return "Octilinear"; }
                                // Class name
};

#endif // _Octilinear_H

// end of header file
// Do not add any stuff under this line.
