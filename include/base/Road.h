//==============================================================================
// Road.h
//  : header file for the Road network
//
//==============================================================================

#ifndef _Road_H        // beginning of header file
#define _Road_H        // notifying that this file is included

//----------------------------------------------------------------------
//  Including header files
//----------------------------------------------------------------------

#include <iostream>
#include <iomanip>
#include <cstdlib>

using namespace std;

#include "base/PathwayData.h"
#include "base/Cell.h"
#include "optimization/Force.h"
#include "graph/UndirectedBaseGraph.h"

//------------------------------------------------------------------------------
//	Defining data types
//------------------------------------------------------------------------------
class Highway
{
public:
    unsigned int id;
    // unsigned int count;
    map< MetaboliteGraph::vertex_descriptor,
         MetaboliteGraph::vertex_descriptor > common;
    Coord2  center;                                   // average position of alias metabolites
    UndirectedBaseGraph::vertex_descriptor routerVD;  // closest vd from center
    vector< UndirectedBaseGraph::vertex_descriptor > path;
};

//----------------------------------------------------------------------
//	Defining macros
//----------------------------------------------------------------------
class Road : public PathwayData
{
private:

    UndirectedBaseGraph             _road;
    vector< vector < Highway > >    _highwayMat;

//------------------------------------------------------------------------------
//  Specific functions
//------------------------------------------------------------------------------
    bool _findClosestVertexInRoad( Coord2 &coord, UndirectedBaseGraph::vertex_descriptor &target );
    bool _findVertexInRoad( Coord2 &coord, UndirectedBaseGraph::vertex_descriptor &target );
    void _findShortestPaths( void );
    void _init( vector< multimap< int, CellComponent > > & cellComponentVec );
    void _clear( void );

protected:

public:
    
    Road();                        // default constructor
    Road( const Road & obj );      // Copy constructor
    virtual ~Road();               // Destructor

//------------------------------------------------------------------------------
//	Reference to members
//------------------------------------------------------------------------------

    UndirectedBaseGraph &          road( void )        { return _road; }
    const UndirectedBaseGraph &    road( void ) const  { return _road; }

    vector< vector < Highway > > &          highwayMat( void )        { return _highwayMat; }
    const vector< vector < Highway > > &    highwayMat( void ) const  { return _highwayMat; }


//------------------------------------------------------------------------------
//  Find conflicts
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Specific functions
//------------------------------------------------------------------------------
    void buildRoad( void );

//------------------------------------------------------------------------------
//  File I/O
//------------------------------------------------------------------------------
    void init( vector< multimap< int, CellComponent > > & __cellComponentVec ) {
        _init( __cellComponentVec );
    }
    void clear( void ) { _clear(); }

//------------------------------------------------------------------------------
//      I/O
//------------------------------------------------------------------------------
    friend ostream & operator << ( ostream & stream, const Road & obj );
                                // Output
    friend istream & operator >> ( istream & stream, Road & obj );
                                // Input

    virtual const char * className( void ) const { return "Road"; }
                                // Class name
};

#endif // _Road_H

// end of header file
// Do not add any stuff under this line.

