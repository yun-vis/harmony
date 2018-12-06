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

bool Road::_findClosestVertexInRoad( Coord2 &coord, UndirectedBaseGraph::vertex_descriptor &target )
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

    if( dist == INFINITY ){
        return false;
    }
    else{

        _road[target].flag = true;
        return true;
    }
}

void Road::_findShortestPaths( void )
{
    vector< MetaboliteGraph >   &subg   = _pathway->subG();
    unsigned int nSystems = subg.size();

    for( unsigned int i = 0; i < nSystems; i++ ) {
        for( unsigned int j = 0; j < nSystems; j++ ){
            if( ( i < j ) && ( _highwayMat[i][j].common.size() > 0 ) && ( _highwayMat[j][i].common.size() > 0 ) ) {

                //UndirectedBaseGraph::vertex_descriptor vd = vertex( 0, _road );
                vector< double > distances( num_vertices( _road ) );
                vector< UndirectedBaseGraph::vertex_descriptor > predecessor( num_vertices( _road ) );
                dijkstra_shortest_paths( _road, _highwayMat[j][i].routerVD,
                                         vertex_index_map( get( &BaseVertexProperty::id, _road ) )                                                              
                                         .predecessor_map( make_iterator_property_map( predecessor.begin(),
                                                                                   get( &BaseVertexProperty::id, _road )))
                                         .distance_map( make_iterator_property_map( distances.begin(),
                                                                                    get( &BaseVertexProperty::id, _road )))
                                         .weight_map( get( &BaseEdgeProperty::weight, _road ) )
                                                                                   );
                
                UndirectedBaseGraph::vertex_descriptor vdTarget = _highwayMat[i][j].routerVD;
                while( true ){

                    if( vdTarget == _highwayMat[j][i].routerVD ){
                        break;
                    }
                    else{
                        _highwayMat[i][j].path.push_back( vdTarget );
                        vdTarget = predecessor[ _road[ vdTarget ].id ];
                    }
                }
                _highwayMat[i][j].path.push_back( vdTarget );

//#ifdef DEBUG

                cerr << "( " << _road[_highwayMat[i][j].routerVD].id
                     << ", " << _road[_highwayMat[j][i].routerVD].id << " ): ";
                for( unsigned int k = 0; k < _highwayMat[i][j].path.size(); k++ ){
                    cerr << _road[ _highwayMat[i][j].path[k] ].id << " ";
                }
                cerr << endl;
//#endif // DEBUG
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

    printGraph( _road );
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

        map< string, int >  count;

        // aliases in the subsystem
        map< MetaboliteGraph::vertex_descriptor, MetaboliteGraph::vertex_descriptor > aliasVDMapO;
        BGL_FORALL_VERTICES( vd, subg[i], MetaboliteGraph ) {
            if( subg[i][vd].isAlias == true ){
                aliasVDMapO.insert( pair< MetaboliteGraph::vertex_descriptor, MetaboliteGraph::vertex_descriptor >( vd, vd ) );
            }
        }

#ifdef DEBUG
        cerr << i << ": mapO: " << endl;
        map< MetaboliteGraph::vertex_descriptor, MetaboliteGraph::vertex_descriptor >::iterator itO;
        for( itO = aliasVDMapO.begin(); itO != aliasVDMapO.end(); itO++ ){
            cerr << *subg[i][ itO->first ].namePtr << ", ";
        }
        cerr << endl << endl;
#endif // DEBUG

        for( unsigned int j = i+1; j < nSystems; j++ ){

            // aliases in the subsystem
            map< MetaboliteGraph::vertex_descriptor, MetaboliteGraph::vertex_descriptor > aliasVDMapI;
            BGL_FORALL_VERTICES( vd, subg[j], MetaboliteGraph ) {

                if( subg[j][vd].isAlias == true ){
                    aliasVDMapI.insert( pair< MetaboliteGraph::vertex_descriptor, MetaboliteGraph::vertex_descriptor >( vd, vd ) );
                }
            }

#ifdef DEBUG
            cerr << j << ": mapI: " << endl;
            map< MetaboliteGraph::vertex_descriptor, MetaboliteGraph::vertex_descriptor >::iterator itI;
            for( itI = aliasVDMapI.begin(); itI != aliasVDMapI.end(); itI++ ){
                cerr << *subg[j][ itI->first ].namePtr << ", ";
            }
            cerr << endl << endl;
#endif // DEBUG

            map< MetaboliteGraph::vertex_descriptor, MetaboliteGraph::vertex_descriptor > commonIJMap;
            map< MetaboliteGraph::vertex_descriptor, MetaboliteGraph::vertex_descriptor > commonJIMap;
            map< MetaboliteGraph::vertex_descriptor, MetaboliteGraph::vertex_descriptor >::iterator itO;
            for( itO = aliasVDMapO.begin(); itO != aliasVDMapO.end(); itO++ ){

                map< MetaboliteGraph::vertex_descriptor, MetaboliteGraph::vertex_descriptor >::iterator itI;
                for( itI = aliasVDMapI.begin(); itI != aliasVDMapI.end(); itI++ ){

#ifdef DEBUG
                    cerr << *subg[i][ itO->first ].namePtr << " ?= " << *subg[j][ itI->first ].namePtr;
                    cerr << " eq = " << subg[i][ itO->first ].namePtr->compare( *subg[j][ itI->first ].namePtr );
                    cerr << " eq2 = " << ( *subg[i][ itO->first ].namePtr == *subg[j][ itI->first ].namePtr ) << endl;
#endif // DEBUG
                    if ( *subg[i][ itO->first ].namePtr == *subg[j][ itI->first ].namePtr ) {
                        commonIJMap.insert( pair< MetaboliteGraph::vertex_descriptor,
                                                  MetaboliteGraph::vertex_descriptor >( itO->first,
                                                                                        itO->first ) );
                        commonJIMap.insert( pair< MetaboliteGraph::vertex_descriptor,
                                            MetaboliteGraph::vertex_descriptor >( itI->first,
                                                                                  itI->first ) );
                    }
                }
            }

            map< UndirectedBaseGraph::vertex_descriptor,
                 UndirectedBaseGraph::vertex_descriptor >::iterator itC;
            Coord2 centerIJ( 0.0, 0.0 );
            Coord2 centerJI( 0.0, 0.0 );
            for( itC = commonIJMap.begin(); itC != commonIJMap.end(); itC++ ){
                centerIJ += *subg[i][ itC->first ].coordPtr;
                //cerr << *mg[ itC->first ].coordPtr << ", ";
            }
            for( itC = commonJIMap.begin(); itC != commonJIMap.end(); itC++ ){
                centerJI += *subg[j][ itC->first ].coordPtr;
                //cerr << *mg[ itC->first ].coordPtr << ", ";
            }

            _highwayMat[i][j].common = commonIJMap;
            _highwayMat[j][i].common = commonJIMap;
            _highwayMat[i][j].center = centerIJ/(double)commonIJMap.size();
            _highwayMat[j][i].center = centerJI/(double)commonJIMap.size();

#ifdef DEBUG
            cerr << "**********************" << endl << endl;
            cerr << "common( " << i << ", " << j << " ): ";
            for( itC = commonIJMap.begin(); itC != commonIJMap.end(); itC++ ){
                cerr << *subg[i][ itC->first ].namePtr << ", ";
            }
            cerr << endl;
            cerr << "common( " << j << ", " << i << " ): ";
            for( itC = commonJIMap.begin(); itC != commonJIMap.end(); itC++ ){
                cerr << *subg[j][ itC->first ].namePtr << ", ";
            }
            cerr << endl;
            cerr << "**********************" << endl << endl;
#endif // DEBUG
        }
    }

    // find routers
    for( unsigned int i = 0; i < nSystems; i++ ) {
        for( unsigned int j = 0; j < nSystems; j++ ){
            if( ( i != j ) && ( _highwayMat[i][j].common.size() > 0 ) ){

                UndirectedBaseGraph::vertex_descriptor vd;
                //Coord2 c = _highwayMat[i][j].center / (double)_highwayMat[i][j].common.size();
                bool found = _findClosestVertexInRoad( _highwayMat[i][j].center, vd );
                assert( found );
                _highwayMat[i][j].routerVD = vd;
#ifdef DEBUG
                cerr << "_highwayMat[" << i << "][" << j << "].center = " << _highwayMat[i][j].center;
                cerr << " routerVD = " << _road[vd].id << endl;
#endif // DEBUG
            }
        }
    }

    _findShortestPaths();

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
