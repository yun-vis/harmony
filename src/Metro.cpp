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

#include "Metro.h"

//------------------------------------------------------------------------------
//	Protected functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------

//
//  Metro::Metro -- default constructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
Metro::Metro( void )
{
    clearGraph( graph );

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
//  Metro::Metro -- copy constructor
//
//  Inputs
//  obj : object of this class
//
//  Outputs
//  none
//
Metro::Metro( const Metro & obj )
{
    graph = obj.g();

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
//  Metro::~Metro --    destructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
Metro::~Metro( void )
{
    clearGraph( graph );

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
//  Metro::clear --    clear the current metro information
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Metro::clear( void )
{
    clearGraph( graph );

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
//  Metro::~checkVEConflicrs --    detect vertex-edge pair that is close to each other
//
//  Inputs
//  none
//
//  Outputs
//  none
//
bool Metro::checkVEConflicts( void )
{
    VertexIDMap         vertexID        = get( vertex_myid, graph );
    VertexSmoothMap     vertexSmooth    = get( vertex_mysmooth, graph );
    VertexIsStationMap  vertexIsStation = get( vertex_myisstation, graph );
    //VertexExternalMap   vertexExternal  = get( vertex_myexternal, graph );
    EdgeIDMap           edgeID          = get( edge_myid, graph );

    bool                doConflict      = false;

    _VEconflict.clear();

    // find the vertex-edge pair that has smaller distance than the threshold
    BGL_FORALL_VERTICES( vertex, graph, Graph )
    {
        double              distThreshold   = _distBeta/2.0;
        //if( vertexIsStation[ vertex ] == false )
        //    distThreshold = vertexExternal[ vertex ].leaderWeight() * _distBeta/2.0;
        //cerr << "vID = " << vertexID[ vertex ] << ", distThreshold = " << distThreshold << endl;
        //cerr << "_dBeta = " << _distBeta << ", distThreshold = " << distThreshold << endl;
        //double              distThreshold   = MIN_DISTANCE;
        //double              distThreshold   = 1.0 * cos( M_PI/4.0 );

        Coord2 coord = vertexSmooth[ vertex ];
        unsigned idV = vertexID[ vertex ];

        BGL_FORALL_EDGES( edge, graph, Graph )
        {
            unsigned idE = edgeID[ edge ];
            VertexDescriptor vdS = source( edge, graph );
            VertexDescriptor vdT = target( edge, graph );

            if ( ( vertex != vdS ) && ( vertex != vdT ) ) {

                Coord2 coordS = vertexSmooth[ vdS ];
                Coord2 coordT = vertexSmooth[ vdT ];

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
        VertexDescriptor vdV = it->second.first;
        EdgeDescriptor ed = it->second.second;
        VertexDescriptor vdS = source( ed, graph );
        VertexDescriptor vdT = target( ed, graph );
        Coord2 & pointV = vertexSmooth[ vdV ];
        Coord2 & pointS = vertexSmooth[ vdS ];
        Coord2 & pointT = vertexSmooth[ vdT ];

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
//  Metro::adjustsize --  adjust size of the layout of the metro network
//
//  Inputs
//      width: window width
//      height: window height
//
//  Outputs
//      none
//
void Metro::adjustsize( const int & width, const int & height )
{
    VertexIDMap         vertexID        = get( vertex_myid, graph );
    VertexGeoMap        vertexGeo       = get( vertex_mygeo, graph );
    VertexSmoothMap     vertexSmooth    = get( vertex_mysmooth, graph );
    VertexCoordMap      vertexCoord     = get( vertex_mycoord, graph );
    //VertexExternalMap   vertexExternal  = get( vertex_myexternal, graph );
    VertexExtstateMap   vertexExtstate  = get( vertex_myextstate, graph );
    EdgeIDMap           edgeID          = get( edge_myid, graph );
    EdgeWeightMap       edgeWeight      = get( edge_weight, graph );
    EdgeIsLineMap       edgeIsLine      = get( edge_myisline, graph );

    // for vertexHome
    double xMin =  INFINITY;
    double xMax = -INFINITY;
    double yMin =  INFINITY;
    double yMax = -INFINITY;
    double aspect = ( double )width/( double )height;

    // Scan all the vertex coordinates first
    BGL_FORALL_VERTICES( vertex, graph, Graph )
    {
        Coord2 coord = vertexGeo[ vertex ];
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
    BGL_FORALL_VERTICES( vertex, graph, Graph )
    {
        Coord2 geo = vertexGeo[ vertex ];
        Coord2 smooth = vertexSmooth[ vertex ];
        Coord2 coord = vertexCoord[ vertex ];
        //Coord2 label = vertexExternal[ vertex ].curSite();

        geo.setX( width  * ( geo.x() - xMid ) / xRange );
        geo.setY( height * ( geo.y() - yMid ) / yRange );
        smooth.setX( width  * ( smooth.x() - xMid ) / xRange );
        smooth.setY( height * ( smooth.y() - yMid ) / yRange );
        coord.setX( width  * ( coord.x() - xMid ) / xRange );
        coord.setY( height * ( coord.y() - yMid ) / yRange );
        //label.setX( width  * ( label.x() - xMid ) / xRange );
        //label.setY( height * ( label.y() - yMid ) / yRange );

        vertexGeo[ vertex ].x() = geo.x();
        vertexGeo[ vertex ].y() = geo.y();
        vertexSmooth[ vertex ].x() = smooth.x();
        vertexSmooth[ vertex ].y() = smooth.y();
        vertexCoord[ vertex ].x() = coord.x();
        vertexCoord[ vertex ].y() = coord.y();
/*
        vertexExternal[ vertex ].geoSite().x() = label.x();
        vertexExternal[ vertex ].curSite().x() = label.x();
        vertexExternal[ vertex ].geoSite().y() = label.y();
        vertexExternal[ vertex ].curSite().y() = label.y();
*/
    }

   // compute the unit length of an edge (ratio)
    int nAlpha = 0;
    int nBeta = 0;
    double totallength = 0.0;
    BGL_FORALL_EDGES( edge, graph, Graph )
    {
        VertexDescriptor vdS = source( edge, graph );
        VertexDescriptor vdT = target( edge, graph );
        unsigned int idS = vertexID[ vdS ];
        unsigned int idT = vertexID[ vdT ];

        Coord2 coord = vertexGeo[ vdT ] - vertexGeo[ vdS ];
        totallength += coord.norm();
        double w = edgeWeight[ edge ];
        if( w == 1.0 ) nBeta++;
        else nAlpha++;
    }
    double magLength = 2.0;
    _distBeta = totallength / ( magLength * nAlpha + nBeta );
    _distAlpha = magLength * _distBeta;

    // cerr << "distBeta = " << _distBeta << endl;
}

//
//  Metro::simplifyLayout --    delete nodes with small distance to the edges bounded by their adjacent nodes
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Metro::simplifyLayout( void )
{
    VertexIndexMap      vertexIndex     = get( vertex_index, graph );
    VertexIDMap         vertexID        = get( vertex_myid, graph );
    VertexCoordMap      vertexCoord     = get( vertex_mycoord, graph );
    VertexIsStationMap  vertexIsStation = get( vertex_myisstation, graph );
    EdgeIndexMap        edgeIndex       = get( edge_index, graph );
    EdgeIDMap           edgeID          = get( edge_myid, graph );
    EdgeWeightMap       edgeWeight      = get( edge_weight, graph );
    EdgeGeoAngleMap     edgeGeoAngle    = get( edge_mygeoangle, graph );
    EdgeSmoAngleMap     edgeSmoAngle    = get( edge_mysmoangle, graph );
    EdgeCurAngleMap     edgeCurAngle    = get( edge_mycurangle, graph );
    EdgeLineIDMap       edgeLineID      = get( edge_mylineid, graph );
    EdgeIsLineMap       edgeIsLine      = get( edge_myisline, graph );

    //double threshold = 2.5;
    double threshold = 10.0;
    int count = 0;
    _removedVertices.clear();
    _removedEdges.clear();
    _removedWeights.clear();
    //printGraph( graph );

    while( true ){
        // find minimal distance of middle node to the line composed by its neighbors
        VertexDescriptor mvd, mvdP, mvdN;
        bool noDegreeTwo = true;
        double minDist = INFINITY;
        BGL_FORALL_VERTICES( vertex, graph, Graph )
        {
            DegreeSizeType degrees = out_degree( vertex, graph );

            if( degrees == 2 && ( vertexIsStation[ vertex ] == true ) ){

                noDegreeTwo = false;
                OutEdgeIterator e, e_end;
                tie( e, e_end ) = out_edges( vertex, graph );
                EdgeDescriptor edP = *e;
                VertexDescriptor vTP = target( edP, graph );
                e++;
                EdgeDescriptor edN = *e;
                VertexDescriptor vTN = target( edN, graph );

                Coord2 p = vertexCoord[ vTP ];
                Coord2 c = vertexCoord[ vertex ];
                Coord2 n = vertexCoord[ vTN ];
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
        // cerr << "count = " << count << ", minID = " << vertexIndex[ mvd ] << ", minDist = " << minDist << endl;
        // if( minDist > threshold || noDegreeTwo == true || vertexIsStation[ mvd ] == false ) break;
        if( minDist > threshold || noDegreeTwo == true ) break;

        // delete vertex and corresponding neighbor edges
        EdgeDescriptor      edP, edN;
        bool                found = false;
        tie( edP, found ) = edge( mvd, mvdP, graph );
        assert( found == true );
        tie( edN, found ) = edge( mvd, mvdN, graph );
        assert( found == true );
        double weight = edgeWeight[ edP ] + edgeWeight[ edN ];

        // remove stations and edges
        remove_edge( edP, graph );
        _removedEdges.push_back( VVIDPair( vertexIndex[ mvd ], vertexIndex[ mvdP ] ) );
        _removedWeights.push_back( edgeWeight[ edP ] );
        remove_edge( edN, graph );
        _removedEdges.push_back( VVIDPair( vertexIndex[ mvd ], vertexIndex[ mvdN ] ) );
        _removedWeights.push_back( edgeWeight[ edN ] );
        clear_vertex( mvd, graph );
        remove_vertex( mvd, graph );
        _removedVertices.push_back( vertexIndex[ mvd ] );

        // cerr << "mv = " << vertexID[ mvd ] << " mvdP = " << vertexID[ mvdP ] << " mvdN = " << vertexID[ mvdN ] << endl;
        // cerr << "wS = " << edgeWeight[ edP ] << " wT = " << edgeWeight[ edN ] << endl;

        // add new edges
        pair< EdgeDescriptor, unsigned int > addE = add_edge( mvdP, mvdN, 1, graph );
        EdgeDescriptor addED = addE.first;

        // calculate geographical angle
        Coord2 coordO;
        Coord2 coordD;
        if( vertexIndex[ mvdP ] < vertexIndex[ mvdN ] ){
            coordO = vertexCoord[ mvdP ];
            coordD = vertexCoord[ mvdN ];
        }
        else{
            coordO = vertexCoord[ mvdN ];
            coordD = vertexCoord[ mvdP ];
        }
        double diffX = coordD.x() - coordO.x();
        double diffY = coordD.y() - coordO.y();
        double angle = atan2( diffY, diffX );

        edgeWeight[ addED ] = weight;
        edgeGeoAngle[ addED ] = angle;
        edgeSmoAngle[ addED ] = angle;
        edgeCurAngle[ addED ] = angle;
        edgeIsLine[ addED ] = true;
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
//  Metro::reorderID --    reorder vertex and edge ID
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Metro::reorderID( void )
{
    VertexIDMap         vertexID        = get( vertex_myid, graph );
    EdgeIDMap           edgeID          = get( edge_myid, graph );

    // reorder vertexID
    _nStations = 0;
    BGL_FORALL_VERTICES( vertex, graph, Graph )
    {
        vertexID[ vertex ]      = _nStations;
        // cerr << "VID = " << vertexIndex[ vertex ] << endl;
        _nStations++;
    }

    _nEdges = 0;
    BGL_FORALL_EDGES( edge, graph, Graph )
    {
        edgeID[ edge ]      = _nEdges;
        // cerr << "EID = " << edgeIndex[ edge ] << endl;
        _nEdges++;
    }
}

//
//  Metro::movebackSmooth --    move the deleted nodes back to the layout
//
//  Inputs
//  obj: the original layout
//
//  Outputs
//  none
//
bool Metro::movebackSmooth( const Metro & obj )
{
    if( ( _removedVertices.size() == 0 ) && ( _removedEdges.size() == 0 ) )
        return true;

    Graph oGraph = obj.g();

    // simplified graph
    VertexIndexMap      vertexIndex     = get( vertex_index, graph );
    VertexIDMap         vertexID        = get( vertex_myid, graph );
    VertexHomeMap       vertexHome      = get( vertex_myhome, graph );
    VertexGeoMap        vertexGeo       = get( vertex_mygeo, graph );
    VertexSmoothMap     vertexSmooth    = get( vertex_mysmooth, graph );
    VertexCoordMap      vertexCoord     = get( vertex_mycoord, graph );
    VertexIsStationMap  vertexIsStation = get( vertex_myisstation, graph );
    EdgeIDMap           edgeID          = get( edge_myid, graph );
    EdgeWeightMap       edgeWeight      = get( edge_weight, graph );
    EdgeGeoAngleMap     edgeGeoAngle    = get( edge_mygeoangle, graph );
    EdgeSmoAngleMap     edgeSmoAngle    = get( edge_mysmoangle, graph );
    EdgeCurAngleMap     edgeCurAngle    = get( edge_mycurangle, graph );
    EdgeIsLineMap       edgeIsLine      = get( edge_myisline, graph );

    // original graph
    VertexHomeMap       oVertexHome     = get( vertex_myhome, oGraph );
    VertexGeoMap        oVertexGeo      = get( vertex_mygeo, oGraph );

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


    // find VertexDescriptor in simplified graph
    VertexDescriptor vdS, vdT;
    BGL_FORALL_VERTICES( vd, graph, Graph ){
        // cerr << "vertexIndex[ vd ] = " << vertexIndex[ vd ] << endl;
        if ( vertexIndex[ vd ] == idS ) vdS = vd;
        if ( vertexIndex[ vd ] == idT ) vdT = vd;
    }

    bool found = false;
    EdgeDescriptor ed;
    tie( ed, found ) = edge( vdS, vdT, graph );
    // remove edge
    remove_edge( ed, graph );
    // add vertex
    VertexDescriptor vdNew = add_vertex( graph );
    vertexIndex[ vdNew ] = idV;
    VertexDescriptor vdO = vertex( idV, oGraph );
    Coord2 coord = ( wT * vertexSmooth[ vdS ] + wS * vertexSmooth[ vdT ] ) / ( wS + wT );
    vertexHome[ vdNew ] = oVertexHome[ vdO ];
    vertexGeo[ vdNew ] = oVertexGeo[ vdO ];
    vertexSmooth[ vdNew ] = coord;
    vertexCoord[ vdNew ] = coord;
    vertexIsStation[ vdNew ] = true;
    // add edge
    ed = add_edge( vdNew, vdS, 1, graph ).first;
    edgeWeight[ ed ] = wS;
    edgeIsLine[ ed ] = true;
    // add  edge
    ed = add_edge( vdNew, vdT, 1, graph ).first;
    edgeWeight[ ed ] = wT;
    edgeIsLine[ ed ] = true;

    // reorder vertexID
    reorderID();

    // update edge angle of simplified metro layout
    BGL_FORALL_EDGES( edge, graph, Graph ){

        VertexDescriptor vdS = source( edge, graph );
        VertexDescriptor vdT = target( edge, graph );

        // calculate geographical angle
        Coord2 coordO;
        Coord2 coordD;
        if( vertexID[ vdS ] < vertexID[ vdT ] ){
            coordO = vertexCoord[ vdS ];
            coordD = vertexGeo[ vdT ];
        }
        else{
            coordO = vertexGeo[ vdT ];
            coordD = vertexGeo[ vdS ];
        }
        double diffX = coordD.x() - coordO.x();
        double diffY = coordD.y() - coordO.y();
        double angle = atan2( diffY, diffX );

        edgeGeoAngle[ edge ] = angle;

        if( vertexID[ vdS ] < vertexID[ vdT ] ){
            coordO = vertexCoord[ vdS ];
            coordD = vertexCoord[ vdT ];
        }
        else{
            coordO = vertexCoord[ vdT ];
            coordD = vertexCoord[ vdS ];
        }
        diffX = coordD.x() - coordO.x();
        diffY = coordD.y() - coordO.y();
        angle = atan2( diffY, diffX );

        edgeSmoAngle[ edge ] = angle;
        edgeCurAngle[ edge ] = angle;

    }

    return false;
}

//
//  Metro::movebackOctilinear --    move the deleted nodes back to the layout
//
//  Inputs
//  obj: the original layout
//
//  Outputs
//  none
//
bool Metro::movebackOctilinear( const Metro & obj )
{
    if( ( _removedVertices.size() == 0 ) && ( _removedEdges.size() == 0 ) )
        return true;

    Graph oGraph = obj.g();

    // simplified graph
    VertexIndexMap      vertexIndex     = get( vertex_index, graph );
    VertexIDMap         vertexID        = get( vertex_myid, graph );
    VertexHomeMap       vertexHome      = get( vertex_myhome, graph );
    VertexGeoMap        vertexGeo       = get( vertex_mygeo, graph );
    VertexSmoothMap     vertexSmooth    = get( vertex_mysmooth, graph );
    VertexCoordMap      vertexCoord     = get( vertex_mycoord, graph );
    VertexIsStationMap  vertexIsStation = get( vertex_myisstation, graph );
    EdgeIDMap           edgeID          = get( edge_myid, graph );
    EdgeWeightMap       edgeWeight      = get( edge_weight, graph );
    EdgeSmoAngleMap     edgeSmoAngle    = get( edge_mysmoangle, graph );
    EdgeCurAngleMap     edgeCurAngle    = get( edge_mycurangle, graph );
    EdgeIsLineMap       edgeIsLine      = get( edge_myisline, graph );

    // original graph
    VertexHomeMap       oVertexHome     = get( vertex_myhome, oGraph );
    VertexGeoMap        oVertexGeo      = get( vertex_mygeo, oGraph );
    VertexSmoothMap     oVertexSmooth   = get( vertex_mysmooth, oGraph );

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


    // find VertexDescriptor in simplified graph
    VertexDescriptor vdS, vdT;
    BGL_FORALL_VERTICES( vd, graph, Graph ){
        // cerr << "vertexIndex[ vd ] = " << vertexIndex[ vd ] << endl;
        if ( vertexIndex[ vd ] == idS ) vdS = vd;
        if ( vertexIndex[ vd ] == idT ) vdT = vd;
    }

    bool found = false;
    EdgeDescriptor ed;
    tie( ed, found ) = edge( vdS, vdT, graph );
    // remove edge
    remove_edge( ed, graph );
    // add vertex
    VertexDescriptor vdNew = add_vertex( graph );
    vertexIndex[ vdNew ] = idV;
    VertexDescriptor vdO = vertex( idV, oGraph );
    Coord2 coord = ( wT * vertexCoord[ vdS ] + wS * vertexCoord[ vdT ] ) / ( wS + wT );
    vertexHome[ vdNew ] = oVertexHome[ vdO ];
    vertexGeo[ vdNew ] = oVertexGeo[ vdO ];
    vertexSmooth[ vdNew ] = oVertexSmooth[ vdO ];
    vertexCoord[ vdNew ] = coord;
    vertexIsStation[ vdNew ] = true;
    // add edge
    ed = add_edge( vdNew, vdS, 1, graph ).first;
    edgeWeight[ ed ] = wS;
    edgeIsLine[ ed ] = true;
    // add  edge
    ed = add_edge( vdNew, vdT, 1, graph ).first;
    edgeWeight[ ed ] = wT;
    edgeIsLine[ ed ] = true;

    // reorder vertexID
    reorderID();

    // update edge angle of simplified metro layout
    BGL_FORALL_EDGES( edge, graph, Graph ){

        VertexDescriptor vdS = source( edge, graph );
        VertexDescriptor vdT = target( edge, graph );

        // calculate geographical angle
        Coord2 coordO;
        Coord2 coordD;
        if( vertexID[ vdS ] < vertexID[ vdT ] ){
            coordO = vertexCoord[ vdS ];
            coordD = vertexCoord[ vdT ];
        }
        else{
            coordO = vertexCoord[ vdT ];
            coordD = vertexCoord[ vdS ];
        }
        double diffX = coordD.x() - coordO.x();
        double diffY = coordD.y() - coordO.y();
        double angle = atan2( diffY, diffX );

        //edgeSmoAngle[ edge ] = angle;
        edgeCurAngle[ edge ] = angle;

    }

    return false;
}

//
//  Metro::cloneLayout --    copy all information of the input metro
//
//  Inputs
//  obj: original metro layout
//
//  Outputs
//  none
//
void Metro::cloneLayout( const Metro & obj )
{
    graph.clear();
    graph = obj.g();

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
        vector< EdgeDescriptor > line;
        for ( unsigned i = 0; i < obj.line().size(); ++i ){
            line.push_back( obj.line()[ k ][ i ] );
        }
        _line.push_back( line );

        // copy line station info
        vector< VertexDescriptor > lineSta;
        for ( unsigned i = 0; i < obj.lineSta().size(); ++i ){
            lineSta.push_back( obj.lineSta()[ k ][ i ] );
        }
        _lineSta.push_back( lineSta );
    }
}

//
//  Metro::cloneSmooth --    copy all smooth vertex position to the input metro
//
//  Inputs
//  obj: original metro layout
//
//  Outputs
//  none
//
void Metro::cloneSmooth( const Metro & obj )
{
    Graph               sGraph          = obj.g();
    VertexIndexMap      sVertexIndex    = get( vertex_index, sGraph );
    VertexSmoothMap     sVertexSmooth   = get( vertex_mysmooth, sGraph );
    VertexCoordMap      sVertexCoord    = get( vertex_mycoord, sGraph );
    VertexIsStationMap  sVertexIsStation= get( vertex_myisstation, sGraph );

    VertexIndexMap      vertexIndex     = get( vertex_index, graph );
    VertexSmoothMap     vertexSmooth    = get( vertex_mysmooth, graph );
    VertexCoordMap      vertexCoord     = get( vertex_mycoord, graph );
    VertexTempMap       vertexTemp      = get( vertex_mytemp, graph );
    VertexExtstateMap   vertexExtstate  = get( vertex_myextstate, graph );
    VertexSelectMagMap  vertexSelectMag = get( vertex_myselectmag, graph );
    VertexIsStationMap  vertexIsStation = get( vertex_myisstation, graph );

    // copy stations
    BGL_FORALL_VERTICES( vdS, sGraph, Graph ){
        BGL_FORALL_VERTICES( vd, graph, Graph ){
            if ( vertexIndex[ vd ] == sVertexIndex[ vdS ] ) {
                vertexSmooth[ vd ] = sVertexSmooth[ vdS ];
                //vertexTemp[ vd ] = vertexCoord[ vdS ];
                vertexCoord[ vd ] = sVertexCoord[ vdS ];
            }
        }
    }

    // copy labels
    vector< VertexDescriptor > vdSVec;
    BGL_FORALL_VERTICES( vdS, sGraph, Graph ){

        if( sVertexIsStation[ vdS ] == false && vdSVec.size() < obj.nLabels() ) {
            vdSVec.push_back( vdS );
        }
    }
    // cerr << "id = " << vdSVec.size() << " nLabels = " << obj.nLabels() << endl;
    int count = 0;
    BGL_FORALL_VERTICES( vd, graph, Graph ){

        if( vertexSelectMag[ vd ] == true && vertexExtstate[ vd ] == true && count < vdSVec.size() ) {

            VertexDescriptor vdL = vdSVec[ count ];
            Coord2 coord = sVertexCoord[ vdL ];
#ifdef  SKIP
            // compute min radius
            double minR = INFINITY;
            OutEdgeIterator e, e_end;
            for ( tie( e, e_end ) = out_edges( vdL, sGraph ); e != e_end; ++e ) {
                EdgeDescriptor ed = *e;
                VertexDescriptor vS = source( ed, sGraph );
                VertexDescriptor vT = target( ed, sGraph );
                double radius = ( sVertexCoord[ vS ] - sVertexCoord[ vT ] ).norm();
                if( vertexIsStation[ vS ] == false && vertexIsStation[ vT ] == false )
                    radius /= 2.0;
                if( radius < minR ) minR = radius;
            }
#endif  // SKIP

            count = count + 1;
        }
    }
    //printGraph( graph );
}

//
//  Metro::cloneOctilinear --    copy all octilinear vertex position to the input metro
//
//  Inputs
//  obj: original metro layout
//
//  Outputs
//  none
//
void Metro::cloneOctilinear( const Metro & obj )
{
    Graph               sGraph          = obj.g();
    VertexIndexMap      sVertexIndex    = get( vertex_index, sGraph );
    VertexCoordMap      sVertexCoord    = get( vertex_mycoord, sGraph );
    VertexIsStationMap  sVertexIsStation= get( vertex_myisstation, sGraph );

    VertexIndexMap      vertexIndex     = get( vertex_index, graph );
    VertexCoordMap      vertexCoord     = get( vertex_mycoord, graph );
    //VertexExternalMap   vertexExternal  = get( vertex_myexternal, graph );
    VertexExtstateMap   vertexExtstate  = get( vertex_myextstate, graph );
    VertexSelectMagMap  vertexSelectMag = get( vertex_myselectmag, graph );
    VertexIsStationMap  vertexIsStation = get( vertex_myisstation, graph );

    // copy stations
    BGL_FORALL_VERTICES( vdS, sGraph, Graph ){
        BGL_FORALL_VERTICES( vd, graph, Graph ){
            if ( vertexIndex[ vd ] == sVertexIndex[ vdS ] ) {
                vertexCoord[ vd ] = sVertexCoord[ vdS ];
            }
        }
    }

    // copy labels
    vector< VertexDescriptor > vdSVec;
    BGL_FORALL_VERTICES( vdS, sGraph, Graph ){

        if( sVertexIsStation[ vdS ] == false && vdSVec.size() < obj.nLabels() ) {
            vdSVec.push_back( vdS );
        }
    }
    //cerr << "id = " << vdSVec.size() << " nLabels = " << obj.nLabels() << endl;
    int count = 0;
    BGL_FORALL_VERTICES( vd, graph, Graph ){

        if( vertexSelectMag[ vd ] == true && vertexExtstate[ vd ] == true && count < vdSVec.size() ) {

            VertexDescriptor vdL = vdSVec[ count ];
            Coord2 coord = sVertexCoord[ vdL ];
#ifdef  SKIP
            // compute min radius
            double minR = INFINITY;
            OutEdgeIterator e, e_end;
            for ( tie( e, e_end ) = out_edges( vdL, sGraph ); e != e_end; ++e ) {
                EdgeDescriptor ed = *e;
                VertexDescriptor vS = source( ed, sGraph );
                VertexDescriptor vT = target( ed, sGraph );
                double radius = ( sVertexCoord[ vS ] - sVertexCoord[ vT ] ).norm();
                if( vertexIsStation[ vS ] == false && vertexIsStation[ vT ] == false )
                    radius /= 2.0;
                if( radius < minR ) minR = radius;
            }

            // compute min distance to metro lines
            BGL_FORALL_EDGES( ed, graph, Graph ){

                VertexDescriptor vdS = source( ed, graph );
                VertexDescriptor vdT = target( ed, graph );
                Coord2 & pointV = coord;
                Coord2 & pointS = vertexCoord[ vdS ];
                Coord2 & pointT = vertexCoord[ vdT ];

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
                if( r > 0 && ( 1-r ) > 0 ) {
                    //cerr << "r = " << r << ", " << vertexIndex[ vdS ] << ", " << vertexIndex[ vdT ] << endl;
                    if( minR > dist ) minR = dist;
                }
            }
#endif  // SKIP
            count = count + 1;
        }
    }
}

void Metro::updateTempCoord( void )
{
    VertexIDMap             vertexID            = get( vertex_myid, graph );
    VertexCoordMap          vertexCoord         = get( vertex_mycoord, graph );
    VertexTempMap           vertexTemp          = get( vertex_mytemp, graph );

    BGL_FORALL_VERTICES( vd, graph, Graph ) {
        vertexTemp[ vd ] = vertexCoord[ vd ];
    }
}

//
//  Metro::load --    load the metro information
//
//  Inputs
//  filename: file name of the network
//
//  Outputs
//  none
//
void Metro::load( const char * filename )
{
    clear();

    VertexIndexMap          vertexIndex         = get( vertex_index, graph );
    VertexIDMap             vertexID            = get( vertex_myid, graph );
    VertexCoordMap          vertexCoord         = get( vertex_mycoord, graph );
    VertexSmoothMap         vertexSmooth        = get( vertex_mysmooth, graph );
    VertexGeoMap            vertexGeo           = get( vertex_mygeo, graph );
    VertexTempMap           vertexTemp          = get( vertex_mytemp, graph );
    VertexHomeMap           vertexHome          = get( vertex_myhome, graph );
    VertexLineIDMap         vertexLineID        = get( vertex_mylineid, graph );
    VertexNameMap           vertexName          = get( vertex_myname, graph );
    VertexScaleMap          vertexScale         = get( vertex_myscale, graph );
    VertexSizeMap           vertexSize          = get( vertex_mysize, graph );
    VertexSelectMagMap      vertexSelectMag     = get( vertex_myselectmag, graph );
    VertexGeodesicMap       vertexGeodesic      = get( vertex_mygeodesic, graph );
    VertexZoneMap           vertexZone          = get( vertex_myzone, graph );
    VertexWeightMap         vertexWeight        = get( vertex_weight, graph );
    VertexIsStationMap      vertexIsStation     = get( vertex_myisstation, graph );
    EdgeIndexMap            edgeIndex           = get( edge_index, graph );
    EdgeIDMap               edgeID              = get( edge_myid, graph );
    EdgeWeightMap           edgeWeight          = get( edge_weight, graph );
    EdgeGeoAngleMap         edgeGeoAngle        = get( edge_mygeoangle, graph );
    EdgeSmoAngleMap         edgeSmoAngle        = get( edge_mysmoangle, graph );
    EdgeCurAngleMap         edgeCurAngle        = get( edge_mycurangle, graph );
    EdgeLineIDMap           edgeLineID          = get( edge_mylineid, graph );
    EdgeSelectShiftMap      edgeSelectShift     = get( edge_myselectshift, graph );
    EdgeSelectCtrlMap       edgeSelectCtrl      = get( edge_myselectctrl, graph );
    EdgeIsLineMap           edgeIsLine          = get( edge_myisline, graph );
    EdgeIsLeaderMap         edgeIsLeader        = get( edge_myisleader, graph );

    ifstream    ifs( filename );
    char        buf[ MAX_STR ];

    if ( !ifs ) {
        cerr << "Cannot open the target file : " << filename << endl;
        return;
    }
    cerr << " now reading the metro network and stations... " << endl;

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
        vector< VertexDescriptor > ptrSta;
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
            VertexDescriptor curVD = NULL;
            BGL_FORALL_VERTICES( vertex, graph, Graph )
            {
                string name = vertexName[ vertex ];
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

                curVD = add_vertex( graph );
                vector< unsigned int > lineID = vertexLineID[ curVD ];
                lineID.push_back( _nLines );

                vertexCoord[ curVD ].x()        = x;
                vertexCoord[ curVD ].y()        = y;
                vertexSmooth[ curVD ].x()       = x;
                vertexSmooth[ curVD ].y()       = y;
                vertexGeo[ curVD ].x()          = x;
                vertexGeo[ curVD ].y()          = y;
                vertexTemp[ curVD ].x()         = x;
                vertexTemp[ curVD ].y()         = y;
                vertexHome[ curVD ].x()         = x;
                vertexHome[ curVD ].y()         = y;
                vertexIndex[ curVD ]            = _nStations;
                vertexID[ curVD ]               = _nStations;
                vertexName[ curVD ].assign( argument );
                setVertexTexName( graph, curVD, argument );
                vertexScale[ curVD ]            = 1.0;
                vertexSelectMag[ curVD ]        = false;
                vertexGeodesic[ curVD ]         = 0.0;
                vertexZone[ curVD ]             = 0.0;
                vertexWeight[ curVD ]           = 1.0;
                vertexIsStation[ curVD ]        = true;

                vertexLineID[ curVD ].push_back( _nLines );
                _nStations++;

            }
            else{
                vertexLineID[ curVD ].push_back( _nLines );
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
        vector< EdgeDescriptor > eachline;
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
            EdgeDescriptor oldED;
            VertexDescriptor oldVS = ptrSta[ origID ];
            VertexDescriptor oldVT = ptrSta[ destID ];
            unsigned int indexS = vertexIndex[ oldVS ];
            unsigned int indexT = vertexIndex[ oldVT ];

            tie( oldED, found ) = edge( oldVS, oldVT, graph );

            if ( found == true ) {

                edgeLineID[ oldED ].push_back( _nLines );
                eachline.push_back( oldED );
                bool test = false;
                tie( oldED, test ) = edge( oldVT, oldVS, graph );
            }
            else{
                VertexDescriptor src = vertex( indexS, graph );
                VertexDescriptor tar = vertex( indexT, graph );

                // handle fore edge
                pair<EdgeDescriptor, unsigned int> foreE = add_edge( src, tar,
                                                                     weight, graph );
                EdgeDescriptor foreED = foreE.first;

                // calculate geographical angle
                Coord2 coordO;
                Coord2 coordD;
                if( vertexIndex[ src ] < vertexIndex[ tar ] ){
                    coordO = vertexCoord[ src ];
                    coordD = vertexCoord[ tar ];
                }
                else{
                    coordO = vertexCoord[ tar ];
                    coordD = vertexCoord[ src ];
                }
                double diffX = coordD.x() - coordO.x();
                double diffY = coordD.y() - coordO.y();
                double angle = atan2( diffY, diffX );

                edgeIndex[ foreED ] = _nEdges;
                edgeID[ foreED ] = _nEdges;
                edgeWeight[ foreED ] = weight;
                edgeGeoAngle[ foreED ] = angle;
                edgeSmoAngle[ foreED ] = angle;
                edgeCurAngle[ foreED ] = angle;
                edgeLineID[ foreED ].push_back( _nLines );
                edgeSelectShift[ foreED ] = false;
                edgeSelectCtrl[ foreED ] = false;
                edgeIsLine[ foreED ] = true;
                edgeIsLeader[ foreED ] = false;

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

#ifdef SKIP
//------------------------------------------------------------------------------
//      Adjust the geogrpahical coordinates according to the aspect ratio
//      between the distances along the lines of longitude and latitude
//------------------------------------------------------------------------------

    //Coord2 sum = Coord2( 0.0, 0.0 );
    //BGL_FORALL_VERTICES( vd, graph, Graph ) {
    //    sum += vertexCoord[ vd ];
    //}
    //Coord2 ave = sum / ( double )num_vertices( graph );

    //double aspect = cos( ave.y() * M_PI / 180.0 );
    //cerr << "********** average coordinates = " << ave;
    //cerr << "********** Geographical aspect ratio = " << aspect << endl;

    BGL_FORALL_VERTICES( vd, graph, Graph ) {

        DegreeSizeType degrees = out_degree( vd, graph );
        //vertexHome[ vd ].x()    *= aspect;
        //vertexGeo[ vd ].x()     *= aspect;
        //vertexSmooth[ vd ].x()  *= aspect;
        //vertexCoord[ vd ].x()   *= aspect;
        if( degrees > 2 ) {
            vertexSize[ vd ] = M_PI * SQUARE( OUTDISK_LARGE );
            _meanVSize += vertexSize[ vd ];
        }
        else {
            vertexSize[ vd ] = M_PI * SQUARE( OUTDISK_SMALL );
            _meanVSize += vertexSize[ vd ];
        }
    }
    _meanVSize /= _nStations;

#endif  // SKIP
    cout << " numStations = " << _nStations << " num_vertices = " << num_vertices( graph ) << endl;
    cout << " numEdges = " << _nEdges << " num_edges = " << num_edges( graph ) << endl;

#ifdef  DEBUG
    printGraph( graph );
#endif  // DEBUG

    return;
}

// end of header file
// Do not add any stuff under this line.
