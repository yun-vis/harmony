//==============================================================================
// LevelCell.h
//  : header file for the LevelCell network
//
//==============================================================================

#ifndef _Cell_H        // beginning of header file
#define _Cell_H        // notifying that this file is included

//----------------------------------------------------------------------
//  Including header files
//----------------------------------------------------------------------

#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <string>

using namespace std;

#include "core/Common.h"
#include "core/Config.h"
#include "core/PathwayData.h"
#include "core/Grid2.h"
#include "core/Contour2.h"
#include "core/RegionBase.h"
#include "core/LevelBase.h"
#include "optimization/Force.h"
#include "optimization/Similarity.h"
#include "gv/GraphVizAPI.h"

//------------------------------------------------------------------------------
//	Defining data types
//------------------------------------------------------------------------------

//----------------------------------------------------------------------
//	Defining macros
//----------------------------------------------------------------------
class CellComponent {

public:
	
	unsigned int id;            // component id
	unsigned int groupID;       // subsystem id
	
	vector< ForceGraph::vertex_descriptor > lsubgVec;   // vd in lsubg
	vector< ForceGraph::vertex_descriptor > cellgVec;   // vd in cell graph
	
	RegionBase componentRegion;
	
	vector< ForceGraph::vertex_descriptor > polygonComplexVD; // vd of simpleContour in the BoundaryGraph
	vector< vector< ForceGraph::vertex_descriptor > > metaboliteVec;   // mcl cluster vertex id
	
};

class LevelCell : public PathwayData, public LevelBase {

private:
	
	vector< RegionBase > _centerVec;    // region for center graphs
	vector< RegionBase > _cellVec;      // region for cell graphs
	
	vector< multimap< int, CellComponent > > _cellComponentVec;         // int: number of nodes in lsubg
	vector< vector< vector< double > > > _cellComponentSimilarityVec;   // cell component similarity
	multimap< Grid2, pair< CellComponent *, CellComponent * > > _interCellComponentMap;            // pair of inter cell component
	multimap< Grid2, pair< CellComponent *, CellComponent * > > _reducedInterCellComponentMap;     // pair of inter cell component
	
	unsigned int _nComponent;            // number of connected component
	
	// configuration parameter
	double _paramAddKa;            // attractive force
	double _paramAddKr;            // repulsive force
	double _paramUnit;             // cell unit

//------------------------------------------------------------------------------
//  Specific functions
//------------------------------------------------------------------------------
	void _buildConnectedComponent( void );
	
	void _computeCellComponentSimilarity( void );
	
	void _buildInterCellComponents( void );
	
	void _buildCellCenterGraphs( map< unsigned int, Polygon2 > *polygonComplexPtr );
	
	void _buildCellComponentGraphs( map< unsigned int, Polygon2 > *polygonComplexPtr );
	
	int _computeMCLClusters( ForceGraph &dg );
	
	void _computeClusters( void );
	
	void _init( void );
	
	void _clear( void );

protected:

public:
	
	LevelCell();                         // default constructor
	LevelCell( const LevelCell &obj );       // Copy constructor
	virtual ~LevelCell();                // Destructor

//------------------------------------------------------------------------------
//	Reference to members
//------------------------------------------------------------------------------
	
	vector< RegionBase > &centerVec( void ) { return _centerVec; }
	
	const vector< RegionBase > &centerVec( void ) const { return _centerVec; }
	
	vector< RegionBase > &cellVec( void ) { return _cellVec; }
	
	const vector< RegionBase > &cellVec( void ) const { return _cellVec; }
	
	vector< multimap< int, CellComponent > > &cellComponentVec( void ) { return _cellComponentVec; }
	
	const vector< multimap< int, CellComponent > > &cellComponentVec( void ) const { return _cellComponentVec; }
	
	multimap< Grid2, pair< CellComponent *, CellComponent * > > &
	interCellComponentMap( void ) { return _interCellComponentMap; }
	
	const multimap< Grid2, pair< CellComponent *, CellComponent * > > &
	interCellComponentMap( void ) const { return _interCellComponentMap; }
	
	multimap< Grid2, pair< CellComponent *, CellComponent * > > &
	reducedInterCellComponentMap( void ) { return _reducedInterCellComponentMap; }
	
	const multimap< Grid2, pair< CellComponent *, CellComponent * > > &
	reducedInterCellComponentMap( void ) const { return _reducedInterCellComponentMap; }

//------------------------------------------------------------------------------
//  Find conflicts
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Specific functions
//------------------------------------------------------------------------------

	void updateCenterCoords( void );
	
	void updatePathwayCoords( void );
	
	void createPolygonComplex( void );
	
	void cleanPolygonComplex( void );
	
	void additionalForcesMiddle( void );
	
	void additionalForcesCenter( void );
	
	void buildBoundaryGraph( void ) override;
	
	void updatePolygonComplex( void ) override;
	
	void prepareForce( map< unsigned int, Polygon2 > *polygonComplexPtr ) override;

//------------------------------------------------------------------------------
//  File I/O
//------------------------------------------------------------------------------
	void init( void ) {
		_init();
	}
	
//------------------------------------------------------------------------------
//      I/O
//------------------------------------------------------------------------------
	friend ostream &operator<<( ostream &stream, const LevelCell &obj );
	
	// Output
	friend istream &operator>>( istream &stream, LevelCell &obj );
	
	// Input
	virtual const char *className( void ) const { return "LevelCell"; }
	// Class name
};

#endif // _Cell_H

// end of header file
// Do not add any stuff under this line.

