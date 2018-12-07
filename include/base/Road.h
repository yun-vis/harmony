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
    Coord2 routerCoord;
    vector< UndirectedBaseGraph::vertex_descriptor > path;
};

class Terminal
{
public:

    map< MetaboliteGraph::vertex_descriptor,
         MetaboliteGraph::vertex_descriptor > common;

    map< MetaboliteGraph::vertex_descriptor,
         UndirectedBaseGraph::vertex_descriptor > commonGates; // in lane

    UndirectedBaseGraph::vertex_descriptor routerVD;        // in lane

    map< UndirectedBaseGraph::vertex_descriptor,
         UndirectedBaseGraph::vertex_descriptor > terminals; // gates + router

    vector< pair< UndirectedBaseGraph::vertex_descriptor,
            UndirectedBaseGraph::vertex_descriptor > > treeEdges;
};

//----------------------------------------------------------------------
//	Defining macros
//----------------------------------------------------------------------
class Road : public PathwayData
{
private:

    UndirectedBaseGraph             _road;
    vector< vector < Highway > >    _highwayMat;

    unsigned int                    _gid;
    vector < Terminal >             _terminalVec;
    //vector < Highway >            * _highwayRoadPtr;

//------------------------------------------------------------------------------
//  Specific functions
//------------------------------------------------------------------------------
    bool _findClosestVertexInRoad( Coord2 &coord, UndirectedBaseGraph::vertex_descriptor &target );
    bool _findClosestVertexInLane( Coord2 &coord, UndirectedBaseGraph::vertex_descriptor &target );
    bool _findVertexInRoad( Coord2 &coord, UndirectedBaseGraph::vertex_descriptor &target );
    void _findShortestPaths( void );
    void _initRoad( vector< multimap< int, CellComponent > > & cellComponentVec );
    void _initLane( unsigned int gid,  multimap< int, CellComponent > & cellComponent, vector < Highway > * highwayRoadPtr );
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

    vector< Terminal > &          terminalVec( void )        { return _terminalVec; }
    const vector< Terminal > &    terminalVec( void ) const  { return _terminalVec; }

//------------------------------------------------------------------------------
//  Find conflicts
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Specific functions
//------------------------------------------------------------------------------
    void buildRoad( void );
    void steinerTree( void );

//------------------------------------------------------------------------------
//  File I/O
//------------------------------------------------------------------------------
    void initRoad( vector< multimap< int, CellComponent > > & __cellComponentVec ) {
        _initRoad( __cellComponentVec );
    }
    void initLane( unsigned int gid, multimap< int, CellComponent > & __cellComponent, vector < Highway > *__highwayRoadPtr ) {
        _initLane( gid, __cellComponent, __highwayRoadPtr );
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

