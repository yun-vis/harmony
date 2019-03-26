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
void Cell::_init( double *veCoveragePtr, map< unsigned int, Polygon2 > * polygonComplexPtr )
{
    _veCoveragePtr = veCoveragePtr;
    vector< ForceGraph > &lsubg     = _pathway->lsubG();

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

    _cellVec.resize( lsubg.size() );

    for( unsigned int i = 0; i < lsubg.size(); i++ ){

        // cerr << "nV = " << num_vertices( lsubg[i] ) << " nE = " << num_edges( lsubg[i] ) << endl;
        map< unsigned int, Polygon2 >::iterator itP = polygonComplexPtr->begin();
        advance( itP, i );
        _cellVec[i].forceBone().init( &_cellVec[i].bone(), &itP->second, "../configs/cell.conf" );
        _cellVec[i].forceBone().id() = i;
        // _cellVec[i].forceBone().contour() = itP->second;
    }

    _buildConnectedComponent();
    _computeCellComponentSimilarity();
    _buildInterCellComponents();
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
    //_cellVec.forceBone().clear();
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
            cc[j].multiple = ceil( pow( ceil( totalArea/(*_veCoveragePtr)/(double)_paramUnit ), 1.8 ) );    // text area size

            // cc[j].multiple = ceil( (double)cc[j].lsubgVec.size()/(double)_paramUnit );   // node size
            // cerr << "totalArea = " << totalArea << " cc[j].multiple = " << cc[j].multiple << endl;
            multimap< int, CellComponent >::iterator itC = cellComponent.insert( pair< int, CellComponent >( cc[j].lsubgVec.size(), cc[j] ) );

            ForceGraph &fg = itC->second.detail.bone();

            // add vertices
            for( unsigned int k = 0; k < cc[j].lsubgVec.size(); k++ ){

                ForceGraph::vertex_descriptor vdNew = add_vertex( fg );

                fg[ vdNew ].id          = k;
                fg[ vdNew ].groupID     = i;
                //fg[ vdNew ].componentID = lsubg[i][ cc[j].lsubgVec[k] ].id;
                fg[ vdNew ].initID      = lsubg[i][ cc[j].lsubgVec[k] ].id;

                fg[ vdNew ].namePtr      = lsubg[i][ cc[j].lsubgVec[k] ].namePtr;
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

    _cellVec.resize( lsubg.size() );
    for( unsigned int i = 0; i < lsubg.size(); i++ ) {

        multimap< int, CellComponent >::iterator itC = _cellComponentVec[i].begin();
        Polygon2 &contour = _cellVec[i].forceBone().contour();

        unsigned int idV = 0, idE = 0;
        for (; itC != _cellComponentVec[i].end(); itC++) {

#ifdef DEBUG
            cerr << "i = " << i << " nG = " << _cellComponentVec.size()
                 << " nM = " << itC->second.multiple << " nV = " << itC->second.cellVec.size() << endl;
#endif // DEBUG
            // add vertices
            vector <ForceGraph::vertex_descriptor> vdVec;
            int multiple = itC->second.multiple;
            // cerr << "multiple = " << multiple << endl;
            for (unsigned int k = 0; k < multiple; k++) {

                // add vertex
                int length = 100;
                ForceGraph::vertex_descriptor vdNew = add_vertex(  _cellVec[i].bone() );
                double x = contour.centroid().x() + rand()%(int)length - 0.5*length;
                double y = contour.centroid().y() + rand()%(int)length - 0.5*length;
                x = 0.0;
                y = 0.0;

                _cellVec[i].bone()[vdNew].id = idV;
                _cellVec[i].bone()[vdNew].groupID = i;
                _cellVec[i].bone()[vdNew].componentID = itC->second.id;
                _cellVec[i].bone()[vdNew].initID = idV;

                _cellVec[i].bone()[vdNew].coordPtr = new Coord2(x, y);
                _cellVec[i].bone()[vdNew].prevCoordPtr = new Coord2(x, y);
                //_cellVec[i].bone()[ vdNew ].coordPtr     = new Coord2( 0.0, 0.0 );
                //_cellVec[i].bone()[ vdNew ].prevCoordPtr = new Coord2( 0.0, 0.0 );
                _cellVec[i].bone()[vdNew].forcePtr = new Coord2(0.0, 0.0);
                _cellVec[i].bone()[vdNew].placePtr = new Coord2(0.0, 0.0);
                _cellVec[i].bone()[vdNew].shiftPtr = new Coord2(0.0, 0.0);
                _cellVec[i].bone()[vdNew].weight = (double) itC->second.lsubgVec.size() / (double) multiple;

                _cellVec[i].bone()[vdNew].widthPtr = new double( sqrt(_paramUnit) );
                _cellVec[i].bone()[vdNew].heightPtr = new double( sqrt(_paramUnit) );
                _cellVec[i].bone()[vdNew].areaPtr = new double( _paramUnit );

                idV++;
                vdVec.push_back(vdNew);
            }
            itC->second.cellgVec.clear();
            itC->second.cellgVec = vdVec;
            // if (vdVec.size() > 1) mergeVec.push_back(vdVec);

            // add edges
            // cerr << "subID = " << i << "vdVec.size() = " << vdVec.size() << endl;
            if ( vdVec.size() > 1 ) {
                for (unsigned int k = 0; k < vdVec.size(); k++) {

                    ForceGraph::vertex_descriptor vdS = vdVec[k];
                    ForceGraph::vertex_descriptor vdT = vdVec[(k+1)%(int)vdVec.size()];

                    bool isFound = false;
                    ForceGraph::edge_descriptor oldED;
                    tie( oldED, isFound ) = edge( vdS, vdT, _cellVec[i].bone() );
                    // cerr << "degreeS = " << degreeS << " degreeT = " << degreeT << endl;
                    if( isFound == false ){

                        pair<ForceGraph::edge_descriptor, unsigned int> foreE = add_edge(vdS, vdT, _cellVec[i].bone());
                        ForceGraph::edge_descriptor foreED = foreE.first;
                        _cellVec[i].bone()[foreED].id = idE;

                        idE++;
                    }
                }

                if( vdVec.size() >= 6  ){

                    ForceGraph::vertex_descriptor vdS = vdVec[0];
                    ForceGraph::vertex_descriptor vdT = vdVec[vdVec.size()/2];

                    bool isFound = false;
                    ForceGraph::edge_descriptor oldED;
                    tie( oldED, isFound ) = edge( vdS, vdT, _cellVec[i].bone() );
                    // cerr << "idS = " << _cellVec[i].bone()[vdS].id << " idT = " << _cellVec[i].bone()[vdT].id << endl;
                    if( isFound == false ){

                        pair<ForceGraph::edge_descriptor, unsigned int> foreE = add_edge(vdS, vdT, _cellVec[i].bone());
                        ForceGraph::edge_descriptor foreED = foreE.first;
                        _cellVec[i].bone()[foreED].id = idE;

                        idE++;
                    }
                }
            }

#ifdef DEBUG
            cerr << " id = " << itC->second.id
                 << " multiple = " << itC->second.multiple
                 << " first = " << itC->first
                 << " size = " << itC->second.cellgVec.size() << endl;
#endif // DEBUG
        }

        printGraph( _cellVec[i].bone() );

#ifdef DEBUG
        multimap< Grid2, pair< CellComponent*, CellComponent* > >::iterator itR;
        for( itR = _interCellComponentMap.begin(); itR != _interCellComponentMap.end(); itR++ ){

            // cerr << i << ": vid: " << _cellVec[i].bone()[ vd ].id << endl;
            cerr << itR->first.p() << ": first: ";
            for( unsigned int p = 0; p < itR->second.first->cellgVec.size(); p++ ){
                cerr << _forceCellGraphVec[itR->first.p()][ itR->second.first->cellgVec[p] ].id << ", ";
            }
            cerr << endl;
            cerr << itR->first.q() << ": second: ";
            for( unsigned int p = 0; p < itR->second.second->cellgVec.size(); p++ ){
                cerr << _forceCellGraphVec[itR->first.q()][ itR->second.second->cellgVec[p] ].id << ", ";
            }
            cerr << endl;
        }
        cerr << endl << endl;
#endif // DEBUG
    }


    // initialize the position
    for( unsigned int i = 0; i < lsubg.size(); i++ ) {

        multimap<int, CellComponent>::iterator itC = _cellComponentVec[i].begin();
        Polygon2 &contourI = _cellVec[i].forceBone().contour();

        vector < vector< ForceGraph::vertex_descriptor > > mergeVec;      // vector of cell need to be merged

        for (; itC != _cellComponentVec[i].end(); itC++) {
            // if ( itC->second.cellgVec.size() > 1 )
            mergeVec.push_back( itC->second.cellgVec );
        }

        // find sets createtd by the similarity measure
        unsigned int size = _cellComponentSimilarityVec[i].size();
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

            if( idVec.size() > 1 ){

                setMap.insert( pair< unsigned int, vector< unsigned int > >( minID, idVec ) );
            }
            else{

                multimap< int, CellComponent >::iterator itM = _cellComponentVec[i].begin();
                advance( itM, m );
            }
        }

        // find target edge border edge center
        vector< Coord2 > targetCoords;
        targetCoords.resize( lsubg.size() );
        for( unsigned int m = 0; m < targetCoords.size(); m++ ){

            Polygon2 &contourM = _cellVec[ m ].forceBone().contour();

            //cerr << "cs = " << contourI << endl;
            Polygon2 & cS = contourI;
            Polygon2 & cT = contourM;
            targetCoords[i].zero();

            double maxLength = 0;
            // cerr << "pSize = " << cS.elements().size() << " qSize = " << cT.elements().size() << endl;
            for( unsigned int p = 0; p < cS.elements().size(); p++ ) {
                for( unsigned int q = 0; q < cT.elements().size(); q++ ) {

                    Coord2 &ps = cS.elements()[p];
                    Coord2 &pt = cS.elements()[(p+1)%(int)cS.elements().size()];

                    Coord2 &qs = cT.elements()[q];
                    Coord2 &qt = cT.elements()[(q+1)%(int)cT.elements().size()];

                    if( (ps == qt) && (pt == qs) ){
                        double length = (ps-pt).norm();

                        if( length > maxLength ){
                            targetCoords[m] = (ps+pt)/2.0;
                            maxLength = length;
                        }
                    }
                }
            }
        }

#ifdef DEBUG
        for( unsigned int m = 0; m < targetCoords.size(); m++ ) {
            cerr << "i = " << i << " m = " << m << " targetCoords[m] = " << targetCoords[m];
        }
#endif // DEBUG

        // add secondary edges, based on the similarity measure
        unsigned int idE = num_edges( _cellVec[i].bone() );
        map< unsigned int, vector< unsigned int > >::iterator itM;
        for( itM = setMap.begin(); itM != setMap.end(); itM++ ){

            vector< unsigned int > &idVec = itM->second;
            // for( unsigned int m = 0; m < idVec.size()-1; m++ ){  // chain
            for( unsigned int m = 0; m < idVec.size(); m++ ){       // circle

                ForceGraph::vertex_descriptor vdS = vertex( idVec[m], _cellVec[i].bone() );
                ForceGraph::vertex_descriptor vdT = vertex( idVec[(m+1)%(int)idVec.size()], _cellVec[i].bone() );

                bool isFound = false;
                ForceGraph::edge_descriptor oldED;
                tie( oldED, isFound ) = edge( vdS, vdT, _cellVec[i].bone() );
                // cerr << "degreeS = " << degreeS << " degreeT = " << degreeT << endl;
                if( isFound == false ){
                    pair< ForceGraph::edge_descriptor, unsigned int > foreE = add_edge( vdS, vdT, _cellVec[i].bone() );
                    ForceGraph::edge_descriptor foreED = foreE.first;
                    _cellVec[i].bone()[ foreED ].id = idE;

                    idE++;
                }
            }
            if( idVec.size() >= 6  ){

                unsigned int step = 1;
                while( true ){

                    unsigned int idS = 1 + step;
                    unsigned int idT = idVec.size() - step;

                    // cerr << "idS = " << idS << " idT = " << idT << endl;
                    ForceGraph::vertex_descriptor vdS = vertex( idVec[ idS ], _cellVec[i].bone() );
                    ForceGraph::vertex_descriptor vdT = vertex( idVec[ idT ], _cellVec[i].bone() );

                    if( (idT-idS) < 3 ) break;

                    bool isFound = false;
                    ForceGraph::edge_descriptor oldED;
                    tie( oldED, isFound ) = edge( vdS, vdT, _cellVec[i].bone() );

                    if( isFound == false ){

                        pair<ForceGraph::edge_descriptor, unsigned int> foreE = add_edge(vdS, vdT, _cellVec[i].bone());
                        ForceGraph::edge_descriptor foreED = foreE.first;
                        _cellVec[i].bone()[foreED].id = idE;

                        idE++;
                    }
                    step++;
                }
            }
        }

        // compute the initial position
        cerr << "mergeVec.size() = " << mergeVec.size()
             << " setMap.size() = " << setMap.size()
             << endl;
        unsigned int total = mergeVec.size() + setMap.size();
        double x = contourI.centroid().x(); // + rand()%100 - 50;
        double y = contourI.centroid().y(); // + rand()%100 - 50;
        double mainRadius = 0.0, secondRadius = 0.0;
        double shiftAngle = 0.0;

        // perturbe to find the main and second radius
        unsigned int perturbeTimes = 100;
        double maxRadius = contourI.maxRadiusInPolygon( Coord2( x, y ) );
        for( unsigned int m = 0; m < perturbeTimes; m++ ){

            Coord2 tmp( x + rand()%(int)maxRadius - 0.5*maxRadius,
                        y + rand()%(int)maxRadius - 0.5*maxRadius );
            double tmpR = contourI.maxRadiusInPolygon( tmp );

            if( contourI.inPolygon( tmp ) && ( tmpR > maxRadius ) ){
                x = tmp.x();
                y = tmp.y();
                maxRadius = tmpR;
            }
        }
        // mainRadius = 2.0*maxRadius/3.0;
        // secondRadius = maxRadius/3.0;

#ifdef DEBUG
        //1cerr << "x = " << x << " y = " << y << " maxR = " << maxRadius << endl;
        //cerr << "mainR = " << mainRadius << " secondR = " << secondRadius << endl;
        center.push_back( Coord2( x, y ) );
        radius.push_back( maxRadius );
        con.push_back( contourI );
#endif // DEBUG

        // initialize cell position
        for( unsigned int m = 0; m < mergeVec.size(); m++ ){

            double cosTheta = cos( 2.0*M_PI*(double)m/(double)total + shiftAngle );
            double sinTheta = sin( 2.0*M_PI*(double)m/(double)total + shiftAngle );
            double cx = x + mainRadius * cosTheta;
            double cy = y + mainRadius * sinTheta;

            int targetID = -1;

            for( unsigned int n = 0; n < mergeVec[m].size(); n++ ){

                ForceGraph::vertex_descriptor &vd = mergeVec[m][n];
#ifdef SKIP
                // find the target subsystem id
                if( n == 0 ){

                    multimap< Grid2, pair< CellComponent*, CellComponent* > >::iterator itR;
                    for( itR = _reducedInterCellComponentMap.begin(); itR != _reducedInterCellComponentMap.end(); itR++ ){

                        if( i == itR->first.p() ){
                            for( unsigned int p = 0; p < itR->second.first->cellgVec.size(); p++ ){
                                if( vd == itR->second.first->cellgVec[p] )
                                    targetID = itR->first.q();
                            }
                        }
                        else if( i == itR->first.q() ){
                            for( unsigned int p = 0; p < itR->second.second->cellgVec.size(); p++ ){
                                if( vd == itR->second.second->cellgVec[p] )
                                    targetID = itR->first.p();
                            }
                        }
                    }

                    if( targetID != -1 ) {

                        //cerr << "targetCoords[ " << targetID << " ] = " << targetCoords[ targetID ];
                        cx += ( targetCoords[ targetID ].x() - cx )/2.0;
                        cy += ( targetCoords[ targetID ].y() - cy )/2.0;
                    }

                }
#endif // SKIP
#ifdef DEBUG
                multimap< Grid2, pair< CellComponent*, CellComponent* > >::iterator itR;
                for( itR = _reducedInterCellComponentMap.begin(); itR != _reducedInterCellComponentMap.end(); itR++ ){

                    cerr << i << ": vid: " << _cellVec[i].bone()[ vd ].id << endl;
                    cerr << itR->first.p() << ": first: ";
                    for( unsigned int p = 0; p < itR->second.first->cellgVec.size(); p++ ){
                        cerr << _forceCellGraphVec[itR->first.p()][ itR->second.first->cellgVec[p] ].id << ", ";
                    }
                    cerr << endl;
                    cerr << itR->first.q() << ": second: ";
                    for( unsigned int p = 0; p < itR->second.second->cellgVec.size(); p++ ){
                        cerr << _forceCellGraphVec[itR->first.q()][ itR->second.second->cellgVec[p] ].id << ", ";
                    }
                    cerr << endl;
                }
                cerr << endl << endl;
#endif // DEBUG

                double cosValue = secondRadius * cos( 2.0*M_PI*(double)n/(double)mergeVec[m].size() + shiftAngle );
                double sinValue = secondRadius * sin( 2.0*M_PI*(double)n/(double)mergeVec[m].size() + shiftAngle );
                _cellVec[i].bone()[vd].coordPtr->x() = cx + cosValue + ( (rand()%(int)PERTUBE_RANGE)/(10*PERTUBE_RANGE) - 0.05 );
                _cellVec[i].bone()[vd].coordPtr->y() = cy + sinValue + ( (rand()%(int)PERTUBE_RANGE)/(10*PERTUBE_RANGE) - 0.05 );
                _cellVec[i].bone()[vd].prevCoordPtr->x() = cx + cosValue;
                _cellVec[i].bone()[vd].prevCoordPtr->y() = cy + sinValue;
            }
        }

/*
        // initialize coord in a circular order
        cerr << "setMap = " << setMap.size() << endl;
        for( unsigned int m = 0; m < setMap.size(); m++ ){

            map< unsigned int, vector< unsigned int > >::iterator itM = setMap.begin();
            advance( itM, m );

            double cosTheta = mainRadius * cos( 2.0*M_PI*(double)m/(double)total + shiftAngle );
            double sinTheta = mainRadius * sin( 2.0*M_PI*(double)m/(double)total + shiftAngle );

            int targetID = -1;
            for( unsigned int n = 0; n < itM->second.size(); n++ ){

                double cx = x + cosTheta;
                double cy = y + sinTheta;
                ForceGraph::vertex_descriptor vd = vertex( (itM->second)[n], _cellVec[i].bone() );
#ifdef SKIP
                // find the target cell component belonging to which subsystem
                multimap< Grid2, pair< CellComponent*, CellComponent* > >::iterator itR;
                for( itR = _reducedInterCellComponentMap.begin(); itR != _reducedInterCellComponentMap.end(); itR++ ){

                    if( i == itR->first.p() ){
                        for( unsigned int p = 0; p < itR->second.first->cellgVec.size(); p++ ){

                            // cerr << _cellVec[i].bone()[vd].id << " ?= " << _cellVec[i].bone()[itR->second.first->cellgVec[p]].id << endl;
                            if( vd == itR->second.first->cellgVec[p] )
                                targetID = itR->first.q();
                        }
                    }
                    else if( i == itR->first.q() ){
                        for( unsigned int p = 0; p < itR->second.second->cellgVec.size(); p++ ){
                            if( vd == itR->second.second->cellgVec[p] )
                                targetID = itR->first.p();
                        }
                    }
                }

                if( targetID != -1 ) {
                    // cerr << "targetCoords[ targetID ] = " << targetCoords[ targetID ];
                    cx += (targetCoords[ targetID ].x() - cx )/2.0;
                    cy += (targetCoords[ targetID ].y() - cy )/2.0;
                }
#endif // SKIP
                double cosValue = secondRadius * cos( 2.0*M_PI*(double)n/(double)itM->second.size() + shiftAngle );
                double sinValue = secondRadius * sin( 2.0*M_PI*(double)n/(double)itM->second.size() + shiftAngle );

                _cellVec[i].bone()[vd].coordPtr->x() = cx + cosValue;
                _cellVec[i].bone()[vd].coordPtr->y() = cy + sinValue;
                _cellVec[i].bone()[vd].prevCoordPtr->x() = cx + cosValue;
                _cellVec[i].bone()[vd].prevCoordPtr->y() = cy + sinValue;
            }
        }
*/
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
                cerr << i << ", " << j << ": " << *_cellVec[i].bone()[ itC->second.cellgVec[j] ].coordPtr << endl;
            }
        }
        cerr << endl;
    }
#endif // DEBUG
}

void Cell::createPolygonComplexFromDetailGraph( void )
{
    unsigned int idC = 0;
    for( unsigned int i = 0; i < _cellComponentVec.size(); i++ ){

        multimap< int, CellComponent > &componentMap = _cellComponentVec[i];
        multimap< int, CellComponent >::iterator itC = componentMap.begin();

        cerr << "id = " << itC->second.id << endl;
        for( ; itC != componentMap.end(); itC++ ){

            Force       &f = itC->second.detail.forceBone();
            ForceGraph &fg = itC->second.detail.bone();

            CellComponent &c = itC->second;
            c.contour.clear();
            int cSize = num_vertices( fg );
            if( cSize > 1 ){

                Contour2 contour;
                vector< Polygon2 > pVec;
                for( unsigned int j = 0; j < cSize; j++ ) {
                    pVec.push_back( (*f.voronoi().seedVec())[j].cellPolygon );
                }

                contour.init( idC, pVec );
                contour.createContour();
                c.contour = contour.contour();
            }
            else{
                // cerr << "csize = 1" << endl;
                // int id = fg[c.cellgVec[0]].id;
                Polygon2 &p = (*f.voronoi().seedVec())[0].cellPolygon;
                p.updateOrientation();;
                c.contour = p;
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

            Force       &f = itC->second.detail.forceBone();
            ForceGraph &fg = itC->second.detail.bone();

            CellComponent &c = itC->second;
            c.contour.cleanPolygon();
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
        Force       &f = _cellVec[i].forceBone();
        ForceGraph &fg = _cellVec[i].bone();
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

            idC++;
        }
    }

    assert( idC == _nComponent );
}

void Cell::updatePolygonComplex( void )
{
    unsigned int idC = 0;
    for( unsigned int i = 0; i < _cellComponentVec.size(); i++ ){

        multimap< int, CellComponent > &componentMap = _cellComponentVec[i];
        multimap< int, CellComponent >::iterator itC = componentMap.begin();

        for( ; itC != componentMap.end(); itC++ ){

            CellComponent &c = itC->second;
            c.contour.cleanPolygon();
        }
    }
}

int Cell::_computeClusters( ForceGraph &dg, vector< MetaboliteGraph::vertex_descriptor > & cluster )
{
    string inputfilename = "../micans/input.txt";
    string outputfilename = "../micans/output.txt";

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
    cm = string( "/Users/yun/Desktop/gitroot/tools/qtborder/micans/bin/./mcl " ) + inputfilename + string( " --abc -o " ) + outputfilename;
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

                int id = std::stoi( i );
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
    printGraph( dg );
#endif // DEBUG

    return cID;
}

void Cell::updateMCLCoords( void )
{
    vector< ForceGraph >            &lsubg  = _pathway->lsubG();

    for( unsigned int i = 0; i < _cellComponentVec.size(); i++ ){

        multimap< int, CellComponent >::iterator itC = _cellComponentVec[i].begin();
        for( ; itC != _cellComponentVec[i].end(); itC++ ){

            // ForceGraph::vertex_descriptor vd = vertex( itC->second.id, _cellVec[i].bone() );
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

            vector< MetaboliteGraph::vertex_descriptor > cluster;
            int nClusters = _computeClusters( itC->second.detail.bone(), cluster );
            assert( nClusters != -1 );

            itC->second.nMCL = nClusters;

            // build mcl graph
            if( nClusters > 1 ){

                ForceGraph &fg = itC->second.detail.bone();
                ForceGraph &mclg = itC->second.mcl.bone();

                unsigned int nVertices = 0, nEdges = 0;
                // add vertices
                for( unsigned int j = 0; j < nClusters; j++ ){

                    ForceGraph::vertex_descriptor vdNew = add_vertex( mclg );
                    double x = avg.x() + rand() % 20 - 10;
                    double y = avg.y() + rand() % 20 - 10;

                    _cellVec[i].bone()[vdNew].id = nVertices;
                    _cellVec[i].bone()[vdNew].groupID = nVertices;
                    _cellVec[i].bone()[vdNew].componentID = itC->second.id;
                    _cellVec[i].bone()[vdNew].initID = nVertices;

                    _cellVec[i].bone()[vdNew].namePtr = new string( "mcl" );
                    _cellVec[i].bone()[vdNew].coordPtr = new Coord2(x, y);
                    _cellVec[i].bone()[vdNew].prevCoordPtr = new Coord2(x, y);
                    //_cellVec[i].bone()[ vdNew ].coordPtr     = new Coord2( 0.0, 0.0 );
                    //_cellVec[i].bone()[ vdNew ].prevCoordPtr = new Coord2( 0.0, 0.0 );
                    _cellVec[i].bone()[vdNew].forcePtr = new Coord2(0.0, 0.0);
                    _cellVec[i].bone()[vdNew].placePtr = new Coord2(0.0, 0.0);
                    _cellVec[i].bone()[vdNew].shiftPtr = new Coord2(0.0, 0.0);
                    _cellVec[i].bone()[vdNew].weight = 1.0;

                    _cellVec[i].bone()[vdNew].widthPtr = new double(sqrt(_paramUnit));
                    _cellVec[i].bone()[vdNew].heightPtr = new double(sqrt(_paramUnit));
                    _cellVec[i].bone()[vdNew].areaPtr = new double(_paramUnit);

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
                printGraph( mclg );
#endif // DEBUG
            }
        }
    }
}

void Cell::updatePathwayCoords( void )
{
    vector< ForceGraph >            &lsubg  = _pathway->lsubG();

    for( unsigned int i = 0; i < _cellComponentVec.size(); i++ ){

        multimap< int, CellComponent >::iterator itC = _cellComponentVec[i].begin();
        for( ; itC != _cellComponentVec[i].end(); itC++ ){

            ForceGraph::vertex_descriptor vd = vertex( itC->second.id, _cellVec[i].bone() );
            Coord2 &avg = itC->second.contour.centroid();
            vector< Coord2 > coordVec;
            if( itC->second.nMCL > 1 ){

                ForceGraph &dg = itC->second.detail.bone();
                ForceGraph &mcl = itC->second.mcl.bone();
                BGL_FORALL_VERTICES( vd, dg, ForceGraph ) {

                    //Coord2 coord( rand()%(int)range-0.5*range, rand()%(int)range-0.5*range );
                    Coord2 coord( (double)(rand()%(int)PERTUBE_RANGE)/(10.0*PERTUBE_RANGE)-0.05,
                                  (double)(rand()%(int)PERTUBE_RANGE)/(10.0*PERTUBE_RANGE)-0.05 );

                    unsigned int mclID = dg[ vd ].label;
                    ForceGraph::vertex_descriptor vdM = vertex( mclID, mcl );
                    dg[ vd ].coordPtr->x() = mcl[ vdM ].coordPtr->x() + coord.x();
                    dg[ vd ].coordPtr->y() = mcl[ vdM ].coordPtr->y() + coord.y();
                    coordVec.push_back( coord );
                }
            }
            else{

                ForceGraph &dg = itC->second.detail.bone();
                BGL_FORALL_VERTICES( vd, dg, ForceGraph ) {

                    // Coord2 coord( rand()%(int)range-0.5*range, rand()%(int)range-0.5*range );
                    Coord2 coord( (double)(rand()%(int)PERTUBE_RANGE)/(10.0*PERTUBE_RANGE)-0.05,
                                  (double)(rand()%(int)PERTUBE_RANGE)/(10.0*PERTUBE_RANGE)-0.05 );

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
                s.init( &itM->second.detail.bone(), &itN->second.detail.bone() );
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
    multimap< Grid2, pair< CellComponent*, CellComponent* > >::iterator itA;

    //for( ; itA != _interCellComponentMap.end(); itA++ ){
    for( itA = _reducedInterCellComponentMap.begin(); itA != _reducedInterCellComponentMap.end(); itA++ ){


        unsigned int idS = itA->first.p();
        unsigned int idT = itA->first.q();
        CellComponent &ccS = *itA->second.first;
        CellComponent &ccT = *itA->second.second;

        ForceGraph &fgS = _cellVec[ idS ].bone();
        ForceGraph &fgT = _cellVec[ idT ].bone();
        ForceGraph::vertex_descriptor vdS = ccS.cellgVec[0];
        ForceGraph::vertex_descriptor vdT = ccT.cellgVec[0];

        double side = 0.25*( _cellVec[ idS ].forceBone().width() + _cellVec[ idS ].forceBone().width() +
                             _cellVec[ idT ].forceBone().width() + _cellVec[ idT ].forceBone().width() );
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
