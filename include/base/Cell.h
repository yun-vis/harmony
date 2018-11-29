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
#include <iomanip>
#include <cstdlib>

using namespace std;

#include "base/Config.h"
#include "base/PathwayData.h"
#include "optimization/Force.h"

//------------------------------------------------------------------------------
//	Defining data types
//------------------------------------------------------------------------------

//----------------------------------------------------------------------
//	Defining macros
//----------------------------------------------------------------------

class CellComponent
{
public:
    unsigned int                            id;         // group id
    Polygon2                                contour;    // contour of the cell component
    double                                  multiple;   // multiple of cell unit
    vector< ForceGraph::vertex_descriptor > lsubgVec;   // vd in lsubg
    vector< ForceGraph::vertex_descriptor > cellgVec;   // vd in cell graph
    Force                                   detail;
    ForceGraph                              detailGraph;
};

class Cell : public PathwayData
{
private:

    vector< Force >                                 _forceCellVec;
    vector< ForceGraph >                            _forceCellGraphVec;
    vector< multimap< int, CellComponent > >        _cellComponentVec;      // int: number of nodes in lsubg
    map< unsigned int, Polygon2 >                  *_polygonComplexPtr;

    unsigned int    _nComponent;            // number of connected component

    // configuration parameter
    double          _paramUnit;             // cell unit

//------------------------------------------------------------------------------
//  Specific functions
//------------------------------------------------------------------------------
    void _buildCellGraphs( void );
    void _buildConnectedComponent( void );
    void _init( map< unsigned int, Polygon2 > * __polygonComplexPtr );
    void _clear( void );

protected:

public:
    
    Cell();                        // default constructor
    Cell( const Cell & obj );     // Copy constructor
    virtual ~Cell();               // Destructor

//------------------------------------------------------------------------------
//	Reference to members
//------------------------------------------------------------------------------

    unsigned int &              nComponent( void )          { return _nComponent; }
    const unsigned int &        nComponent( void ) const    { return _nComponent; }

    vector< Force > &           forceCellVec( void )        { return _forceCellVec; }
    const vector< Force > &     forceCellVec( void ) const  { return _forceCellVec; }

    vector< ForceGraph > &          forceCellGraphVec( void )        { return _forceCellGraphVec; }
    const vector< ForceGraph > &    forceCellGraphVec( void ) const  { return _forceCellGraphVec; }

    map< unsigned int, Polygon2 > * polygonComplex( void )              { return _polygonComplexPtr; }
    const map< unsigned int, Polygon2 > * polygonComplex( void ) const  { return _polygonComplexPtr; }

    vector< multimap< int, CellComponent > > &  cellComponentVec( void )                { return _cellComponentVec; }
    const vector< multimap< int, CellComponent > > &  cellComponentVec( void ) const    { return _cellComponentVec; }

//------------------------------------------------------------------------------
//  Find conflicts
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Specific functions
//------------------------------------------------------------------------------
    void updatePathwayCoords( void );
    void createPolygonComplex( void );
    bool findVertexInComplex( Coord2 &coord, ForceGraph &complex,
                              ForceGraph::vertex_descriptor &target );

//------------------------------------------------------------------------------
//  File I/O
//------------------------------------------------------------------------------
    void init( map< unsigned int, Polygon2 > * __polygonComplexPtr ) {
        _init( __polygonComplexPtr );
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

