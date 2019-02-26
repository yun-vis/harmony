//==============================================================================
// LevelMiddle.h
//  : header file for the LevelMiddle network
//
//==============================================================================

#ifndef _LevelMiddle_H        // beginning of header file
#define _LevelMiddle_H        // notifying that this file is included

//----------------------------------------------------------------------
//  Including header files
//----------------------------------------------------------------------

#include <iostream>
#include <iomanip>
#include <string>
#include <tinyxml.h>

using namespace std;

#include "base/Grid2.h"
#include "base/Package.h"
#include "graph/SkeletonGraph.h"
#include "optimization/Force.h"
#include "optimization/Smooth.h"
#include "optimization/Octilinear.h"

//------------------------------------------------------------------------------
//	Defining data types
//------------------------------------------------------------------------------


//----------------------------------------------------------------------
//	Defining macros
//----------------------------------------------------------------------

class LevelMiddle : public Bone
{
private:

    // optimization
    Smooth                      _smoothBoundary;    // smooth boundary
    Octilinear                  _octilinearBoundary;// octilinear boundary

protected:
    

    double                      _stringToDouble( string str );
    void                        _init( SkeletonGraph & skeletonGraph, Polygon2 &__contour );

public:
    
    LevelMiddle();                        // default constructor
    LevelMiddle( const LevelMiddle & obj );     // Copy constructor
    virtual ~LevelMiddle();               // Destructor

//------------------------------------------------------------------------------
//	Reference to members
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Specific functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  File I/O
//------------------------------------------------------------------------------
    void init( SkeletonGraph &__skeletonGraph, Polygon2 &__contour ) {
        _init( __skeletonGraph, __contour );
    }
    void clear( void );

//------------------------------------------------------------------------------
//      I/O
//------------------------------------------------------------------------------
    friend ostream & operator << ( ostream & stream, const LevelMiddle & obj );
                                // Output
    friend istream & operator >> ( istream & stream, LevelMiddle & obj );
                                // Input

    virtual const char * className( void ) const { return "LevelMiddle"; }
                                // Class name
};

#endif // _LevelMiddle_H

// end of header file
// Do not add any stuff under this line.

