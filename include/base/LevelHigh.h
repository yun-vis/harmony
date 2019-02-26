//==============================================================================
// LevelHigh.h
//  : header file for the LevelHigh network
//
//==============================================================================

#ifndef _LevelHigh_H        // beginning of header file
#define _LevelHigh_H        // notifying that this file is included

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

class LevelHigh : public Bone
{
private:

    ForceGraph                  _skeleton;

protected:
    
    void                        _init( SkeletonGraph & skeletonGraph, Polygon2 &contour );

public:
    
    LevelHigh();                        // default constructor
    LevelHigh( const LevelHigh & obj );     // Copy constructor
    virtual ~LevelHigh();               // Destructor

//------------------------------------------------------------------------------
//	Reference to members
//------------------------------------------------------------------------------
    const ForceGraph &		    skeleton( void ) const  { return _skeleton; }
    ForceGraph &			    skeleton( void )	    { return _skeleton; }

//------------------------------------------------------------------------------
//  Specific functions
//------------------------------------------------------------------------------
    void normalizeSkeleton( const int & width, const int & height );   // normalize the LevelHigh size
    void normalizeBone( const int & width, const int & height );   // normalize the LevelHigh size
    void decomposeSkeleton( void );

#ifdef SKIP
    void readPolygonComplex( void );
    void writePolygonComplex( void );
#endif // SKIP

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
    friend ostream & operator << ( ostream & stream, const LevelHigh & obj );
                                // Output
    friend istream & operator >> ( istream & stream, LevelHigh & obj );
                                // Input

    virtual const char * className( void ) const { return "LevelHigh"; }
                                // Class name
};

#endif // _LevelHigh_H

// end of header file
// Do not add any stuff under this line.

