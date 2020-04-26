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
#include "steinertree/include.h"

//------------------------------------------------------------------------------
//	Protected functions
//------------------------------------------------------------------------------
bool Road::_findVertexInTree( Coord2 &coord, UndirectedBaseGraph::vertex_descriptor &target )
{
    UndirectedBaseGraph & g = _curvytree.tree();

    bool isFound = false;
    BGL_FORALL_VERTICES( vd, g, UndirectedBaseGraph )
    {
        if( ( coord - *g[vd].coordPtr ).norm() < 1e-2 ){
            target = vd;
            isFound = true;
        }
    }

    //cerr << "isFound = " << isFound << endl;
    return isFound;
}

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

bool Road::_findClosestVertexInLane( Coord2 &coord, UndirectedBaseGraph::vertex_descriptor &target )
{
    double dist = INFINITY;
    BGL_FORALL_VERTICES( vd, _road, UndirectedBaseGraph )
    {
        double distance = ( coord - *_road[vd].coordPtr ).norm();
        if( distance < dist ){
            target = vd;
            dist = distance;
        }
    }

    if( dist == INFINITY ){
        return false;
    }
    return true;
}

bool Road::_findClosestVertexInRoad( Coord2 &coord, UndirectedBaseGraph::vertex_descriptor &target )
{
    double dist = INFINITY;
    double minDist = INFINITY;
    UndirectedBaseGraph::vertex_descriptor minTarget;
    BGL_FORALL_VERTICES( vd, _road, UndirectedBaseGraph )
    {
        double distance = ( coord - *_road[vd].coordPtr ).norm();
        if( ( _road[vd].flag == false ) && ( distance < dist ) ){
            target = vd;
            dist = distance;
        }
        if( distance < dist ){
            minTarget = vd;
            minDist = distance;
        }
    }

    if( dist == INFINITY ){
        target = minTarget;
    }
    else{
        _road[target].flag = true;
    }
    return true;
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

#ifdef DEBUG

                cerr << "( " << _road[_highwayMat[i][j].routerVD].id
                     << ", " << _road[_highwayMat[j][i].routerVD].id << " ): ";
                for( unsigned int k = 0; k < _highwayMat[i][j].path.size(); k++ ){
                    cerr << _road[ _highwayMat[i][j].path[k] ].id << " ";
                }
                cerr << endl;
#endif // DEBUG
            }
        }
    }
}

bool Road::_findVertexOnLine( Coord2 &coord, UndirectedBaseGraph::edge_descriptor &edT )
{
    BGL_FORALL_EDGES( ed, _road, UndirectedBaseGraph ) {

        UndirectedBaseGraph::vertex_descriptor vdS = source( ed, _road );
        UndirectedBaseGraph::vertex_descriptor vdT = target( ed, _road );

        Coord2 &coordS = *_road[ vdS ].coordPtr;
        Coord2 &coordT = *_road[ vdT ].coordPtr;

        bool isFound = Line2::isOnLine( coord, coordS, coordT );
        if( isFound == true ){
            edT = ed;
            return true;
        }
    }

    return false;
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
/*
void Road::_initLane( unsigned int gid, int selectedID,
                      multimap< int, CellComponent > & cellComponent,
                      vector < Highway > *highwayRoadPtr )
{
    vector< MetaboliteGraph >   &subg       = _pathway->subG();
    vector< ForceGraph >        &lsubg      = _pathway->lsubG();
    map < unsigned int , vector < UndirectedBaseGraph::vertex_descriptor > > polygonVD;

    _gid = gid;
    // cerr << endl << endl;

    unsigned int nVertices = 0, nEdges = 0;

#ifdef DEBUG
    BGL_FORALL_VERTICES( vd, lsubg[_gid], ForceGraph ){
        cerr << "vid = " << lsubg[_gid][vd].id
             << " initID = " << lsubg[_gid][vd].initID  << endl;
    }
#endif // DEBUG

    // build terminals
    vector< UndirectedBaseGraph::vertex_descriptor > gates;
    _terminalVec.resize( highwayRoadPtr->size() );
    for( unsigned int i = 0; i < _terminalVec.size(); i++ ){

        _terminalVec[i].common = (*highwayRoadPtr)[i].common;

        // cerr << "size = " << _terminalVec[i].common.size() << endl;

        map< UndirectedBaseGraph::vertex_descriptor,
                UndirectedBaseGraph::vertex_descriptor >::iterator it;
        for( it = _terminalVec[i].common.begin();
             it != _terminalVec[i].common.end(); it++ ){

            // add vertices
            Coord2 coord( subg[_gid][it->first].coordPtr->x(),
                          subg[_gid][it->first].coordPtr->y() );

            // cerr << "_gid = " << _gid << " gateID = " << subg[_gid][it->first].id << endl;
            // cerr << "_gid = " << _gid << " gateinitID = " << lsubg[_gid][ vertex( subg[_gid][it->first].id,lsubg[_gid] ) ].id << endl;
            UndirectedBaseGraph::vertex_descriptor vdNew;
            bool isFound = _findVertexInRoad( coord, vdNew );
            if( isFound ){
                // gates.push_back( vd );
            }
            else{
                vdNew = add_vertex( _road );
                _road[ vdNew ].id = nVertices;
                _road[ vdNew ].initID = subg[_gid][it->first].id;      // record poygon ID
                _road[ vdNew ].coordPtr = new Coord2( coord.x(), coord.y() );

                // vdVec.push_back( vdNew );
                nVertices++;
                gates.push_back( vdNew );
            }

            _terminalVec[i].commonGates.insert( pair< MetaboliteGraph::vertex_descriptor,
                    UndirectedBaseGraph::vertex_descriptor >( it->first, vdNew ) );
            _terminalVec[i].terminals.insert( pair< UndirectedBaseGraph::vertex_descriptor,
                    UndirectedBaseGraph::vertex_descriptor >( vdNew, vdNew ) );
        }

#ifdef DEBUG
        map< UndirectedBaseGraph::vertex_descriptor,
                UndirectedBaseGraph::vertex_descriptor >::iterator it;
        for( it = _terminalVec[i].common.begin();
             it != _terminalVec[i].common.end(); it++ ){
            cerr << *_pathway->g()[ it->first ].namePtr << ", ";
        }
        cerr << endl;
#endif // DEBUG
    }
    // cerr << "nVertices = " << num_vertices( _road ) << endl;

    // build lane network
    // clean up vertices on a straight edge segment, maybe move to the code after octilinear optimization
    multimap< int, CellComponent >::iterator itC;
    for( itC = cellComponent.begin(); itC != cellComponent.end(); itC++ ){

        vector< Seed > &seedVec = *itC->second.detail.forceBone().voronoi().seedVec();

        for( unsigned int i = 0; i < seedVec.size(); i++ ){

            Polygon2 &p = seedVec[i].cellPolygon;
            vector< Coord2 > coordVec;
            for( unsigned int j = 1; j <= p.elements().size(); j++ ) {

                Coord2 &prev = p.elements()[j - 1];
                Coord2 &curr = p.elements()[(j)%(int)p.elements().size()];
                Coord2 &next = p.elements()[(j + 1)%(int)p.elements().size()];

                // cerr << "j-1 = " << j-1 << " j = " << (j)%p.elements().size() << " j+1 = " << (j+1)%p.elements().size() << endl;
                bool isOnLine = Line2::isOnLine(curr, prev, next);
                if (isOnLine == false) {
                    coordVec.push_back( curr );
                }
            }
            // cerr << endl;
            p.elements().clear();
            p.elements() = coordVec;
        }
    }

    // add vertices
    for( itC = cellComponent.begin(); itC != cellComponent.end(); itC++ ){

        vector< Seed > &seedVec = *itC->second.detail.forceBone().voronoi().seedVec();

        for( unsigned int i = 0; i < seedVec.size(); i++ ){

            vector < UndirectedBaseGraph::vertex_descriptor > polygons;
            Polygon2 &p = seedVec[i].cellPolygon;
            unsigned int size =  p.elements().size();

            for( unsigned int j = 0; j < size; j++ ){

                Coord2 coord( p.elements()[j].x(),
                              p.elements()[j].y() );
                UndirectedBaseGraph::vertex_descriptor vd = NULL;
                bool isFound = _findVertexInRoad( coord, vd );

                if( !isFound ){

                    UndirectedBaseGraph::vertex_descriptor vdNew = add_vertex( _road );
                    _road[ vdNew ].id = nVertices;
                    _road[ vdNew ].coordPtr = new Coord2( coord.x(), coord.y() );
                    nVertices++;

                    polygons.push_back( vdNew );
                }
                else{
                    polygons.push_back( vd );
                }
            }
            ForceGraph &fg = itC->second.detail.bone();
            ForceGraph::vertex_descriptor vd = vertex( i, fg );
            // cerr << "id = " << fg[ vd ].id << " initID = " << fg[ vd ].initID << endl;
            polygonVD.insert( pair< unsigned int, vector < UndirectedBaseGraph::vertex_descriptor > >( fg[ vd ].initID, polygons ) );
        }
    }

    // add intermediate vertices
    vector< vector< UndirectedBaseGraph::vertex_descriptor > > gatesVD;
    gatesVD.resize( gates.size() );
    for( unsigned int i = 0; i < gates.size(); i++ ){

        Coord2 &coord = *_road[ gates[i] ].coordPtr;

        unsigned int id = _road[ gates[i] ].initID;
        map < unsigned int , vector < UndirectedBaseGraph::vertex_descriptor > > ::iterator itV = polygonVD.begin();
        advance( itV, id );
        vector < UndirectedBaseGraph::vertex_descriptor > &vdVec = itV->second;
        // cerr << "vdVec.size() = " << vdVec.size() << endl;
        for( unsigned int j = 0; j < vdVec.size(); j++ ){

            Coord2 &coordM = *_road[ vdVec[j] ].coordPtr;
            Coord2 &coordN = *_road[ vdVec[(j+1)%(int)vdVec.size()] ].coordPtr;
            Coord2 mnVec = coordN - coordM;
            Coord2 cmVec = coord - coordM;
            double D = ( mnVec * cmVec ) / mnVec.squaredNorm();
            Coord2 coordD = coordM + D*mnVec;

            if( Line2::isOnLine( coordD, coordM, coordN )){

                // check if exist
                UndirectedBaseGraph::vertex_descriptor vd = NULL;
                bool isFound = _findVertexInRoad( coordD, vd );

                if( isFound ){
                    gatesVD[i].push_back( vd );
                }
                else {

                    // add intermediate vertices
                    UndirectedBaseGraph::vertex_descriptor vdNew = add_vertex( _road );
                    _road[ vdNew ].id = nVertices;
                    _road[ vdNew ].coordPtr = new Coord2( coordD.x(), coordD.y() );
                    nVertices++;
                    gatesVD[i].push_back( vdNew );
                }
            }
        }
    }

    // add edges
    for( itC = cellComponent.begin(); itC != cellComponent.end(); itC++ ){

        vector< Seed > &seedVec = *itC->second.detail.forceBone().voronoi().seedVec();

        for( unsigned int i = 0; i < seedVec.size(); i++ ){

            Polygon2 &p = seedVec[i].cellPolygon;
            vector < UndirectedBaseGraph::vertex_descriptor > polygons;
            unsigned int size =  p.elements().size();
            for( unsigned int j = 0; j < size; j++ ){

                Coord2 coordS( p.elements()[j].x(), p.elements()[j].y() );
                Coord2 coordT( p.elements()[ (j+1)%(int)size ].x(), p.elements()[ (j+1)%(int)size ].y() );
                UndirectedBaseGraph::vertex_descriptor vdS = NULL, vdT = NULL;
                _findVertexInRoad( coordS, vdS );
                _findVertexInRoad( coordT, vdT );

                bool found = false;
                UndirectedBaseGraph::edge_descriptor oldED;
                tie( oldED, found ) = edge( vdS, vdT, _road );

                if( found ){

                    // cerr << "idS = " << _road[ vdS ].id << " idT = " << _road[ vdT ].id << endl;
                    _road[ oldED ].visitedTimes += 1;
                    polygons.push_back( vdS );
                }
                else{

                    // collect vertices on the edges
                    vector< UndirectedBaseGraph::vertex_descriptor > vdVec;
                    BGL_FORALL_VERTICES( vd, _road, UndirectedBaseGraph ) {

                        Coord2 &coord = *_road[vd].coordPtr;
                        bool isOnLine = Line2::isOnLine( coord, coordS, coordT );
                        if( isOnLine ) vdVec.push_back( vd );

#ifdef DEBUG
                        if( _road[vdS].id == 7 && _road[vdT].id == 8 ){
                            cerr << "id = " << _road[vd].id << " isOnLine = " << isOnLine << endl;
                        }
#endif // DEBUG
                    }

                    // sort by distance
                    map< double, UndirectedBaseGraph::vertex_descriptor > vdMap;
                    for( unsigned int k = 0; k < vdVec.size(); k++ ){
                        double dist = ( *_road[ vdVec[ k ] ].coordPtr - coordS ).norm();
                        vdMap.insert( pair< double, UndirectedBaseGraph::vertex_descriptor >( dist, vdVec[k] ) );
                    }

                    // add edges
                    map< double, UndirectedBaseGraph::vertex_descriptor >::iterator itM = vdMap.begin();
                    map< double, UndirectedBaseGraph::vertex_descriptor >::iterator itN = itM;
                    itN++;
                    for( ; itN != vdMap.end(); itN++ ){

                        bool found = false;
                        UndirectedBaseGraph::edge_descriptor oldED;
                        tie( oldED, found ) = edge( itM->second, itN->second, _road );

                        // cerr << "idM = " << _road[ itM->second ].id << " idN = " << _road[ itN->second ].id << endl;

                        if( found ){
                            _road[ oldED ].visitedTimes += 1;
                            polygons.push_back( source( oldED, _road ) );
                        }
                        else{

                            pair< UndirectedBaseGraph::edge_descriptor, unsigned int > foreE = add_edge( itM->second, itN->second, _road );
                            UndirectedBaseGraph::edge_descriptor foreED = foreE.first;
                            _road[ foreED ].id = nEdges;
                            _road[ foreED ].visitedTimes = 0;
                            _road[ foreED ].weight = (*_road[ itM->second ].coordPtr - *_road[ itN->second ].coordPtr).norm();

                            polygons.push_back( source( foreED, _road ) );
                            nEdges++;
                        }
                        itM = itN;
                    }
                }
            }

            ForceGraph &fg = itC->second.detail.bone();
            ForceGraph::vertex_descriptor vd = vertex( i, fg );
            // cerr << "id = " << fg[ vd ].id << " initID = " << fg[ vd ].initID << endl;
            polygonVD.insert( pair< unsigned int, vector < UndirectedBaseGraph::vertex_descriptor > >( fg[ vd ].initID, polygons ) );
        }
    }

    // remove boundary edges
    vector< UndirectedBaseGraph::edge_descriptor > edVec;
    BGL_FORALL_EDGES( ed, _road, UndirectedBaseGraph ){

        if( _road[ ed ].visitedTimes == 0 ) {
            edVec.push_back( ed );
        }
    }
#ifdef SKIP
    for( unsigned int i = 0; i < edVec.size(); i++ ){
        remove_edge( edVec[i], _road );
    }
#endif // SKIP
    // reorder edge id
    nEdges = 0;
    BGL_FORALL_EDGES( ed, _road, UndirectedBaseGraph ){

        _road[ ed ].id = nEdges;
        nEdges++;
    }

#ifdef DEBUG
    for( unsigned int i = 0; i < polygonVD.size(); i++ ){
        cerr << "i = " << i << endl;
        for( unsigned int j = 0; j < polygonVD[i].size(); j++ ){
            cerr << _road[ polygonVD[i][j] ].id << ", ";
        }
        cerr << endl;
    }
#endif // DEBUG

    // find router and add to terminals
    for( unsigned int i = 0; i < _terminalVec.size(); i++ ) {

        UndirectedBaseGraph::vertex_descriptor minVD;
        Coord2 &coord = (*highwayRoadPtr)[i].routerCoord;

        bool found = _findClosestVertexInLane( coord, minVD );
        assert( found );

        _terminalVec[i].routerVD = minVD;

        // cerr << "minID = " << _road[minVD].id << endl;
        _terminalVec[i].terminals.insert( pair< UndirectedBaseGraph::vertex_descriptor,
                                                UndirectedBaseGraph::vertex_descriptor >( minVD, minVD ) );
    }

    // connect gates and lane
    for( unsigned int i = 0; i < gates.size(); i++ ){

        UndirectedBaseGraph::vertex_descriptor &vdG = gates[i];
        Coord2 &coordG = *_road[ vdG ].coordPtr;
        for( unsigned int j = 0; j < gatesVD[i].size(); j++ ){

            UndirectedBaseGraph::vertex_descriptor &vdI = gatesVD[i][j];
            Coord2 &coordI = *_road[ vdI ].coordPtr;

            // add edges
            pair< UndirectedBaseGraph::edge_descriptor, unsigned int > foreE = add_edge( vdG, vdI, _road );
            UndirectedBaseGraph::edge_descriptor foreED = foreE.first;
            _road[ foreED ].id = nEdges;
            _road[ foreED ].weight = (coordG - coordI).norm();
            _road[ foreED ].visitedTimes = 0;

            nEdges++;
        }

#ifdef SKIP
        UndirectedBaseGraph::vertex_descriptor minVD;
        unsigned int id = _road[ gates[i] ].initID;
        map < unsigned int , vector < UndirectedBaseGraph::vertex_descriptor > > ::iterator itV = polygonVD.begin();
        advance( itV, id );
        vector < UndirectedBaseGraph::vertex_descriptor > &vdVec = itV->second;
        for( unsigned int j = 0; j < vdVec.size(); j++ ){

            Coord2 &coordM = *_road[ vdVec[j] ].coordPtr;
            Coord2 &coordN = *_road[ vdVec[(j+1)%vdVec.size()] ].coordPtr;
            Coord2 mnVec = coordN - coordM;
            Coord2 cmVec = coord - coordM;
            double D = ( mnVec * cmVec ) / mnVec.squaredNorm();
            Coord2 coordD = coordM + D*mnVec;

            if( Line2::isOnLine( coordD, coordM, coordN )){

                // check if exist
                UndirectedBaseGraph::vertex_descriptor vd = NULL;
                bool isFound = _findVertexInRoad( coordD, vd );

                if( isFound ){
                    // cerr << "gateID = " << _road[ gates[i] ].id << " vID = " << _road[ vd ] << endl;
                }
                else{

                    // add intermediate vertices
                    UndirectedBaseGraph::vertex_descriptor vdNew = add_vertex( _road );
                    _road[ vdNew ].id = nVertices;
                    _road[ vdNew ].coordPtr = new Coord2( coordD.x(), coordD.y() );
                    nVertices++;

                    // split the edge into two segments
                    bool found = false;
                    UndirectedBaseGraph::edge_descriptor oldED;
                    tie( oldED, found ) = edge( vdVec[j], vdVec[(j+1)%vdVec.size()], _road );
                    //assert( found );

                    // 1st segment
                    pair< UndirectedBaseGraph::edge_descriptor, unsigned int > foreE = add_edge( vdVec[j], vdNew, _road );
                    UndirectedBaseGraph::edge_descriptor foreED = foreE.first;
                    _road[ foreED ].id = _road[ oldED ].id;
                    _road[ foreED ].weight = (coordM - coordD).norm();
                    _road[ foreED ].visitedTimes = 0;

                    // 2nd segment
                    foreE = add_edge( vdVec[(j+1)%vdVec.size()], vdNew, _road );
                    foreED = foreE.first;
                    _road[ foreED ].id = nEdges;
                    _road[ foreED ].weight = (coordN - coordD).norm();
                    _road[ foreED ].visitedTimes = 0;
                    nEdges++;

                    //remove_edge( oldED, _road );

                    // add edges
                    foreE = add_edge( gates[i], vdNew, _road );
                    foreED = foreE.first;
                    _road[ foreED ].id = nEdges;
                    _road[ foreED ].weight = (coord - coordD).norm();
                    _road[ foreED ].visitedTimes = 0;

                    nEdges++;
                }
            }
        }
#endif // SKIP
    }
}
*/

//
//  Road::init -- initialize
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Road::_initSteinerNet( vector< multimap< int, CellComponent > > & cellComponentVec,
                            int selectedID )
{
    _selectedID = selectedID;
    vector< MetaboliteGraph >   &subG    = _pathway->subG();
    vector< ForceGraph >        &lsubg   = _pathway->lsubG();
    unsigned int nVertices = 0, nEdges = 0;
    _citeVec.clear();

#ifdef DEBUG
    BGL_FORALL_VERTICES( vd, lsubg[_gid], ForceGraph ){
        cerr << "vid = " << lsubg[_gid][vd].id
             << " initID = " << lsubg[_gid][vd].initID  << endl;
    }
#endif // DEBUG

    for( unsigned int m = 0; m < cellComponentVec.size(); m++ ){

        multimap< int, CellComponent > &cellComponent = cellComponentVec[m];
        map < unsigned int , vector < UndirectedBaseGraph::vertex_descriptor > > polygonVD;
        vector< UndirectedBaseGraph::vertex_descriptor > gates;

        // build lane network
        // clean up vertices on a straight edge segment, maybe move to the code after octilinear optimization
        multimap< int, CellComponent >::iterator itC;
        for( itC = cellComponent.begin(); itC != cellComponent.end(); itC++ ){

            vector< Seed > &seedVec = *itC->second.detail.forceBone().voronoi().seedVec();

            for( unsigned int i = 0; i < seedVec.size(); i++ ){

                Polygon2 &p = seedVec[i].cellPolygon;
                vector< Coord2 > coordVec;
                for( unsigned int j = 1; j <= p.elements().size(); j++ ) {

                    Coord2 &prev = p.elements()[j - 1];
                    Coord2 &curr = p.elements()[(j)%(int)p.elements().size()];
                    Coord2 &next = p.elements()[(j + 1)%(int)p.elements().size()];

                    // cerr << "j-1 = " << j-1 << " j = " << (j)%p.elements().size() << " j+1 = " << (j+1)%p.elements().size() << endl;
                    bool isOnLine = Line2::isOnLine(curr, prev, next);
                    if (isOnLine == false) {
                        coordVec.push_back( curr );
                    }
                }
                // cerr << endl;
                p.elements().clear();
                p.elements() = coordVec;
            }
        }

        // add selected
        BGL_FORALL_VERTICES( vd, subG[m], MetaboliteGraph ) {

            if( ( *subG[m][vd].isSelectedPtr == true ) && ( *subG[m][vd].selectedIDPtr == _selectedID ) ){

                Coord2 &coord = *subG[m][vd].coordPtr;

                UndirectedBaseGraph::vertex_descriptor vdNew = add_vertex( _road );
                _road[ vdNew ].id = nVertices;
                _road[ vdNew ].initID = subG[m][vd].id;
                _road[ vdNew ].coordPtr = new Coord2( coord.x(), coord.y() );
                gates.push_back( vdNew );
                _citeVec.push_back( vdNew );
                //cerr << "isselected = " << *subG[m][vd].isSelectedPtr << endl;
                //cerr << "vid = " << _road[ vdNew ].id << " initID = " << _road[ vdNew ].initID << endl;
                nVertices++;
            }
        }

        // add vertices
        for( itC = cellComponent.begin(); itC != cellComponent.end(); itC++ ){

            vector< Seed > &seedVec = *itC->second.detail.forceBone().voronoi().seedVec();

            for( unsigned int i = 0; i < seedVec.size(); i++ ){

                vector < UndirectedBaseGraph::vertex_descriptor > polygons;
                Polygon2 &p = seedVec[i].cellPolygon;
                unsigned int size =  p.elements().size();

                for( unsigned int j = 0; j < size; j++ ){

                    Coord2 coord( p.elements()[j].x(),
                                  p.elements()[j].y() );
                    UndirectedBaseGraph::vertex_descriptor vd = NULL;
                    bool isFound = _findVertexInRoad( coord, vd );

                    if( !isFound ){

                        UndirectedBaseGraph::vertex_descriptor vdNew = add_vertex( _road );
                        _road[ vdNew ].id = nVertices;
                        _road[ vdNew ].coordPtr = new Coord2( coord.x(), coord.y() );
                        nVertices++;

                        polygons.push_back( vdNew );
                    }
                    else{
                        polygons.push_back( vd );
                    }
                }
                ForceGraph &fg = itC->second.detail.bone();
                ForceGraph::vertex_descriptor vd = vertex( i, fg );
                //cerr << "id = " << fg[ vd ].id << " initID = " << fg[ vd ].initID << endl;
#ifdef DEBUG
                for( unsigned int i = 0; i < polygons.size(); i++ ){
                    cerr << _road[polygons[i]].id << ",";
                }
                cerr << endl;
#endif // DEBUG
                polygonVD.insert( pair< unsigned int, vector < UndirectedBaseGraph::vertex_descriptor > >( fg[ vd ].initID, polygons ) );
            }
        }

        // add intermediate vertices
        vector< vector< UndirectedBaseGraph::vertex_descriptor > > gatesVD;
        gatesVD.resize( gates.size() );
        for( unsigned int i = 0; i < gates.size(); i++ ){

            Coord2 &coord = *_road[ gates[i] ].coordPtr;

            unsigned int id = _road[ gates[i] ].initID;
            map < unsigned int , vector < UndirectedBaseGraph::vertex_descriptor > > ::iterator itV = polygonVD.find( id );

            vector < UndirectedBaseGraph::vertex_descriptor > &vdVec = itV->second;
            //cerr << "vdVec.size() = " << vdVec.size() << endl;
            for( unsigned int j = 0; j < vdVec.size(); j++ ){

                //cerr << _road[ vdVec[j] ].id << ", ";
                Coord2 &coordM = *_road[ vdVec[j] ].coordPtr;
                Coord2 &coordN = *_road[ vdVec[(j+1)%(int)vdVec.size()] ].coordPtr;
                Coord2 mnVec = coordN - coordM;
                Coord2 cmVec = coord - coordM;
                double D = ( mnVec * cmVec ) / mnVec.squaredNorm();
                Coord2 coordD = coordM + D*mnVec;

                if( Line2::isOnLine( coordD, coordM, coordN )){

                    // check if exist
                    UndirectedBaseGraph::vertex_descriptor vd = NULL;
                    bool isFound = _findVertexInRoad( coordD, vd );

                    if( isFound ){
                        gatesVD[i].push_back( vd );
                    }
                    else {

                        // add intermediate vertices
                        UndirectedBaseGraph::vertex_descriptor vdNew = add_vertex( _road );
                        _road[ vdNew ].id = nVertices;
                        _road[ vdNew ].coordPtr = new Coord2( coordD.x(), coordD.y() );
                        nVertices++;
                        gatesVD[i].push_back( vdNew );
                    }
                }
            }
            //cerr << endl << "size = " << gatesVD[i].size() << endl;
        }

        // add edge to the intermediate vertex
        for( unsigned int i = 0; i < gates.size(); i++ ) {

            UndirectedBaseGraph::vertex_descriptor vdS = gates[i];
            for( unsigned int j = 0; j < gatesVD[i].size(); j++ ) {

                UndirectedBaseGraph::vertex_descriptor vdT = gatesVD[i][j];

                pair< UndirectedBaseGraph::edge_descriptor, unsigned int > foreE = add_edge( vdS, vdT, _road );
                UndirectedBaseGraph::edge_descriptor foreED = foreE.first;
                _road[ foreED ].id = nEdges;
                _road[ foreED ].visitedTimes = 0;
                _road[ foreED ].weight = (*_road[ vdS ].coordPtr - *_road[ vdT ].coordPtr).norm();

                nEdges++;
            }
        }

        // cerr << "here" << endl;

        // add edges
        for( itC = cellComponent.begin(); itC != cellComponent.end(); itC++ ){

            vector< Seed > &seedVec = *itC->second.detail.forceBone().voronoi().seedVec();

            for( unsigned int i = 0; i < seedVec.size(); i++ ){

                Polygon2 &p = seedVec[i].cellPolygon;
                vector < UndirectedBaseGraph::vertex_descriptor > polygons;
                unsigned int size =  p.elements().size();
                for( unsigned int j = 0; j < size; j++ ){

                    Coord2 coordS( p.elements()[j].x(), p.elements()[j].y() );
                    Coord2 coordT( p.elements()[ (j+1)%(int)size ].x(), p.elements()[ (j+1)%(int)size ].y() );
                    UndirectedBaseGraph::vertex_descriptor vdS = NULL, vdT = NULL;
                    _findVertexInRoad( coordS, vdS );
                    _findVertexInRoad( coordT, vdT );

                    bool found = false;
                    UndirectedBaseGraph::edge_descriptor oldED;
                    tie( oldED, found ) = edge( vdS, vdT, _road );

                    if( found ){

                        // cerr << "idS = " << _road[ vdS ].id << " idT = " << _road[ vdT ].id << endl;
                        _road[ oldED ].visitedTimes += 1;
                        polygons.push_back( vdS );
                    }
                    else{


                        // collect vertices on the edges
                        vector< UndirectedBaseGraph::vertex_descriptor > vdVec;
                        BGL_FORALL_VERTICES( vd, _road, UndirectedBaseGraph ) {

                                Coord2 &coord = *_road[vd].coordPtr;
                                bool isOnLine = Line2::isOnLine( coord, coordS, coordT );
                                if( isOnLine && (coord!=coordS) && (coord!=coordT) ) vdVec.push_back( vd );

#ifdef DEBUG
                            if( _road[vdS].id == 7 && _road[vdT].id == 8 ){
                                cerr << "id = " << _road[vd].id << " isOnLine = " << isOnLine << endl;
                            }
#endif // DEBUG
                        }

                        if( vdVec.size() == 0 ){

                            bool found = false;
                            UndirectedBaseGraph::edge_descriptor oldED;
                            tie( oldED, found ) = edge( vdS, vdT, _road );

                            // cerr << "idM = " << _road[ itM->second ].id << " idN = " << _road[ itN->second ].id << endl;

                            if( found ){
                                _road[ oldED ].visitedTimes += 1;
                                polygons.push_back( source( oldED, _road ) );
                            }
                            else{

                                pair< UndirectedBaseGraph::edge_descriptor, unsigned int > foreE = add_edge( vdS, vdT, _road );
                                UndirectedBaseGraph::edge_descriptor foreED = foreE.first;
                                _road[ foreED ].id = nEdges;
                                _road[ foreED ].visitedTimes = 0;
                                _road[ foreED ].weight = (*_road[ vdS ].coordPtr - *_road[ vdT ].coordPtr).norm();

                                polygons.push_back( source( foreED, _road ) );
                                nEdges++;
                            }

                        }
                        else{

                            // sort by distance
                            vdVec.push_back( vdS );
                            vdVec.push_back( vdT );
                            map< double, UndirectedBaseGraph::vertex_descriptor > vdMap;
                            for( unsigned int k = 0; k < vdVec.size(); k++ ){
                                double dist = ( *_road[ vdVec[ k ] ].coordPtr - coordS ).norm();
                                vdMap.insert( pair< double, UndirectedBaseGraph::vertex_descriptor >( dist, vdVec[k] ) );
                            }

                            // add edges
                            map< double, UndirectedBaseGraph::vertex_descriptor >::iterator itM = vdMap.begin();
                            map< double, UndirectedBaseGraph::vertex_descriptor >::iterator itN = itM;
                            itN++;
                            for( ; itN != vdMap.end(); itN++ ){

                                bool found = false;
                                UndirectedBaseGraph::edge_descriptor oldED;
                                tie( oldED, found ) = edge( itM->second, itN->second, _road );

                                // cerr << "idM = " << _road[ itM->second ].id << " idN = " << _road[ itN->second ].id << endl;

                                if( found ){
                                    _road[ oldED ].visitedTimes += 1;
                                    polygons.push_back( source( oldED, _road ) );
                                }
                                else{

                                    pair< UndirectedBaseGraph::edge_descriptor, unsigned int > foreE = add_edge( itM->second, itN->second, _road );
                                    UndirectedBaseGraph::edge_descriptor foreED = foreE.first;
                                    _road[ foreED ].id = nEdges;
                                    _road[ foreED ].visitedTimes = 0;
                                    _road[ foreED ].weight = (*_road[ itM->second ].coordPtr - *_road[ itN->second ].coordPtr).norm();

                                    polygons.push_back( source( foreED, _road ) );
                                    nEdges++;
                                }
                                itM = itN;
                            }
                        }
                    }
                }

                //ForceGraph &fg = itC->second.detail.bone();
                //ForceGraph::vertex_descriptor vd = vertex( i, fg );
                // cerr << "id = " << fg[ vd ].id << " initID = " << fg[ vd ].initID << endl;
                //polygonVD.insert( pair< unsigned int, vector < UndirectedBaseGraph::vertex_descriptor > >( fg[ vd ].initID, polygons ) );
            }
        }
    }
    cerr << "ending" << endl;

#ifdef DEBUG
    for( unsigned int i = 0; i < polygonVD.size(); i++ ){
        cerr << "i = " << i << endl;
        for( unsigned int j = 0; j < polygonVD[i].size(); j++ ){
            cerr << _road[ polygonVD[i][j] ].id << ", ";
        }
        cerr << endl;
    }
#endif // DEBUG
}

//
//  Road::init -- initialize
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Road::_initRoad( Cell *cellPtr, int __selectedID )
{
    _clear();

    vector< multimap< int, CellComponent > > & cellComponentVec = cellPtr->cellComponentVec();
    unsigned int nVertices = 0, nEdges = 0;

    // build subsystem contour
    //vector< Contour2 > contourVec;
    _contourVec.clear();
    _contourVec.resize( cellPtr->cellVec().size() );     // subsystem size

    // collect cell boundary contour polygons
    for( unsigned int i = 0; i < cellComponentVec.size(); i++ ){

        multimap< int, CellComponent > & cellComponentMap = cellComponentVec[i];
        multimap< int, CellComponent >::iterator itC;

        // cerr << "i = " << i << " size = " << cellComponentMap.size() << endl;
        for( itC = cellComponentMap.begin(); itC != cellComponentMap.end(); itC++ ){

            CellComponent &component = itC->second;
            unsigned int subsysID = component.groupID;
            Polygon2 &c = component.contour.contour();

            // if( subsysID == 2 ) cerr << "myc = " << c << endl;
            _contourVec[ subsysID ].polygons().push_back( c );
        }
    }

    // create subsystem contour
    for( unsigned int i = 0; i < _contourVec.size(); i++ ){

        _contourVec[ i ].id() = i;
        _contourVec[ i ].createContour();
    }

    // build road graph
    for( unsigned int i = 0; i < _contourVec.size(); i++ ){

        Polygon2 &contour = _contourVec[i].contour();
        vector< UndirectedBaseGraph::vertex_descriptor > vdVec;

        for( unsigned j = 0; j < contour.elements().size(); j++ ){

            // Coord2 coord( contour.elements()[j].x(), contour.elements()[j].y() );
            UndirectedBaseGraph::vertex_descriptor vd = NULL;
            bool isFound = _findVertexInRoad( contour.elements()[j], vd );

            if( isFound ){
                vdVec.push_back( vd );
            }
            else{

                UndirectedBaseGraph::vertex_descriptor vdNew = add_vertex( _road );
                _road[ vdNew ].id = nVertices;
                _road[ vdNew ].coordPtr = & contour.elements()[j];

                vdVec.push_back( vdNew );
                nVertices++;
            }
        }

        // cerr << "numV = " << num_vertices( _road ) << endl;
        for( unsigned j = 0; j < vdVec.size(); j++ ){

            UndirectedBaseGraph::vertex_descriptor vdS = vdVec[ j ];
            UndirectedBaseGraph::vertex_descriptor vdT = vdVec[ (j+1)%(int)vdVec.size() ];

            bool found = false;
            UndirectedBaseGraph::edge_descriptor oldED;
            tie( oldED, found ) = edge( vdS, vdT, _road );

            if( found == false ){
                pair< UndirectedBaseGraph::edge_descriptor, unsigned int > foreE = add_edge( vdS, vdT, _road );
                UndirectedBaseGraph::edge_descriptor foreED = foreE.first;
                _road[ foreED ].id = nEdges;
                _road[ foreED ].weight = (*_road[ vdS ].coordPtr - *_road[ vdT ].coordPtr).norm();

                nEdges++;
            }
        }
    }

    // printGraph( _road );
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
    _highwayMat.clear();
}

//
//  Road::_runRoadChaikinCurve -- run road Chaikin's curve
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Road::_runRoadChaikinCurve( int num )
{
    for( unsigned int i = 0; i < _contourVec.size(); i++ ){
        _contourVec[i].computeChaikinCurve( 5, 50 );
    }
}


//
//  Road::_initLaneChaikinCurve -- init lane Chaikin's curve
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Road::_initLaneChaikinCurve( void )
{
    _laneChaikinCurve.clear();

#ifdef DEBUG
    for( unsigned int k = 0; k < _treeEdgeVec.size(); k++ ){

        Coord2 &coordS = *_road[ _treeEdgeVec[k].first ].coordPtr;
        Coord2 &coordT = *_road[ _treeEdgeVec[k].second ].coordPtr;

        cerr << " source = " << coordS
             << " target = " << coordT;
    }
#endif // DEBUG

    // sort the edge segments to a path
    map< int, pair< Coord2, Coord2 > > pathmap;
    deque < Coord2 > path;
    // initialization
    for( unsigned int i = 0; i < _treeEdgeVec.size(); i++ ) {

        Coord2 &coordS = *_road[ _treeEdgeVec[i].first ].coordPtr;
        Coord2 &coordT = *_road[ _treeEdgeVec[i].second ].coordPtr;

        pathmap.insert( pair< int, pair< Coord2, Coord2 > >( i, pair< Coord2, Coord2 >( coordS, coordT ) ) );
    }

    map< int, pair< Coord2, Coord2 > >::iterator itM = pathmap.begin();

    path.push_back( itM->second.first );
    path.push_back( itM->second.second );
    pathmap.erase( itM );
    // itM = pathmap.begin();
    while( pathmap.size() > 0 ){

        Coord2 & coordF = path[ 0 ];
        Coord2 & coordB = path[ path.size()-1 ];

        cerr << " coordF = " << coordF << " coordB = " << coordB << endl;
        map< int, pair< Coord2, Coord2 > >::iterator itF = pathmap.end(), itB = pathmap.end();
        for( itM = pathmap.begin(); itM != pathmap.end(); itM++ ){

            // backward
            if( coordB == itM->second.first ){
                path.push_back( itM->second.second );
                itB = itM;
            }
            else if( coordB == itM->second.second ){
                path.push_back( itM->second.first );
                itB = itM;
            }

            // forward
            if( coordF == itM->second.first ){
                path.push_front( itM->second.second );
                itF = itM;
            }
            else if( coordF == itM->second.second ){
                path.push_front( itM->second.first );
                itF = itM;
            }
        }

        // remove the selected items
        if( itF != pathmap.end() ) pathmap.erase( itF );
        if( itB != pathmap.end() ) pathmap.erase( itB );
    }

    // store initial the path
    for( unsigned int i = 0; i < path.size(); i++ ){

        _laneChaikinCurve.push_back( path[i] );
    }

//#ifdef DEBUG
    for( unsigned int i = 0; i < path.size(); i++ ){

        cout << path[i];
    }
//#endif // DEBUG

}

//
//  Road::_runLaneChaikinCurve -- run lane Chaikin's curve
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Road::_runLaneChaikinCurve( int num )
{
#ifdef DEBUG
    for( unsigned int i = 0; i < _ChaikinCurve.size(); i++ ){

        cout << _ChaikinCurve[i];
    }
#endif // DEBUG
    for( int i = 0; i < num; i++ ){

        vector< Coord2 > core;
        for( unsigned int i = 0; i < _laneChaikinCurve.size(); i++ ){

            core.push_back( _laneChaikinCurve[i] );
        }

        // compute Chaikin Curve
        double interval = 4.0;
        _laneChaikinCurve.clear();
        for( unsigned int i = 0; i < core.size()-1; i++ ){

            Coord2 &p1 = core[i];
            Coord2 &p2 = core[i+1];
            Coord2 q = (1.0-1.0/interval)*p1 + (1.0/interval)*p2;
            Coord2 r = (1.0/interval)*p1 + (1.0-1.0/interval)*p2;

            if( i == 0 ) _laneChaikinCurve.push_back( p1 );
            _laneChaikinCurve.push_back( q );
            _laneChaikinCurve.push_back( r );
            if( i == core.size()-2 ) _laneChaikinCurve.push_back( p2 );
        }
    }
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
            if( ( *subg[i][vd].isSelectedPtr == true ) && ( *subg[i][vd].selectedIDPtr == _selectedID ) ) {
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

                if( ( *subg[j][vd].isSelectedPtr == true ) && ( *subg[j][vd].selectedIDPtr == _selectedID ) ){
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
                //cerr << "center = " << _highwayMat[i][j].center << endl;
                bool found = _findClosestVertexInRoad( _highwayMat[i][j].center, vd );
                assert( found );
                _highwayMat[i][j].routerVD = vd;
                _highwayMat[i][j].routerCoord = *_road[vd].coordPtr;

#ifdef DEBUG
                cerr << "_highwayMat[" << i << "][" << j << "].center = " << _highwayMat[i][j].center;
                cerr << " routerVD = " << _road[vd].id << endl;
#endif // DEBUG
            }
        }
    }

    _findShortestPaths();
    _runRoadChaikinCurve( 5 );

#ifdef DEBUG
    for( unsigned int i = 0; i < nSystems; i++ ) {
        for( unsigned int j = 0; j < nSystems; j++ ){

            cerr << "i = " << i << " j = " << j << endl;
            map< MetaboliteGraph::vertex_descriptor,
                 MetaboliteGraph::vertex_descriptor > &common = _highwayMat[i][j].common;
            map< MetaboliteGraph::vertex_descriptor,
                 MetaboliteGraph::vertex_descriptor >::iterator it;
            for( it = common.begin(); it != common.end(); it++ ){
                cerr << setw(5) << *subg[i][ it->first ].namePtr << ", ";
            }
            cerr << endl;
        }
        cerr << endl;
    }
    cerr << endl;
#endif // DEBUG
}

//
//  Road::steinerTree -- compute steiner tree
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Road::steinerTree( void )
{
    vector< pair< unsigned int, unsigned int > > graph;
    vector< unsigned int > terminals;
    _treeEdgeVec.clear();

    //cerr << "gid = " << _gid << endl;
    // add edges
    BGL_FORALL_EDGES( ed, _road, UndirectedBaseGraph ) {
        UndirectedBaseGraph::vertex_descriptor vdS = source( ed, _road );
        UndirectedBaseGraph::vertex_descriptor vdT = target( ed, _road );

        graph.push_back( pair< unsigned int, unsigned int >( _road[vdS].id+1, _road[vdT].id+1 ) );
        //cerr << "( " << _road[vdS].id << ", " << _road[vdT].id << " )" << endl;
    }

    // add terminals
    //map< UndirectedBaseGraph::vertex_descriptor,
    //        UndirectedBaseGraph::vertex_descriptor >::iterator itT;
    for( unsigned int i = 0; i < _citeVec.size(); i++ ){
        terminals.push_back( _road[ _citeVec[i] ].id+1 );
        //cerr << _road[ itT->first ].id << ", ";
    }
    //cerr << endl;

    //printGraph( _road );
    //cerr << "nV = " << num_vertices( _road ) << " nE = " << num_vertices( _road ) << endl;
    vector< pair< unsigned int, unsigned int > > treeedges;
    steinertree( num_vertices( _road ), num_edges( _road ), graph, terminals, treeedges );

    // cerr << "treeedges:" << endl;
    for( unsigned int j = 0; j < treeedges.size(); j++ ){
        // cerr << treeedges[j].first << "," << treeedges[j].second << endl;
        UndirectedBaseGraph::vertex_descriptor vdS = vertex( treeedges[j].first, _road );
        UndirectedBaseGraph::vertex_descriptor vdT = vertex( treeedges[j].second, _road );
        _treeEdgeVec.push_back( pair< UndirectedBaseGraph::vertex_descriptor,
                UndirectedBaseGraph::vertex_descriptor >( vdS, vdT ) );
    }
    // cerr << endl;

    // compute the curvy tree
    unsigned int nVertices = 0, nEdges = 0;
    UndirectedBaseGraph & tree = _curvytree.tree();
    for( unsigned int i = 0; i < _treeEdgeVec.size(); i++ ){

        // add vertices
        UndirectedBaseGraph::vertex_descriptor vdS, vdT;
        for( unsigned int j = 0; j < 2; j++ ){

            UndirectedBaseGraph::vertex_descriptor vd;
            if( j == 0 ) vd = _treeEdgeVec[i].first;
            else vd = _treeEdgeVec[i].second;

            UndirectedBaseGraph::vertex_descriptor vdNew;
            Coord2 &coord = *_road[ vd ].coordPtr;
            bool isFound = _findVertexInTree( coord, vdNew );

            if( isFound == false ){

                vdNew = add_vertex( tree );
                tree[ vdNew ].id = nVertices;
                tree[ vdNew ].initID = _road[ vd ].id;      // record road ID
                tree[ vdNew ].coordPtr = &coord;

                nVertices++;
            }

            if( j == 0 ) vdS = vdNew;
            else vdT = vdNew;
        }

        // add edges
        bool found = false;
        UndirectedBaseGraph::edge_descriptor oldED;
        tie( oldED, found ) = edge( vdS, vdT, tree );
        if( found == false ) {

            pair< UndirectedBaseGraph::edge_descriptor, unsigned int > foreE = add_edge( vdS, vdT, tree );
            UndirectedBaseGraph::edge_descriptor foreED = foreE.first;
            tree[ foreED ].id = nEdges;
            tree[ foreED ].weight = 1.0;

            nEdges++;
        }
    }

    // printGraph( tree );
    _curvytree.computeFineCurve( 5, 100 );

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
    _selectedID = -1;
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
