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
#include <boost/graph/max_cardinality_matching.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/foreach.hpp>

#include <QtWidgets/QApplication>

#include "base/Cell.h"

typedef std::vector< std::string > Tokens;

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
void Cell::_init( double *veCoveragePtr, double *veRatioPtr, map< unsigned int, Polygon2 > * polygonComplexPtr )
{
    _veCoveragePtr = veCoveragePtr;
    _veRatioPtr = veRatioPtr;
    vector< ForceGraph > &lsubg     = _pathwayPtr->lsubG();

    //read config file
    string configFilePath = "config/addition.conf";
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

    // initalize parameters of centers
    _centerVec.resize( lsubg.size() );
    for( unsigned int i = 0; i < _centerVec.size(); i++ ){

        // cerr << "nV = " << num_vertices( lsubg[i] ) << " nE = " << num_edges( lsubg[i] ) << endl;
        map< unsigned int, Polygon2 >::iterator itP = polygonComplexPtr->begin();
        advance( itP, i );
        _centerVec[i].force().init( &_centerVec[i].forceGraph(), &itP->second, LEVEL_MIDDLE, "config/center.conf" );
        _centerVec[i].force().id() = i;
    }

    // initalize parameters of cells
    _cellVec.resize( lsubg.size() );
    for( unsigned int i = 0; i < _cellVec.size(); i++ ){

        // cerr << "nV = " << num_vertices( lsubg[i] ) << " nE = " << num_edges( lsubg[i] ) << endl;
        map< unsigned int, Polygon2 >::iterator itP = polygonComplexPtr->begin();
        advance( itP, i );
        _cellVec[i].force().init( &_cellVec[i].forceGraph(), &itP->second, LEVEL_MIDDLE, "config/cell.conf" );
        _cellVec[i].force().id() = i;
        // _cellVec[i].force().contour() = itP->second;
    }

    _buildConnectedComponent();
    _computeClusters();
    _computeCellComponentSimilarity();
    _buildInterCellComponents();
    _buildCenterGraphs();
    _buildCellGraphs();

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
    for( unsigned int i = 0; i < _cellVec.size(); i++ ){
        _cellVec.clear();
    }
    //_cellVec.force().clear();
    _cellComponentVec.clear();
}


//
//  Cell::buildConnectedComponent -- find connected component
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Cell::_buildConnectedComponent( void )
{
    MetaboliteGraph             &g      = _pathwayPtr->g();
    ForceGraph                  &lg     = _pathwayPtr->lg();
    vector< ForceGraph >        &lsubg  = _pathwayPtr->lsubG();
    vector< MetaboliteGraph >   &subg   = _pathwayPtr->subG();
    map< string, Subdomain * >  &sub    = _pathwayPtr->subsys();

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
            cc[ component[ j ] ].groupID = i;
            //cc[ component[ j ] ].subgID = j;
            // cerr << component[ j ] << endl;
        }
        // cerr << endl << endl;

        multimap< int, CellComponent > &cellComponent = _cellComponentVec[i];
        double unit = INFINITY;
        double powUnit = 1.0; // pathway
        //double powUnit = 1.2; // metabolic
        // compute unit
        for( unsigned int j = 0; j < num; j++ ){

            double totalArea = 0.0;
            for( unsigned int k = 0; k < cc[j].lsubgVec.size(); k++ ){
                double textW = *lsubg[i][ cc[j].lsubgVec[k] ].namePixelWidthPtr;
                double textH = *lsubg[i][ cc[j].lsubgVec[k] ].namePixelHeightPtr;
                totalArea += textW * textH;
            }

            double veCoverage = num_vertices( lsubg[i] ) + (*_veRatioPtr)*num_edges( lsubg[i] );
            double multiple = ceil( pow( ceil( totalArea/veCoverage/(double)_paramUnit ), powUnit ) );    // text area size
            if( unit > multiple ){
                unit = multiple;
            }
        }

        cellComponent.clear();
        for( unsigned int j = 0; j < num; j++ ){

            double totalArea = 0.0;
            for( unsigned int k = 0; k < cc[j].lsubgVec.size(); k++ ){
                double textW = *lsubg[i][ cc[j].lsubgVec[k] ].namePixelWidthPtr;
                double textH = *lsubg[i][ cc[j].lsubgVec[k] ].namePixelHeightPtr;
                totalArea += textW * textH;
            }

            //cc[j].multiple = ceil( totalArea/(*_veCoveragePtr)/(double)_paramUnit );    // text area size
            //cc[j].multiple = SQUARE( ceil( totalArea/(*_veCoveragePtr)/(double)_paramUnit ) );    // text area size
            //totalArea += totalArea * (*_veRatioPtr) * cellComponent.size();
            double veCoverage = num_vertices( lsubg[i] ) + (*_veRatioPtr)*num_edges( lsubg[i] );
            cc[j].multiple = ceil( pow( ceil( totalArea/veCoverage/(double)_paramUnit ), powUnit )/unit );    // text area size

#ifdef DEBUG
            if( i == 0 ){
                cerr << "i = " << i << endl;
                cerr << "cc[j].multiple = " << cc[j].multiple
                     << " j = " << j << " totalArea = " << totalArea
                     << " *_veCoveragePtr = " << *_veCoveragePtr << " paramUnit = " << _paramUnit << endl;
            }
#endif // DEBUG

            // cc[j].multiple = ceil( (double)cc[j].lsubgVec.size()/(double)_paramUnit );   // node size
            // cerr << "totalArea = " << totalArea << " cc[j].multiple = " << cc[j].multiple << endl;
            multimap< int, CellComponent >::iterator itC = cellComponent.insert( pair< int, CellComponent >( cc[j].lsubgVec.size(), cc[j] ) );

            //ForceGraph &fg = itC->second.detail.forceGraph();

            // add vertices
            for( unsigned int k = 0; k < cc[j].lsubgVec.size(); k++ ){

                ForceGraph::vertex_descriptor vdNew = add_vertex( itC->second.detail.forceGraph() );

                itC->second.detail.forceGraph()[ vdNew ].id          = k;
                itC->second.detail.forceGraph()[ vdNew ].groupID     = i;
                //itC->second.detail.forceGraph()[ vdNew ].componentID = lsubg[i][ cc[j].lsubgVec[k] ].id;
                itC->second.detail.forceGraph()[ vdNew ].initID      = lsubg[i][ cc[j].lsubgVec[k] ].id;

                itC->second.detail.forceGraph()[ vdNew ].namePtr         = lsubg[i][ cc[j].lsubgVec[k] ].namePtr;
                itC->second.detail.forceGraph()[ vdNew ].namePixelWidthPtr   = lsubg[i][ cc[j].lsubgVec[k] ].namePixelWidthPtr;
                itC->second.detail.forceGraph()[ vdNew ].namePixelHeightPtr  = lsubg[i][ cc[j].lsubgVec[k] ].namePixelHeightPtr;
                itC->second.detail.forceGraph()[ vdNew ].coordPtr     = lsubg[i][ cc[j].lsubgVec[k] ].coordPtr;
                itC->second.detail.forceGraph()[ vdNew ].prevCoordPtr = new Coord2( lsubg[i][ cc[j].lsubgVec[k] ].coordPtr->x(),
                                                       lsubg[i][ cc[j].lsubgVec[k] ].coordPtr->y() );
                itC->second.detail.forceGraph()[ vdNew ].forcePtr    = new Coord2( 0, 0 );
                itC->second.detail.forceGraph()[ vdNew ].placePtr    = new Coord2( 0, 0 );
                itC->second.detail.forceGraph()[ vdNew ].shiftPtr    = new Coord2( 0, 0 );
                itC->second.detail.forceGraph()[ vdNew ].weight      = 0.0;

                itC->second.detail.forceGraph()[ vdNew ].widthPtr    = new double( 10.0 );
                itC->second.detail.forceGraph()[ vdNew ].heightPtr   = new double( 10.0 );
                itC->second.detail.forceGraph()[ vdNew ].areaPtr     = new double( 100.0 );
            }

            // add edges
            unsigned int idE = 0;
            for( unsigned int k = 1; k < cc[j].lsubgVec.size(); k++ ){
                for( unsigned int l = 0; l < k; l++ ){

                    ForceGraph::vertex_descriptor vdS = vertex( k, itC->second.detail.forceGraph() );
                    ForceGraph::vertex_descriptor vdT = vertex( l, itC->second.detail.forceGraph() );
                    ForceGraph::vertex_descriptor vdLS = cc[j].lsubgVec[k];
                    ForceGraph::vertex_descriptor vdLT = cc[j].lsubgVec[l];

                    bool isFound = false;
                    ForceGraph::edge_descriptor oldED;
                    tie( oldED, isFound ) = edge( vdLS, vdLT, lsubg[i] );
                    if( isFound == true ){
                    // if( false ){

                        pair< ForceGraph::edge_descriptor, unsigned int > foreE = add_edge( vdS, vdT, itC->second.detail.forceGraph() );
                        ForceGraph::edge_descriptor foreED = foreE.first;
                        itC->second.detail.forceGraph()[ foreED ].id = idE;

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
        //printGraph( lg );
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
//  Cell::_buildCenterGraphs -- build cell graphs
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Cell::_buildCenterGraphs( void )
{
    vector< ForceGraph >            &lsubg  = _pathwayPtr->lsubG();

    // _centerVec.resize( lsubg.size() );    // subsystem size
    for( unsigned int i = 0; i < lsubg.size(); i++ ) {

        multimap< int, CellComponent >::iterator itC = _cellComponentVec[i].begin();
        Polygon2 &contour = _cellVec[i].force().contour();

        // cerr << "contour = " << contour.centroid() << endl;
        // cerr << "init nV = " << num_vertices( _centerVec[i].forceGraph() ) << endl;

        unsigned int idV = 0;
        for (; itC != _cellComponentVec[i].end(); itC++) {

#ifdef DEBUG
            cerr << "i = " << i << " nG = " << _cellComponentVec.size()
                 << " nM = " << itC->second.multiple << " nV = " << itC->second.cellVec.size() << endl;
#endif // DEBUG

            // add vertex
            int length = 10;
            // int length = 50;
            double radius = 50;
            // double radius = 100;
            ForceGraph::vertex_descriptor vdNew = add_vertex( _centerVec[i].forceGraph() );
            double x = radius/2.0 - radius*(double)(rand()%2) + contour.centroid().x() + rand()%(int)length - 0.5*length;
            double y = radius/2.0 - radius*(double)(rand()%2) + contour.centroid().y() + rand()%(int)length - 0.5*length;

            _centerVec[i].forceGraph()[vdNew].id = idV;
            _centerVec[i].forceGraph()[vdNew].groupID = i;
            _centerVec[i].forceGraph()[vdNew].componentID = itC->second.id;
            _centerVec[i].forceGraph()[vdNew].initID = idV;

            _centerVec[i].forceGraph()[vdNew].coordPtr = new Coord2(x, y);
            _centerVec[i].forceGraph()[vdNew].prevCoordPtr = new Coord2(x, y);
            _centerVec[i].forceGraph()[vdNew].forcePtr = new Coord2(0.0, 0.0);
            _centerVec[i].forceGraph()[vdNew].placePtr = new Coord2(0.0, 0.0);
            _centerVec[i].forceGraph()[vdNew].shiftPtr = new Coord2(0.0, 0.0);
            _centerVec[i].forceGraph()[vdNew].weight = (double) itC->second.lsubgVec.size();

            _centerVec[i].forceGraph()[vdNew].widthPtr = new double( sqrt(_paramUnit) );
            _centerVec[i].forceGraph()[vdNew].heightPtr = new double( sqrt(_paramUnit) );
            _centerVec[i].forceGraph()[vdNew].areaPtr = new double( _paramUnit );

            idV++;
        }
        //cerr << "after nV = " << num_vertices( _cellVec[i].forceGraph() ) << endl;
    }

    // add similarity edges
    for( unsigned int i = 0; i < lsubg.size(); i++ ) {

        multimap<int, CellComponent>::iterator itC = _cellComponentVec[i].begin();

        // find sets createtd by the similarity measure
        unsigned int size = 0;
        if( _cellComponentSimilarityVec.size() != 0 ) size = _cellComponentSimilarityVec[i].size();
        map< unsigned int, vector< unsigned int > > setMap;
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

            // go to next
            if( idVec.size() > 1 ){
                setMap.insert( pair< unsigned int, vector< unsigned int > >( minID, idVec ) );
            }
            else{
                multimap< int, CellComponent >::iterator itM = _cellComponentVec[i].begin();
                advance( itM, m );
            }
        }

        // add secondary edges, based on the similarity measure
        unsigned int idE = 0;
        map< unsigned int, vector< unsigned int > >::iterator itM;
        for( itM = setMap.begin(); itM != setMap.end(); itM++ ){

            vector< unsigned int > &idVec = itM->second;
            // for( unsigned int m = 0; m < idVec.size()-1; m++ ){  // chain
            for( unsigned int m = 0; m < idVec.size(); m++ ){       // circle

                ForceGraph::vertex_descriptor vdS = vertex( idVec[m], _centerVec[i].forceGraph() );
                ForceGraph::vertex_descriptor vdT = vertex( idVec[(m+1)%(int)idVec.size()], _centerVec[i].forceGraph() );

                bool isFound = false;
                ForceGraph::edge_descriptor oldED;
                tie( oldED, isFound ) = edge( vdS, vdT, _centerVec[i].forceGraph() );
                cerr << "idVec[m] = " << idVec[m] << " idVec[(m+1)%(int)idVec.size() = " << idVec[(m+1)%(int)idVec.size()] << endl;
                if( isFound == false ){
                    pair< ForceGraph::edge_descriptor, unsigned int > foreE = add_edge( vdS, vdT, _centerVec[i].forceGraph() );
                    ForceGraph::edge_descriptor foreED = foreE.first;
                    _centerVec[i].forceGraph()[ foreED ].id = idE;

                    idE++;
                }
            }
/*
            if( idVec.size() >= 6  ){

                unsigned int step = 1;
                while( true ){

                    unsigned int idS = 1 + step;
                    unsigned int idT = idVec.size() - step;

                    // cerr << "idS = " << idS << " idT = " << idT << endl;
                    ForceGraph::vertex_descriptor vdS = vertex( idVec[ idS ], _centerVec[i].forceGraph() );
                    ForceGraph::vertex_descriptor vdT = vertex( idVec[ idT ], _centerVec[i].forceGraph() );

                    if( (idT-idS) < 3 ) break;

                    bool isFound = false;
                    ForceGraph::edge_descriptor oldED;
                    tie( oldED, isFound ) = edge( vdS, vdT, _centerVec[i].forceGraph() );

                    if( isFound == false ){

                        pair<ForceGraph::edge_descriptor, unsigned int> foreE = add_edge(vdS, vdT, _centerVec[i].forceGraph());
                        ForceGraph::edge_descriptor foreED = foreE.first;
                        fg[foreED].id = idE;

                        idE++;
                    }
                    step++;
                }
            }
*/
        }
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
    vector< ForceGraph >            &lsubg  = _pathwayPtr->lsubG();

    // _cellVec.resize( lsubg.size() );    // subsystem size
    for( unsigned int i = 0; i < lsubg.size(); i++ ) {

        multimap< int, CellComponent >::iterator itC = _cellComponentVec[i].begin();
        Polygon2 &contour = _cellVec[i].force().contour();

        cerr << "init nV = " << num_vertices( _cellVec[i].forceGraph() ) << " nE = " << num_edges( _cellVec[i].forceGraph() ) << endl;
        // cerr << "contour = " << contour.centroid() << endl;
        unsigned int idV = 0, idE = 0;
        for (; itC != _cellComponentVec[i].end(); itC++) {

#ifdef DEBUG
            cerr << "i = " << i << " nG = " << _cellComponentVec.size()
                 << " nM = " << itC->second.multiple << " nV = " << itC->second.cellVec.size() << endl;
#endif // DEBUG


            // add vertices
            vector <ForceGraph::vertex_descriptor> vdVec;
            ForceGraph &dg = itC->second.detail.forceGraph();

            int multiple = itC->second.metaboliteVec.size();
            // int multiple = itC->second.multiple;
            // cerr << "multiple = " << multiple << endl;
            for (unsigned int k = 0; k < multiple; k++) {

                // add vertex
                int length = 50;
                ForceGraph::vertex_descriptor vdNew = add_vertex( _cellVec[i].forceGraph() );
                double x = -100.0 + 200.0*(double)k/(double)multiple + contour.centroid().x() + rand()%(int)length - 0.5*length;
                double y = -50.0 + 100.0*(double)(rand()%2) + contour.centroid().y() + rand()%(int)length - 0.5*length;

                _cellVec[i].forceGraph()[vdNew].id = idV;
                _cellVec[i].forceGraph()[vdNew].groupID = i;
                _cellVec[i].forceGraph()[vdNew].componentID = itC->second.id;
                _cellVec[i].forceGraph()[vdNew].initID = idV;

                _cellVec[i].forceGraph()[vdNew].coordPtr = new Coord2(x, y);
                _cellVec[i].forceGraph()[vdNew].prevCoordPtr = new Coord2(x, y);
                //_cellVec[i].forceGraph()[ vdNew ].coordPtr     = new Coord2( 0.0, 0.0 );
                //_cellVec[i].forceGraph()[ vdNew ].prevCoordPtr = new Coord2( 0.0, 0.0 );
                _cellVec[i].forceGraph()[vdNew].forcePtr = new Coord2(0.0, 0.0);
                _cellVec[i].forceGraph()[vdNew].placePtr = new Coord2(0.0, 0.0);
                _cellVec[i].forceGraph()[vdNew].shiftPtr = new Coord2(0.0, 0.0);
                _cellVec[i].forceGraph()[vdNew].weight = (double) itC->second.lsubgVec.size() / (double) multiple;

                _cellVec[i].forceGraph()[vdNew].widthPtr = new double( sqrt(_paramUnit) );
                _cellVec[i].forceGraph()[vdNew].heightPtr = new double( sqrt(_paramUnit) );
                _cellVec[i].forceGraph()[vdNew].areaPtr = new double( _paramUnit );

                idV++;
                vdVec.push_back(vdNew);
            }
            itC->second.cellgVec.clear();
            itC->second.cellgVec = vdVec;

            // add edges
            if ( vdVec.size() > 1 ) {

                BGL_FORALL_EDGES( ed, dg, ForceGraph ) {

                    ForceGraph::vertex_descriptor vdS = source( ed, dg );
                    ForceGraph::vertex_descriptor vdT = target( ed, dg );

                    unsigned int labelS = dg[vdS].label;
                    unsigned int labelT = dg[vdT].label;

                    bool isConnected = false;
                    if( labelS != labelT ){

                        bool isFound = false;
                        ForceGraph::edge_descriptor oldED;
                        tie( oldED, isFound ) = edge( vdS, vdT, dg );
                        if( isFound == true ){
                            isConnected = true;
                        }
                    }
                    // cerr << "labelS = " << labelS << " labelT = " << labelT << endl;

                    if( isConnected == true ){

                        ForceGraph::vertex_descriptor vdS = vdVec[ labelS ];
                        ForceGraph::vertex_descriptor vdT = vdVec[ labelT ];
                        //ForceGraph::vertex_descriptor vdS = vertex( labelS, _cellVec[i].forceGraph() );
                        //ForceGraph::vertex_descriptor vdT = vertex( labelT, _cellVec[i].forceGraph() );

                        bool isFound = false;
                        ForceGraph::edge_descriptor oldED;
                        tie( oldED, isFound ) = edge( vdS, vdT, _cellVec[i].forceGraph() );
                        // cerr << "degreeS = " << degreeS << " degreeT = " << degreeT << endl;
                        if( isFound == false ){

                            pair<ForceGraph::edge_descriptor, unsigned int> foreE = add_edge( vdS, vdT, _cellVec[i].forceGraph() );
                            ForceGraph::edge_descriptor foreED = foreE.first;
                            _cellVec[i].forceGraph()[foreED].id = idE;

                            idE++;
                        }
                    }
                }

                cerr << "subID = " << i << " vdVec.size() = " << vdVec.size() << endl;
                cerr << "nV = " << num_vertices( _cellVec[i].forceGraph() ) << " nE = " << num_edges( _cellVec[i].forceGraph() ) << endl;
            }
        }
    }
}

void Cell::createPolygonComplexFromDetailGraph( void )
{
    unsigned int idC = 0;
    for( unsigned int i = 0; i < _cellComponentVec.size(); i++ ){

        multimap< int, CellComponent > &componentMap = _cellComponentVec[i];
        multimap< int, CellComponent >::iterator itC = componentMap.begin();

        // cerr << "id = " << itC->second.id << endl;
        for( ; itC != componentMap.end(); itC++ ){

            Force       &f = itC->second.detail.force();
            ForceGraph &fg = itC->second.detail.forceGraph();

            CellComponent &c = itC->second;
            c.contour.clear();
            int cSize = num_vertices( fg );
            if( cSize > 1 ){

                Contour2 contour;
                vector< Polygon2 > pVec;

                contour.init( idC, pVec );
                contour.createContour();
                c.contour = contour;
            }
            else{

                Polygon2 &p = (*f.voronoi().seedVec())[0].cellPolygon;
                p.updateOrientation();;
                c.contour.contour() = p;
            }

            idC++;
        }
    }

    assert( idC == _nComponent );
}

void Cell::updatePolygonComplexFromDetailGraph( void )
{
    unsigned int idC = 0;
    for( unsigned int i = 0; i < _cellComponentVec.size(); i++ ){

        multimap< int, CellComponent > &componentMap = _cellComponentVec[i];
        multimap< int, CellComponent >::iterator itC = componentMap.begin();

        for( ; itC != componentMap.end(); itC++ ){

            Force       &f = itC->second.detail.force();
            ForceGraph &fg = itC->second.detail.forceGraph();

            CellComponent &c = itC->second;
            c.contour.contour().cleanPolygon();
            idC++;
        }
    }
    assert( idC == _nComponent );
}

void Cell::createPolygonComplex( void )
{
    unsigned int idC = 0;
    for( unsigned int i = 0; i < _cellComponentVec.size(); i++ ){

        multimap< int, CellComponent > &componentMap = _cellComponentVec[i];
        Force       &f = _cellVec[i].force();
        ForceGraph &fg = _cellVec[i].forceGraph();
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
                c.contour.contour() = contour.contour();
            }
            else{
                // cerr << "csize = 1" << endl;
                int id = fg[c.cellgVec[0]].id;
                Polygon2 &p = (*f.voronoi().seedVec())[id].cellPolygon;
                p.updateOrientation();;
                c.contour.contour() = p;
            }

            idC++;
        }
    }

    assert( idC == _nComponent );
}

void Cell::cleanPolygonComplex( void )
{
    unsigned int idC = 0;
    for( unsigned int i = 0; i < _cellComponentVec.size(); i++ ){

        multimap< int, CellComponent > &componentMap = _cellComponentVec[i];
        multimap< int, CellComponent >::iterator itC = componentMap.begin();

        for( ; itC != componentMap.end(); itC++ ){

            CellComponent &c = itC->second;
            c.contour.contour().cleanPolygon();
        }
    }
}

int Cell::_computeMCLClusters( ForceGraph &dg )
{
    string inputfilename = "third_party/micans/input.txt";
    string outputfilename = "third_party/micans/output.txt";

    // write the graph
    ofstream ofs( inputfilename.c_str() );
    if ( !ofs ) {
        cerr << "Cannot open the target file : " << inputfilename << endl;
        return -1;
    }

    string cm;
    BGL_FORALL_EDGES( ed, dg, ForceGraph ){

        ForceGraph::vertex_descriptor vdS = source( ed, dg );
        ForceGraph::vertex_descriptor vdT = target( ed, dg );

        ofs << dg[ vdS ].id << "\t" << dg[vdT].id << "\t" << dg[ed].weight << endl;
    }

#ifdef __linux__
    cm = string( "/home/yun/Desktop/gitroot/tools/qtborder/micans/bin/mcl " ) + inputfilename + string( " --abc -o " ) + outputfilename;
    cerr << "cm = " << cm << endl;
#endif	// __linux__
#ifdef __APPLE__
    cm = (qApp->applicationDirPath() + QString( "/third_party/micans/bin/./mcl ") ).toStdString() + inputfilename + string( " --abc -o " ) + outputfilename;
    cerr << "cm = " << cm << endl;
#endif	// __MAC__

    system( cm.c_str() );

    // read the clustering info
    ifstream ifs( outputfilename.c_str() );
    if ( !ifs ) {
        cerr << "Cannot open the target file : " << outputfilename << endl;
        // assert( false );
        return -1;
    }
    else if ( ifs.peek() == std::ifstream::traits_type::eof() ){
        cerr << "The file is empty : " << outputfilename << endl;
        return -1;
    }

    unsigned int cID = 0;
    string str;
    while ( std::getline( ifs, str ) ) {

        if( str.size() > 0 ){

            Tokens tokens;
            boost::split( tokens, str, boost::is_any_of( "\t" ) );

            // cerr << "cID = " << cID << ": ";
            BOOST_FOREACH( const std::string& i, tokens ) {

                int id = stoi( i );
                // cerr << id << ", ";
                ForceGraph::vertex_descriptor vd = vertex( id, dg );
                dg[ vd ].label = cID;
            }
            cID++;
            // cerr << endl;

#ifdef DEBUG
            cout << tokens.size() << " tokens" << endl;
            BOOST_FOREACH( const std::string& i, tokens ) {
                cout << "'" << i << "'" << ", ";
            }
#endif // DEBUG
        }
    }
    ifs.close();


#ifdef DEBUG
    //printGraph( dg );
#endif // DEBUG

    cerr << "cID = " << cID << endl;
    return cID;
}

void Cell::_computeClusters( void )
{
    for( unsigned int i = 0; i < _cellComponentVec.size(); i++ ){ // subsystems number

        multimap< int, CellComponent >::iterator itC = _cellComponentVec[i].begin();
        for( ; itC != _cellComponentVec[i].end(); itC++ ){

            Coord2 &avg = itC->second.contour.contour().centroid();

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

            // vector< MetaboliteGraph::vertex_descriptor > cluster;
            ForceGraph &dg = itC->second.detail.forceGraph();
            int nClusters = _computeMCLClusters( dg );

            itC->second.metaboliteVec.resize( nClusters );
            BGL_FORALL_VERTICES( vd, dg, ForceGraph ){

                unsigned int label = dg[ vd ].label;
                itC->second.metaboliteVec[ label ].push_back( vd );
            }

            cerr << "nV = " << num_vertices( dg ) << " nE = " << num_edges( dg ) << endl;
            // cerr << "computeClusters nCluster = " << nClusters
            //     << " ?= " << itC->second.metaboliteVec.size() << endl;
        }
    }
}

/*
void Cell::updateMCLCoords( void )
{
    vector< ForceGraph >            &lsubg  = _pathwayPtr->lsubG();

    for( unsigned int i = 0; i < _cellComponentVec.size(); i++ ){

        multimap< int, CellComponent >::iterator itC = _cellComponentVec[i].begin();
        for( ; itC != _cellComponentVec[i].end(); itC++ ){

            // ForceGraph::vertex_descriptor vd = vertex( itC->second.id, _cellVec[i].forceGraph() );
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

            vector< vector< ForceGraph::vertex_descriptor > > & cluster = itC->second.metaboliteVec;
            int nClusters = cluster.size();

            // cerr << "nClusters = " << nClusters << endl;
            // int nClusters = _computeClusters( itC->second.detail.forceGraph(), cluster );
            assert( nClusters != -1 );

            itC->second.nMCL = nClusters;

            // build mcl graph
            if( nClusters > 1 ){

                ForceGraph &fg = itC->second.detail.forceGraph();
                ForceGraph &mclg = itC->second.mcl.forceGraph();

                unsigned int nVertices = 0, nEdges = 0;
                // add vertices
                for( unsigned int j = 0; j < nClusters; j++ ){

                    ForceGraph::vertex_descriptor vdNew = add_vertex( mclg );
                    double x = avg.x() + rand() % 20 - 10;
                    double y = avg.y() + rand() % 20 - 10;

                    _cellVec[i].forceGraph()[vdNew].id = nVertices;
                    _cellVec[i].forceGraph()[vdNew].groupID = nVertices;
                    _cellVec[i].forceGraph()[vdNew].componentID = itC->second.id;
                    _cellVec[i].forceGraph()[vdNew].initID = nVertices;

                    _cellVec[i].forceGraph()[vdNew].namePtr = new string( "mcl" );
                    _cellVec[i].forceGraph()[vdNew].coordPtr = new Coord2(x, y);
                    _cellVec[i].forceGraph()[vdNew].prevCoordPtr = new Coord2(x, y);
                    //_cellVec[i].forceGraph()[ vdNew ].coordPtr     = new Coord2( 0.0, 0.0 );
                    //_cellVec[i].forceGraph()[ vdNew ].prevCoordPtr = new Coord2( 0.0, 0.0 );
                    _cellVec[i].forceGraph()[vdNew].forcePtr = new Coord2(0.0, 0.0);
                    _cellVec[i].forceGraph()[vdNew].placePtr = new Coord2(0.0, 0.0);
                    _cellVec[i].forceGraph()[vdNew].shiftPtr = new Coord2(0.0, 0.0);
                    _cellVec[i].forceGraph()[vdNew].weight = 1.0;

                    _cellVec[i].forceGraph()[vdNew].widthPtr = new double(sqrt(_paramUnit));
                    _cellVec[i].forceGraph()[vdNew].heightPtr = new double(sqrt(_paramUnit));
                    _cellVec[i].forceGraph()[vdNew].areaPtr = new double(_paramUnit);

                    nVertices++;
                }

                // add edges
                BGL_FORALL_EDGES( ed, fg, ForceGraph ) {

                    ForceGraph::vertex_descriptor vdS = source( ed, fg );
                    ForceGraph::vertex_descriptor vdT = target( ed, fg );
                    unsigned int labelS = fg[vdS].label;
                    unsigned int labelT = fg[vdT].label;

                    if( labelS != labelT ){
                        ForceGraph::vertex_descriptor vdMCLS = vertex( labelS, mclg );
                        ForceGraph::vertex_descriptor vdMCLT = vertex( labelT, mclg );

                        bool isFound = false;
                        ForceGraph::edge_descriptor oldED;
                        tie( oldED, isFound ) = edge( vdMCLS, vdMCLT, mclg );
                        if( isFound == false ){
                            pair< ForceGraph::edge_descriptor, unsigned int > foreE = add_edge( vdMCLS, vdMCLT, mclg );
                            ForceGraph::edge_descriptor foreED = foreE.first;
                            mclg[ foreED ].id = nEdges;

                            nEdges++;
                        }
                    }
                }
#ifdef DEBUG
                //printGraph( mclg );
#endif // DEBUG
            }
        }
    }

}
*/

void Cell::updateCenterCoords( void )
{
    unsigned int nSystems   = _cellVec.size();
    double radius = 30.0;
    // double radius = 60.0;    // VHM
    // double radius = 100.0;       // KEGG

    double w = *_pathwayPtr->width();
    double h = *_pathwayPtr->height();
    for( unsigned int i = 0; i < _cellVec.size(); i++ ){

        ForceGraph &g = _cellVec[i].forceGraph();
        GraphVizAPI graphvizapi;
        graphvizapi.initGraph< ForceGraph >( &g, NULL, w, h );
        //graphvizapi.initGraph( & (BaseGraph &) g, NULL );
        graphvizapi.layoutGraph< ForceGraph >( &g, NULL );
        graphvizapi.normalization< ForceGraph >( &g, NULL, Coord2( 0.0, 0.0 ), radius*w/h, radius );
        //graphvizapi.normalization< ForceGraph >( &g, NULL, Coord2( 0.0, 0.0 ), _pathwayPtr->width(), _pathwayPtr->height() );
    }

    for( unsigned int i = 0; i < nSystems; i++ ) {

        // draw vertices
        ForceGraph &centerG = _centerVec[i].forceGraph();
        ForceGraph &cellG = _cellVec[i].forceGraph();
        // cerr << i << " centerG.size() = " << num_vertices( centerG ) << endl;
        BGL_FORALL_VERTICES( vd, cellG, ForceGraph ) {

            unsigned int id = cellG[vd].componentID;
            // cerr << "label = " << id << endl;
            ForceGraph::vertex_descriptor vdC = vertex( id, centerG );
            cellG[vd].coordPtr->x() += centerG[vdC].coordPtr->x();// + (double)(rand()%(int)PERTUBE_RANGE)/(PERTUBE_RANGE/radius)-0.5*radius;
            cellG[vd].coordPtr->y() += centerG[vdC].coordPtr->y();// + (double)(rand()%(int)PERTUBE_RANGE)/(PERTUBE_RANGE/radius)-0.5*radius;
        }
    }
}

void Cell::updatePathwayCoords( void )
{
    vector< ForceGraph >            &lsubg  = _pathwayPtr->lsubG();
    double radius = 30.0;
    // double radius = 50.0;
    for( unsigned int i = 0; i < _cellComponentVec.size(); i++ ){

        multimap< int, CellComponent >::iterator itC = _cellComponentVec[i].begin();
        ForceGraph &cg = _cellVec[i].forceGraph();
        for( ; itC != _cellComponentVec[i].end(); itC++ ){

            ForceGraph::vertex_descriptor vd = vertex( itC->second.id, _cellVec[i].forceGraph() );
            Coord2 &avg = itC->second.contour.contour().centroid();
            vector< Coord2 > coordVec;
            if( itC->second.metaboliteVec.size() > 1 ){

                ForceGraph &dg = itC->second.detail.forceGraph();
                // ForceGraph &mcl = itC->second.mcl.forceGraph();
                BGL_FORALL_VERTICES( vd, dg, ForceGraph ) {

                    //Coord2 coord( rand()%(int)range-0.5*range, rand()%(int)range-0.5*range );
                    Coord2 coord( (double)(rand()%(int)PERTUBE_RANGE)/(PERTUBE_RANGE/radius)-0.5*radius,
                                  (double)(rand()%(int)PERTUBE_RANGE)/(PERTUBE_RANGE/radius)-0.5*radius );

                    unsigned int mclID = dg[ vd ].label;
                    // ForceGraph::vertex_descriptor vdM = vertex( mclID, mcl );
                    ForceGraph::vertex_descriptor vdM = itC->second.cellgVec[ mclID ];
                    dg[ vd ].coordPtr->x() = cg[ vdM ].coordPtr->x() + coord.x();
                    dg[ vd ].coordPtr->y() = cg[ vdM ].coordPtr->y() + coord.y();
                    //dg[ vd ].coordPtr->x() = mcl[ vdM ].coordPtr->x() + coord.x();
                    //dg[ vd ].coordPtr->y() = mcl[ vdM ].coordPtr->y() + coord.y();
                    coordVec.push_back( coord );
                }
            }
            else{

                ForceGraph &dg = itC->second.detail.forceGraph();
                BGL_FORALL_VERTICES( vd, dg, ForceGraph ) {

                    // Coord2 coord( rand()%(int)range-0.5*range, rand()%(int)range-0.5*range );
                    Coord2 coord( (double)(rand()%(int)PERTUBE_RANGE)/(PERTUBE_RANGE/radius)-0.5*radius,
                                  (double)(rand()%(int)PERTUBE_RANGE)/(PERTUBE_RANGE/radius)-0.5*radius );

                    dg[ vd ].coordPtr->x() = avg.x() + coord.x();
                    dg[ vd ].coordPtr->y() = avg.y() + coord.y();
                    coordVec.push_back( coord );
                }
            }

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
                s.init( &itM->second.detail.forceGraph(), &itN->second.detail.forceGraph() );
                bool val = s.isSimilar(); // isomorphism
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
    MetaboliteGraph             &g      = _pathwayPtr->g();
    ForceGraph                  &lg     = _pathwayPtr->lg();
    SkeletonGraph               &s      = _pathwayPtr->skeletonG();

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

                                    multimap< Grid2, pair< CellComponent*, CellComponent* > >::iterator it;
                                    bool exist = false;
                                    for( it = _interCellComponentMap.begin(); it != _interCellComponentMap.end(); it++  ){

                                        if( ( idSA == it->first.p() ) && ( idSB == it->first.q() ) &&
                                            ( cca.id == it->second.first->id ) && ( ccb.id == it->second.second->id ) ){
                                            exist = true;
                                        }
                                    }

                                    if( !exist ){
                                        _interCellComponentMap.insert( pair< Grid2,
                                                                       pair< CellComponent*, CellComponent* > >( grid,
                                                        pair< CellComponent*, CellComponent* >( &cca, &ccb ) ) );
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
    multimap< Grid2, pair< CellComponent*, CellComponent* > >::iterator itC;
    UndirectedPropertyGraph reduce;
    VertexIndexMap      vertexIndex     = get( vertex_index, reduce );
    VertexXMap          vertexX         = get( vertex_myx, reduce );
    VertexYMap          vertexY         = get( vertex_myy, reduce );

    for( itC = _interCellComponentMap.begin(); itC != _interCellComponentMap.end(); itC++  ){

        UndirectedPropertyGraph::vertex_descriptor vdS, vdT;
        bool existedS = false,
             existedT = false;

        BGL_FORALL_VERTICES( vd, reduce, UndirectedPropertyGraph ) {
            if( ( vertexX[ vd ] == itC->first.p() ) && ( vertexY[ vd ] == itC->second.first->id ) ){
                vdS = vd;
                existedS = true;
            }
            if( ( vertexX[ vd ] == itC->first.q() ) && ( vertexY[ vd ] == itC->second.second->id ) ){
                vdT = vd;
                existedT = true;
            }
        }

        if( !existedS ){
            vdS = add_vertex( reduce );
            vertexX[ vdS ] = itC->first.p();
            vertexY[ vdS ] = itC->second.first->id;
        }
        if( !existedT ){
            vdT = add_vertex( reduce );
            vertexX[ vdT ] = itC->first.q();
            vertexY[ vdT ] = itC->second.second->id;
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
                    pair< CellComponent*, CellComponent* > >( grid,
                    pair< CellComponent*, CellComponent* >( &ccS, &ccT ) ) );

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
//  Cell::additionalForcesCenter -- additional forces
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Cell::additionalForcesCenter( void )
{
    multimap< Grid2, pair< CellComponent*, CellComponent* > >::iterator itA;

    for( itA = _reducedInterCellComponentMap.begin();
         itA != _reducedInterCellComponentMap.end(); itA++ ){

        unsigned int idS = itA->first.p();
        unsigned int idT = itA->first.q();
        CellComponent &ccS = *itA->second.first;
        CellComponent &ccT = *itA->second.second;

        ForceGraph &fgS = _centerVec[ idS ].forceGraph();
        ForceGraph &fgT = _centerVec[ idT ].forceGraph();
        ForceGraph::vertex_descriptor vdS = vertex( ccS.id, _centerVec[idS].forceGraph() );
        ForceGraph::vertex_descriptor vdT = vertex( ccT.id, _centerVec[idT].forceGraph() );

        //double side = 0.25*( *_centerVec[idS].forceGraph().width() + *_centerVec[idS].forceGraph().width() +
        //                     *_centerVec[idT].forceGraph().width() + *_centerVec[idT].forceGraph().width() );
        double side = 0.25*( _cellVec[ idS ].force().width() + _cellVec[ idS ].force().width() +
                             _cellVec[ idT ].force().width() + _cellVec[ idT ].force().width() );
        double LA = sqrt( side / ( double )max( 1.0, ( double )num_vertices( fgS ) ) );
        double LB = sqrt( side / ( double )max( 1.0, ( double )num_vertices( fgT ) ) );
        double L = 0.5*( LA+LB );

#ifdef DEBUG
        cerr << "side = " << side << endl;
        cerr << "L = " << L << endl;
        cerr << "( " << idS << ", " << idT << " ) = ( " << fgS[ vdS ].id << ", " << fgT[ vdT ].id << " )" << endl;
#endif // DEBUG

        Coord2 diff, unit;
        double dist;

        diff = *fgT[ vdT ].coordPtr - *fgS[ vdS ].coordPtr;
        dist = diff.norm();
        if( dist == 0 ) unit.zero();
        else unit = diff.unit();

        *fgS[ vdS ].forcePtr += _paramAddKa * ( dist - L ) * unit;
        *fgT[ vdT ].forcePtr -= _paramAddKa * ( dist - L ) * unit;
    }
}

//
//  Cell::additionalForcesMiddle -- additional forces
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Cell::additionalForcesMiddle( void )
{
    //multimap< Grid2, pair< CellComponent, CellComponent > >::iterator itA = _interCellComponentMap.begin();
    multimap< Grid2, pair< CellComponent*, CellComponent* > >::iterator itA;

    //for( ; itA != _interCellComponentMap.end(); itA++ ){
    for( itA = _reducedInterCellComponentMap.begin(); itA != _reducedInterCellComponentMap.end(); itA++ ){


        unsigned int idS = itA->first.p();
        unsigned int idT = itA->first.q();
        CellComponent &ccS = *itA->second.first;
        CellComponent &ccT = *itA->second.second;

        ForceGraph &fgS = _cellVec[ idS ].forceGraph();
        ForceGraph &fgT = _cellVec[ idT ].forceGraph();
        ForceGraph::vertex_descriptor vdS = ccS.cellgVec[0];
        ForceGraph::vertex_descriptor vdT = ccT.cellgVec[0];

        double side = 0.25*( _cellVec[ idS ].force().width() + _cellVec[ idS ].force().width() +
                             _cellVec[ idT ].force().width() + _cellVec[ idT ].force().width() );
        double LA = sqrt( side / ( double )max( 1.0, ( double )num_vertices( fgS ) ) );
        double LB = sqrt( side / ( double )max( 1.0, ( double )num_vertices( fgT ) ) );
        double L = 0.5*( LA+LB );

#ifdef DEBUG
        cerr << "L = " << L << endl;
        cerr << "( " << idS << ", " << idT << " ) = ( " << fgS[ vdS ].id << ", " << fgT[ vdT ].id << " )" << endl;
#endif // DEBUG

        Coord2 diff, unit;
        double dist;

        diff = *fgT[ vdT ].coordPtr - *fgS[ vdS ].coordPtr;
        dist = diff.norm();
        if( dist == 0 ) unit.zero();
        else unit = diff.unit();

        //cerr << "BEFORE: " << *fgS[ vdS ].forcePtr;
        //cerr << "FORCE: " << _paramAddKa * ( dist - L ) * unit;
        *fgS[ vdS ].forcePtr += _paramAddKa * ( dist - L ) * unit;
        *fgT[ vdT ].forcePtr -= _paramAddKa * ( dist - L ) * unit;
        //cerr << "AFTER: " << *fgS[ vdS ].forcePtr;
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
    _veCoveragePtr = NULL;
    _veRatioPtr = NULL;
    _cellVec.clear();

    _cellComponentVec.clear();
    _cellComponentSimilarityVec.clear();
    _interCellComponentMap.clear();
    _reducedInterCellComponentMap.clear();
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
