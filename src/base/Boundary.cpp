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

#include "base/Boundary.h"

//------------------------------------------------------------------------------
//	Protected functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------

//
//  Boundary::Boundary -- default constructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
Boundary::Boundary( void )
{
    clearGraph( _graph );

    _shortestPathM.clear();
    _line.clear();
    _lineSta.clear();
    // _lineColor.clear();
    // _lineName.clear();

    _nLines = 0;
    _nStations  = 0;
    _nEdges = 0;
    _meanVSize = 0.0;

    _removedVertices.clear();
    _removedEdges.clear();
    _removedWeights.clear();

    _VEconflict.clear();
    _ratioR.clear();
}

//
//  Boundary::Boundary -- copy constructor
//
//  Inputs
//  obj : object of this class
//
//  Outputs
//  none
//
Boundary::Boundary( const Boundary & obj )
{
    _graph = obj.g();

    _shortestPathM = obj.spM();
    _line = obj.line();
    _lineSta = obj.lineSta();

    for ( unsigned int k = 0; k < _nLines; ++k ) {
        for ( unsigned i = 0; i < 3; ++i )
            _lineColor[ k ][ i ] = obj._lineColor[ k ][ i ];
        strcpy( _lineName[ k ], obj._lineName[ k ] );
    }

    _nLines     = obj._nLines;
    _nStations  = obj._nStations;
    _nEdges     = obj._nEdges;
    _meanVSize = obj._meanVSize;

    _removedVertices = obj.removedVertices();
    _removedEdges   = obj.removedEdges();
    _removedWeights = obj.removedWeights();

    _VEconflict = obj.VEconflict();
    _ratioR     = obj.ratioR();
}


//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------

//
//  Boundary::~Boundary --    destructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
Boundary::~Boundary( void )
{
    clearGraph( _graph );

    _shortestPathM.clear();
    _line.clear();
    _lineSta.clear();

    _nLines = 0;
    _nStations  = 0;
    _nEdges  = 0;
    _meanVSize = 0.0;

    _removedVertices.clear();
    _removedEdges.clear();
    _removedWeights.clear();

    //_VEconflict.clear();
    _ratioR.clear();
}

//
//  Boundary::clear --    clear the current Boundary information
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Boundary::clear( void )
{
    clearGraph( _graph );

    _shortestPathM.clear();
    _line.clear();
    _lineSta.clear();

    _nLines = 0;
    _nStations  = 0;
    _nEdges  = 0;
    _meanVSize = 0.0;

    _removedVertices.clear();
    _removedEdges.clear();
    _removedWeights.clear();

    _VEconflict.clear();
    _ratioR.clear();
}

//
//  Boundary::checkVEConflicrs --    detect vertex-edge pair that is close to each other
//
//  Inputs
//  none
//
//  Outputs
//  none
//
bool Boundary::checkVEConflicts( void )
{
    bool                doConflict      = false;

    _VEconflict.clear();

    // find the vertex-edge pair that has smaller distance than the threshold
    BGL_FORALL_VERTICES( vertex, _graph, BoundaryGraph )
    {
        double              distThreshold   = _distBeta/2.0;

        Coord2 coord = *_graph[ vertex ].smoothPtr;
        unsigned idV = _graph[ vertex ].id;

        BGL_FORALL_EDGES( edge, _graph, BoundaryGraph )
        {
            unsigned idE = _graph[ edge ].id;
            BoundaryGraph::vertex_descriptor vdS = source( edge, _graph );
            BoundaryGraph::vertex_descriptor vdT = target( edge, _graph );

            if ( ( vertex != vdS ) && ( vertex != vdT ) ) {

                Coord2 coordS = *_graph[ vdS ].smoothPtr;
                Coord2 coordT = *_graph[ vdT ].smoothPtr;

                double m = ( coordS.y() - coordT.y() ) / ( coordS.x() - coordT.x() );
                double k = coordS.y() - m * coordS.x();
                double dist = fabs( m * coord.x() - coord.y() + k ) / sqrt( SQUARE( m ) + 1.0 );
                Coord2 vec, p;
                vec.x() = ( coordS - coordT ).y();
                vec.y() = -( coordS - coordT ).x();
                vec = vec / vec.norm();
                p = coord + dist * vec;
                if( fabs( p.y() - m * p.x() - k ) > 1.0e-8 ){
                    p = coord - dist * vec;
#ifdef  DEBUG
                    cerr << "p.y() - m * p.x() + k = " << p.y() - m * p.x() - k << endl;
                    cerr << "s.y() - m * s.x() + k = " << coordS.y() - m * coordS.x() - k << endl;
                    cerr << "t.y() - m * t.x() + k = " << coordT.y() - m * coordT.x() - k << endl;
#endif  // DEBUG
                }
                double r = ( p - coordT ).x() / ( coordS - coordT ).x() ;
                if( ( dist < distThreshold ) && ( 0.0 <= r ) && ( r <= 1.0 ) ) {
#ifdef  DEBUG
                    cerr << "V(" << idV << ") x E(" << vertexID[ vdS ] << ", " << vertexID[ vdT ]
                         << ") dist = " << dist << " r = " << r << endl;
                    cerr << "  V(" << idV << ") = " << coord;
                    cerr << "  V(" << vertexID[ vdS ] << ") = " << coordS.y() - m * coordS.x() - k << " " << coordS;
                    cerr << "  V(" << vertexID[ vdT ] << ") = " << coordT.y() - m * coordT.x() - k << " " << coordT;
                    cerr << "  P = " << p.y() - m * p.x() - k << " " << p;
#endif  // DEBUG
                    //if( ( idV == 2 ) && ( idE == 3 ) )
                    _VEconflict.insert( pair< Grid2, VEPair >( Grid2( idV, idE ), VEPair( vertex, edge ) ) );
                }
            }
        }
    }

    // update ratioR
    _ratioR.clear();
    _ratioR.resize( _VEconflict.size() );
    unsigned int countVE = 0;
    for ( VEMap::iterator it = _VEconflict.begin();
          it != _VEconflict.end(); ++it ) {
        BoundaryGraph::vertex_descriptor vdV = it->second.first;
        BoundaryGraph::edge_descriptor ed = it->second.second;
        BoundaryGraph::vertex_descriptor vdS = source( ed, _graph );
        BoundaryGraph::vertex_descriptor vdT = target( ed, _graph );
        Coord2 & pointV = *_graph[ vdV ].smoothPtr;
        Coord2 & pointS = *_graph[ vdS ].smoothPtr;
        Coord2 & pointT = *_graph[ vdT ].smoothPtr;

        double m = ( pointS.y() - pointT.y() ) / ( pointS.x() - pointT.x() );
        double k = pointS.y() - m * pointS.x();
        double dist = fabs( m * pointV.x() - pointV.y() + k ) / sqrt( SQUARE( m ) + 1.0 );
        Coord2 vec, p;
        vec.x() = ( pointS - pointT ).y();
        vec.y() = -( pointS - pointT ).x();
        vec = vec / vec.norm();
        p = pointV + dist * vec;
        if( fabs( p.y() - m * p.x() - k ) > 1.0e-8 ){
            p = pointV - dist * vec;
        }
        double r = ( p - pointT ).x() / ( pointS - pointT ).x() ;

        _ratioR[ countVE ] = r;

#ifdef DEBUG
        cout << " VEConflict[ " << setw( 3 ) << countVE << " ] : "
             << setw( 3 ) << it->first.p() << " , " << setw( 3 ) << it->first.q()
             << " (" << vertexID[ vdS ] << ", " << vertexID[ vdT ] << ") " << endl
             << " V = " << pointV
             << " S = " << pointS
             << " T = " << pointT
             << " P = " << p
             << " (V-P)*(S-T) = " << (pointV-p)*(pointS-pointT) << endl
             << " distance = " << dist << endl;
#endif  // DEBUG
        countVE++;
    }

    return doConflict;
}


//
//  Boundary::adjustsize --  adjust size of the layout of the Boundary network
//
//  Inputs
//      width: window width
//      height: window height
//
//  Outputs
//      none
//
void Boundary::adjustsize( const int & width, const int & height )
{
    // for vertexGeo
    double xMin =  INFINITY;
    double xMax = -INFINITY;
    double yMin =  INFINITY;
    double yMax = -INFINITY;
    double aspect = ( double )width/( double )height;

    // Scan all the vertex coordinates first
    BGL_FORALL_VERTICES( vertex, _graph, BoundaryGraph )
    {
        Coord2 coord = *_graph[ vertex ].geoPtr;
        if ( coord.x() < xMin ) xMin = coord.x();
        if ( coord.x() > xMax ) xMax = coord.x();
        if ( coord.y() < yMin ) yMin = coord.y();
        if ( coord.y() > yMax ) yMax = coord.y();
    }

    // double range = 0.5 * MAX2( xMax - xMin, yMax - yMin );
    double xRange;
    double yRange;
    double xMid;
    double yMid;
    if( ( xMax - xMin ) / width > ( yMax - yMin ) / height ) {
        xRange  = 0.5 * ( xMax - xMin );
        yRange  = 0.5 * ( xMax - xMin ) * ( 1.0/ aspect );
    }
    else {
        xRange  = 0.5 * ( yMax - yMin ) * aspect;
        yRange  = 0.5 * ( yMax - yMin );
    }

    xRange *= 1.05;
    yRange *= 1.05;
    xMid    = 0.5 * ( xMin + xMax );
    yMid    = 0.5 * ( yMin + yMax );

    // Normalize the coordinates
    BGL_FORALL_VERTICES( vertex, _graph, BoundaryGraph )
    {
        Coord2 geo = *_graph[ vertex ].geoPtr;
        Coord2 smooth = *_graph[ vertex ].smoothPtr;
        Coord2 coord = *_graph[ vertex ].coordPtr;

        geo.setX( width  * ( geo.x() - xMid ) / xRange );
        geo.setY( height * ( geo.y() - yMid ) / yRange );
        smooth.setX( width  * ( smooth.x() - xMid ) / xRange );
        smooth.setY( height * ( smooth.y() - yMid ) / yRange );
        coord.setX( width  * ( coord.x() - xMid ) / xRange );
        coord.setY( height * ( coord.y() - yMid ) / yRange );

        _graph[ vertex ].geoPtr->x() = geo.x();
        _graph[ vertex ].geoPtr->y() = geo.y();
        _graph[ vertex ].smoothPtr->x() = smooth.x();
        _graph[ vertex ].smoothPtr->y() = smooth.y();
        _graph[ vertex ].coordPtr->x() = coord.x();
        _graph[ vertex ].coordPtr->y() = coord.y();

    }

   // compute the unit length of an edge (ratio)
    int nAlpha = 0;
    int nBeta = 0;
    double totallength = 0.0;
    BGL_FORALL_EDGES( edge, _graph, BoundaryGraph )
    {
        BoundaryGraph::vertex_descriptor vdS = source( edge, _graph );
        BoundaryGraph::vertex_descriptor vdT = target( edge, _graph );

        Coord2 coord = *_graph[ vdT ].geoPtr - *_graph[ vdS ].geoPtr;
        totallength += coord.norm();
        double w = _graph[ edge ].weight;
        if( w == 1.0 ) nBeta++;
        else nAlpha++;
    }
    double magLength = 2.0;
    _distBeta = totallength / ( magLength * nAlpha + nBeta );
    _distAlpha = magLength * _distBeta;

    // cerr << "distBeta = " << _distBeta << endl;
}

//
//  Boundary::simplifyLayout --    delete nodes with small distance to the edges bounded by their adjacent nodes
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Boundary::simplifyLayout( void )
{
    //double threshold = 2.5;
    double threshold = 15.0;
    int count = 0;
    _removedVertices.clear();
    _removedEdges.clear();
    _removedWeights.clear();
    //printGraph( _graph );

    while( true ){
        // find minimal distance of middle node to the line composed by its neighbors
        BoundaryGraph::vertex_descriptor mvd, mvdP, mvdN;
        bool noDegreeTwo = true;
        double minDist = INFINITY;
        BGL_FORALL_VERTICES( vertex, _graph, BoundaryGraph )
        {
            BoundaryGraph::degree_size_type degrees = out_degree( vertex, _graph );

            if( degrees == 2 ){

                noDegreeTwo = false;
                BoundaryGraph::out_edge_iterator e, e_end;
                tie( e, e_end ) = out_edges( vertex, _graph );
                BoundaryGraph::edge_descriptor edP = *e;
                BoundaryGraph::vertex_descriptor vTP = target( edP, _graph );
                e++;
                BoundaryGraph::edge_descriptor edN = *e;
                BoundaryGraph::vertex_descriptor vTN = target( edN, _graph );

                Coord2 p = *_graph[ vTP ].coordPtr;
                Coord2 c = *_graph[ vertex ].coordPtr;
                Coord2 n = *_graph[ vTN ].coordPtr;
                double m = ( p.y() - n.y() ) / ( p.x() - n.x() );
                double k = p.y() - m * p.x();
                double dist = fabs( m * c.x() - c.y() + k ) / sqrt( SQUARE( m ) + 1.0 );

#ifdef  DEBUG
                cerr << "V(" << vertexIndex[ vertex ] << "), d = " << degrees
                     << ", dist =  " << dist << endl;
                cerr << "p = " << p;
                cerr << "c = " << c;
                cerr << "n = " << n;
#endif  // DEBUG
                if( dist < minDist ) {
                    minDist = dist;
                    mvd = vertex;
                    mvdP = vTP;
                    mvdN = vTN;
                }
            }
        }

        // check condition
        cerr << "count = " << count << ", minID = " << _graph[ mvd ].initID << ", minDist = " << minDist << endl;
        // if( minDist > threshold || noDegreeTwo == true || vertexIsStation[ mvd ] == false ) break;
        if( minDist > threshold || noDegreeTwo == true ) break;

        // delete vertex and corresponding neighbor edges
        BoundaryGraph::edge_descriptor      edP, edN;
        bool                found = false;
        tie( edP, found ) = edge( mvd, mvdP, _graph );
        assert( found == true );
        tie( edN, found ) = edge( mvd, mvdN, _graph );
        assert( found == true );
        double weight = _graph[ edP ].weight + _graph[ edN ].weight;

        // remove stations and edges
        remove_edge( edP, _graph );
        _removedEdges.push_back( VVIDPair( _graph[ mvd ].initID, _graph[ mvdP ].initID ) );
        _removedWeights.push_back( _graph[ edP ].weight );
        remove_edge( edN, _graph );
        _removedEdges.push_back( VVIDPair( _graph[ mvd ].initID, _graph[ mvdN ].initID ) );
        _removedWeights.push_back( _graph[ edN ].weight );
        clear_vertex( mvd, _graph );
        remove_vertex( mvd, _graph );
        _removedVertices.push_back( _graph[ mvd ].weight );

        // cerr << "mv = " << vertexID[ mvd ] << " mvdP = " << vertexID[ mvdP ] << " mvdN = " << vertexID[ mvdN ] << endl;
        // cerr << "wS = " << edgeWeight[ edP ] << " wT = " << edgeWeight[ edN ] << endl;

        // add new edges
        pair< BoundaryGraph::edge_descriptor, unsigned int > addE = add_edge( mvdP, mvdN, _graph );
        BoundaryGraph::edge_descriptor addED = addE.first;

        // calculate geographical angle
        Coord2 coordO;
        Coord2 coordD;
        if( _graph[ mvdP ].initID < _graph[ mvdN ].initID ){
            coordO = *_graph[ mvdP ].coordPtr;
            coordD = *_graph[ mvdN ].coordPtr;
        }
        else{
            coordO = *_graph[ mvdN ].coordPtr;
            coordD = *_graph[ mvdP ].coordPtr;
        }
        double diffX = coordD.x() - coordO.x();
        double diffY = coordD.y() - coordO.y();
        double angle = atan2( diffY, diffX );

        _graph[ addED ].weight = weight;
        _graph[ addED ].geoAngle = angle;
        _graph[ addED ].smoothAngle = angle;
        _graph[ addED ].angle = angle;

#ifdef  DEBUG
        cerr << "addWeight = " << weight <<endl;
        cerr << "count = " << count << ", edgeWeight[ addED ] = " << edgeWeight[ addED ] << endl;
#endif  // DEBUG

        reorderID();
        count++;
    }

    assert( ( 2*_removedVertices.size() ) == _removedEdges.size() );

#ifdef  DEBUG
    for( unsigned int i = 0; i < _removedVertices.size(); i++ ){
        cerr << "removedV(" << _removedVertices[ i ] << "), ";
    }
    cerr << endl;
    for( unsigned int i = 0; i < _removedEdges.size(); i++ ){
        cerr << "removedE(" << _removedEdges[ i ].first << ", "
             << _removedEdges[ i ].second << "), ";
    }
    cerr << endl;
    for( unsigned int i = 0; i < _removedWeights.size(); i++ ){
        cerr << "removedW(" << _removedWeights[ i ] << "), ";
    }
    cerr << endl;
#endif  // DEBUG
}

//
//  Boundary::reorderID --    reorder vertex and edge ID
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Boundary::reorderID( void )
{
    // reorder vertexID
    _nStations = 0;
    BGL_FORALL_VERTICES( vertex, _graph, BoundaryGraph )
    {
        _graph[ vertex ].id      = _nStations;
        // cerr << "VID = " << vertexIndex[ vertex ] << endl;
        _nStations++;
    }

    _nEdges = 0;
    BGL_FORALL_EDGES( edge, _graph, BoundaryGraph )
    {
        _graph[ edge ].id      = _nEdges;
        // cerr << "EID = " << edgeIndex[ edge ] << endl;
        _nEdges++;
    }
}


//
//  Boundary::movebackNodes --    move the deleted nodes back to the layout
//
//  Inputs
//  obj: the original layout
//
//  Outputs
//  none
//
bool Boundary::movebackNodes( const Boundary & obj, const LAYOUTTYPE type )
{
    if( ( _removedVertices.size() == 0 ) && ( _removedEdges.size() == 0 ) )
        return true;

    BoundaryGraph oGraph = obj.g();

    //cerr << "_removedVerteces = " << _removedVertices.size() << endl;
    //cerr << "_removedEdges = " << _removedEdges.size() << endl;

    unsigned int idV = _removedVertices.back();
    _removedVertices.pop_back();
    VVIDPair pair = _removedEdges.back();
    _removedEdges.pop_back();
    unsigned int idT = pair.second;
    double wT = _removedWeights.back();
    _removedWeights.pop_back();
    pair = _removedEdges.back();
    _removedEdges.pop_back();
    unsigned int idS = pair.second;
    double wS = _removedWeights.back();
    _removedWeights.pop_back();


    // find BoundaryGraph::vertex_descriptor in simplified _graph
    BoundaryGraph::vertex_descriptor vdS, vdT;
    BGL_FORALL_VERTICES( vd, _graph, BoundaryGraph ){
            // cerr << "vertexIndex[ vd ] = " << vertexIndex[ vd ] << endl;
            if ( _graph[ vd ].initID == idS ) vdS = vd;
            if ( _graph[ vd ].initID == idT ) vdT = vd;
        }

    bool found = false;
    BoundaryGraph::edge_descriptor ed;
    tie( ed, found ) = edge( vdS, vdT, _graph );
    // remove edge
    remove_edge( ed, _graph );
    // add vertex
    BoundaryGraph::vertex_descriptor vdNew = add_vertex( _graph );
    _graph[ vdNew ].initID = idV;
    BoundaryGraph::vertex_descriptor vdO = vertex( idV, oGraph );
    Coord2 coord = ( wT * *_graph[ vdS ].coordPtr + wS * *_graph[ vdT ].coordPtr ) / ( wS + wT );
    _graph[ vdNew ].geoPtr->x() = oGraph[ vdO ].geoPtr->x();
    _graph[ vdNew ].geoPtr->y() = oGraph[ vdO ].geoPtr->y();
    _graph[ vdNew ].smoothPtr->x() = oGraph[ vdO ].smoothPtr->x();
    _graph[ vdNew ].smoothPtr->y() = oGraph[ vdO ].smoothPtr->y();
    _graph[ vdNew ].coordPtr->x() = coord.x();
    _graph[ vdNew ].coordPtr->y() = coord.y();

    // add edge
    ed = add_edge( vdNew, vdS, _graph ).first;
    _graph[ ed ].weight = wS;
    // add  edge
    ed = add_edge( vdNew, vdT, _graph ).first;
    _graph[ ed ].weight = wT;

    // reorder vertexID
    reorderID();

    // update edge angle of simplified Boundary layout
    BGL_FORALL_EDGES( edge, _graph, BoundaryGraph ){

        BoundaryGraph::vertex_descriptor vdS = source( edge, _graph );
        BoundaryGraph::vertex_descriptor vdT = target( edge, _graph );

        // calculate geographical angle
        Coord2 coordO, coordD;

        if( type == TYPE_SMOOTH || type == TYPE_OCTILINEAR ){

            // geo angle
            if( _graph[ vdS ].id < _graph[ vdT ].id ){
                coordO = *_graph[ vdS ].geoPtr;
                coordD = *_graph[ vdT ].geoPtr;
            }
            else{
                coordO = *_graph[ vdT ].geoPtr;
                coordD = *_graph[ vdS ].geoPtr;
            }
            double diffX = coordD.x() - coordO.x();
            double diffY = coordD.y() - coordO.y();
            double angle = atan2( diffY, diffX );

            _graph[ edge ].geoAngle = angle;

            // current angle
            if( _graph[ vdS ].id < _graph[ vdT ].id ){
                coordO = *_graph[ vdS ].coordPtr;
                coordD = *_graph[ vdT ].coordPtr;
            }
            else{
                coordO = *_graph[ vdT ].coordPtr;
                coordD = *_graph[ vdS ].coordPtr;
            }
            diffX = coordD.x() - coordO.x();
            diffY = coordD.y() - coordO.y();
            angle = atan2( diffY, diffX );

            _graph[ edge ].angle = angle;
        }

        if( type == TYPE_OCTILINEAR ){

            // smooth angle
            if( _graph[ vdS ].id < _graph[ vdT ].id ){
                coordO = *_graph[ vdS ].smoothPtr;
                coordD = *_graph[ vdT ].smoothPtr;
            }
            else{
                coordO = *_graph[ vdT ].smoothPtr;
                coordD = *_graph[ vdS ].smoothPtr;
            }
            double diffX = coordD.x() - coordO.x();
            double diffY = coordD.y() - coordO.y();
            double angle = atan2( diffY, diffX );

            _graph[ edge ].smoothAngle = angle;
        }

    }

    return false;
}

//
//  Boundary::cloneLayout --    copy all information of the input Boundary
//
//  Inputs
//  obj: original Boundary layout
//
//  Outputs
//  none
//
void Boundary::cloneLayout( const Boundary & obj )
{
    _graph.clear();
    _graph = obj.g();

    _nLines = obj._nLines;
    _nStations = obj._nStations;
    _nEdges = obj._nEdges;
    _nLabels = obj._nLabels;
    _distAlpha = obj._distAlpha;
    _distBeta = obj._distBeta;
    _meanVSize = obj._meanVSize;

    for ( unsigned int k = 0; k < _nLines; ++k ) {

        // copy the line color
        for ( unsigned i = 0; i < 3; ++i )
            _lineColor[ k ][ i ] = obj._lineColor[ k ][ i ];

        // copy the line name
        strcpy( _lineName[ k ], obj._lineName[ k ] );

        // copy line info
        vector< BoundaryGraph::edge_descriptor > line;
        for ( unsigned i = 0; i < obj.line().size(); ++i ){
            line.push_back( obj.line()[ k ][ i ] );
        }
        _line.push_back( line );

        // copy line station info
        vector< BoundaryGraph::vertex_descriptor > lineSta;
        for ( unsigned i = 0; i < obj.lineSta().size(); ++i ){
            lineSta.push_back( obj.lineSta()[ k ][ i ] );
        }
        _lineSta.push_back( lineSta );
    }
}

//
//  Boundary::cloneSmooth --    copy all smooth vertex position to the input Boundary
//
//  Inputs
//  obj: original Boundary layout
//
//  Outputs
//  none
//
void Boundary::cloneSmooth( const Boundary & obj )
{
    BoundaryGraph               sGraph          = obj.g();

    // copy stations
    BGL_FORALL_VERTICES( vdS, sGraph, BoundaryGraph ){
        BGL_FORALL_VERTICES( vd, _graph, BoundaryGraph ){
            if ( _graph[ vd ].initID == sGraph[ vdS ].initID ) {
                _graph[ vd ].smoothPtr->x() = sGraph[ vdS ].smoothPtr->x();
                _graph[ vd ].smoothPtr->y() = sGraph[ vdS ].smoothPtr->y();
                _graph[ vd ].coordPtr->x() = sGraph[ vdS ].coordPtr->x();
                _graph[ vd ].coordPtr->y() = sGraph[ vdS ].coordPtr->y();
            }
        }
    }

    //printGraph( _graph );
}

//
//  Boundary::cloneOctilinear --    copy all octilinear vertex position to the input Boundary
//
//  Inputs
//  obj: original Boundary layout
//
//  Outputs
//  none
//
void Boundary::cloneOctilinear( const Boundary & obj )
{
    BoundaryGraph               sGraph          = obj.g();

    // copy stations
    BGL_FORALL_VERTICES( vdS, sGraph, BoundaryGraph ){
        BGL_FORALL_VERTICES( vd, _graph, BoundaryGraph ){

            if ( _graph[ vd ].initID == sGraph[ vdS ].initID ) {
                _graph[ vd ].coordPtr->x() = sGraph[ vdS ].coordPtr->x();
                _graph[ vd ].coordPtr->y() = sGraph[ vdS ].coordPtr->y();
            }
        }
    }
}


//
//  Boundary::load --    load the Boundary information
//
//  Inputs
//  filename: file name of the network
//
//  Outputs
//  none
//
void Boundary::load( const char * filename )
{
    clear();

    ifstream    ifs( filename );
    char        buf[ MAX_STR ];

    if ( !ifs ) {
        cerr << "Cannot open the target file : " << filename << endl;
        return;
    }
    cerr << " now reading the Boundary network and stations... " << endl;

    _line.clear();
    _lineSta.clear();
    _nLines = 0;
    _nStations = 0;
    _nEdges = 0;

    while ( true ) {

        istringstream istr;
        char prompt[ MAX_STR ], argument[ MAX_STR ];

        //------------------------------------------------------------------------------
        //  Read line name
        //------------------------------------------------------------------------------

        ifs.getline( buf, sizeof( buf ) );
        if ( strcmp( "End", buf ) == 0 ) break;

        istr.clear();
        istr.str( buf );
        istr >> prompt >> argument;
#ifdef DEBUG
        cerr << " prompt = " << prompt << endl;
#endif  // DEBUG
        assert( strcmp( prompt, "Line:" ) == 0 );
        strcpy( _lineName[ _nLines ], argument );
#ifdef DEBUG
        cerr << "Line name = " << _lineName[ _nLines ] << endl;
#endif  // DEBUG

        //------------------------------------------------------------------------------
        //  Read line color
        //------------------------------------------------------------------------------
        double r, g, b;

        ifs.getline( buf, sizeof( buf ) );
        istr.clear();
        istr.str( buf );
        istr >> prompt >> r >> g >> b;
        assert( strcmp( prompt, "Color:" ) == 0 );
        _lineColor[ _nLines ][ 0 ] = r/255.0;
        _lineColor[ _nLines ][ 1 ] = g/255.0;
        _lineColor[ _nLines ][ 2 ] = b/255.0;
#ifdef DEBUG
        cerr << "Line color = "
             << _lineColor[ _nLines ][ 0 ] << ", "
             << _lineColor[ _nLines ][ 1 ] << ", "
             << _lineColor[ _nLines ][ 2 ]
             << endl;
#endif  // DEBUG

        //------------------------------------------------------------------------------
        //  Read station data
        //------------------------------------------------------------------------------
        vector< BoundaryGraph::vertex_descriptor > ptrSta;
        while ( true ) {

            double x, y, pri, w, h;

            ifs.getline( buf, sizeof( buf ) );
            // the end of station info.
            if ( buf[ 0 ] == '#' ) {
                cerr << "The end of the line: nStations = " << _nStations << endl;
                break;
            }

            istr.clear();
            istr.str( buf );
            istr >> argument >> y >> x >> pri >> w >> h;
#ifdef DEBUG
            cerr << " station name = " << argument << " x = " << x << " y = " << y << endl;
#endif  // DEBUG

            // Check if the station exists or not
            BoundaryGraph::vertex_descriptor curVD = NULL;
            BGL_FORALL_VERTICES( vertex, _graph, BoundaryGraph )
            {
                string name = _graph[ vertex ].name;
                if( strcmp( argument, name.c_str() ) == 0 ){

#ifdef DEBUG
                    cerr << "name = " << argument << " old = " << name << endl;
                    cerr << "The station has been in the database." << endl;
#endif  // DEBUG

                    curVD = vertex;
                    break;
                }
            }

            if ( curVD == NULL ){

                curVD = add_vertex( _graph );
                vector< unsigned int > lineID = _graph[ curVD ].lineID;
                lineID.push_back( _nLines );

                _graph[ curVD ].geoPtr = new Coord2( x, y );
                _graph[ curVD ].smoothPtr = new Coord2( x, y );
                _graph[ curVD ].coordPtr = new Coord2( x, y );
                _graph[ curVD ].id = _graph[ curVD ].initID = _nStations;
                _graph[ curVD ].name = argument;
                //vertexName[ curVD ].assign( argument );
                _graph[ curVD ].weight = 1.0;
                _graph[ curVD ].lineID.push_back( _nLines );

                _nStations++;

            }
            else{
                _graph[ curVD ].lineID.push_back( _nLines );
#ifdef DEBUG
                cerr << "[Existing] curV : lineID = " << endl;
                for ( unsigned int k = 0; k < vertexLineID[ curVD ].size(); ++k )
                    cerr << "lineID[ " << setw( 3 ) << k << " ] = " << vertexLineID[ curVD ][ k ] << endl;
#endif  // DEBUG
            }

#ifdef DEBUG
            cout << "VID = " << vertexIndex[curVD] << " (" << vertexCoord[ curVD ].x() << ", " << vertexCoord[ curVD ].y() << " )";
#endif  // DEBUG
            ptrSta.push_back( curVD );
        }
        _lineSta.push_back( ptrSta );

        //------------------------------------------------------------------------------
        //  Read line data
        //------------------------------------------------------------------------------
        vector< BoundaryGraph::edge_descriptor > eachline;
        while ( true ) {
            int     origID, destID;
            double  weight;

            ifs.getline( buf, sizeof( buf ) );

            // the end of station info.
            if ( buf[ 0 ] == '#' ) {
                cerr << "The end of the network data." << endl;
                break;
            }

            istr.clear();
            istr.str( buf );
            istr >> origID >> destID >> weight;
#ifdef DEBUG
            cerr << " Edge : " << origID << " == " << destID << "( " << weight << " )" << endl;
#endif  // DEBUG

            // search previous edge
            bool found = false;
            BoundaryGraph::edge_descriptor oldED;
            BoundaryGraph::vertex_descriptor oldVS = ptrSta[ origID ];
            BoundaryGraph::vertex_descriptor oldVT = ptrSta[ destID ];
            unsigned int indexS = _graph[ oldVS ].initID;
            unsigned int indexT = _graph[ oldVT ].initID;

            tie( oldED, found ) = edge( oldVS, oldVT, _graph );

            if ( found == true ) {

                _graph[ oldED ].lineID.push_back( _nLines );
                eachline.push_back( oldED );
                //bool test = false;
                //tie( oldED, test ) = edge( oldVT, oldVS, _graph );
            }
            else{
                BoundaryGraph::vertex_descriptor src = vertex( indexS, _graph );
                BoundaryGraph::vertex_descriptor tar = vertex( indexT, _graph );

                // handle fore edge
                pair<BoundaryGraph::edge_descriptor, unsigned int> foreE = add_edge( src, tar, _graph );
                BoundaryGraph::edge_descriptor foreED = foreE.first;

                // calculate geographical angle
                Coord2 coordO;
                Coord2 coordD;
                if( _graph[ src ].initID < _graph[ tar ].initID ){
                    coordO = *_graph[ src ].coordPtr;
                    coordD = *_graph[ tar ].coordPtr;
                }
                else{
                    coordO = *_graph[ tar ].coordPtr;
                    coordD = *_graph[ src ].coordPtr;
                }
                double diffX = coordD.x() - coordO.x();
                double diffY = coordD.y() - coordO.y();
                double angle = atan2( diffY, diffX );

                _graph[ foreED ].initID = _graph[ foreED ].id = _nEdges;
                _graph[ foreED ].weight = weight;
                _graph[ foreED ].geoAngle = angle;
                _graph[ foreED ].smoothAngle = angle;
                _graph[ foreED ].angle = angle;
                _graph[ foreED ].lineID.push_back( _nLines );

                eachline.push_back( foreED );
#ifdef  DEBUG
                cout << "nEdges = " << _nEdges << " angle = " << angle << endl;
#endif  // DEBUG
                _nEdges++;
            }

        }

        _line.push_back( eachline );
        _nLines++;

    }
    ifs.close();

    cout << " numStations = " << _nStations << " num_vertices = " << num_vertices( _graph ) << endl;
    cout << " numEdges = " << _nEdges << " num_edges = " << num_edges( _graph ) << endl;

#ifdef  DEBUG
    printGraph( _graph );
#endif  // DEBUG

    return;
}

//
//  Boundary::buildBoundaryGraph --    build the boundary from the voronoi cell
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Boundary::buildBoundaryGraph( void )
{
    for( unsigned int i = 0; i < _polygons.size(); i++ ){

        Polygon2 &polygon = _polygons[i];

        //cerr << endl << "i = " << i << endl;
        for( unsigned int j = 1; j < polygon.elements().size(); j++ ){

            Coord2 &coordS = polygon.elements()[j-1];
            Coord2 &coordT = polygon.elements()[j];

            // Check if the station exists or not
            BoundaryGraph::vertex_descriptor curVDS = NULL;
            BoundaryGraph::vertex_descriptor curVDT = NULL;

            // add vertices
            for( unsigned int k = j-1; k <= j; k++ ){

                BoundaryGraph::vertex_descriptor curVD = NULL;

                // Check if the station exists or not
                BGL_FORALL_VERTICES( vertex, _graph, BoundaryGraph )
                {
                    Coord2 &c = *_graph[ vertex ].coordPtr;
                    if( fabs( (polygon.elements()[k]-c).norm() ) < 1e-2 ){

#ifdef DEBUG
                        cerr << "The node has been in the database." << endl;
#endif  // DEBUG
                        if( k == j-1 ) curVD = curVDS = vertex;
                        if( k == j ) curVD = curVDT = vertex;
                        break;
                    }
                }

                if ( curVD == NULL ){

                    curVD = add_vertex( _graph );
                    vector< unsigned int > lineID = _graph[ curVD ].lineID;
                    lineID.push_back( _nLines );

                    double x = polygon.elements()[k].x();
                    double y = polygon.elements()[k].y();
                    _graph[ curVD ].geoPtr = new Coord2( x, y );
                    _graph[ curVD ].smoothPtr = new Coord2( x, y );
                    _graph[ curVD ].coordPtr = new Coord2( x, y );
                    _graph[ curVD ].forcePtr = new Coord2( 0, 0 );
                    _graph[ curVD ].id = _graph[ curVD ].initID = _nStations;
                    _graph[ curVD ].name = to_string( _graph[ curVD ].id );
                    _graph[ curVD ].weight = 1.0;
                    _graph[ curVD ].lineID.push_back( _nLines );

                    if( k == j-1 ) curVDS = curVD;
                    if( k == j ) curVDT = curVD;
                    _nStations++;
                }
                else{
                    _graph[ curVD ].lineID.push_back( _nLines );
#ifdef DEBUG
                    cerr << "[Existing] curV : lineID = " << endl;
                for ( unsigned int k = 0; k < vertexLineID[ curVD ].size(); ++k )
                    cerr << "lineID[ " << setw( 3 ) << k << " ] = " << vertexLineID[ curVD ][ k ] << endl;
#endif  // DEBUG
                }
            }

            //cerr << "( " << _graph[ curVDS ].id << ", " << _graph[ curVDT ].id << " )" << endl;

            // add edges
            // search previous edge
            bool found = false;
            BoundaryGraph::edge_descriptor oldED;
            //BoundaryGraph::vertex_descriptor oldVS = ptrSta[ origID ];
            //BoundaryGraph::vertex_descriptor oldVT = ptrSta[ destID ];
            //unsigned int indexS = _graph[ curVDS ].initID;
            //unsigned int indexT = _graph[ curVDT ].initID;
            tie( oldED, found ) = edge( curVDS, curVDT, _graph );


            if ( found == true ) {

                _graph[ oldED ].lineID.push_back( _nLines );
                //eachline.push_back( oldED );
                //bool test = false;
                //tie( oldED, test ) = edge( oldVT, oldVS, _graph );
            }
            else{

                //BoundaryGraph::vertex_descriptor src = vertex( indexS, _graph );
                //BoundaryGraph::vertex_descriptor tar = vertex( indexT, _graph );

                // handle fore edge
                pair<BoundaryGraph::edge_descriptor, unsigned int> foreE = add_edge( curVDS, curVDT, _graph );
                BoundaryGraph::edge_descriptor foreED = foreE.first;

                // calculate geographical angle
                Coord2 coordO;
                Coord2 coordD;
                if( _graph[ curVDS ].initID < _graph[ curVDT ].initID ){
                    coordO = *_graph[ curVDS ].coordPtr;
                    coordD = *_graph[ curVDT ].coordPtr;
                }
                else{
                    coordO = *_graph[ curVDT ].coordPtr;
                    coordD = *_graph[ curVDS ].coordPtr;
                }
                double diffX = coordD.x() - coordO.x();
                double diffY = coordD.y() - coordO.y();
                double angle = atan2( diffY, diffX );

                _graph[ foreED ].initID = _graph[ foreED ].id = _nEdges;
                _graph[ foreED ].weight = 1.0;
                _graph[ foreED ].geoAngle = angle;
                _graph[ foreED ].smoothAngle = angle;
                _graph[ foreED ].angle = angle;
                _graph[ foreED ].lineID.push_back( _nLines );

                //eachline.push_back( foreED );
#ifdef  DEBUG
                cout << "nEdges = " << _nEdges << " angle = " << angle << endl;
#endif  // DEBUG
                _nEdges++;
            }

        }
    }

    cerr << "nV = " << num_vertices( _graph ) << " nE = " << num_edges( _graph ) << endl;
}


//
//  Boundary::buildSkeleton --    build the skeleton
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Boundary::buildSkeleton( void )
{
    int size = _seeds.size();
    for( unsigned int i = 0; i < size; i++ ){

        SkeletonGraph::vertex_descriptor vdNew = add_vertex( _skeleton );
        _skeleton[ vdNew ].id = i;
        _skeleton[ vdNew ].initID = i;
        _skeleton[ vdNew ].coordPtr = new Coord2( _seeds[i].x(), _seeds[i].y() );
    }

    unsigned int value = 15;
    for( unsigned int i = 0; i < value; i++ ) {

        unsigned idS = rand()%size;
        unsigned idT = rand()%size;

        if( idS != idT ){

            SkeletonGraph::vertex_descriptor vdS = vertex( idS, _skeleton );
            SkeletonGraph::vertex_descriptor vdT = vertex( idT, _skeleton );

            bool found = false;
            SkeletonGraph::edge_descriptor oldED;
            tie( oldED, found ) = edge( vdS, vdT, _skeleton );

            if( found == false ){

                // handle fore edge
                pair<BoundaryGraph::edge_descriptor, unsigned int> foreE = add_edge( vdS, vdT, _skeleton );
                BoundaryGraph::edge_descriptor foreED = foreE.first;

            }
        }

    }
}

// end of header file
// Do not add any stuff under this line.
