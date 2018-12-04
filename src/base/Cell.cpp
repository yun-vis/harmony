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
    string configFilePath = "../configs/addition.conf";
    Base::Config conf( configFilePath );

    if ( conf.has( "ka" ) ){
        string paramAddKa = conf.gets( "ka" );
        _paramAddKa = stringToDouble( paramAddKa );
    }

    if ( conf.has( "kr" ) ){
        string paramAddKr = conf.gets( "kr" );
        _paramAddKr = stringToDouble( paramAddKr );
    }

    if ( conf.has( "unit" ) ){
        string paramUnit = conf.gets( "unit" );
        _paramUnit = stringToDouble( paramUnit );
    }

    _forceCellVec.resize( lsubg.size() );
    _buildConnectedComponent();
    _computeCellComponentSimilarity();
    _buildCellGraphs();
    _buildInterCellComponents();

    //for( unsigned int i = 1; i < 2; i++ ){
    for( unsigned int i = 0; i < lsubg.size(); i++ ){

        // cerr << "nV = " << num_vertices( lsubg[i] ) << " nE = " << num_edges( lsubg[i] ) << endl;
        map< unsigned int, Polygon2 >::iterator itP = _polygonComplexPtr->begin();
        advance( itP, i );
        _forceCellVec[i].init( &_forceCellGraphVec[i], itP->second, "../configs/cell.conf" );
        _forceCellVec[i].id() = i;
    }

    cerr << "filepath: " << configFilePath << endl;
    cerr << "addKa: " << _paramAddKa << endl;
    cerr << "addKr: " << _paramAddKr << endl;
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
    ForceGraph                  &lg     = _pathway->lg();
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

//#ifdef DEBUG
        cerr << "nV = " << nV << endl;
        cout << "subID = " << i << " total number of components: " << num << endl;
//#endif // DEBUG

        vector< CellComponent > cc;
        cc.resize( num );
        for( unsigned int j = 0; j < nV; j++ ){

            ForceGraph::vertex_descriptor vd = vertex( j, lsubg[i] );
            cc[ component[ j ] ].lsubgVec.push_back( vd );
            cc[ component[ j ] ].id = component[ j ];
            cc[ component[ j ] ].groupID = i;
            //cc[ component[ j ] ].subgID = j;
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
                //fg[ vdNew ].componentID = lsubg[i][ cc[j].lsubgVec[k] ].id;
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
        cerr << "lg:" << endl;
        printGraph( lg );
        cerr << "component:" << endl;
#endif // DEBUG

        // update component id
        multimap< int, CellComponent >::iterator itC;
        unsigned int idC = 0;
        for( itC = cellComponent.begin(); itC != cellComponent.end(); itC++ ){

            CellComponent & component = itC->second;
            component.id = idC;
            for( unsigned int k = 0; k < component.lsubgVec.size(); k++ ){

                MetaboliteGraph::vertex_descriptor vdM = vertex( lg[component.lsubgVec[k]].initID, g );
                ForceGraph::vertex_descriptor vdL = vertex( lg[component.lsubgVec[k]].initID, lg );
                g[vdM].componentID = idC;
                lg[vdL].componentID = idC;
#ifdef DEBUG
                cerr << "initID = " << lg[component.lsubgVec[k]].initID << endl;
                cerr << "g[vdM].componentID = " << g[vdM].componentID << endl;
                cerr << "lg[vdL].componentID = " << lg[vdL].componentID << endl;
#endif // DEBUG
            }
            idC++;
        }

#ifdef DEBUG
        // print cellGroup
        //multimap< int, CellComponent >::iterator itC = cellComponent.begin();
        for( itC = cellComponent.begin(); itC != cellComponent.end(); itC++ ){
            cerr << " id = " << itC->second.id
                 << " multiple = " << itC->second.multiple
                 << " first = " << itC->first
                 << " size = " << itC->second.cellgVec.size() << endl;
        }
        cerr << endl;
#endif // DEBUG
    }

#ifdef DEBUG
    BGL_FORALL_VERTICES( vd, lg, ForceGraph ) {
        cerr << "initID = " << lg[vd].initID << " cid = " << lg[vd].componentID << endl;
    }
#endif // DEBUG
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

        vector< vector< ForceGraph::vertex_descriptor > > mergeVec;      // vector of cell need to be merged

        unsigned int idV = 0, idE = 0;
        for( ; itC != _cellComponentVec[i].end(); itC++ ){

#ifdef DEBUG
            cerr << "i = " << i << " nG = " << _cellComponentVec.size()
                 << " nM = " << itC->second.multiple << " nV = " << itC->second.cellVec.size() << endl;
#endif // DEBUG
            // add vertices
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
                //_forceCellGraphVec[i][ vdNew ].coordPtr     = new Coord2( 0.0, 0.0 );
                //_forceCellGraphVec[i][ vdNew ].prevCoordPtr = new Coord2( 0.0, 0.0 );
                _forceCellGraphVec[i][ vdNew ].forcePtr     = new Coord2( 0.0, 0.0 );
                _forceCellGraphVec[i][ vdNew ].placePtr     = new Coord2( 0.0, 0.0 );
                _forceCellGraphVec[i][ vdNew ].shiftPtr     = new Coord2( 0.0, 0.0 );
                _forceCellGraphVec[i][ vdNew ].weight       = (double)itC->second.lsubgVec.size()/(double)multiple;

                _forceCellGraphVec[i][ vdNew ].widthPtr    = new double( sqrt( _paramUnit ) );
                _forceCellGraphVec[i][ vdNew ].heightPtr   = new double( sqrt( _paramUnit ) );
                _forceCellGraphVec[i][ vdNew ].areaPtr     = new double( _paramUnit );

                idV++;
                vdVec.push_back( vdNew );
            }
            itC->second.cellgVec.clear();
            itC->second.cellgVec = vdVec;
            if( vdVec.size() > 1 ) mergeVec.push_back( vdVec );

            // add edges
            // cerr << "vdVec.size() = " << vdVec.size() << endl;
            if( vdVec.size() > 1 ){
                for( unsigned int k = 0; k < vdVec.size(); k++ ){

                    ForceGraph::vertex_descriptor vdS = vdVec[ k ];
                    ForceGraph::vertex_descriptor vdT = vdVec[ (k+1)%vdVec.size() ];

                    pair< ForceGraph::edge_descriptor, unsigned int > foreE = add_edge( vdS, vdT, _forceCellGraphVec[i] );
                    ForceGraph::edge_descriptor foreED = foreE.first;
                    _forceCellGraphVec[i][ foreED ].id = idE;

                    idE++;
                }
            }

#ifdef DEBUG
            cerr << " id = " << itC->second.id
                 << " multiple = " << itC->second.multiple
                 << " first = " << itC->first
                 << " size = " << itC->second.cellgVec.size() << endl;
#endif // DEBUG
        }

#ifdef DEBUG
        BGL_FORALL_VERTICES( vd, _forceCellGraphVec[i], ForceGraph ) {

            ForceGraph::degree_size_type degrees = out_degree( vd, _forceCellGraphVec[i] );
            cerr << "id = " << _forceCellGraphVec[i][vd].id << " deg = " << degrees << endl;
        }
#endif // DEBUG

        // find sets
        unsigned int size = _cellComponentSimilarityVec[i].size();
        map< unsigned int, vector< unsigned int > > setMap;
        vector< unsigned int > unique;
        for( unsigned int m = 0; m < size; m++ ){

            vector< unsigned int > idVec;
            idVec.push_back( m );
            unsigned int minID = m;
            for( unsigned int n = 0; n < size; n++ ){

                if(  _cellComponentSimilarityVec[i][m][n] == 1.0 ) {
                    idVec.push_back( n );
                    if( minID > n ) minID = n;
                }
            }

            if( idVec.size() > 1 ){

                setMap.insert( pair< unsigned int, vector< unsigned int > >( minID, idVec ) );
            }
            else{

                multimap< int, CellComponent >::iterator itM = _cellComponentVec[i].begin();
                advance( itM, m );
                //cerr << "m = " << m << " itM->second.cellgVec.size() = " << itM->second.cellgVec.size() << endl;
                if( itM->second.cellgVec.size() == 1 ) unique.push_back( m );
            }
        }

        // add secondary edges
        map< unsigned int, vector< unsigned int > >::iterator itM;
        for( itM = setMap.begin(); itM != setMap.end(); itM++ ){

            vector< unsigned int > &idVec = itM->second;
            for( unsigned int m = 0; m < idVec.size()-1; m++ ){

                ForceGraph::vertex_descriptor vdS = vertex( idVec[m], _forceCellGraphVec[i] );
                ForceGraph::vertex_descriptor vdT = vertex( idVec[(m+1)%idVec.size()], _forceCellGraphVec[i] );

                bool isFound = false;
                ForceGraph::edge_descriptor oldED;
                tie( oldED, isFound ) = edge( vdS, vdT, _forceCellGraphVec[i] );
                // cerr << "degreeS = " << degreeS << " degreeT = " << degreeT << endl;
                if( isFound == false ){
                    pair< ForceGraph::edge_descriptor, unsigned int > foreE = add_edge( vdS, vdT, _forceCellGraphVec[i] );
                    ForceGraph::edge_descriptor foreED = foreE.first;
                    _forceCellGraphVec[i][ foreED ].id = idE;

                    idE++;
                }
            }
        }

        unsigned int total = mergeVec.size() + setMap.size() + unique.size();
        double x = itP->second.centroid().x(); // + rand()%100 - 50;
        double y = itP->second.centroid().y(); // + rand()%100 - 50;
        double mainRadius = 50.0, secondRadius = 10.0;

        // initialize cell position
        unsigned int index = 0;
        for( unsigned int m = 0; m < mergeVec.size(); m++ ){

            double cosTheta = mainRadius * cos( 2.0*M_PI*(double)index/(double)total );
            double sinTheta = mainRadius * sin( 2.0*M_PI*(double)index/(double)total );
            double cx = x + cosTheta;
            double cy = y + sinTheta;
            for( unsigned int n = 0; n < mergeVec[m].size(); n++ ){

                ForceGraph::vertex_descriptor &vd = mergeVec[m][n];

                double cosValue = secondRadius * cos( 2.0*M_PI*(double)n/(double)mergeVec[m].size() );
                double sinValue = secondRadius * sin( 2.0*M_PI*(double)n/(double)mergeVec[m].size() );
                _forceCellGraphVec[i][vd].coordPtr->x() = cx + cosValue;
                _forceCellGraphVec[i][vd].coordPtr->y() = cy + sinValue;
                _forceCellGraphVec[i][vd].prevCoordPtr->x() = cx + cosValue;
                _forceCellGraphVec[i][vd].prevCoordPtr->y() = cy + sinValue;
            }
            index++;
        }
        assert( index == mergeVec.size() );

        for( unsigned int m = 0; m < setMap.size(); m++ ){

            map< unsigned int, vector< unsigned int > >::iterator itM = setMap.begin();
            advance( itM, m );

            double cosTheta = mainRadius * cos( 2.0*M_PI*(double)index/(double)total );
            double sinTheta = mainRadius * sin( 2.0*M_PI*(double)index/(double)total );
            double cx = x + cosTheta;
            double cy = y + sinTheta;

            for( unsigned int n = 0; n < itM->second.size(); n++ ){

                ForceGraph::vertex_descriptor vd = vertex( (itM->second)[n], _forceCellGraphVec[i] );

                double cosValue = secondRadius * cos( 2.0*M_PI*(double)n/(double)itM->second.size() );
                double sinValue = secondRadius * sin( 2.0*M_PI*(double)n/(double)itM->second.size() );
                _forceCellGraphVec[i][vd].coordPtr->x() = cx + cosValue;
                _forceCellGraphVec[i][vd].coordPtr->y() = cy + sinValue;
                _forceCellGraphVec[i][vd].prevCoordPtr->x() = cx + cosValue;
                _forceCellGraphVec[i][vd].prevCoordPtr->y() = cy + sinValue;
            }
            index++;
        }
        assert( index == mergeVec.size()+setMap.size() );

#ifdef SKIP
        for( unsigned int m = 0; m < unique.size(); m++ ){

            ForceGraph::vertex_descriptor vd = vertex( unique[m], _forceCellGraphVec[i] );

            double cosTheta = mainRadius * cos( 2.0*M_PI*(double)index/(double)total );
            double sinTheta = mainRadius * sin( 2.0*M_PI*(double)index/(double)total );
            _forceCellGraphVec[i][vd].coordPtr->x() = x + cosTheta; // + cosTheta;
            _forceCellGraphVec[i][vd].coordPtr->y() = y + sinTheta; // + sinTheta;
            cerr << setw(5) << "( x, y ) = ( " << x + cosTheta << " , " << y + sinTheta << " ) " << endl;
            //_forceCellGraphVec[i][vd].coordPtr->x() = x + rand()%100 - 50; // + cosTheta;
            //_forceCellGraphVec[i][vd].coordPtr->y() = y + rand()%100 - 50; // + sinTheta;
            _forceCellGraphVec[i][vd].prevCoordPtr->x() = _forceCellGraphVec[i][vd].coordPtr->x();
            _forceCellGraphVec[i][vd].prevCoordPtr->y() = _forceCellGraphVec[i][vd].coordPtr->y();
            index++;
        }
        assert( index == mergeVec.size()+setMap.size()+unique.size() );
#endif  // SKIP

#ifdef DEBUG
        for( itM = setMap.begin(); itM != setMap.end(); itM++ ){

            vector< unsigned int > &idVec = itM->second;
            cerr << "setID = " << itM->first << endl;
            for( unsigned int m = 0; m < idVec.size(); m++ ){
                cerr << idVec[m] << " ";
            }
            cerr << endl;
        }
        cerr << endl;
#endif // DEBUG
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
            //c.detail.mode() = TYPE_BARNES_HUT;
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
//  Cell::computeCellComponentSimilarity -- compute pair-wise Similarity of Cell Components
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Cell::_computeCellComponentSimilarity( void )
{
    for( unsigned int i = 0; i < _cellComponentVec.size(); i++ ){

        vector< vector< double > > cellComponentSimilarity;
        unsigned int size = _cellComponentVec[i].size();

        // initialization
        cellComponentSimilarity.resize( size );
        for( unsigned int m = 0; m < size; m++ ){
            cellComponentSimilarity[m].resize( size );
        }

        // compute pair-wise similarity
        for( unsigned int m = 0; m < size-1; m++ ){
            multimap< int, CellComponent >::iterator itM = _cellComponentVec[i].begin();
            advance( itM, m );

            for( unsigned int n = m+1; n < size; n++ ){

                multimap< int, CellComponent >::iterator itN = _cellComponentVec[i].begin();
                advance( itN, n );

                unsigned int idM = m;
                unsigned int idN = n;
                //unsigned int idM = itM->second.id;
                //unsigned int idN = itN->second.id;

#ifdef DEBUG
                BGL_FORALL_VERTICES( vd, itM->second.detailGraph, ForceGraph ) {
                    cerr << "idS = " << itM->second.detailGraph[vd].id << endl;
                }
                BGL_FORALL_VERTICES( vd, itN->second.detailGraph, ForceGraph ) {
                    cerr << "idT = " << itN->second.detailGraph[vd].id << endl;
                }
#endif // DEBUG

                Similarity s;
                s.init( &itM->second.detailGraph, &itN->second.detailGraph );
                bool val = s.isSimilar();
                //cerr << "val = " << val << endl;
                cellComponentSimilarity[idM][idN] = cellComponentSimilarity[idN][idM] = val;
            }
        }
        _cellComponentSimilarityVec.push_back( cellComponentSimilarity );
    }

#ifdef DEBUG
    for( unsigned int i = 0; i < _cellComponentSimilarityVec.size(); i++ ){

        cerr << "i = " << i << endl;
        for( unsigned int m = 0; m < _cellComponentSimilarityVec[i].size(); m++ ){
            for( unsigned int n = 0; n < _cellComponentSimilarityVec[i][m].size(); n++ ) {
                cerr<< _cellComponentSimilarityVec[i][m][n] << " ";
            }
            cerr << endl;
        }
        cerr << endl << endl;
    }
#endif // DEBUG
}

//
//  Cell::buildInterCellComponents -- build inter-cell components
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Cell::_buildInterCellComponents( void )
{
    MetaboliteGraph             &g      = _pathway->g();
    ForceGraph                  &lg     = _pathway->lg();
    SkeletonGraph               &s      = _pathway->skeletonG();

    // initialization
    _interCellComponentMap.clear();
    _reducedInterCellComponentMap.clear();

    BGL_FORALL_VERTICES( vd, g, MetaboliteGraph ) {

        if( g[ vd ].type == "metabolite" ){
            if( ( g[ vd ].metaPtr != NULL ) ){
                if( g[ vd ].metaPtr->subsystems.size() > 1 ){

                    // cerr << "HERE" << endl;
                    ForceGraph::vertex_descriptor vdL = vertex( g[vd].id, lg );

                    // out edges
                    ForceGraph::out_edge_iterator eoa, eoa_end;
                    for( tie( eoa, eoa_end ) = out_edges( vdL, lg ); eoa != eoa_end; ++eoa ){

                        ForceGraph::edge_descriptor edA = *eoa;
                        ForceGraph::vertex_descriptor vdAS = source( edA, lg );
                        ForceGraph::vertex_descriptor vdAT = target( edA, lg );

                        ForceGraph::out_edge_iterator eob, eob_end;
                        for( tie( eob, eob_end ) = out_edges( vdL, lg ); eob != eob_end; ++eob ){

                            ForceGraph::edge_descriptor edB = *eob;
                            ForceGraph::vertex_descriptor vdBS = source( edB, lg );
                            ForceGraph::vertex_descriptor vdBT = target( edB, lg );

                            unsigned int idSA = g[ vertex( lg[ vdAT ].id, g ) ].reactPtr->subsystem->id;
                            unsigned int idSB = g[ vertex( lg[ vdBT ].id, g ) ].reactPtr->subsystem->id;
                            if( ( vdAT != vdBT ) && ( idSA != idSB ) ){

                                if( idSA > idSB ){
                                    unsigned int temp = idSA;
                                    idSA = idSB;
                                    idSB = temp;
                                    ForceGraph::vertex_descriptor tempVD = vdAT;
                                    vdAT = vdBT;
                                    vdBT = tempVD;
                                }

                                Grid2 grid( idSA, idSB );
                                unsigned idA = lg[ vdAT ].componentID;
                                unsigned idB = lg[ vdBT ].componentID;
                                multimap< int, CellComponent >::iterator itA = _cellComponentVec[idSA].begin();
                                multimap< int, CellComponent >::iterator itB = _cellComponentVec[idSB].begin();
                                advance( itA, idA );
                                advance( itB, idB );
                                CellComponent &cca = itA->second;
                                CellComponent &ccb = itB->second;

#ifdef DEBUG
                                cerr << "idSA = " << idSA << " idSB = " << idSB << endl;
                                cerr << "lg[ vdAT ].id = " << lg[ vdAT ].id << " lg[ vdBT ].id = " << lg[ vdBT ].id << endl;
                                cerr << "idA = " << idA << " idB = " << idB << endl;
                                cerr << "ccA = " << cca.id << " ccB = " << ccb.id << endl;
                                cerr << "ccA.size() = " << cca.cellgVec.size() << " ccB.size() = " << ccb.cellgVec.size() << endl;
#endif // DEBUG
                                bool found = false;
                                SkeletonGraph::vertex_descriptor vdSS = vertex( idSA, s );
                                SkeletonGraph::vertex_descriptor vdST = vertex( idSB, s );
                                SkeletonGraph::edge_descriptor oldED;
                                tie( oldED, found ) = edge( vdSS, vdST, s );
                                if( found ){
                                //if( true ){

                                    multimap< Grid2, pair< CellComponent, CellComponent > >::iterator it;
                                    bool exist = false;
                                    for( it = _interCellComponentMap.begin(); it != _interCellComponentMap.end(); it++  ){

                                        if( ( idSA == it->first.p() ) && ( idSB == it->first.q() ) &&
                                            ( cca.id == it->second.first.id ) && ( ccb.id == it->second.second.id ) ){
                                            exist = true;
                                        }
                                    }

                                    if( !exist ){
                                        _interCellComponentMap.insert( pair< Grid2,
                                                                       pair< CellComponent, CellComponent > >( grid,
                                                        pair< CellComponent, CellComponent >( cca, ccb ) ) );
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // find a maximal matching
    multimap< Grid2, pair< CellComponent, CellComponent > >::iterator itC;
    UndirectedPropertyGraph reduce;
    VertexIndexMap      vertexIndex     = get( vertex_index, reduce );
    VertexXMap          vertexX         = get( vertex_myx, reduce );
    VertexYMap          vertexY         = get( vertex_myy, reduce );

    for( itC = _interCellComponentMap.begin(); itC != _interCellComponentMap.end(); itC++  ){

        UndirectedPropertyGraph::vertex_descriptor vdS, vdT;
        bool existedS = false,
             existedT = false;

        BGL_FORALL_VERTICES( vd, reduce, UndirectedPropertyGraph ) {
            if( ( vertexX[ vd ] == itC->first.p() ) && ( vertexY[ vd ] == itC->second.first.id ) ){
                vdS = vd;
                existedS = true;
            }
            if( ( vertexX[ vd ] == itC->first.q() ) && ( vertexY[ vd ] == itC->second.second.id ) ){
                vdT = vd;
                existedT = true;
            }
        }

        if( !existedS ){
            vdS = add_vertex( reduce );
            vertexX[ vdS ] = itC->first.p();
            vertexY[ vdS ] = itC->second.first.id;
        }
        if( !existedT ){
            vdT = add_vertex( reduce );
            vertexX[ vdT ] = itC->first.q();
            vertexY[ vdT ] = itC->second.second.id;
        }

        bool found = false;
        UndirectedPropertyGraph::edge_descriptor oldED;
        tie( oldED, found ) = edge( vdS, vdT, reduce );
        if( found == false ){

            pair< UndirectedPropertyGraph::edge_descriptor, unsigned int > foreE = add_edge( vdS, vdT, reduce );
            // UndirectedPropertyGraph::edge_descriptor edL = foreE.first;
        }
    }

    // compute maximal matching
    vector< graph_traits< UndirectedPropertyGraph >::vertex_descriptor > mate( num_vertices( reduce ) );
    bool success = checked_edmonds_maximum_cardinality_matching( reduce, &mate[0] );
    assert( success );

    graph_traits< UndirectedPropertyGraph >::vertex_iterator vi, vi_end;
    for( boost::tie( vi, vi_end ) = vertices( reduce ); vi != vi_end; ++vi ){
        if( mate[*vi] != graph_traits< UndirectedPropertyGraph >::null_vertex() && *vi < mate[*vi] ){
            // cout << "{" << *vi << ", " << mate[*vi] << "}" << std::endl;

            UndirectedPropertyGraph::vertex_descriptor vdS = vertex( *vi, reduce );
            UndirectedPropertyGraph::vertex_descriptor vdT = vertex( mate[*vi], reduce );

            unsigned int idS = vertexX[ vdS ];
            unsigned int idT = vertexX[ vdT ];
            unsigned int idCS = vertexY[ vdS ];
            unsigned int idCT = vertexY[ vdT ];
            if( idS > idT ){
                unsigned int tmp = idS;
                idS = idT;
                idT = tmp;
                unsigned int tmpC = idCS;
                idCS = idCT;
                idCT = tmpC;
            }

            Grid2 grid( idS, idT );
            multimap< int, CellComponent >::iterator itS = _cellComponentVec[idS].begin();
            multimap< int, CellComponent >::iterator itT = _cellComponentVec[idT].begin();
            advance( itS, idCS );
            advance( itT, idCT );
            CellComponent &ccS = itS->second;
            CellComponent &ccT = itT->second;
            _reducedInterCellComponentMap.insert( pair< Grid2,
                    pair< CellComponent, CellComponent > >( grid,
                    pair< CellComponent, CellComponent >( ccS, ccT ) ) );

            //cerr << "" << vertexX[ vdS ] << " ( " << vertexY[ vdS ] << " ) x ";
            //cerr << "" << vertexX[ vdT ] << " ( " << vertexY[ vdT ] << " )" << endl;
        }
    }

#ifdef DEBUG
    //multimap< Grid2, pair< CellComponent, CellComponent > >::iterator itC;
    cerr << "_interCellComponentMap.size() = " << _interCellComponentMap.size() << endl;
    for( itC = _interCellComponentMap.begin(); itC != _interCellComponentMap.end(); itC++  ){
        cerr << "" << itC->first.p() << " ( " << itC->second.first.id << " ) x ";
        cerr << "" << itC->first.q() << " ( " << itC->second.second.id << " )" << endl;
    }

    cerr << "_reducedInterCellComponentMap.size() = " << _reducedInterCellComponentMap.size() << endl;
    for( itC = _reducedInterCellComponentMap.begin(); itC != _reducedInterCellComponentMap.end(); itC++  ){
        cerr << "" << itC->first.p() << " ( " << itC->second.first.id << " ) x ";
        cerr << "" << itC->first.q() << " ( " << itC->second.second.id << " )" << endl;
    }
#endif // DEBUG
}

//
//  Cell::additionalForces -- additional forces
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Cell::additionalForces( void )
{
    //multimap< Grid2, pair< CellComponent, CellComponent > >::iterator itA = _interCellComponentMap.begin();
    multimap< Grid2, pair< CellComponent, CellComponent > >::iterator itA = _reducedInterCellComponentMap.begin();

    //for( ; itA != _interCellComponentMap.end(); itA++ ){
    for( ; itA != _reducedInterCellComponentMap.end(); itA++ ){


        unsigned int idS = itA->first.p();
        unsigned int idT = itA->first.q();
        CellComponent &ccS = itA->second.first;
        CellComponent &ccT = itA->second.second;

        ForceGraph &fgS = _forceCellGraphVec[ idS ];
        ForceGraph &fgT = _forceCellGraphVec[ idT ];
        ForceGraph::vertex_descriptor vdS = vertex( ccS.id, fgS );
        ForceGraph::vertex_descriptor vdT = vertex( ccT.id, fgT );

        double side = 0.25*( _forceCellVec[ idS ].width() + _forceCellVec[ idS ].width() +
                             _forceCellVec[ idT ].width() + _forceCellVec[ idT ].width() );
        double LA = sqrt( side / ( double )max( 1.0, ( double )num_vertices( fgS ) ) );
        double LB = sqrt( side / ( double )max( 1.0, ( double )num_vertices( fgT ) ) );
        double L = 0.5*( LA+LB );

        Coord2 diff, unit;
        double dist;

        diff = *fgS[ vdS ].coordPtr - *fgT[ vdT ].coordPtr;
        dist = diff.norm();
        if( dist == 0 ) unit.zero();
        else unit = diff.unit();

        *fgS[ vdS ].forcePtr -= _paramAddKa * ( dist - L ) * unit;
        *fgT[ vdT ].forcePtr += _paramAddKa * ( dist - L ) * unit;
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
