//==============================================================================
// RegionData.h
//  : header file for the map
//
//==============================================================================

#ifndef _REGIONDATA_H        // beginning of header file
#define _REGIONDATA_H        // notifying that this file is included

//----------------------------------------------------------------------
//  Including header files
//----------------------------------------------------------------------
#include <iostream>

#include <QtGui/QPainter>
#include <QtCore/QString>

using namespace std;

#ifndef Q_MOC_RUN
#include "optimization/Force.h"
#include "optimization/Stress.h"
#include "optimization/Octilinear.h"
#include "base/Road.h"
#include "base/LevelBorder.h"
#endif // Q_MOC_RUN

class RegionData
{
protected:

    // levelhigh
    vector< Boundary >      *_boundaryVecPtr;
    vector< Octilinear * >  *_octilinearVecPtr;

    LevelBorder             *_levelBorderPtr;

    // cells of subgraphs
    Cell                    *_cellPtr;
    vector< Road >          *_roadPtr;
    vector< Road >          *_lanePtr;

public:

    RegionData();                // default constructor
    RegionData( const RegionData & obj );     // Copy constructor
    ~RegionData();             // Destructor

//------------------------------------------------------------------------------
//  Specific functions
//------------------------------------------------------------------------------

    void setRegionData(LevelBorder * __levelhigh,
                       vector< Boundary > *__boundaryVecPtr,
                       Cell * __cellPtr,
                       vector< Road > * __roadPtr,
                       vector< Road > * __lanePtr ){
        _levelBorderPtr = __levelhigh;
        _boundaryVecPtr = __boundaryVecPtr;
        _cellPtr = __cellPtr;
        _roadPtr = __roadPtr;
        _lanePtr = __lanePtr;
    }

//------------------------------------------------------------------------------
//      I/O
//------------------------------------------------------------------------------
    friend ostream & operator << ( ostream & stream, const RegionData & obj );
    // Output
    friend istream & operator >> ( istream & stream, RegionData & obj );
    // Input

    virtual const char * className( void ) const { return "RegionData"; }
    // Class name
};



#endif // _REGIONDATA_H

// end of header file
// Do not add any stuff under this line.
