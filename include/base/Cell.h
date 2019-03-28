//==============================================================================
// Cell.h
//  : header file for the Cell network
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

#include "base/Common.h"
#include "base/Config.h"
#include "base/PathwayData.h"
#include "base/Grid2.h"
#include "base/Contour2.h"
#include "base/Package.h"
#include "optimization/Force.h"
#include "optimization/Similarity.h"
#include "optimization/Smooth.h"
#include "optimization/Octilinear.h"

//------------------------------------------------------------------------------
//	Defining data types
//------------------------------------------------------------------------------

//----------------------------------------------------------------------
//	Defining macros
//----------------------------------------------------------------------
class CellComponent
{
public:
    unsigned int                            id;         // component id
    // unsigned int                            componentID;// id in subg
    unsigned int                            groupID;    // subsystem id
    unsigned int                            nMCL;       // number of mcl clustering

    Polygon2                                contour;    // contour of the cell component
    double                                  multiple;   // multiple of cell unit
    vector< ForceGraph::vertex_descriptor > lsubgVec;   // vd in lsubg
    vector< ForceGraph::vertex_descriptor > cellgVec;   // vd in cell graph

    Bone                                    mcl;
    Bone                                    detail;

    vector< ForceGraph::vertex_descriptor > polygonComplexVD; // vd of contour in the BoundaryGraph
};

class Cell : public PathwayData, public Common
{
private:

    double                                         *_veCoveragePtr;
    double                                         *_veRatioPtr;
    vector< Bone >                                  _cellVec;

    vector< multimap< int, CellComponent > >        _cellComponentVec;              // int: number of nodes in lsubg
    vector< vector< vector< double > > >            _cellComponentSimilarityVec;    // cell component similarity
    multimap< Grid2, pair< CellComponent*, CellComponent* > > _interCellComponentMap;            // pair of inter cell component
    multimap< Grid2, pair< CellComponent*, CellComponent* > > _reducedInterCellComponentMap;     // pair of inter cell component

    unsigned int    _nComponent;            // number of connected component

    // configuration parameter
    double          _paramAddKa;            // attractive force
    double          _paramAddKr;            // repulsive force
    double          _paramUnit;             // cell unit

//------------------------------------------------------------------------------
//  Specific functions
//------------------------------------------------------------------------------
    void _buildConnectedComponent( void );
    void _computeCellComponentSimilarity( void );
    void _buildInterCellComponents( void );
    void _buildCellGraphs( void );
    int _computeClusters( ForceGraph &dg,
                          vector< MetaboliteGraph::vertex_descriptor > & cluster );
    void _init( double *veCoveragePtr, double *__veRatioPtr, map< unsigned int, Polygon2 > * polygonComplexPtr );
    void _clear( void );

protected:

public:
    
    Cell();                         // default constructor
    Cell( const Cell & obj );       // Copy constructor
    virtual ~Cell();                // Destructor

    //vector< Coord2 > center;
    //vector< double > radius;
    //vector< Polygon2 > con;

//------------------------------------------------------------------------------
//	Reference to members
//------------------------------------------------------------------------------

    unsigned int &              nComponent( void )          { return _nComponent; }
    const unsigned int &        nComponent( void ) const    { return _nComponent; }

    vector< Bone > &            cellVec( void )        { return _cellVec; }
    const vector< Bone > &      cellVec( void ) const  { return _cellVec; }

    vector< multimap< int, CellComponent > > &  cellComponentVec( void )                { return _cellComponentVec; }
    const vector< multimap< int, CellComponent > > &  cellComponentVec( void ) const    { return _cellComponentVec; }

    multimap< Grid2, pair< CellComponent*, CellComponent* > > & interCellComponentMap( void )                { return _interCellComponentMap; }
    const multimap< Grid2, pair< CellComponent*, CellComponent* > > & interCellComponentMap( void ) const    { return _interCellComponentMap; }

    multimap< Grid2, pair< CellComponent*, CellComponent* > > & reducedInterCellComponentMap( void )                { return _reducedInterCellComponentMap; }
    const multimap< Grid2, pair< CellComponent*, CellComponent* > > & reducedInterCellComponentMap( void ) const    { return _reducedInterCellComponentMap; }

//------------------------------------------------------------------------------
//  Find conflicts
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Specific functions
//------------------------------------------------------------------------------
    void updateMCLCoords( void );
    void updatePathwayCoords( void );
    void createPolygonComplex( void );
    void updatePolygonComplex( void );
    void createPolygonComplexFromDetailGraph( void );
    void updatePolygonComplexFromDetailGraph( void );
    void additionalForces( void );

//------------------------------------------------------------------------------
//  File I/O
//------------------------------------------------------------------------------
    void init( double *__veCoveragePtr, double *__veRatioPtr, map< unsigned int, Polygon2 > * __polygonComplexPtr ) {
        _init( __veCoveragePtr, __veRatioPtr, __polygonComplexPtr );
    }
    void clear( void ) { _clear(); }

//------------------------------------------------------------------------------
//      I/O
//------------------------------------------------------------------------------
    friend ostream & operator << ( ostream & stream, const Cell & obj );
                                // Output
    friend istream & operator >> ( istream & stream, Cell & obj );
                                // Input

    virtual const char * className( void ) const { return "Cell"; }
                                // Class name
};

#endif // _Cell_H

// end of header file
// Do not add any stuff under this line.

