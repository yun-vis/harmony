//==============================================================================
// Package.h
//  : header file for the Package network
//
//==============================================================================

#ifndef _Package_H        // beginning of header file
#define _Package_H        // notifying that this file is included

//----------------------------------------------------------------------
//  Including header files
//----------------------------------------------------------------------

#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <cstring>
#include <cmath>

using namespace std;

#include "base/Grid2.h"
#include "base/Contour2.h"
#include "base/Boundary.h"
#include "base/Bone.h"
#include "graph/BoundaryGraph.h"
#include "graph/ForceGraph.h"
#include "optimization/Force.h"
#include "optimization/Stress.h"

//------------------------------------------------------------------------------
//	Defining data types
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Defining macros
//------------------------------------------------------------------------------
class Package : public Boundary, public Bone
{
protected:

//------------------------------------------------------------------------------
//  Specific functions
//------------------------------------------------------------------------------
    void _init( void );
    void _clear( void );

public:
    
    Package();                        // default constructor
    Package( const Package & obj );   // Copy constructor
    virtual ~Package();               // Destructor

//------------------------------------------------------------------------------
//	Reference to members
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//      Find conflicts
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Specific functions
//------------------------------------------------------------------------------
    void buildBoundaryGraph( void );
    void updatePolygonComplex( void );      // update coordinates after optimization

//------------------------------------------------------------------------------
//  File I/O
//------------------------------------------------------------------------------
    void init( void ) { _init(); }
    void clear( void ) { _clear(); }

//------------------------------------------------------------------------------
//      I/O
//------------------------------------------------------------------------------
    friend ostream & operator << ( ostream & stream, const Package & obj );
                                // Output
    friend istream & operator >> ( istream & stream, Package & obj );
                                // Input

    virtual const char * className( void ) const { return "Package"; }
                                // Class name
};

#endif // _Package_H

// end of header file
// Do not add any stuff under this line.

