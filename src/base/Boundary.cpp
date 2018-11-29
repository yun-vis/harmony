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
#include "graph/UndirectedPropertyGraph.h"

//------------------------------------------------------------------------------
//	Protected functions
//------------------------------------------------------------------------------
//  Boundary::_stringToDouble -- convert string to double
//
//  Inputs
//      string
//
//  Outputs
//  double
//
double Boundary::_stringToDouble( string str )
{
    double d = 0.0;

    stringstream ss;
    ss << str;
    ss >> d;

    return d;
}

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
    clearGraph( _boundary );
    clearGraph( _skeleton );

    _shortestPathM.clear();
    _line.clear();
    _lineSta.clear();
    // _lineColor.clear();
    // _lineName.clear();

    _nLines = 0;
    _nVertices  = 0;
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
    _boundary = obj.boundary();

    _shortestPathM = obj.spM();
    _line = obj.line();
    _lineSta = obj.lineSta();

    for ( unsigned int k = 0; k < _nLines; ++k ) {
        for ( unsigned i = 0; i < 3; ++i )
            _lineColor[ k ][ i ] = obj._lineColor[ k ][ i ];
        strcpy( _lineName[ k ], obj._lineName[ k ] );
    }

    _nLines     = obj._nLines;
    //_nStations  = obj._nStations;
    //_nEdges     = obj._nEdges;
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
    clearGraph( _boundary );

    _shortestPathM.clear();
    _line.clear();
    _lineSta.clear();

    _nLines = 0;
    //_nStations  = 0;
    //_nEdges  = 0;
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
    clearGraph( _boundary );

    _shortestPathM.clear();
    _line.clear();
    _lineSta.clear();

    _nLines = 0;
    //_nStations  = 0;
    //_nEdges  = 0;
    _meanVSize = 0.0;

    _removedVertices.clear();
    _removedEdges.clear();
    _removedWeights.clear();

    _VEconflict.clear();
    _ratioR.clear();
}

//
//  Boundary::findVertexInComplex --    detect vertex-edge pair that is close to each other
//
//  Inputs
//  coord: coordinates of a point
//  complex: the graph
//  vd: vertex descriptor
//
//  Outputs
//  isFound: binary
//
bool Boundary::findVertexInComplex( Coord2 &coord, ForceGraph &complex,
                                    ForceGraph::vertex_descriptor &target )
{
    bool isFound = false;

    BGL_FORALL_VERTICES( vd, complex, ForceGraph )
    {
        //cerr << " vd " << *complex[vd].coordPtr << endl;
        if( ( coord - *complex[vd].coordPtr ).norm() < 1e-2 ){
            target = vd;
            isFound = true;
        }
    }

    //cerr << "isFound = " << isFound << endl;
    return isFound;
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
    BGL_FORALL_VERTICES( vertex, _boundary, BoundaryGraph )
    {
        double              distThreshold   = _distBeta/2.0;

        Coord2 coord = *_boundary[ vertex ].smoothPtr;
        unsigned idV = _boundary[ vertex ].id;

        BGL_FORALL_EDGES( edge, _boundary, BoundaryGraph )
        {
            unsigned idE = _boundary[ edge ].id;
            BoundaryGraph::vertex_descriptor vdS = source( edge, _boundary );
            BoundaryGraph::vertex_descriptor vdT = target( edge, _boundary );

            if ( ( vertex != vdS ) && ( vertex != vdT ) ) {

                Coord2 coordS = *_boundary[ vdS ].smoothPtr;
                Coord2 coordT = *_boundary[ vdT ].smoothPtr;

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
        BoundaryGraph::vertex_descriptor vdS = source( ed, _boundary );
        BoundaryGraph::vertex_descriptor vdT = target( ed, _boundary );
        Coord2 & pointV = *_boundary[ vdV ].smoothPtr;
        Coord2 & pointS = *_boundary[ vdS ].smoothPtr;
        Coord2 & pointT = *_boundary[ vdT ].smoothPtr;

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
    BGL_FORALL_VERTICES( vertex, _boundary, BoundaryGraph )
    {
        Coord2 coord = *_boundary[ vertex ].geoPtr;
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
    BGL_FORALL_VERTICES( vertex, _boundary, BoundaryGraph )
    {
        Coord2 geo = *_boundary[ vertex ].geoPtr;
        Coord2 smooth = *_boundary[ vertex ].smoothPtr;
        Coord2 coord = *_boundary[ vertex ].coordPtr;

        geo.setX( width  * ( geo.x() - xMid ) / xRange );
        geo.setY( height * ( geo.y() - yMid ) / yRange );
        smooth.setX( width  * ( smooth.x() - xMid ) / xRange );
        smooth.setY( height * ( smooth.y() - yMid ) / yRange );
        coord.setX( width  * ( coord.x() - xMid ) / xRange );
        coord.setY( height * ( coord.y() - yMid ) / yRange );

        _boundary[ vertex ].geoPtr->x() = geo.x();
        _boundary[ vertex ].geoPtr->y() = geo.y();
        _boundary[ vertex ].smoothPtr->x() = smooth.x();
        _boundary[ vertex ].smoothPtr->y() = smooth.y();
        _boundary[ vertex ].coordPtr->x() = coord.x();
        _boundary[ vertex ].coordPtr->y() = coord.y();

    }

   // compute the unit length of an edge (ratio)
    int nAlpha = 0;
    int nBeta = 0;
    double totallength = 0.0;
    BGL_FORALL_EDGES( edge, _boundary, BoundaryGraph )
    {
        BoundaryGraph::vertex_descriptor vdS = source( edge, _boundary );
        BoundaryGraph::vertex_descriptor vdT = target( edge, _boundary );

        Coord2 coord = *_boundary[ vdT ].geoPtr - *_boundary[ vdS ].geoPtr;
        totallength += coord.norm();
        double w = _boundary[ edge ].weight;
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
    //printGraph( _boundary );

    while( true ){
        // find minimal distance of middle node to the line composed by its neighbors
        BoundaryGraph::vertex_descriptor mvd, mvdP, mvdN;
        bool noDegreeTwo = true;
        double minDist = INFINITY;
        BGL_FORALL_VERTICES( vertex, _boundary, BoundaryGraph )
        {
            BoundaryGraph::degree_size_type degrees = out_degree( vertex, _boundary );

            if( degrees == 2 ){

                noDegreeTwo = false;
                BoundaryGraph::out_edge_iterator e, e_end;
                tie( e, e_end ) = out_edges( vertex, _boundary );
                BoundaryGraph::edge_descriptor edP = *e;
                BoundaryGraph::vertex_descriptor vTP = target( edP, _boundary );
                e++;
                BoundaryGraph::edge_descriptor edN = *e;
                BoundaryGraph::vertex_descriptor vTN = target( edN, _boundary );

                Coord2 p = *_boundary[ vTP ].coordPtr;
                Coord2 c = *_boundary[ vertex ].coordPtr;
                Coord2 n = *_boundary[ vTN ].coordPtr;
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
        cerr << "count = " << count << ", minID = " << _boundary[ mvd ].initID << ", minDist = " << minDist << endl;
        // if( minDist > threshold || noDegreeTwo == true || vertexIsStation[ mvd ] == false ) break;
        if( minDist > threshold || noDegreeTwo == true ) break;

        // delete vertex and corresponding neighbor edges
        BoundaryGraph::edge_descriptor      edP, edN;
        bool                found = false;
        tie( edP, found ) = edge( mvd, mvdP, _boundary );
        assert( found == true );
        tie( edN, found ) = edge( mvd, mvdN, _boundary );
        assert( found == true );
        double weight = _boundary[ edP ].weight + _boundary[ edN ].weight;

        // remove stations and edges
        remove_edge( edP, _boundary );
        _removedEdges.push_back( VVIDPair( _boundary[ mvd ].initID, _boundary[ mvdP ].initID ) );
        _removedWeights.push_back( _boundary[ edP ].weight );
        remove_edge( edN, _boundary );
        _removedEdges.push_back( VVIDPair( _boundary[ mvd ].initID, _boundary[ mvdN ].initID ) );
        _removedWeights.push_back( _boundary[ edN ].weight );
        clear_vertex( mvd, _boundary );
        remove_vertex( mvd, _boundary );
        _removedVertices.push_back( _boundary[ mvd ].weight );

        // cerr << "mv = " << vertexID[ mvd ] << " mvdP = " << vertexID[ mvdP ] << " mvdN = " << vertexID[ mvdN ] << endl;
        // cerr << "wS = " << edgeWeight[ edP ] << " wT = " << edgeWeight[ edN ] << endl;

        // add new edges
        pair< BoundaryGraph::edge_descriptor, unsigned int > addE = add_edge( mvdP, mvdN, _boundary );
        BoundaryGraph::edge_descriptor addED = addE.first;

        // calculate geographical angle
        Coord2 coordO;
        Coord2 coordD;
        if( _boundary[ mvdP ].initID < _boundary[ mvdN ].initID ){
            coordO = *_boundary[ mvdP ].coordPtr;
            coordD = *_boundary[ mvdN ].coordPtr;
        }
        else{
            coordO = *_boundary[ mvdN ].coordPtr;
            coordD = *_boundary[ mvdP ].coordPtr;
        }
        double diffX = coordD.x() - coordO.x();
        double diffY = coordD.y() - coordO.y();
        double angle = atan2( diffY, diffX );

        _boundary[ addED ].weight = weight;
        _boundary[ addED ].geoAngle = angle;
        _boundary[ addED ].smoothAngle = angle;
        _boundary[ addED ].angle = angle;

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
    unsigned int nVertices = 0;
    BGL_FORALL_VERTICES( vertex, _boundary, BoundaryGraph )
    {
        _boundary[ vertex ].id      = nVertices;
        // cerr << "VID = " << vertexIndex[ vertex ] << endl;
        nVertices++;
    }

    unsigned int nEdges = 0;
    BGL_FORALL_EDGES( edge, _boundary, BoundaryGraph )
    {
        _boundary[ edge ].id      = nEdges;
        // cerr << "EID = " << edgeIndex[ edge ] << endl;
        nEdges++;
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

    BoundaryGraph oGraph = obj.boundary();

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


    // find BoundaryGraph::vertex_descriptor in simplified _boundary
    BoundaryGraph::vertex_descriptor vdS, vdT;
    BGL_FORALL_VERTICES( vd, _boundary, BoundaryGraph ){
            // cerr << "vertexIndex[ vd ] = " << vertexIndex[ vd ] << endl;
            if ( _boundary[ vd ].initID == idS ) vdS = vd;
            if ( _boundary[ vd ].initID == idT ) vdT = vd;
        }

    bool found = false;
    BoundaryGraph::edge_descriptor ed;
    tie( ed, found ) = edge( vdS, vdT, _boundary );
    // remove edge
    remove_edge( ed, _boundary );
    // add vertex
    BoundaryGraph::vertex_descriptor vdNew = add_vertex( _boundary );
    _boundary[ vdNew ].initID = idV;
    BoundaryGraph::vertex_descriptor vdO = vertex( idV, oGraph );
    Coord2 coord = ( wT * *_boundary[ vdS ].coordPtr + wS * *_boundary[ vdT ].coordPtr ) / ( wS + wT );
    _boundary[ vdNew ].geoPtr->x() = oGraph[ vdO ].geoPtr->x();
    _boundary[ vdNew ].geoPtr->y() = oGraph[ vdO ].geoPtr->y();
    _boundary[ vdNew ].smoothPtr->x() = oGraph[ vdO ].smoothPtr->x();
    _boundary[ vdNew ].smoothPtr->y() = oGraph[ vdO ].smoothPtr->y();
    _boundary[ vdNew ].coordPtr->x() = coord.x();
    _boundary[ vdNew ].coordPtr->y() = coord.y();

    // add edge
    ed = add_edge( vdNew, vdS, _boundary ).first;
    _boundary[ ed ].weight = wS;
    // add  edge
    ed = add_edge( vdNew, vdT, _boundary ).first;
    _boundary[ ed ].weight = wT;

    // reorder vertexID
    reorderID();

    // update edge angle of simplified Boundary layout
    BGL_FORALL_EDGES( edge, _boundary, BoundaryGraph ){

        BoundaryGraph::vertex_descriptor vdS = source( edge, _boundary );
        BoundaryGraph::vertex_descriptor vdT = target( edge, _boundary );

        // calculate geographical angle
        Coord2 coordO, coordD;

        if( type == TYPE_SMOOTH || type == TYPE_OCTILINEAR ){

            // geo angle
            if( _boundary[ vdS ].id < _boundary[ vdT ].id ){
                coordO = *_boundary[ vdS ].geoPtr;
                coordD = *_boundary[ vdT ].geoPtr;
            }
            else{
                coordO = *_boundary[ vdT ].geoPtr;
                coordD = *_boundary[ vdS ].geoPtr;
            }
            double diffX = coordD.x() - coordO.x();
            double diffY = coordD.y() - coordO.y();
            double angle = atan2( diffY, diffX );

            _boundary[ edge ].geoAngle = angle;

            // current angle
            if( _boundary[ vdS ].id < _boundary[ vdT ].id ){
                coordO = *_boundary[ vdS ].coordPtr;
                coordD = *_boundary[ vdT ].coordPtr;
            }
            else{
                coordO = *_boundary[ vdT ].coordPtr;
                coordD = *_boundary[ vdS ].coordPtr;
            }
            diffX = coordD.x() - coordO.x();
            diffY = coordD.y() - coordO.y();
            angle = atan2( diffY, diffX );

            _boundary[ edge ].angle = angle;
        }

        if( type == TYPE_OCTILINEAR ){

            // smooth angle
            if( _boundary[ vdS ].id < _boundary[ vdT ].id ){
                coordO = *_boundary[ vdS ].smoothPtr;
                coordD = *_boundary[ vdT ].smoothPtr;
            }
            else{
                coordO = *_boundary[ vdT ].smoothPtr;
                coordD = *_boundary[ vdS ].smoothPtr;
            }
            double diffX = coordD.x() - coordO.x();
            double diffY = coordD.y() - coordO.y();
            double angle = atan2( diffY, diffX );

            _boundary[ edge ].smoothAngle = angle;
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
    _boundary.clear();
    _boundary = obj.boundary();

    _nLines = obj._nLines;
    _nVertices = obj._nVertices;
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
    BoundaryGraph               sGraph          = obj.boundary();

    // copy stations
    BGL_FORALL_VERTICES( vdS, sGraph, BoundaryGraph ){
        BGL_FORALL_VERTICES( vd, _boundary, BoundaryGraph ){
            if ( _boundary[ vd ].initID == sGraph[ vdS ].initID ) {
                _boundary[ vd ].smoothPtr->x() = sGraph[ vdS ].smoothPtr->x();
                _boundary[ vd ].smoothPtr->y() = sGraph[ vdS ].smoothPtr->y();
                _boundary[ vd ].coordPtr->x() = sGraph[ vdS ].coordPtr->x();
                _boundary[ vd ].coordPtr->y() = sGraph[ vdS ].coordPtr->y();
            }
        }
    }

    //printGraph( _boundary );
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
    BoundaryGraph               sGraph          = obj.boundary();

    // copy stations
    BGL_FORALL_VERTICES( vdS, sGraph, BoundaryGraph ){
        BGL_FORALL_VERTICES( vd, _boundary, BoundaryGraph ){

            if ( _boundary[ vd ].initID == sGraph[ vdS ].initID ) {
                _boundary[ vd ].coordPtr->x() = sGraph[ vdS ].coordPtr->x();
                _boundary[ vd ].coordPtr->y() = sGraph[ vdS ].coordPtr->y();
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
    unsigned int _nStations = 0;
    unsigned int _nEdges = 0;

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
                // cerr << "The end of the line: nStations = " << _nStations << endl;
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
            BGL_FORALL_VERTICES( vertex, _boundary, BoundaryGraph )
            {
                string name = *_boundary[ vertex ].namePtr;
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

                curVD = add_vertex( _boundary );
                vector< unsigned int > lineID = _boundary[ curVD ].lineID;
                lineID.push_back( _nLines );

                _boundary[ curVD ].geoPtr = new Coord2( x, y );
                _boundary[ curVD ].smoothPtr = new Coord2( x, y );
                _boundary[ curVD ].coordPtr = new Coord2( x, y );
                _boundary[ curVD ].id = _boundary[ curVD ].initID = _nStations;
                _boundary[ curVD ].namePtr = new string( argument );
                //vertexName[ curVD ].assign( argument );
                _boundary[ curVD ].weight = 1.0;
                _boundary[ curVD ].lineID.push_back( _nLines );

                _nStations++;

            }
            else{
                _boundary[ curVD ].lineID.push_back( _nLines );
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
            unsigned int indexS = _boundary[ oldVS ].initID;
            unsigned int indexT = _boundary[ oldVT ].initID;

            tie( oldED, found ) = edge( oldVS, oldVT, _boundary );

            if ( found == true ) {

                _boundary[ oldED ].lineID.push_back( _nLines );
                eachline.push_back( oldED );
                //bool test = false;
                //tie( oldED, test ) = edge( oldVT, oldVS, _boundary );
            }
            else{
                BoundaryGraph::vertex_descriptor src = vertex( indexS, _boundary );
                BoundaryGraph::vertex_descriptor tar = vertex( indexT, _boundary );

                // handle fore edge
                pair<BoundaryGraph::edge_descriptor, unsigned int> foreE = add_edge( src, tar, _boundary );
                BoundaryGraph::edge_descriptor foreED = foreE.first;

                // calculate geographical angle
                Coord2 coordO;
                Coord2 coordD;
                if( _boundary[ src ].initID < _boundary[ tar ].initID ){
                    coordO = *_boundary[ src ].coordPtr;
                    coordD = *_boundary[ tar ].coordPtr;
                }
                else{
                    coordO = *_boundary[ tar ].coordPtr;
                    coordD = *_boundary[ src ].coordPtr;
                }
                double diffX = coordD.x() - coordO.x();
                double diffY = coordD.y() - coordO.y();
                double angle = atan2( diffY, diffX );

                _boundary[ foreED ].initID = _boundary[ foreED ].id = _nEdges;
                _boundary[ foreED ].weight = weight;
                _boundary[ foreED ].geoAngle = angle;
                _boundary[ foreED ].smoothAngle = angle;
                _boundary[ foreED ].angle = angle;
                _boundary[ foreED ].lineID.push_back( _nLines );

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

    cout << " numStations = " << _nStations << " num_vertices = " << num_vertices( _boundary ) << endl;
    cout << " numEdges = " << _nEdges << " num_edges = " << num_edges( _boundary ) << endl;

#ifdef  DEBUG
    printGraph( _boundary );
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
    // initialization
    _polygonComplexVD.clear();
    clearGraph( _boundary );
    _nVertices = 0;
    _nEdges = 0;
    _nLines = 0;

    // create boundary graph
    for( unsigned int i = 0; i < _polygonComplex.size(); i++ ){

        Polygon2 &polygon = _polygonComplex[i];
        vector< ForceGraph::vertex_descriptor > vdVec;

        unsigned int size = polygon.elements().size();
        for( unsigned int j = 1; j < size+1; j++ ){

            //Coord2 &coordS = polygon.elements()[j-1];
            //Coord2 &coordT = polygon.elements()[j%polygon.elements().size()];

            // Check if the station exists or not
            BoundaryGraph::vertex_descriptor curVDS = NULL;
            BoundaryGraph::vertex_descriptor curVDT = NULL;

            // add vertices
            for( unsigned int k = 0; k < 2; k++ ){

                BoundaryGraph::vertex_descriptor curVD = NULL;

                // Check if the station exists or not
                BGL_FORALL_VERTICES( vd, _boundary, BoundaryGraph )
                {
                    Coord2 &c = *_boundary[ vd ].coordPtr;
                    if( fabs( ( polygon.elements()[ (j-1+k)%size]-c ).norm() ) < 1e-2 ){

#ifdef DEBUG
                        cerr << "The node has been in the database." << endl;
#endif  // DEBUG
                        if( k == 0 ) curVD = curVDS = vd;
                        if( k == 1 ) curVD = curVDT = vd;
                        break;
                    }
                }

                if ( curVD == NULL ){

                    curVD = add_vertex( _boundary );
                    vector< unsigned int > lineID = _boundary[ curVD ].lineID;
                    lineID.push_back( _nLines );

                    double x = polygon.elements()[j-1+k].x();
                    double y = polygon.elements()[j-1+k].y();
                    _boundary[ curVD ].geoPtr       = new Coord2( x, y );
                    _boundary[ curVD ].smoothPtr    = new Coord2( x, y );
                    _boundary[ curVD ].coordPtr     = new Coord2( x, y );

                    _boundary[ curVD ].id = _boundary[ curVD ].initID = _nVertices;
                    _boundary[ curVD ].namePtr = new string( to_string( _boundary[ curVD ].id ) );
                    _boundary[ curVD ].weight = 1.0;
                    _boundary[ curVD ].lineID.push_back( _nLines );

                    if( k == 0 ) curVDS = curVD;
                    if( k == 1 ) curVDT = curVD;
                    _nVertices++;
                }
                else{
                    _boundary[ curVD ].lineID.push_back( _nLines );
#ifdef DEBUG
                    cerr << "[Existing] curV : lineID = " << endl;
                for ( unsigned int k = 0; k < vertexLineID[ curVD ].size(); ++k )
                    cerr << "lineID[ " << setw( 3 ) << k << " ] = " << vertexLineID[ curVD ][ k ] << endl;
#endif  // DEBUG
                }
            }
            vdVec.push_back( curVDS );
            // cerr << _boundary[ curVDS ].id << " ";

            //cerr << "( " << _boundary[ curVDS ].id << ", " << _boundary[ curVDT ].id << " )" << endl;

            // add edges
            // search previous edge
            bool found = false;
            BoundaryGraph::edge_descriptor oldED;
            //BoundaryGraph::vertex_descriptor oldVS = ptrSta[ origID ];
            //BoundaryGraph::vertex_descriptor oldVT = ptrSta[ destID ];
            //unsigned int indexS = _boundary[ curVDS ].initID;
            //unsigned int indexT = _boundary[ curVDT ].initID;
            tie( oldED, found ) = edge( curVDS, curVDT, _boundary );


            if ( found == true ) {

                _boundary[ oldED ].lineID.push_back( _nLines );
                //eachline.push_back( oldED );
                //bool test = false;
                //tie( oldED, test ) = edge( oldVT, oldVS, _boundary );
            }
            else{

                //BoundaryGraph::vertex_descriptor src = vertex( indexS, _boundary );
                //BoundaryGraph::vertex_descriptor tar = vertex( indexT, _boundary );

                // handle fore edge
                pair<BoundaryGraph::edge_descriptor, unsigned int> foreE = add_edge( curVDS, curVDT, _boundary );
                BoundaryGraph::edge_descriptor foreED = foreE.first;

                // calculate geographical angle
                Coord2 coordO;
                Coord2 coordD;
                if( _boundary[ curVDS ].initID < _boundary[ curVDT ].initID ){
                    coordO = *_boundary[ curVDS ].coordPtr;
                    coordD = *_boundary[ curVDT ].coordPtr;
                }
                else{
                    coordO = *_boundary[ curVDT ].coordPtr;
                    coordD = *_boundary[ curVDS ].coordPtr;
                }
                double diffX = coordD.x() - coordO.x();
                double diffY = coordD.y() - coordO.y();
                double angle = atan2( diffY, diffX );

                _boundary[ foreED ].initID = _boundary[ foreED ].id = _nEdges;
                _boundary[ foreED ].weight = 1.0;
                _boundary[ foreED ].geoAngle = angle;
                _boundary[ foreED ].smoothAngle = angle;
                _boundary[ foreED ].angle = angle;
                _boundary[ foreED ].lineID.push_back( _nLines );

                //eachline.push_back( foreED );
#ifdef  DEBUG
                cout << "nEdges = " << _nEdges << " angle = " << angle << endl;
#endif  // DEBUG
                _nEdges++;
            }
        }
        _polygonComplexVD.insert( pair< unsigned int, vector< ForceGraph::vertex_descriptor > >( i, vdVec ) );
    }

    // create lines
    // _line.resize( _polygonComplexVD.size() );
    map< unsigned int, vector< BoundaryGraph::vertex_descriptor > >::iterator itS;
    // map< unsigned int, vector< BoundaryGraph::vertex_descriptor > >::itereator itT;
    for( itS = _polygonComplexVD.begin(); itS != _polygonComplexVD.end(); itS++ ){
        vector< BoundaryGraph::vertex_descriptor > &vdVec = itS->second;
        _lineSta.push_back( vdVec );
    }
    _nLines = _polygonComplexVD.size();

    cerr << "nV = " << _nVertices << " nE = " << _nEdges
         << " nL = " << _nLines << endl;
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
    clearGraph( _skeleton );

    // create a xml document
    TiXmlDocument *xmlPtr = new TiXmlDocument( "../data/skeleton-A.xml" );
    //TiXmlDocument *xmlPtr = new TiXmlDocument( "../data/skeleton-major.xml" );
    //TiXmlDocument *xmlPtr = new TiXmlDocument( "../data/skeleton-full.xml" );
    xmlPtr->LoadFile();

    TiXmlElement *elemPtr = xmlPtr->FirstChildElement()->FirstChildElement()->FirstChildElement();

    // should always have a valid root but handle gracefully if it does
    if ( !elemPtr ) return;

    for( ; elemPtr; elemPtr = elemPtr->NextSiblingElement() ) {

        const string key = elemPtr->Value();

        if( key == "node" ){
            string id = elemPtr->Attribute( "id" );
            id.erase( 0, 1 );
            int vid = stoi( id );
            const string sx = elemPtr->Attribute( "x" );
            double x = stod( sx );
            const string sy = elemPtr->Attribute( "y" );
            double y = stod( sy );
            const string sw = elemPtr->Attribute( "width" );
            double w = stod( sw );
            const string sh = elemPtr->Attribute( "height" );
            double h = stod( sh );
            const string sa = elemPtr->Attribute( "area" );
            double a = stod( sa );
#ifdef DEBUG
            cerr << "key = " << key << " id = " << i << " x = " << x << " y = " << y
                 << " w = " << w << " h = " << h << " a = " << a << endl;
#endif // DEBUG
            ForceGraph::vertex_descriptor vdNew = add_vertex( _skeleton );
            _skeleton[ vdNew ].id = vid;
            _skeleton[ vdNew ].initID = vid;
            _skeleton[ vdNew ].coordPtr = new Coord2( x, y );
            _skeleton[ vdNew ].widthPtr = new double( w );
            _skeleton[ vdNew ].heightPtr = new double( h );
            _skeleton[ vdNew ].areaPtr = new double( a );
            // cerr << "area = " << a << endl;
        }
        else if ( key == "edge" ){
            string id = elemPtr->Attribute( "id" );
            id.erase( 0, 1 );
            int eid = stoi( id );
            string source = elemPtr->Attribute( "source" );
            source.erase( 0, 1 );
            int s = stoi( source );
            string target = elemPtr->Attribute( "target" );
            target.erase( 0, 1 );
            int t = stoi( target );
#ifdef DEBUG
            cerr << "key = " << key << " id = " << i << " s = " << s << " t = " << t << endl;
#endif // DEBUG
            ForceGraph::vertex_descriptor vdS = vertex( s, _skeleton );
            ForceGraph::vertex_descriptor vdT = vertex( t, _skeleton );

            bool found = false;
            ForceGraph::edge_descriptor oldED;
            tie( oldED, found ) = edge( vdS, vdT, _skeleton );

            if( found == false ){

                // handle fore edge
                pair<ForceGraph::edge_descriptor, unsigned int> foreE = add_edge( vdS, vdT, _skeleton );
                ForceGraph::edge_descriptor foreED = foreE.first;
                _skeleton[ foreED ].id = eid;
            }
        }
        else {
            cerr << "wrong key at " << __LINE__ << " in " << __FILE__ << endl;
        }
    }
    //cerr << "nV = " << num_vertices( _skeleton ) << " nE = " << num_edges( _skeleton ) << endl;
}

//
//  Boundary::decomposeSkeleton --    decompose the skeleton
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Boundary::decomposeSkeleton( void )
{
    double unit = 8000.0;
    unsigned int index = num_vertices( _skeleton );

    // copy skeleton to composite
    BGL_FORALL_VERTICES( vd, _skeleton, ForceGraph )
    {
        ForceGraph::vertex_descriptor vdNew = add_vertex( _composite );
        _composite[ vdNew ].id = _skeleton[vd].id;
        _composite[ vdNew ].initID = _skeleton[vd].initID;

        _composite[ vdNew ].coordPtr = new Coord2( _skeleton[vd].coordPtr->x(), _skeleton[vd].coordPtr->y() );
        _composite[ vdNew ].prevCoordPtr = new Coord2( _skeleton[vd].coordPtr->x(), _skeleton[vd].coordPtr->y() );
        _composite[ vdNew ].forcePtr     = new Coord2( 0, 0 );
        _composite[ vdNew ].placePtr     = new Coord2( 0, 0 );
        _composite[ vdNew ].shiftPtr     = new Coord2( 0, 0 );

        _composite[ vdNew ].widthPtr = new double( *_skeleton[vd].widthPtr );
        _composite[ vdNew ].heightPtr = new double( *_skeleton[vd].heightPtr );
        _composite[ vdNew ].areaPtr = new double( *_skeleton[vd].areaPtr );
    }
    BGL_FORALL_EDGES( ed, _skeleton, ForceGraph )
    {
        ForceGraph::vertex_descriptor vdS = source( ed, _skeleton );
        ForceGraph::vertex_descriptor vdT = target( ed, _skeleton );
        ForceGraph::vertex_descriptor vdCompoS = vertex( _skeleton[vdS].id, _composite );
        ForceGraph::vertex_descriptor vdCompoT = vertex( _skeleton[vdT].id, _composite );

        // add edge
        pair< ForceGraph::edge_descriptor, unsigned int > foreE = add_edge( vdCompoS, vdCompoT, _composite );
        BoundaryGraph::edge_descriptor foreED = foreE.first;
        _composite[ foreED ].id = _skeleton[ed].id;
    }

#ifdef DEBUG
    BGL_FORALL_EDGES( ed, _composite, ForceGraph )
    {
        cerr << "eID = " << _composite[ed].id << endl;
    }
#endif // DEBUG

    BGL_FORALL_VERTICES( vd, _composite, ForceGraph )
    {
        int mag = round((*_composite[ vd ].widthPtr) * (*_composite[ vd ].heightPtr)/unit);

#ifdef DEBUG
            cerr << "area = " << (*_composite[ vd ].widthPtr) * (*_composite[ vd ].heightPtr)
             << " mag = " << mag << endl;
#endif // DEBUG

        vector< ForceGraph::edge_descriptor > removeEVec;

        if( mag > 1 ){

            ForceGraph::vertex_descriptor vdC = vd;
            ForceGraph::out_edge_iterator eo, eo_end;
            Coord2 origin = *_composite[ vd ].coordPtr;
            vector< ForceGraph::vertex_descriptor > extendVD;

            // record the adjacent vertices for edge connection
            map< unsigned int, ForceGraph::vertex_descriptor > vdAdjacent;
            // cerr << "degree = " << out_degree( vd, _composite ) << endl;
            for( tie( eo, eo_end ) = out_edges( vd, _composite ); eo != eo_end; eo++ ){

                ForceGraph::edge_descriptor ed = *eo;
                ForceGraph::vertex_descriptor vdT = target( ed, _composite );
                vdAdjacent.insert( pair< unsigned int, ForceGraph::vertex_descriptor >( _composite[ed].id, vdT ) );
                //remove_edge( *eo, _composite );
                removeEVec .push_back( ed );
            }

            // add composite vertices
            for( int i = 0; i < mag; i++ ){

                double cosTheta = 1.0 * cos( 2.0*M_PI*(double)i/(double)mag );
                double sinTheta = 1.0 * sin( 2.0*M_PI*(double)i/(double)mag );
                double x = origin.x() + 100*cosTheta;
                double y = origin.y() + 100*sinTheta;

                if( i == 0 ){

                    _composite[ vd ].coordPtr->x() = x;
                    _composite[ vd ].coordPtr->y() = y;

                    // post processing
                    extendVD.push_back( vdC );
                }
                else{

                    // add vertex
                    ForceGraph::vertex_descriptor vdNew = add_vertex( _composite );
                    _composite[ vdNew ].id = index;
                    _composite[ vdNew ].initID = _composite[vd].initID;

                    _composite[ vdNew ].coordPtr     = new Coord2( x, y );
                    _composite[ vdNew ].prevCoordPtr = new Coord2( x, y);
                    _composite[ vdNew ].forcePtr     = new Coord2( 0, 0 );
                    _composite[ vdNew ].placePtr     = new Coord2( 0, 0 );
                    _composite[ vdNew ].shiftPtr     = new Coord2( 0, 0 );

                    _composite[ vdNew ].widthPtr    = new double( sqrt( unit ) );
                    _composite[ vdNew ].heightPtr   = new double( sqrt( unit ) );
                    _composite[ vdNew ].areaPtr     = new double( unit );

                    // add edge
                    pair< ForceGraph::edge_descriptor, unsigned int > foreE = add_edge( vdC, vdNew, _composite );
                    BoundaryGraph::edge_descriptor foreED = foreE.first;
                    _composite[ foreED ].id = num_edges( _composite );

                    // add last edge to form a circle
                    if( i == mag-1 ){
                        pair< ForceGraph::edge_descriptor, unsigned int > foreE = add_edge( vd, vdNew, _composite );
                        BoundaryGraph::edge_descriptor foreED = foreE.first;
                        _composite[ foreED ].id = num_edges( _composite );
                    }

                    // post processing
                    extendVD.push_back( vdNew );
                    vdC = vdNew;
                    index++;
                }
            }

            // add edges
            // cerr << "size = " << vdAdjacent.size() << " ext = " << extendVD.size() << endl;
            map< unsigned int, ForceGraph::vertex_descriptor >::iterator itA;
            for( itA = vdAdjacent.begin(); itA != vdAdjacent.end(); itA++ ){

                ForceGraph::vertex_descriptor cloestVD = extendVD[0];
                ForceGraph::vertex_descriptor vdT = itA->second;
                double minDist = (*_composite[ cloestVD ].coordPtr - *_composite[ vdT ].coordPtr).norm();
                for( unsigned int i = 1; i < extendVD.size(); i++ ){

                    ForceGraph::vertex_descriptor vdS = extendVD[ i ];
                    double dist = (*_composite[ vdS ].coordPtr - *_composite[ vdT ].coordPtr).norm();

                    if( dist < minDist ){
                        cloestVD = extendVD[i];
                        minDist = dist;
                    }
                }

                pair< ForceGraph::edge_descriptor, unsigned int > foreE = add_edge( vdT, cloestVD, _composite );
                ForceGraph::edge_descriptor foreED = foreE.first;
                _composite[ foreED ].id = itA->first;
            }

            // remove edges
            // cerr << "eSize = " << removeEVec.size() << endl;
            for( unsigned int i = 0; i < removeEVec.size(); i++ ){
                // cerr << "eID = " << _composite[removeEVec[i]].id << endl;
                remove_edge( removeEVec[i], _composite );
            }
        }
    }

}

//
//  Boundary::normalizeSkeleton --    normalize the skeleton
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Boundary::normalizeSkeleton( const int & width, const int & height )
{
    // initialization
    double xMin =  INFINITY;
    double xMax = -INFINITY;
    double yMin =  INFINITY;
    double yMax = -INFINITY;

    // Scan all the vertex coordinates first
    BGL_FORALL_VERTICES( vd, _composite, ForceGraph )
    {
        Coord2 coord = *_composite[ vd ].coordPtr;
        if ( coord.x() < xMin ) xMin = coord.x();
        if ( coord.x() > xMax ) xMax = coord.x();
        if ( coord.y() < yMin ) yMin = coord.y();
        if ( coord.y() > yMax ) yMax = coord.y();
    }

    // Normalize the coordinates
    BGL_FORALL_VERTICES( vd, _skeleton, ForceGraph )
    {
        Coord2 coord;
        coord.x() = 0.8* (double)width * ( _skeleton[ vd ].coordPtr->x() - xMin )/( xMax - xMin ) - 0.4*(double)width;
        coord.y() = 0.8* (double)height * ( _skeleton[ vd ].coordPtr->y() - yMin )/( yMax - yMin ) - 0.4*(double)height;
        _skeleton[ vd ].coordPtr->x() = coord.x();
        _skeleton[ vd ].coordPtr->y() = coord.y();
        // cerr << coord;
    }

    BGL_FORALL_VERTICES( vd, _composite, ForceGraph )
    {
        Coord2 coord;
        coord.x() = 0.8* (double)width * ( _composite[ vd ].coordPtr->x() - xMin )/( xMax - xMin ) - 0.4*(double)width;
        coord.y() = 0.8* (double)height * ( _composite[ vd ].coordPtr->y() - yMin )/( yMax - yMin ) - 0.4*(double)height;
        _composite[ vd ].coordPtr->x() = coord.x();
        _composite[ vd ].coordPtr->y() = coord.y();
        // cerr << coord;
    }
}


//
//  Boundary::createPolygonComplex --    create the ploygon complex
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Boundary::createPolygonComplex( void )
{
    vector< vector< Polygon2 > > _polygonMat;
    vector< Seed > &seedVec = *_forceBoundary.voronoi().seedVec();

    // initialization
    _polygonComplex.clear();

    // find the sets of the polygons of the same group
    int nV = num_vertices( _skeleton );
    _polygonMat.resize( nV );
    assert( seedVec.size() == num_vertices( _composite ) );
    for( unsigned int i = 0; i < seedVec.size(); i++ ){

        int gid = _composite[ vertex( i, _composite ) ].initID;
        _polygonMat[ gid ].push_back( seedVec[i].cellPolygon );
    }

    for( unsigned int i = 0; i < _polygonMat.size(); i++ ){

        // copy cells to a graph
        ForceGraph complex;
        unsigned int id = 0;
        for( unsigned int j = 0; j < _polygonMat[i].size(); j++ ){

            Polygon2 &p = _polygonMat[i][j];
            vector< ForceGraph::vertex_descriptor > vdVec;

            // add vertices
            for( unsigned int k = 0; k < p.elements().size(); k++ ) {

                ForceGraph::vertex_descriptor vd = NULL;
                //cerr << "p[" << k << "] = " << p.elements()[k];
                bool isFound = findVertexInComplex( p.elements()[k], complex, vd );
                if( isFound == true ){
                    vdVec.push_back( vd );
                }
                else{
                    vd = add_vertex( complex );
                    complex[ vd ].id = id;
                    complex[ vd ].initID = i;
                    complex[ vd ].coordPtr = new Coord2( p.elements()[k].x(), p.elements()[k].y() );
                    complex[ vd ].widthPtr = new double( 0.0 );
                    complex[ vd ].heightPtr = new double( 0.0 );
                    complex[ vd ].areaPtr = new double( 0.0 );
                    vdVec.push_back( vd );
                    id++;
                }
                //cerr << complex[ vd ].id << " ";
            }
            //cerr << endl;

            // add edges
            unsigned int eid = 0;
            for( unsigned int k = 1; k < vdVec.size(); k++ ){

                ForceGraph::vertex_descriptor vdS = vdVec[ k-1 ];
                ForceGraph::vertex_descriptor vdT = vdVec[ k%vdVec.size() ];

                bool isFound = false;
                ForceGraph::edge_descriptor oldED;
                tie( oldED, isFound ) = edge( vdS, vdT, complex );
                if( isFound == true ){

                    complex[ oldED ].weight += 1;
                }
                else{

                    pair<ForceGraph::edge_descriptor, unsigned int> foreE = add_edge( vdS, vdT, complex );
                    ForceGraph::edge_descriptor foreED = foreE.first;
                    complex[ foreED ].id = eid;
                    complex[ foreED ].weight = 0;
                    eid++;
                }
            }
        }

        // remove inner edges
        vector< ForceGraph::edge_descriptor > edVec;
        BGL_FORALL_EDGES( ed, complex, ForceGraph )
        {
            if( complex[ed].weight > 0 ){
                edVec.push_back( ed );
                //cerr << "w = " << complex[ed].weight << endl;
            }
        }
        //cerr << "bnV = " << num_vertices( complex ) << " ";
        //cerr << "bnE = " << num_edges( complex ) << endl;
        for( unsigned int j = 0; j < edVec.size(); j++ ){
            remove_edge( edVec[j], complex );
        }
        // reorder edge id
        unsigned int eid = 0;
        BGL_FORALL_EDGES( ed, complex, ForceGraph )
        {
            complex[ed].id = eid;
            complex[ed].visit = false;
            eid++;
        }

        // find the vertex with degree > 0
        //cerr << "nV = " << num_vertices( complex ) << " ";
        //cerr << "nE = " << num_edges( complex ) << endl;
        ForceGraph::vertex_descriptor vdS;
        BGL_FORALL_VERTICES( vd, complex, ForceGraph )
        {
#ifdef DEBUG
            if ( true ){
                cerr << " degree = " << out_degree( vd, complex );
            }
#endif // DEBUG
            if( out_degree( vd, complex ) > 0 ) {
                vdS = vd;
                break;
            }
        }

        // find the contour
        ForceGraph::out_edge_iterator eo, eo_end;
        ForceGraph::vertex_descriptor vdC = vdS;
        //cerr << "idC = " << complex[vdC].id << " ";
        Polygon2 polygon;
        polygon.elements().push_back( Coord2( complex[vdS].coordPtr->x(), complex[vdS].coordPtr->y() ) );
        while( true ){

            ForceGraph::vertex_descriptor vdT = NULL;
            for( tie( eo, eo_end ) = out_edges( vdC, complex ); eo != eo_end; eo++ ){

                ForceGraph::edge_descriptor ed = *eo;
                if( complex[ed].visit == false ) {
                    vdT = target( ed, complex );
                    complex[ed].visit = true;
                    break;
                }
                //cerr << "(" << complex[source( ed, complex )].id << "," << complex[target( ed, complex )].id << ")" << endl;
            }
            if( vdS == vdT ) break;
            else assert( vdT != NULL );
            //cerr << complex[vdT].id << " ";
            polygon.elements().push_back( Coord2( complex[vdT].coordPtr->x(), complex[vdT].coordPtr->y() ) );
            vdC = vdT;
        }
        // add the first point
        //polygon.elements().push_back( Coord2( complex[vdS].coordPtr->x(), complex[vdS].coordPtr->y() ) );
        polygon.updateCentroid();

        //cerr << "p[" << i << "] = " << polygon << endl;

        _polygonComplex.insert( pair< unsigned int, Polygon2 >( i, polygon ) );
    }

}

//
//  Boundary::updatePolygonComplex --    update the ploygon complex
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Boundary::updatePolygonComplex( void )
{
    cerr << "updating polygonComplex..." << endl;

    map< unsigned int, vector< ForceGraph::vertex_descriptor > >::iterator itP;
    map< unsigned int, Polygon2 >::iterator itC = _polygonComplex.begin();
    for( itP = _polygonComplexVD.begin(); itP != _polygonComplexVD.end(); itP++ ){
        vector< ForceGraph::vertex_descriptor > &p = itP->second;
        for( unsigned int i = 0; i < p.size(); i++ ){
            itC->second.elements()[i].x() = _boundary[ p[i] ].coordPtr->x();
            itC->second.elements()[i].y() = _boundary[ p[i] ].coordPtr->y();
        }
        itC++;
    }
}

//
//  Boundary::writePolygonComplex --    export the ploygon complex
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Boundary::writePolygonComplex( void )
{
    // graphml
    string filename = "../data/boundary.graphml";

    ofstream ofs( filename.c_str() );
    if ( !ofs ) {
        cerr << "Cannot open the target file : " << filename << endl;
        return;
    }

    UndirectedPropertyGraph pg;

#ifdef DEBUG
    cerr << "Boundary::writePolygonComplex: "
         << "nV = " << num_vertices( _boundary )
         << " nE = " << num_edges( _boundary ) << endl;
#endif // DEBUG

    BGL_FORALL_VERTICES( vd, _boundary, BoundaryGraph ) {

        UndirectedPropertyGraph::vertex_descriptor pgVD = add_vertex( pg );
        //put( vertex_index, pg, pgVD, _boundary[ vd ].id ); // vid is automatically assigned
        put( vertex_myx, pg, pgVD, _boundary[ vd ].coordPtr->x() );
        put( vertex_myy, pg, pgVD, _boundary[ vd ].coordPtr->y() );
    }
    BGL_FORALL_EDGES( ed, _boundary, BoundaryGraph ) {

        BoundaryGraph::vertex_descriptor vdS = source( ed, _boundary );
        BoundaryGraph::vertex_descriptor vdT = target( ed, _boundary );
        unsigned int idS = _boundary[ vdS ].id;
        unsigned int idT = _boundary[ vdT ].id;

        UndirectedPropertyGraph::vertex_descriptor pgVDS = vertex( idS, pg );
        UndirectedPropertyGraph::vertex_descriptor pgVDT = vertex( idT, pg );
        pair< UndirectedPropertyGraph::edge_descriptor, unsigned int > foreE = add_edge( pgVDS, pgVDT, pg );
        UndirectedPropertyGraph::edge_descriptor pgED = foreE.first;
        put( edge_index, pg, pgED, _boundary[ ed ].id );
        put( edge_weight, pg, pgED, _boundary[ ed ].weight );
    }

    dynamic_properties dp;
    dp.property( "vid", get( vertex_index_t(), pg ) );
    dp.property( "vx", get( vertex_myx_t(), pg ) );
    dp.property( "vy", get( vertex_myy_t(), pg ) );
    dp.property( "eid", get( edge_index_t(), pg ) );
    dp.property( "weight", get( edge_weight_t(), pg ) );

    write_graphml( ofs, pg, dp, true );

    // polygon contour
    string polygon_filename = "../data/boundary.txt";

    ofstream pfs( polygon_filename.c_str() );
    if ( !pfs ) {
        cerr << "Cannot open the target file : " << polygon_filename << endl;
        return;
    }

    map< unsigned int, vector< ForceGraph::vertex_descriptor > >::iterator itP;
    for( itP = _polygonComplexVD.begin(); itP != _polygonComplexVD.end(); itP++ ){
        vector< ForceGraph::vertex_descriptor > &p = itP->second;
        for( unsigned int i = 0; i < p.size(); i++ ){
            pfs << _boundary[p[i]].id << "\t";
        }
        pfs << endl;
    }
}

//
//  Boundary::readPolygonComplex --    read the ploygon complex
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Boundary::readPolygonComplex( void )
{
    string filename = "../data/boundary.graphml";

    // initialization
    clearGraph( _boundary );

    // load GraphML file
    TiXmlDocument xmlDoc( filename.c_str() );

    xmlDoc.LoadFile();

    if( xmlDoc.ErrorId() > 0 ) {
        cerr << "ErrorId = " << xmlDoc.ErrorDesc() << " : " << filename << endl;
        assert( false );
        return;
    }

    TiXmlElement* root = xmlDoc.RootElement();
    if( !root ) return;

    // reading metabolites
    for( TiXmlElement* rootTag = root->FirstChildElement(); rootTag; rootTag = rootTag->NextSiblingElement() ) {

        string tagname = rootTag->Value();
        if( tagname == "graph" ){

            //cerr << "rootTag = " << rootTag->Value() << endl;
            for( TiXmlElement* graphElement = rootTag->FirstChildElement(); graphElement; graphElement = graphElement->NextSiblingElement() ) {
                string elementname = graphElement->Value();

                if ( elementname == "node" ) {

                    TiXmlElement* node = graphElement->FirstChildElement();
#ifdef DEBUG
                    cerr << "node = " << graphElement->Attribute( "id" ) << ", ";
                    cerr << "( x, y ) = ( " << geometry->Attribute( "X" ) << ", " << geometry->Attribute( "Y" ) << " )" << endl;
#endif // DEBUG
                    string dataName = node->Value();
                    if( dataName == "data" ){

                        // cerr << "value = " << dataName << endl;
                        string keyAttribute = node->Attribute( "key" );

                        // key1:vid
                        string key1Text = node->GetText();

                        // key2:vx
                        node = node->NextSiblingElement();
                        string key2Text = node->GetText();
                        // cerr << "x = " << key2Text << endl;

                        // key3:vy
                        node = node->NextSiblingElement();
                        string key3Text = node->GetText();
                        // cerr << "y = " << key3Text << endl;

                        // cerr <<  node->GetText() << endl;
                        BoundaryGraph::vertex_descriptor vd = add_vertex( _boundary  );
                        double x = _stringToDouble( key2Text );
                        double y = _stringToDouble( key3Text );
                        _boundary[ vd ].geoPtr = new Coord2( x, y );
                        _boundary[ vd ].smoothPtr = new Coord2( x, y );
                        _boundary[ vd ].coordPtr = new Coord2( x, y );
                        _boundary[ vd ].id = _boundary[ vd ].initID = stoi( key1Text );
                        _boundary[ vd ].namePtr = new string( to_string( _boundary[ vd ].id ) );
                        _boundary[ vd ].weight = 1.0;
                        _boundary[ vd ].lineID.push_back( _nLines );
                    }
                }
                else if ( elementname == "edge" ) {

                    TiXmlElement* xmledge = graphElement->FirstChildElement();
                    TiXmlElement* dataElement = xmledge->FirstChildElement();

#ifdef DEBUG
                    cerr << "edge = " << graphElement->Attribute( "id" ) << " "
                         << graphElement->Attribute( "source" ) << " "
                         << graphElement->Attribute( "target" ) << endl;
                    cerr << "tagname = " << tagname << endl;
#endif // DEBUG

                    string nodenameS = graphElement->Attribute( "source" );
                    string nodeIDnameS = nodenameS.substr( 1, nodenameS.length()-1 );
                    string nodenameT = graphElement->Attribute( "target" );
                    string nodeIDnameT = nodenameT.substr( 1, nodenameT.length()-1 );

                    int sourceID = stoi( nodeIDnameS );
                    int targetID = stoi( nodeIDnameT );

                    //update edge information
                    BoundaryGraph::vertex_descriptor vdS = vertex( sourceID, _boundary );
                    BoundaryGraph::vertex_descriptor vdT = vertex( targetID, _boundary );

                    // read edge

                    // key0:eid
                    string key0Text = xmledge->GetText();
                    // cerr << "eid = " << key0Text << endl;

                    // key4:weight
                    xmledge = xmledge->NextSiblingElement();
                    string key4Text = xmledge->GetText();
                    // cerr << "w = " << key4Text << endl;

                    pair< BoundaryGraph::edge_descriptor, unsigned int > foreE = add_edge( vdS, vdT, _boundary );
                    BoundaryGraph::edge_descriptor foreED = foreE.first;

                    // calculate geographical angle
                    Coord2 coordO;
                    Coord2 coordD;
                    if( _boundary[ vdS ].initID < _boundary[ vdT ].initID ){
                        coordO = *_boundary[ vdS ].coordPtr;
                        coordD = *_boundary[ vdT ].coordPtr;
                    }
                    else{
                        coordO = *_boundary[ vdT ].coordPtr;
                        coordD = *_boundary[ vdS ].coordPtr;
                    }
                    double diffX = coordD.x() - coordO.x();
                    double diffY = coordD.y() - coordO.y();
                    double angle = atan2( diffY, diffX );

                    _boundary[ foreED ].initID = _boundary[ foreED ].id = stoi( key0Text );
                    _boundary[ foreED ].weight = _stringToDouble( key4Text );
                    _boundary[ foreED ].geoAngle = angle;
                    _boundary[ foreED ].smoothAngle = angle;
                    _boundary[ foreED ].angle = angle;
                    _boundary[ foreED ].lineID.push_back( _nLines );
                }
            }
        }
    }
    // printGraph( _boundary );

    // read polygon contour
    _polygonComplexVD.clear();
    _polygonComplex.clear();
    string polygon_filename = "../data/boundary.txt";

    ifstream pfs( polygon_filename.c_str() );
    if ( !pfs ) {
        cerr << "Cannot open the target file : " << polygon_filename << endl;
        return;
    }

    unsigned int id = 0;
    while( !pfs.eof() ){

        string polyVDVec;
        getline( pfs, polyVDVec );

        string buf; // have a buffer string
        stringstream ss( polyVDVec ); // insert the string into a stream

        // a vector to hold our words
        vector< string > tokens;
        while (ss >> buf) tokens.push_back( buf );
        vector< ForceGraph::vertex_descriptor > vdVec;
        Polygon2 polygon;
        for( unsigned int i = 0; i < tokens.size(); i++ ){

            // cerr << tokens[i] << " ";
            BoundaryGraph::vertex_descriptor vd = vertex( stoi( tokens[i] ), _boundary );
            vdVec.push_back( vd );
            polygon.elements().push_back( *_boundary[ vd ].coordPtr );
            // cerr << _boundary[ vd ].id << " ";
        }
        // cerr << endl;
        _polygonComplex.insert( pair< unsigned int, Polygon2 > ( id, polygon ) );
        _polygonComplexVD.insert( pair< unsigned int, vector< BoundaryGraph::vertex_descriptor > >( id, vdVec ) );
        id++;
        //_metaFreq.insert( pair< string, unsigned int >( tokens[0], stoi( tokens[1] ) ) );
        //cerr << tokens[0] << " = " << tokens[1] << endl;
    }
    _polygonComplex.erase( std::prev( _polygonComplex.end() ) );
    _polygonComplexVD.erase( std::prev( _polygonComplexVD.end() ) );

    _nVertices = num_vertices( _boundary );
    _nEdges = num_edges( _boundary );
    // _nLines = _polygonComplex.size();

    // create lines
    // _line.resize( _polygonComplexVD.size() );
    map< unsigned int, vector< BoundaryGraph::vertex_descriptor > >::iterator itS;
    // map< unsigned int, vector< BoundaryGraph::vertex_descriptor > >::itereator itT;
    for( itS = _polygonComplexVD.begin(); itS != _polygonComplexVD.end(); itS++ ){
        vector< BoundaryGraph::vertex_descriptor > &vdVec = itS->second;
        _lineSta.push_back( vdVec );
    }
    _nLines = _polygonComplexVD.size();

#ifdef DEBUG
    cerr << "Boundary::readPolygonComplex:"
         << "nV = " << _nVertices << " nE = " << _nEdges
         << " nL = " << _nLines << endl;

    map< unsigned int, vector< ForceGraph::vertex_descriptor > >::iterator itP;
    for( itP = _polygonComplexVD.begin(); itP != _polygonComplexVD.end(); itP++ ){
        vector< ForceGraph::vertex_descriptor > &p = itP->second;
        cerr << "p[" << itP->first << "] = ";
        for( unsigned int i = 0; i < p.size(); i++ ){
            cerr << _boundary[p[i]].id << " ";
        }
        cerr << endl;
    }
#endif // DEBUG
}

// end of header file
// Do not add any stuff under this line.
