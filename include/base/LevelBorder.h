//==============================================================================
// LevelBorder.h
//  : header file for the LevelBorder network
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
#include "base/RegionBase.h"
#include "graph/SkeletonGraph.h"
#include "optimization/Force.h"
//#include "optimization/Smooth.h"
//#include "optimization/Octilinear.h"

//------------------------------------------------------------------------------
//	Defining data types
//------------------------------------------------------------------------------


//----------------------------------------------------------------------
//	Defining macros
//----------------------------------------------------------------------

class LevelBorder : public RegionBase
{
private:

    ForceGraph                  _skeletonForceGraph;
    double                     *_content_widthPtr;
    double                     *_content_heightPtr;
    double                     *_veCoveragePtr;

protected:
    
    void                        _init( double *widthPtr, double *heightPtr,
                                       double *veCoveragePtr, Polygon2 *contourPtr,
                                       SkeletonGraph & skeletonGraph );
    void                        _clear( void );

public:
    
    LevelBorder();                              // default constructor
    LevelBorder(const LevelBorder & obj );      // Copy constructor
    virtual ~LevelBorder();                     // Destructor

//------------------------------------------------------------------------------
//	Reference to members
//------------------------------------------------------------------------------
    const ForceGraph &		    skeletonForceGraph(void ) const  { return _skeletonForceGraph; }
    ForceGraph &			    skeletonForceGraph(void )	    { return _skeletonForceGraph; }

//------------------------------------------------------------------------------
//  Specific functions
//------------------------------------------------------------------------------
    void normalizeSkeleton( void );
    void normalizeBone( void );
    void decomposeSkeleton( void );

//------------------------------------------------------------------------------
//  File I/O
//------------------------------------------------------------------------------
    void init( double *__widthPtr, double *__heightPtr,
               double *__veCoveragePtr, Polygon2 *__contourPtr,
               SkeletonGraph &__skeletonGraph ) {
        _init( __widthPtr, __heightPtr,
               __veCoveragePtr, __contourPtr,
               __skeletonGraph );
    }
    void clear( void ) { _clear(); }

//------------------------------------------------------------------------------
//      I/O
//------------------------------------------------------------------------------
    friend ostream & operator << ( ostream & stream, const LevelBorder & obj );
                                // Output
    friend istream & operator >> (istream & stream, LevelBorder & obj );
                                // Input

    virtual const char * className( void ) const { return "LevelBorder"; }
                                // Class name
};

#endif // _LevelHigh_H

// end of header file
// Do not add any stuff under this line.

