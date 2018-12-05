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

void Road::_findClosestVertexInRoad( Coord2 &coord, UndirectedBaseGraph::vertex_descriptor &target )
{
    double dist = INFINITY;
    BGL_FORALL_VERTICES( vd, _road, UndirectedBaseGraph )
    {
        double distance = ( coord - *_road[vd].coordPtr ).norm();
        if( ( _road[vd].flag == false ) && ( distance < dist ) ){
            target = vd;
            dist = distance;
        }
    }

    _road[target].flag = true;
}

void Road::_findShortestPaths( void )
{
    vector< MetaboliteGraph >   &subg   = _pathway->subG();
    unsigned int nSystems = subg.size();

    for( unsigned int i = 0; i < nSystems; i++ ) {
        for( unsigned int j = 0; j < nSystems; j++ ){
            if( ( i != j ) && ( _highwayMat[i][j].count > 0 ) ) {

                //UndirectedBaseGraph::vertex_descriptor vd = vertex( 0, _road );
                vector<double> distances( num_vertices( _road ) );
                dijkstra_shortest_paths( _road, _highwayMat[i][j].routerVD,
                                         vertex_index_map( get( &BaseVertexProperty::id, _road ) )
                                        .weight_map( get( &BaseEdgeProperty::weight, _road ) )
                                        .distance_map( make_iterator_property_map( distances.begin(),
                                                                                   get( &BaseVertexProperty::id, _road ))));
            }
        }
    }
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
                    _road[ foreED ].weight = (*_road[ vdS ].coordPtr - *_road[ vdT ].coordPtr).norm();

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
    vector< MetaboliteGraph >   &subg   = _pathway->subG();
    unsigned int nSystems = subg.size();

    // initialization
    _highwayMat.resize( nSystems );
    for( unsigned int i = 0; i < nSystems; i++ ){
        _highwayMat[i].resize( nSystems );
    }
    BGL_FORALL_VERTICES( vd, _road, UndirectedBaseGraph ) {
        _road[vd].flag = false;
    }

    // compute highway
    for( unsigned int i = 0; i < nSystems; i++ ){

        MetaboliteGraph & mg = subg[i];
        map< string, int >  count;

        BGL_FORALL_VERTICES( vd, mg, MetaboliteGraph ) {

            if( mg[vd].isAlias == true ){

                map< string, int >::iterator itA = count.find( *mg[ vd ].namePtr );
                if( itA != count.end() ){
                    itA->second++;
                }
                else{
                    count.insert( pair< string, int >( *mg[ vd ].namePtr, 1 ) );
                }
            }
        }

        map< string, int >::iterator itC = count.begin();
        for( itC = count.begin(); itC != count.end(); itC++ ){
            cerr << itC->first << " = " << itC->second << endl;
        }
        cerr << endl;

        for( unsigned int j = 0; j < nSystems; j++ ){

            BGL_FORALL_VERTICES( vd, subg[j], MetaboliteGraph ) {

                if( subg[j][vd].isAlias == true ){

                    map< string, int >::iterator itC = count.begin();
                    for( itC = count.begin(); itC != count.end(); itC++ ){

                        if( *subg[j][vd].namePtr == itC->first ){
                            _highwayMat[i][j].count += itC->second;
                            _highwayMat[i][j].center += *subg[j][vd].coordPtr;
                        }
                    }
                }
            }
        }
    }

    for( unsigned int i = 0; i < nSystems; i++ ) {
        for( unsigned int j = 0; j < nSystems; j++ ){
            if( ( i != j ) && ( _highwayMat[i][j].count > 0 ) ){

                UndirectedBaseGraph::vertex_descriptor vd;
                Coord2 c = _highwayMat[i][j].center / _highwayMat[i][j].count;
                _findClosestVertexInRoad( c, vd );
                _highwayMat[i][j].routerVD = vd;
            }
        }
    }

#ifdef DEBUG
    for( unsigned int i = 0; i < nSystems; i++ ) {
        for( unsigned int j = 0; j < nSystems; j++ ){
            cerr << setw(5) << _highwayMat[i][j].count;
        }
        cerr << endl;
    }
    cerr << endl;
#endif // DEBUG
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
