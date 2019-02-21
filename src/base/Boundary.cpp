//==============================================================================
// Boundary.cc
//  : program file for the region data
//
//------------------------------------------------------------------------------
//
//              Date: Thu Feb 21 04:28:26 2019
//
//==============================================================================

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------

#include "base/Boundary.h"


//------------------------------------------------------------------------------
//	Protected functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------
//
//  Boundary::init -- initialize
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Boundary::_init( void )
{
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

    _distAlpha = obj._distAlpha;
    _distBeta = obj._distBeta;
    _meanVSize = obj._meanVSize;

    for ( unsigned int k = 0; k < _nLines; ++k ) {

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
//  Boundary::clear --    clear the current Boundary information
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Boundary::_clear( void )
{
}


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
}


// end of header file
// Do not add any stuff under this line.
