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

#include "graph/ForceGraph.h"
#include "base/Config.h"

//------------------------------------------------------------------------------
//	Defining data types
//------------------------------------------------------------------------------
//#define STRESS_CONFLICT
//#define STRESS_BOUNDARY

//----------------------------------------------------------------------
//	Defining macros
//----------------------------------------------------------------------

class Stress
{
private:

    // Graph
    ForceGraph     *_forceGraphPtr;
    unsigned long   _nVertices;
    unsigned long   _nEdges;


    // Matrix
    Eigen::VectorXd _var;           // x
    Eigen::VectorXd _output;        // b
    Eigen::MatrixXd _coef;          // A
    double          _half_width;    // window_width
    double          _half_height;   // window_height

    // variables
    unsigned int    _nVars;
    unsigned int    _nConstrs;
    
    // weight of constraints
    double          _w_angle, _w_position;
    double          _w_contextlength;
    double          _w_boundary, _w_crossing;
    double          _w_labelangle;

    double          _unit_length;
    // double          _d_Alpha;
    // double          _d_Beta;

protected:

    void            _setVars        ( unsigned int & nRows );
    void            _setConstraints ( unsigned int & nRows );
    void            _initVars       ( void );
    void            _initCoefs      ( void );
    void            _initOutputs    ( void );
    void            _updateCoefs    ( void );
    void            _updateOutputs  ( void );
    virtual void    _init           ( ForceGraph     *__forceGraphPtr,
                                      double __half_width, double __half_height );

public:

    Stress();                     // default constructor
    Stress( const Stress & obj ); // Copy constructor
    virtual ~Stress();            // Destructor

//------------------------------------------------------------------------------
//  Reference to members
//------------------------------------------------------------------------------
    const unsigned long &			nVertices( void ) const { return _nVertices; }
    const unsigned long &			nEdges( void ) const { return _nEdges; }

//------------------------------------------------------------------------------
//  Specific functions
//------------------------------------------------------------------------------
    double LeastSquare( unsigned long iter );
    double ConjugateGradient( unsigned long iter );

//------------------------------------------------------------------------------
//      Initialization functions
//------------------------------------------------------------------------------
    void prepare( ForceGraph     *__forceGraphPtr,
                  double __half_width, double __half_height ) {
        _init( __forceGraphPtr, __half_width, __half_height );
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
    friend ostream & operator << ( ostream & stream, const Stress & obj );
                                // Output
    friend istream & operator >> ( istream & stream, Stress & obj );
                                // Input

    virtual const char * className( void ) const { return "Stress"; }
                                // Class name
};

#endif // _Stress_H

// end of header file
// Do not add any stuff under this line.
