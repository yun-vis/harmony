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
        _forceCellVec[i].init( &_forceCellGraphVec[i], itP->second );
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
    _cellGroupVec.clear();
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

    _cellGroupVec.resize( lsubg.size() );
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

        vector< CellGroup > cg;
        cg.resize( num );
        for( unsigned int j = 0; j < nV; j++ ){

            ForceGraph::vertex_descriptor vd = vertex( j, lsubg[i] );
            cg[ component[ j ] ].cellVec.push_back( vd );
            cg[ component[ j ] ].id = component[ j ];
            // cerr << component[ j ] << endl;
        }
        // cerr << endl << endl;

        multimap< int, CellGroup > &cellGroup = _cellGroupVec[i];
        cellGroup.clear();
        for( unsigned int j = 0; j < num; j++ ){
            cg[j].multiple = ceil( (double)cg[j].cellVec.size()/(double)_paramUnit );
            cellGroup.insert( pair< int, CellGroup >( cg[j].cellVec.size(), cg[j] ) );
        }

#ifdef DEBUG
        // print cellGroup
        multimap< int, CellGroup >::iterator itC = _cellGroupVec[i].begin();
        for( ; itC != _cellGroupVec[i].end(); itC++ ){
            cerr << "id = " << itC->second.id
                 << " multiple = " << itC->second.multiple
                 << " first = " << itC->first
                 << " size = " << itC->second.cellVec.size() << endl;
        }
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

    for( unsigned int i = 0; i < lsubg.size(); i++ ) {

        multimap< int, CellGroup >::iterator itC = _cellGroupVec[i].begin();
        map< unsigned int, Polygon2 >::iterator itP = _polygonComplexPtr->begin();
        advance( itP, i );

        ForceGraph fg;
        unsigned int idV = 0, idE = 0;
        for( ; itC != _cellGroupVec[i].end(); itC++ ){

            //cerr << "i = " << i << " nG = " << _cellGroupVec.size()
            //     << " nM = " << itC->second.multiple << " nV = " << itC->second.cellVec.size() << endl;
            vector< ForceGraph::vertex_descriptor > vdVec;
            for( unsigned int k = 0; k < itC->second.multiple; k++ ){

                // add vertex
                ForceGraph::vertex_descriptor vdNew = add_vertex( fg );
                double x = itP->second.centroid().x() + rand()%50 - 25;
                double y = itP->second.centroid().y() + rand()%50 - 25;

                fg[ vdNew ].id          = idV;
                fg[ vdNew ].groupID     = i;
                fg[ vdNew ].initID      = idV;

                fg[ vdNew ].coordPtr     = new Coord2( x, y );
                fg[ vdNew ].prevCoordPtr = new Coord2( x, y );
                fg[ vdNew ].forcePtr     = new Coord2( 0, 0 );
                fg[ vdNew ].placePtr     = new Coord2( 0, 0 );
                fg[ vdNew ].shiftPtr     = new Coord2( 0, 0 );

                fg[ vdNew ].widthPtr    = new double( sqrt( _paramUnit ) );
                fg[ vdNew ].heightPtr   = new double( sqrt( _paramUnit ) );
                fg[ vdNew ].areaPtr     = new double( _paramUnit );

                idV++;
                vdVec.push_back( vdNew );
            }

            for( unsigned int k = 0; k < vdVec.size(); k++ ){

                ForceGraph::vertex_descriptor vdS = vdVec[ k ];
                ForceGraph::vertex_descriptor vdT = vdVec[ (k+1)%vdVec.size() ];

                pair< ForceGraph::edge_descriptor, unsigned int > foreE = add_edge( vdS, vdT, fg );
                ForceGraph::edge_descriptor foreED = foreE.first;
                fg[ foreED ].id = idE;

                idE++;
            }

#ifdef DEBUG
            cerr << " id = " << itC->second.id
                 << " multiple = " << itC->second.multiple
                 << " first = " << itC->first
                 << " size = " << itC->second.cellVec.size() << endl;
#endif // DEBUG
        }
        //cerr << "nVFG = " << num_vertices( fg ) << endl;
        _forceCellGraphVec.push_back( fg );
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
