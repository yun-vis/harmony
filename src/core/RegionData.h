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
#include "core/Road.h"
#include "core/LevelBorder.h"
#include "core/LevelCell.h"
#include "core/LevelDetail.h"
#endif // Q_MOC_RUN

class RegionData {

protected:
	
	vector< Octilinear * > *_octilinearBoundaryVecPtr;
	LEVELTYPE *_levelTypePtr;

	// level border
	LevelBorder *_levelBorderPtr;

	// level cell
	LevelCell *_levelCellPtr;

	// level componentRegion
	LevelDetail *_levelDetailPtr;
	
	// level route
	vector< Road > *_roadPtr;
	vector< Road > *_lanePtr;

public:
	
	RegionData();                // default constructor
	RegionData( const RegionData &obj );     // Copy constructor
	~RegionData();             // Destructor
	
//------------------------------------------------------------------------------
//  Specific functions
//------------------------------------------------------------------------------
	
	void setRegionData( LEVELTYPE *__levelTypePtr, vector< Octilinear * > *__octilinearBoundaryVecPtr,
	                    LevelBorder *__levelhigh, LevelCell *__cellPtr, LevelDetail *__detailPtr,
	                    vector< Road > *__roadPtr, vector< Road > *__lanePtr ) {
		_levelTypePtr = __levelTypePtr;
		_octilinearBoundaryVecPtr = __octilinearBoundaryVecPtr;
		_levelBorderPtr = __levelhigh;
		_levelCellPtr = __cellPtr;
		_levelDetailPtr = __detailPtr;
		_roadPtr = __roadPtr;
		_lanePtr = __lanePtr;
	}

//------------------------------------------------------------------------------
//      I/O
//------------------------------------------------------------------------------
	friend ostream &operator<<( ostream &stream, const RegionData &obj );
	
	// Output
	friend istream &operator>>( istream &stream, RegionData &obj );
	// Input
	
	virtual const char *className( void ) const { return "RegionData"; }
	// Class name
};


#endif // _REGIONDATA_H

// end of header file
// Do not add any stuff under this line.
