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
         UndirectedBaseGraph::vertex_descriptor > commonGates;  // in lane

    UndirectedBaseGraph::vertex_descriptor routerVD;            // in lane

    map< UndirectedBaseGraph::vertex_descriptor,
         UndirectedBaseGraph::vertex_descriptor > terminals;    // gates + router

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

    vector< Contour2 >              _contourVec;    // subsystem contours
    vector< vector< Coord2 > >      _roadChaikinCurve;
    vector< Coord2 >                _laneChaikinCurve;

    unsigned int                    _gid;
    vector < Terminal >             _terminalVec;

    vector< UndirectedBaseGraph::vertex_descriptor > _citeVec;
    vector< pair< UndirectedBaseGraph::vertex_descriptor,
            UndirectedBaseGraph::vertex_descriptor > > _treeEdgeVec;

//------------------------------------------------------------------------------
//  Specific functions
//------------------------------------------------------------------------------
    bool _findClosestVertexInRoad( Coord2 &coord, UndirectedBaseGraph::vertex_descriptor &target );
    bool _findClosestVertexInLane( Coord2 &coord, UndirectedBaseGraph::vertex_descriptor &target );
    bool _findVertexInRoad( Coord2 &coord, UndirectedBaseGraph::vertex_descriptor &target );
    void _findShortestPaths( void );
    bool _findVertexOnLine( Coord2 &coord, UndirectedBaseGraph::edge_descriptor &edT );
    void _initRoad( Cell *cellPtr );
    void _initLane( unsigned int gid,  multimap< int, CellComponent > & cellComponent, vector < Highway > * highwayRoadPtr );
    void _clear( void );
    void _initSteinerNet( vector< multimap< int, CellComponent > > & __cellComponentVec );

    // curve computation
    void _initRoadChaikinCurve( void );
    void _initLaneChaikinCurve( void );
    void _runRoadChaikinCurve( int num );
    void _runLaneChaikinCurve( int num );

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

    const vector< pair< UndirectedBaseGraph::vertex_descriptor,
            UndirectedBaseGraph::vertex_descriptor > > & treeEdgeVec( void ) const { return _treeEdgeVec; }
    vector< pair< UndirectedBaseGraph::vertex_descriptor,
            UndirectedBaseGraph::vertex_descriptor > > & treeEdgeVec( void ) { return _treeEdgeVec; }

    vector< Contour2 > &        subsysContour( void )       { return _contourVec; }
    const vector< Contour2 > &  subsysContour( void ) const { return _contourVec; }

    vector< vector< Coord2 > > &          roadChaikinCurve( void )        { return _roadChaikinCurve; }
    const vector< vector< Coord2 > > &    roadChaikinCurve( void ) const  { return _roadChaikinCurve; }
    vector< Coord2 > &          laneChaikinCurve( void )        { return _laneChaikinCurve; }
    const vector< Coord2 > &    laneChaikinCurve( void ) const  { return _laneChaikinCurve; }


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
    void initSteinerNet( vector< multimap< int, CellComponent > > & __cellComponentVec )
    {
        _initSteinerNet( __cellComponentVec );
    }
    void initRoad( Cell *__cellPtr ) {
        _initRoad( __cellPtr );
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

