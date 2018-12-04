//==============================================================================
// Metro.cc
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


#include <boost/graph/max_cardinality_matching.hpp>

#include "base/Road.h"

//------------------------------------------------------------------------------
//	Protected functions
//------------------------------------------------------------------------------
bool Road::_findVertexInRoad( Coord2 &coord, UndirectedBaseGraph::vertex_descriptor &target )
{
    bool isFound = false;
    BGL_FORALL_VERTICES( vd, _road, UndirectedBaseGraph )
    {
        if( ( coord - *_road[vd].coordPtr ).norm() < 1e-2 ){
            target = vd;
            isFound = true;
        }
    }

    //cerr << "isFound = " << isFound << endl;
    return isFound;
}

//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------
//
//  Road::init -- initialize
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Road::_init( vector< multimap< int, CellComponent > > & cellComponentVec )
{
    _clear();

    unsigned int nVertices = 0, nEdges = 0;
    for( unsigned int i = 0; i < cellComponentVec.size(); i++ ){

        multimap< int, CellComponent > & cellComponentMap = cellComponentVec[i];
        multimap< int, CellComponent >::iterator itC;
        for( itC = cellComponentMap.begin(); itC != cellComponentMap.end(); itC++ ){

            CellComponent &component = itC->second;
            Polygon2 &contour = component.contour;

            vector< UndirectedBaseGraph::vertex_descriptor > vdVec;
            for( unsigned j = 0; j < contour.elements().size(); j++ ){

                Coord2 coord( contour.elements()[j].x(),
                              contour.elements()[j].y() );
                UndirectedBaseGraph::vertex_descriptor vd = NULL;
                bool isFound = _findVertexInRoad( coord, vd );

                if( isFound ){
                    vdVec.push_back( vd );
                }
                else{

                    UndirectedBaseGraph::vertex_descriptor vdNew = add_vertex( _road );
                    _road[ vdNew ].id = nVertices;
                    _road[ vdNew ].coordPtr = new Coord2( coord.x(), coord.y() );

                    vdVec.push_back( vdNew );
                    nVertices++;
                }
            }

            // cerr << "numV = " << num_vertices( _road ) << endl;
            for( unsigned j = 0; j < vdVec.size(); j++ ){

                UndirectedBaseGraph::vertex_descriptor vdS = vdVec[ j ];
                UndirectedBaseGraph::vertex_descriptor vdT = vdVec[ (j+1)%vdVec.size() ];

                bool found = false;
                UndirectedBaseGraph::edge_descriptor oldED;
                tie( oldED, found ) = edge( vdT, vdT, _road );

                if( found == false ){
                    pair< UndirectedBaseGraph::edge_descriptor, unsigned int > foreE = add_edge( vdS, vdT, _road );
                    UndirectedBaseGraph::edge_descriptor foreED = foreE.first;
                    _road[ foreED ].id = nEdges;

                    nEdges++;
                }
            }
        }
    }
}


//
//  Road::clear --    clear the current Road information
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Road::_clear( void )
{
    clearGraph( _road );
}


//
//  Road::findConnectedComponent -- find connected component
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Road::buildRoad( void )
{
}


//
//  Road::Road -- default constructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
Road::Road( void )
{
    ;
}

//
//  Road::Road -- copy constructor
//
//  Inputs
//  obj : object of this class
//
//  Outputs
//  none
//
Road::Road( const Road & obj )
{
    ;
}


//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------
//
//  Road::~Road --    destructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
Road::~Road( void )
{
    ;
}


// end of header file
// Do not add any stuff under this line.
