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

#include "base/Cell.h"

//------------------------------------------------------------------------------
//	Protected functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------
//
//  Cell::init -- initialize
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Cell::_init( map< unsigned int, Polygon2 > * __polygonComplexPtr )
{
    _polygonComplexPtr = __polygonComplexPtr;
    vector< ForceGraph > &lsubg         = _pathway->lsubG();

    //read config file
    string configFilePath = "../configs/cell.conf";
    Base::Config conf( configFilePath );

    if ( conf.has( "unit" ) ){
        string paramUnit = conf.gets( "unit" );
        _paramUnit = stringToDouble( paramUnit );
    }

    _forceCellVec.resize( lsubg.size() );
    _buildConnectedComponent();
    _buildCellGraphs();

    //for( unsigned int i = 1; i < 2; i++ ){
    for( unsigned int i = 0; i < lsubg.size(); i++ ){

        // cerr << "nV = " << num_vertices( lsubg[i] ) << " nE = " << num_edges( lsubg[i] ) << endl;
        map< unsigned int, Polygon2 >::iterator itP = _polygonComplexPtr->begin();
        advance( itP, i );
        _forceCellVec[i].init( &_forceCellGraphVec[i], itP->second, "../configs/cell.conf" );
        _forceCellVec[i].id() = i;
    }

    cerr << "filepath: " << configFilePath << endl;
    cerr << "unit: " << _paramUnit << endl;
}


//
//  Cell::clear --    clear the current Cell information
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Cell::_clear( void )
{
    _forceCellVec.clear();
    _cellComponentVec.clear();
}


//
//  Cell::findConnectedComponent -- find connected component
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Cell::_buildConnectedComponent( void )
{
    MetaboliteGraph             &g      = _pathway->g();
    vector< ForceGraph >        &lsubg  = _pathway->lsubG();
    vector< MetaboliteGraph >   &subg   = _pathway->subG();
    map< string, Subdomain * >  &sub    = _pathway->subsys();

    // initialization
    _nComponent = 0;
    _cellComponentVec.resize( lsubg.size() );

    for( unsigned int i = 0; i < lsubg.size(); i++ ){

        vector< int > component;
        unsigned int nV = num_vertices( lsubg[ i ] );

        component.resize( nV );
        int num = connected_components( lsubg[ i ],
                                        make_iterator_property_map( component.begin(),
                                                                    get( &ForceVertexProperty::id, lsubg[ i ] )),
                                        boost::color_map( get( &ForceVertexProperty::color, lsubg[ i ] ) ));

#ifdef DEBUG
        cerr << "nV = " << nV << endl;
        cout << "subID = " << i << " total number of components: " << num << endl;
#endif // DEBUG

        vector< CellComponent > cc;
        cc.resize( num );
        for( unsigned int j = 0; j < nV; j++ ){

            ForceGraph::vertex_descriptor vd = vertex( j, lsubg[i] );
            cc[ component[ j ] ].lsubgVec.push_back( vd );
            cc[ component[ j ] ].id = component[ j ];
            // cerr << component[ j ] << endl;
        }
        // cerr << endl << endl;

        multimap< int, CellComponent > &cellComponent = _cellComponentVec[i];
        cellComponent.clear();
        for( unsigned int j = 0; j < num; j++ ){
            cc[j].multiple = ceil( (double)cc[j].lsubgVec.size()/(double)_paramUnit );
            multimap< int, CellComponent >::iterator itC = cellComponent.insert( pair< int, CellComponent >( cc[j].lsubgVec.size(), cc[j] ) );

            ForceGraph &fg = itC->second.detailGraph;

            // add vertices
            for( unsigned int k = 0; k < cc[j].lsubgVec.size(); k++ ){

                ForceGraph::vertex_descriptor vdNew = add_vertex( fg );

                fg[ vdNew ].id          = k;
                fg[ vdNew ].groupID     = i;
                fg[ vdNew ].initID      = lsubg[i][ cc[j].lsubgVec[k] ].id;

                fg[ vdNew ].coordPtr     = lsubg[i][ cc[j].lsubgVec[k] ].coordPtr;
                fg[ vdNew ].prevCoordPtr = new Coord2( lsubg[i][ cc[j].lsubgVec[k] ].coordPtr->x(),
                                                       lsubg[i][ cc[j].lsubgVec[k] ].coordPtr->y() );
                fg[ vdNew ].forcePtr    = new Coord2( 0, 0 );
                fg[ vdNew ].placePtr    = new Coord2( 0, 0 );
                fg[ vdNew ].shiftPtr    = new Coord2( 0, 0 );
                fg[ vdNew ].weight      = 0.0;

                fg[ vdNew ].widthPtr    = new double( 10.0 );
                fg[ vdNew ].heightPtr   = new double( 10.0 );
                fg[ vdNew ].areaPtr     = new double( 100.0 );
            }

            // add edges
            unsigned int idE = 0;
            for( unsigned int k = 1; k < cc[j].lsubgVec.size(); k++ ){
                for( unsigned int l = 0; l < k; l++ ){

                    ForceGraph::vertex_descriptor vdS = vertex( k, fg );
                    ForceGraph::vertex_descriptor vdT = vertex( l, fg );
                    ForceGraph::vertex_descriptor vdLS = cc[j].lsubgVec[k];
                    ForceGraph::vertex_descriptor vdLT = cc[j].lsubgVec[l];

                    bool isFound = false;
                    ForceGraph::edge_descriptor oldED;
                    tie( oldED, isFound ) = edge( vdLS, vdLT, lsubg[i] );
                    if( isFound == true ){
                    // if( false ){

                        pair< ForceGraph::edge_descriptor, unsigned int > foreE = add_edge( vdS, vdT, fg );
                        ForceGraph::edge_descriptor foreED = foreE.first;
                        fg[ foreED ].id = idE;

                        idE++;
                    }
                }
            }

            //cerr << "nV = " << num_vertices( fg ) << " nE = " << num_edges( fg ) << endl;
            //itC->second.detail.init( &fg, itC->second.contour );
            _nComponent++;
        }

#ifdef DEBUG
        // print cellGroup
        multimap< int, CellComponent >::iterator itC = _cellComponentVec[i].begin();
        for( ; itC != _cellComponentVec[i].end(); itC++ ){
            cerr << "id = " << itC->second.id
                 << " multiple = " << itC->second.multiple
                 << " first = " << itC->first
                 << " size = " << itC->second.cellVec.size() << endl;
        }
        cerr << endl;
#endif // DEBUG
    }
}

//
//  Cell::_buildCellGraphs -- build cell graphs
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Cell::_buildCellGraphs( void )
{
    vector< ForceGraph >            &lsubg  = _pathway->lsubG();

    _forceCellGraphVec.resize( lsubg.size() );
    for( unsigned int i = 0; i < lsubg.size(); i++ ) {

        multimap< int, CellComponent >::iterator itC = _cellComponentVec[i].begin();
        map< unsigned int, Polygon2 >::iterator itP = _polygonComplexPtr->begin();
        advance( itP, i );

        unsigned int idV = 0, idE = 0;
        for( ; itC != _cellComponentVec[i].end(); itC++ ){

            //cerr << "i = " << i << " nG = " << _cellComponentVec.size()
            //     << " nM = " << itC->second.multiple << " nV = " << itC->second.cellVec.size() << endl;
            vector< ForceGraph::vertex_descriptor > vdVec;
            int multiple = itC->second.multiple;
            for( unsigned int k = 0; k < multiple; k++ ){

                // add vertex
                ForceGraph::vertex_descriptor vdNew = add_vertex( _forceCellGraphVec[i] );
                double x = itP->second.centroid().x() + rand()%100 - 50;
                double y = itP->second.centroid().y() + rand()%100 - 50;

                _forceCellGraphVec[i][ vdNew ].id          = idV;
                _forceCellGraphVec[i][ vdNew ].groupID     = i;
                _forceCellGraphVec[i][ vdNew ].initID      = idV;

                _forceCellGraphVec[i][ vdNew ].coordPtr     = new Coord2( x, y );
                _forceCellGraphVec[i][ vdNew ].prevCoordPtr = new Coord2( x, y );
                _forceCellGraphVec[i][ vdNew ].forcePtr     = new Coord2( 0, 0 );
                _forceCellGraphVec[i][ vdNew ].placePtr     = new Coord2( 0, 0 );
                _forceCellGraphVec[i][ vdNew ].shiftPtr     = new Coord2( 0, 0 );
                _forceCellGraphVec[i][ vdNew ].weight       = (double)itC->second.lsubgVec.size()/(double)multiple;

                _forceCellGraphVec[i][ vdNew ].widthPtr    = new double( sqrt( _paramUnit ) );
                _forceCellGraphVec[i][ vdNew ].heightPtr   = new double( sqrt( _paramUnit ) );
                _forceCellGraphVec[i][ vdNew ].areaPtr     = new double( _paramUnit );

                idV++;
                vdVec.push_back( vdNew );
            }
            itC->second.cellgVec.clear();
            itC->second.cellgVec = vdVec;

            for( unsigned int k = 0; k < vdVec.size(); k++ ){

                ForceGraph::vertex_descriptor vdS = vdVec[ k ];
                ForceGraph::vertex_descriptor vdT = vdVec[ (k+1)%vdVec.size() ];

                pair< ForceGraph::edge_descriptor, unsigned int > foreE = add_edge( vdS, vdT, _forceCellGraphVec[i] );
                ForceGraph::edge_descriptor foreED = foreE.first;
                _forceCellGraphVec[i][ foreED ].id = idE;

                idE++;
            }
#ifdef DEBUG
            cerr << " id = " << itC->second.id
                 << " multiple = " << itC->second.multiple
                 << " first = " << itC->first
                 << " size = " << itC->second.cellVec.size() << endl;
#endif // DEBUG
        }
    }

#ifdef DEBUG
    // print cellGroup
    assert( lsubg.size() == _forceCellGraphVec.size() );
    for( unsigned int i = 0; i < lsubg.size(); i++ ) {

        multimap< int, CellComponent >::iterator itC = _cellComponentVec[i].begin();
        for( ; itC != _cellComponentVec[i].end(); itC++ ){
            cerr << "id = " << itC->second.id
                 << " multiple = " << itC->second.multiple
                 << " first = " << itC->first
                 << " size = " << itC->second.cellgVec.size() << endl;
            for( unsigned int j = 0; j < itC->second.cellgVec.size(); j++ ){
                cerr << i << ", " << j << ": " << *_forceCellGraphVec[i][ itC->second.cellgVec[j] ].coordPtr << endl;
            }
        }
        cerr << endl;
    }
#endif // DEBUG
}

void Cell::createPolygonComplex( void )
{
    unsigned int idC = 0;
    for( unsigned int i = 0; i < _cellComponentVec.size(); i++ ){

        multimap< int, CellComponent > &componentMap = _cellComponentVec[i];
        Force       &f = _forceCellVec[i];
        ForceGraph &fg = _forceCellGraphVec[i];
        multimap< int, CellComponent >::iterator itC = componentMap.begin();

        for( ; itC != componentMap.end(); itC++ ){

            CellComponent &c = itC->second;
            c.contour.clear();
            int cSize = c.cellgVec.size();
            if( cSize > 1 ){

                Contour2 contour;
                vector< Polygon2 > pVec;
                for( unsigned int j = 0; j < c.cellgVec.size(); j++ ) {
                    int id = fg[c.cellgVec[j]].id;
                    pVec.push_back( (*f.voronoi().seedVec())[id].cellPolygon );
                }
                contour.init( idC, pVec );
                contour.createContour();
                c.contour = contour.contour();
            }
            else{
                // cerr << "csize = 1" << endl;
                int id = fg[c.cellgVec[0]].id;
                Polygon2 &p = (*f.voronoi().seedVec())[id].cellPolygon;
                p.updateOrientation();;
                c.contour = p;
            }

            c.detail.init( &c.detailGraph, c.contour, "../configs/pathway.conf" );
            c.detail.mode() = TYPE_BARNES_HUT;
            c.detail.id() = idC;
            idC++;
        }
    }

    assert( idC == _nComponent );
}

void Cell::updatePathwayCoords( void )
{
    vector< ForceGraph >            &lsubg  = _pathway->lsubG();

    for( unsigned int i = 0; i < _cellComponentVec.size(); i++ ){

        multimap< int, CellComponent >::iterator itC = _cellComponentVec[i].begin();
        for( ; itC != _cellComponentVec[i].end(); itC++ ){

            ForceGraph::vertex_descriptor vd = vertex( itC->second.id, _forceCellGraphVec[i] );

            Coord2 &avg = itC->second.contour.centroid();
#ifdef SKIP
            cerr << "avg = " << avg;
            Coord2 avg( 0.0, 0.0 );
            unsigned int cellSize = itC->second.cellgVec.size();
            for( unsigned int j = 0; j < cellSize; j++ ){
                //cerr << "id = " << itC->second.id << " j = " << j << endl;
                //cerr << "coord = " << *_forceCellGraphVec[ i ][ itC->second.cellgVec[j] ].coordPtr << endl;
                avg = avg + *_forceCellGraphVec[ i ][ itC->second.cellgVec[j] ].coordPtr;
            }
            avg /= (double)cellSize;
#endif // SKIP
            unsigned int lsubgSize = itC->second.lsubgVec.size();
            for( unsigned int j = 0; j < lsubgSize; j++ ){

                //ForceGraph::vertex_descriptor vdC = vertex( itC->second.cellVec[j], lsubg[i] );
                lsubg[i][ itC->second.lsubgVec[j] ].coordPtr->x() = avg.x() + rand()%20-10.0;
                lsubg[i][ itC->second.lsubgVec[j] ].coordPtr->y() = avg.y() + rand()%20-10.0;
                //lsubg[i][ itC->second.lsubgVec[j] ].coordPtr->x() = avg.x() + rand()%100-50.0;
                //lsubg[i][ itC->second.lsubgVec[j] ].coordPtr->y() = avg.y() + rand()%100-50.0;
            }
        }
    }
}

//
//  Cell::Cell -- default constructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
Cell::Cell( void )
{
    ;
}

//
//  Cell::Cell -- copy constructor
//
//  Inputs
//  obj : object of this class
//
//  Outputs
//  none
//
Cell::Cell( const Cell & obj )
{
    ;
}


//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------
//
//  Cell::~Cell --    destructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
Cell::~Cell( void )
{
    ;
}


// end of header file
// Do not add any stuff under this line.
