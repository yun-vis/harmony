//==============================================================================
// LevelMiddle.cc
//  : program file for the metro network
//
//------------------------------------------------------------------------------
//
//              Date: Mon Dec 10 04:28:26 2012
//
//==============================================================================

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------

#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <cstring>
#include <cmath>

using namespace std;

#include "base/LevelMiddle.h"
#include "graph/UndirectedPropertyGraph.h"

//------------------------------------------------------------------------------
//	Private functions
//------------------------------------------------------------------------------
//  LevelMiddle::_init -- initialization
//
//  Inputs
//      string
//
//  Outputs
//  double
//
void LevelMiddle::_init( SkeletonGraph & skeletonGraph, Polygon2 &__contour )
{
    clearGraph( _skeleton );

    // copy skeleton to composite
    BGL_FORALL_VERTICES( vd, skeletonGraph, SkeletonGraph )
    {
        ForceGraph::vertex_descriptor vdNew = add_vertex( _skeleton );
        _skeleton[ vdNew ].id          = skeletonGraph[vd].id;
        _skeleton[ vdNew ].groupID     = skeletonGraph[vd].id;
        _skeleton[ vdNew ].initID      = skeletonGraph[vd].id;

        _skeleton[ vdNew ].coordPtr     = new Coord2( skeletonGraph[vd].coordPtr->x(), skeletonGraph[vd].coordPtr->y() );
        _skeleton[ vdNew ].prevCoordPtr = new Coord2( skeletonGraph[vd].coordPtr->x(), skeletonGraph[vd].coordPtr->y() );
        _skeleton[ vdNew ].forcePtr     = new Coord2( 0, 0 );
        _skeleton[ vdNew ].placePtr     = new Coord2( 0, 0 );
        _skeleton[ vdNew ].shiftPtr     = new Coord2( 0, 0 );
        _skeleton[ vdNew ].weight       = 0.0;

        _skeleton[ vdNew ].widthPtr    = new double( *skeletonGraph[vd].widthPtr );
        _skeleton[ vdNew ].heightPtr   = new double( *skeletonGraph[vd].heightPtr );
        _skeleton[ vdNew ].areaPtr     = new double( *skeletonGraph[vd].areaPtr );
    }

    BGL_FORALL_EDGES( ed, skeletonGraph, SkeletonGraph )
    {
        SkeletonGraph::vertex_descriptor vdS = source( ed, skeletonGraph );
        SkeletonGraph::vertex_descriptor vdT = target( ed, skeletonGraph );
        ForceGraph::vertex_descriptor vdCompoS = vertex( skeletonGraph[vdS].id, _skeleton );
        ForceGraph::vertex_descriptor vdCompoT = vertex( skeletonGraph[vdT].id, _skeleton );

        // add edge
        pair< ForceGraph::edge_descriptor, unsigned int > foreE = add_edge( vdCompoS, vdCompoT, _skeleton );
        BoundaryGraph::edge_descriptor foreED = foreE.first;
        _skeleton[ foreED ].id = _skeleton[ed].id;
    }
}

//------------------------------------------------------------------------------
//	Protected functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------
//
//  LevelMiddle::LevelMiddle -- default constructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
LevelMiddle::LevelMiddle( void )
{
    clearGraph( _boundary );
    clearGraph( _skeleton );

    //_shortestPathM.clear();
    _line.clear();
    _lineSta.clear();

    _nLines = 0;
    _nVertices  = 0;
    _nEdges = 0;
    _meanVSize = 0.0;

    _removedVertices.clear();
    _removedEdges.clear();
    _removedWeights.clear();

    _VEconflict.clear();
    _ratioR.clear();
}

//
//  LevelMiddle::LevelMiddle -- copy constructor
//
//  Inputs
//  obj : object of this class
//
//  Outputs
//  none
//
LevelMiddle::LevelMiddle( const LevelMiddle & obj )
{
    _boundary = obj.boundary();

    //_shortestPathM = obj.spM();
    _line = obj.line();
    _lineSta = obj.lineSta();

    _nLines     = obj._nLines;
    _nVertices  = obj._nVertices;
    _nEdges     = obj._nEdges;
    _meanVSize  = obj._meanVSize;

    _removedVertices    = obj.removedVertices();
    _removedEdges       = obj.removedEdges();
    _removedWeights     = obj.removedWeights();

    _VEconflict = obj.VEconflict();
    _ratioR     = obj.ratioR();
}


//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------

//
//  LevelMiddle::~LevelMiddle --    destructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
LevelMiddle::~LevelMiddle( void )
{
    clearGraph( _boundary );

    //_shortestPathM.clear();
    _line.clear();
    _lineSta.clear();

    _nLines = 0;
    _nVertices  = 0;
    _nEdges  = 0;
    _meanVSize = 0.0;

    _removedVertices.clear();
    _removedEdges.clear();
    _removedWeights.clear();

    //_VEconflict.clear();
    _ratioR.clear();
}

//
//  LevelMiddle::clear --    clear the current LevelMiddle information
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void LevelMiddle::clear( void )
{
    clearGraph( _boundary );

    //_shortestPathM.clear();
    _line.clear();
    _lineSta.clear();

    _nLines = 0;
    _nVertices  = 0;
    _nEdges  = 0;
    _meanVSize = 0.0;

    _removedVertices.clear();
    _removedEdges.clear();
    _removedWeights.clear();

    _VEconflict.clear();
    _ratioR.clear();
}


// end of header file
// Do not add any stuff under this line.
