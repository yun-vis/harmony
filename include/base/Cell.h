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
class CellGroup
{
public:
    unsigned int                            id;
    double                                  multiple;  // multiple of cell unit
    vector< ForceGraph::vertex_descriptor > cellVec;
};

class Cell : public PathwayData
{
private:

    vector< Force >                         _forceCellVec;
    vector< ForceGraph >                    _forceCellGraphVec;
    vector< multimap< int, CellGroup > >    _cellGroupVec;
    map< unsigned int, Polygon2 >          *_polygonComplexPtr;

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

    vector< Force > &          forceCellVec( void )        { return _forceCellVec; }
    const vector< Force > &    forceCellVec( void ) const  { return _forceCellVec; }

    vector< ForceGraph > &          forceCellGraphVec( void )        { return _forceCellGraphVec; }
    const vector< ForceGraph > &    forceCellGraphVec( void ) const  { return _forceCellGraphVec; }

    map< unsigned int, Polygon2 > * polygonComplex( void )        { return _polygonComplexPtr; }
    map< unsigned int, Polygon2 > * polygonComplex( void ) const  { return _polygonComplexPtr; }

//------------------------------------------------------------------------------
//  Find conflicts
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Specific functions
//------------------------------------------------------------------------------
    void updatePathwayCoords( void );

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

