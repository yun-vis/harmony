// Octilinear.cpp
//  : program file for the boundary network
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

#include "optimization/Octilinear.h"

//------------------------------------------------------------------------------
//	Protected functions
//------------------------------------------------------------------------------
//
//  Octilinear::_init --        initialize the constrained optimization problem
//
//  Inputs
//      __boundary : pointer to boundary
//
//  Outputs
//      none
//
void Octilinear::_init( Boundary * __boundary, double __half_width, double __half_height )
{
    _boundary                   = __boundary;
    BoundaryGraph        & g    = _boundary->boundary();
    unsigned int nVertices      = _boundary->nVertices();
    unsigned int nEdges         = _boundary->nEdges();
    _d_Alpha                    = _boundary->dAlpha();
    _d_Beta                     = _boundary->dBeta();

    // initialization
    _nVars = _nConstrs = 0;
    _half_width = __half_width;
    _half_height = __half_height;

    string configFilePath = "../configs/octilinear.conf";

    //read config file
    Base::Config conf( configFilePath );

    //_w_octilinear = sqrt( 10.0 );
    //_w_position = sqrt( 0.00001 );
    //_w_boundary = sqrt( 20.0 );
    //_w_crossing = sqrt( 20.0 );

    if ( conf.has( "w_octilinear" ) ){
        string paramOctilinear = conf.gets( "w_octilinear" );
        _w_octilinear = sqrt( stringToDouble( paramOctilinear ) );
    }

    if ( conf.has( "w_position" ) ){
        string paramPosition = conf.gets( "w_position" );
        _w_position = sqrt( stringToDouble( paramPosition ) );
    }

    if ( conf.has( "w_boundary" ) ){
        string paramBoundary = conf.gets( "w_boundary" );
        _w_boundary = sqrt( stringToDouble( paramBoundary ) );
    }

    if ( conf.has( "w_crossing" ) ){
        string paramCrossing = conf.gets( "w_crossing" );
        _w_crossing = sqrt( stringToDouble( paramCrossing ) );
    }

    if ( conf.has( "opttype" ) ){
        string paramType = conf.gets( "opttype" );
        if( paramType == "LEAST_SQUARE" )
            _opttype = LEAST_SQUARE;
        else if( paramType == "CONJUGATE_GRADIENT" )
            _opttype = CONJUGATE_GRADIENT;
        else{
            assert( false );
        }
    }

#ifdef DEBUG
    cerr << "nAlpha = " << nAlpha << " nBeta = " << nBeta << " nVertices = " << nEdges << endl;
    cerr << "_d_Alpha = " << _d_Alpha << " _d_Beta = " << _d_Beta << endl;
#endif  // DEBUG

//------------------------------------------------------------------------------
//      Total number of linear variables
//------------------------------------------------------------------------------
    _nVars = 2 * nVertices;

//------------------------------------------------------------------------------
//      Total number of linear constraints
//------------------------------------------------------------------------------
    // Regular edge length 
    _nConstrs += 2 * nEdges;

    // Positional constraints
    _nConstrs += 2 * nVertices;

    _initCoefs();
    _initVars();
    _initOutputs();
    _updateCoefs();
    _updateOutputs();

#ifdef  DEBUG
    printGraph( g );
    cerr << " nVertices = " << nVertices << " nEdges = " << nEdges << endl;
    cerr << " nVars = " << _nVars << " nConstrs = " << _nConstrs << endl;
    cerr << "Finished initializing the linear system" << endl;
    cerr << "_opttype = " << _opttype << endl;
#endif  // DEBUG
}


//
//  Octilinear::_initCoefs --        initialize the coefficient
//
//  Inputs
//      none
//
//  Outputs
//      none
//
void Octilinear::_initCoefs( void )
{
    BoundaryGraph        & g      = _boundary->boundary();
    unsigned int nVertices        = _boundary->nVertices();
    //unsigned int nEdges         = _boundary->nEdges();

    // initialization
    unsigned int nRows = 0;
    _coef.resize( _nConstrs, _nVars );
    _coef << Eigen::MatrixXd::Zero( _nConstrs, _nVars );

    // Regular edge octilinearty
    BGL_FORALL_EDGES( edge, g, BoundaryGraph )
    {
        BoundaryGraph::vertex_descriptor vdS = source( edge, g );
        BoundaryGraph::vertex_descriptor vdT = target( edge, g );
        unsigned int idS = MIN2( g[ vdS ].id, g[ vdT ].id );
        unsigned int idT = MAX2( g[ vdS ].id, g[ vdT ].id );

        // x
        _coef( nRows, idS ) = _w_octilinear;
        _coef( nRows, idT ) = -_w_octilinear;
        nRows++;

        // y
        _coef( nRows, idS + nVertices ) = _w_octilinear;
        _coef( nRows, idT + nVertices ) = -_w_octilinear;
        nRows++;
    }

    // Positional constraints
    BGL_FORALL_VERTICES( vertex, g, BoundaryGraph ){

        unsigned int id = g[ vertex ].id;

        // x
        _coef( nRows, id ) = _w_position;
        nRows++;

        // y
        _coef( nRows, id + nVertices ) = _w_position;
        nRows++;
    }

#ifdef  DEBUG
    cerr << "_coef:" << endl;
    cerr << _coef << endl;
#endif  // DEBUG
}


//
//  Octilinear::_initVars --        initialize the variables
//
//  Inputs
//      none
//
//  Outputs
//      none
//
void Octilinear::_initVars( void )
{
    BoundaryGraph        & g            = _boundary->boundary();
    unsigned int nVertices      = _boundary->nVertices();

    // initialization
    _var.resize( _nVars );

    unsigned int nRows = 0;
    BGL_FORALL_VERTICES( vertex, g, BoundaryGraph ){

        _var( nRows, 0 ) = g[ vertex ].coordPtr->x();
        _var( nRows + nVertices, 0 ) = g[ vertex ].coordPtr->y();
        nRows++;
    }

#ifdef  DEBUG
    cerr << "_initvar:" << endl;
    cerr << _var << endl; 
#endif  // DEBUG
}


void Octilinear::_updateEdgeCurAngle( void )
{
    BoundaryGraph        & g            = _boundary->boundary();

    // initialization
    BGL_FORALL_EDGES( edge, g, BoundaryGraph ){

        BoundaryGraph::vertex_descriptor vS = source( edge, g );
        BoundaryGraph::vertex_descriptor vT = target( edge, g );
        Coord2 vi, vj;
        if( g[ vS ].id < g[ vT ].id ){
            vi = *g[ vS ].coordPtr;
            vj = *g[ vT ].coordPtr;
        }
        else{
            vi = *g[ vT ].coordPtr;
            vj = *g[ vS ].coordPtr;
        }
        Coord2 vji = vj - vi;

        double angle = atan2( vji.y(), vji.x() );
        g[ edge ].angle = angle;
    }

#ifdef  DEBUG
    BGL_FORALL_EDGES( edge, g, BoundaryGraph ){
        cerr << "EID = " << g[ edge ].id
             << ", edgeCurAngle = " << g[ edge ].angle << endl;
    }
#endif  // DEBUG
}

void Octilinear::_setTargetAngle( void )
{
    BoundaryGraph        &g            = _boundary->boundary();

    double sector[ 9 ] = { -M_PI, -3.0*M_PI/4.0, -M_PI/2.0, -M_PI/4.0, 0.0,
                           M_PI/4.0, M_PI/2.0, 3.0*M_PI/4.0, M_PI };

    // initialization
    BGL_FORALL_EDGES( edge, g, BoundaryGraph ){

        double targetAngle = 2.0*M_PI;
        double minDist = 2.0*M_PI + 1.0;
        for( unsigned int i = 0; i < 9; i++ ){
            double dist = fabs( g[ edge ].angle - sector[i] );
            if( dist < minDist ){
                minDist = dist;
                targetAngle = sector[i];
            }
        }
        g[ edge ].targetAngle = targetAngle;
    }
}

#ifdef  SKIP
void Octilinear::_setTargetAngle( void )
{
    BoundaryGraph        & g            = _boundary->boundary();

    double sector[ 9 ] = { -M_PI, -3.0*M_PI/4.0, -M_PI/2.0, -M_PI/4.0, 0.0,
                           M_PI/4.0, M_PI/2.0, 3.0*M_PI/4.0, M_PI };

    vector< vector< BoundaryGraph::vertex_descriptor > > vdVec( OCTILINEAR_SECTOR );
    BGL_FORALL_VERTICES( vertex, g, BoundaryGraph ){
        BoundaryGraph::degree_size_type degrees = out_degree( vertex, g );
        vdVec[ OCTILINEAR_SECTOR - degrees ].push_back( vertex );
    }

#ifdef  DEBUG
    for( int i = 0; i < vdVec.size(); i++ ){
        cerr << " i = " << OCTILINEAR_SECTOR-i << " ";
        for( int j = 0; j < vdVec[ i ].size(); j++ ){
            cerr << vertexCoord[ vdVec[i][j] ] <<  " ";
        }
        cerr << endl;
    }
#endif  // DEBUG

    // initialization
    BGL_FORALL_EDGES( edge, g, BoundaryGraph ){
        g[ edge ].targetAngle = 2.0*M_PI;
    }

    // set target angles
    for( int i = 0; i < vdVec.size(); i++ ){
        for( int j = 0; j < vdVec[ i ].size(); j++ ){

            map< double, BoundaryGraph::edge_descriptor > circM;
            // sort the angle
            BoundaryGraph::out_edge_iterator e, e_end;
            for ( tie( e, e_end ) = out_edges( vdVec[i][j], g ); e != e_end; ++e ) {
                BoundaryGraph::edge_descriptor ed = *e;
                BoundaryGraph::vertex_descriptor vS = source( ed, g );
                BoundaryGraph::vertex_descriptor vT = target( ed, g );
                double angle = g[ ed ].angle;

                if ( g[ vS ].id > g[ vT ].id ) {
                    if ( angle > 0 ) angle = -M_PI + g[ ed ].angle;
                    else angle = M_PI + g[ ed ].angle;
                }
                circM.insert( pair< double, BoundaryGraph::edge_descriptor >( angle, ed ) );
                //cerr << "eID = " << edgeID[ ed ] << " angle = " << angle << " curAngle = " << edgeCurAngle[ ed ] << endl;
                // cerr << "sID = " << vertexID[ vS ] << " tID = " << vertexID[ vT ] << endl;
            }

            // assign the sector
            int index = 0;
            for ( map< double, BoundaryGraph::edge_descriptor >::iterator it = circM.begin();
                  it != circM.end(); it++ ) {

                BoundaryGraph::edge_descriptor ed = it->second;
                BoundaryGraph::vertex_descriptor vS = source( ed, g );
                BoundaryGraph::vertex_descriptor vT = target( ed, g );

                if( g[ ed ].targetAngle == 2.0*M_PI ){
                    double angle = g[ ed ].angle;
                    if ( g[ vS ].id > g[ vT ].id ) {
                        if ( angle > 0 ) angle = -M_PI + g[ ed ].angle;
                        else angle = M_PI + g[ ed ].angle;
                    }
                    double target = 0.0, minDist = M_PI;

                    for ( int k = index; k < 9 ; k++ ) {

                        double dist = fabs( sector[ k ] - angle );
                        if( minDist > dist ) {
                            minDist = dist;
                            target = sector[ k ];
                            index = k+1;
                        }
                    }
                    if ( g[ vS ].id > g[ vT ].id ) {
                        if ( target > 0 ) g[ ed ].targetAngle = -M_PI + target;
                        else g[ ed ].targetAngle = M_PI + target;
                    }
                    else{
                        g[ ed ].targetAngle = target;
                    }
                    //cerr << "EID = " << edgeID[ ed ] << ", index = " << index << ", target = " << edgeTarget[ ed ] << endl;
                    //cerr << "EID = " << edgeID[ ed ] << ", angle = " << edgeCurAngle[ ed ] << ", target = " << edgeTarget[ ed ] << endl;
                }
                else{
                    double target = g[ ed ].targetAngle;

                    if ( g[ vS ].id > g[ vT ].id ) {
                        if ( target > 0 ) target = -M_PI + g[ ed ].targetAngle;
                        else target = M_PI + g[ ed ].targetAngle;
                    }
                    for( int k = index; k < 9; k++ ){
                        if( target == sector[ k ] ) index = k+1;
                    }
                    //cerr << "EID = " << edgeID[ ed ] << " skipping... " << index << endl;
                }
            }
        }
    }

#ifdef  DEBUG
    BGL_FORALL_EDGES( edge, g, BoundaryGraph ){
        cerr << "eid = "<< edgeID[ edge ] << " angle = " << edgeCurAngle[ edge ] 
             << " target = " << edgeTarget[ edge ] + edgeCurAngle[ edge ] 
             << " rotate = " << edgeTarget[ edge ] << endl;
    }
#endif  // DEBUG
}
#endif // SKIP

#ifdef  SKIP
double Octilinear::_findRotateAngle( double input )
{
    double target = 0.0, minDist = M_PI;
    double sector[ 9 ] = { -M_PI, -3.0*M_PI/4.0, -M_PI/2.0, -M_PI/4.0, 0.0,
                           M_PI/4.0, M_PI/2.0, 3.0*M_PI/4.0, M_PI };

    for ( int i = 0; i < 9 ; i++ ) {

        double dist = fabs( sector[ i ] - input );
        if( minDist > dist ) {
            minDist = dist;
            target = sector[ i ];
        }
    }

#ifdef  DEBUG
    cerr << "inputAngle = " << input << " targetAngle = " << target << " rotate = " << target-input << endl;
#endif  // DEBUG
    return target - input;
}
#endif  // SKIP

//
//  Octilinear::_initOutputs --        initialize the output
//
//  Inputs
//      none
//
//  Outputs
//      none
//
void Octilinear::_initOutputs( void )
{
    BoundaryGraph        & g            = _boundary->boundary();

    // initialization
    unsigned int nRows = 0;
    _output.resize( _nConstrs );
    _output << Eigen::VectorXd::Zero( _nConstrs );

    _updateEdgeCurAngle();
    _setTargetAngle();
    // Regular edge octilinearty
    BGL_FORALL_EDGES( edge, g, BoundaryGraph )
    {
        BoundaryGraph::vertex_descriptor vdS = source( edge, g );
        BoundaryGraph::vertex_descriptor vdT = target( edge, g );
        Coord2 vi, vj;
        if( g[ vdS ].id < g[ vdT ].id ){
            vi = *g[ vdS ].smoothPtr;
            vj = *g[ vdT ].smoothPtr;
        }
        else{
            vi = *g[ vdT ].smoothPtr;
            vj = *g[ vdS ].smoothPtr;
        }
        Coord2 vji = vi - vj;
#ifdef  DEBUG
        cerr << "vji = " << vji;
#endif  // DEBUG
        double angle = g[ edge ].angle;
        double theta = g[ edge ].targetAngle - angle;
#ifdef  DEBUG
        cerr << "e( " << g[vdS].id << "," << g[vdT].id << " )"
             << " targetAngle = " << g[ edge ].targetAngle
             << " angle = " << g[ edge ].angle << endl;
        cerr << "eid = " << edgeID[ edge ] << " ";
#endif  // DEBUG
        double cosTheta = cos( theta ), sinTheta = sin( theta );
        //double s = 1.0;
        double s = _d_Beta * g[ edge ].weight / vji.norm();
        //double s = edgeWeight[ edge ] / vji.norm();

        // x
        _output( nRows, 0 ) = _w_octilinear * s * ( cosTheta * vji.x() - sinTheta * vji.y() );
        nRows++;

        // y
        _output( nRows, 0 ) = _w_octilinear * s * ( sinTheta * vji.x() + cosTheta * vji.y() );
        nRows++;
    }


    // Positional constraints
    BGL_FORALL_VERTICES( vertex, g, BoundaryGraph ){

        _output( nRows, 0 ) = _w_position * g[ vertex ].smoothPtr->x();
        nRows++;
        _output( nRows, 0 ) = _w_position * g[ vertex ].smoothPtr->y();
        nRows++;
    }

#ifdef  DEBUG
    cerr << "_initOutput:" << endl;
    cerr << _output << endl;
#endif  // DEBUG
}


//
//  Octilinear::_updateCoefs --        update the coefs
//
//  Inputs
//      none
//
//  Outputs
//      none
//
void Octilinear::_updateCoefs( void )
{
    BoundaryGraph               & g             = _boundary->boundary();
    unsigned int        nVertices       = _boundary->nVertices();
    unsigned int        nVE             = 0;
    unsigned int        nB              = 0;
    vector< double >    ratioR          = _boundary->ratioR();

    Eigen::MatrixXd     oldCoef;
    oldCoef = _coef.block( 0, 0, _nConstrs, _nVars );

#ifdef OCTILINEAR_CONFLICT
    nVE = _boundary->VEconflict().size();
#endif // OCTILINEAR_CONFLICT
#ifdef OCTILINEAR_BOUNDARY
    BGL_FORALL_VERTICES( vd, g, BoundaryGraph )
    {
        double minD = _d_Beta/2.0;
        //if( vertexIsStation[ vd ] == false )
        //    minD = vertexExternal[ vd ].leaderWeight() * _d_Beta/2.0;
        if ( g[ vd ].coordPtr->x() <= -( _half_width - minD ) ) nB++;
        if ( g[ vd ].coordPtr->x() >= ( _half_width - minD ) ) nB++;
        if ( g[ vd ].coordPtr->y() <= -( _half_height - minD ) ) nB++;
        if ( g[ vd ].coordPtr->y() >= ( _half_height - minD ) ) nB++;
    }
#endif // OCTILINEAR_BOUNDARY
    _coef.resize( _nConstrs + nB + 2*nVE, _nVars );
    // _coef << Eigen::MatrixXd::Zero( _nConstrs + 2*nVE, _nVars );

    // copy old coefficient
    _coef << oldCoef, Eigen::MatrixXd::Zero( nB + 2*nVE, _nVars );

    // cerr << "nVE = " << nVE << endl;
    unsigned int nRows = _nConstrs;

#ifdef  OCTILINEAR_BOUNDARY
    // add boundary coefficient
    BGL_FORALL_VERTICES( vd, g, BoundaryGraph ){

        unsigned int id = g[ vd ].id;
        double minD = _d_Beta/2.0;
        //if( vertexIsStation[ vd ] == false )
        //    minD = vertexExternal[ vd ].leaderWeight() * _d_Beta/2.0;

        if ( g[ vd ].coordPtr->x() <= -( _half_width - minD ) ) {
            _coef( nRows, id ) = _w_boundary;
            nRows++;
        }
        if ( g[ vd ].coordPtr->x() >= ( _half_width - minD ) ) {
            _coef( nRows, id ) = _w_boundary;
            nRows++;
        }
        if ( g[ vd ].coordPtr->y() <= -( _half_height - minD ) ) {
            _coef( nRows, id + nVertices ) = _w_boundary;
            nRows++;
        }
        if ( g[ vd ].coordPtr->y() >= ( _half_height - minD ) ) {
            _coef( nRows, id + nVertices ) = _w_boundary;
            nRows++;
        }
    }
#endif  // OCTILINEAR_BOUNDARY

#ifdef  OCTILINEAR_CONFLICT
    // copy conflict coefficient
    unsigned int countVE = 0;
    for ( VEMap::iterator it = _boundary->VEconflict().begin();
          it != _boundary->VEconflict().end(); ++it ) {
        BoundaryGraph::vertex_descriptor vdV = it->second.first;
        BoundaryGraph::edge_descriptor ed = it->second.second;
        BoundaryGraph::vertex_descriptor vdS = source( ed, g );
        BoundaryGraph::vertex_descriptor vdT = target( ed, g );
        unsigned int idV = g[ vdV ].id;
        unsigned int idS = g[ vdS ].id;
        unsigned int idT = g[ vdT ].id;
        double r = ratioR[ countVE ];

        // x
        _coef( nRows, idV ) = 1.0 * _w_crossing;
        _coef( nRows, idS ) = -r * _w_crossing;
        _coef( nRows, idT ) = ( r - 1.0 ) * _w_crossing;
        nRows++;

        // y
        _coef( nRows, idV + nVertices ) = 1.0 * _w_crossing;
        _coef( nRows, idS + nVertices ) = -r * _w_crossing;
        _coef( nRows, idT + nVertices ) = ( r - 1.0 ) * _w_crossing;
        nRows++;
        
        countVE++;
    } 
#endif  // OCTILINEAR_CONFLICT

#ifdef  DEBUG
    cerr << "###############" << endl;
    cerr << "newCoef:" << endl;
    cerr << _coef << endl;
    cerr << "###############" << endl;
#endif  // DEBUG
}

//
//  Octilinear::_updateOutputs --        update the output
//
//  Inputs
//      none
//
//  Outputs
//      none
//
void Octilinear::_updateOutputs( void )
{
    BoundaryGraph               & g             = _boundary->boundary();
    unsigned int        nVE             = 0;
    unsigned int        nB              = 0;
    vector< double >    ratioR          = _boundary->ratioR();

    unsigned int nRows = 0;
    Eigen::VectorXd     oldOutput;
    oldOutput = _output;
#ifdef  OCTILINEAR_CONFLICT
    nVE = _boundary->VEconflict().size();
#endif  // OCTILINEAR_CONFLICT
#ifdef  OCTILINEAR_BOUNDARY
    BGL_FORALL_VERTICES( vd, g, BoundaryGraph )
    {
        double minD = _d_Beta/2.0;
        //if( vertexIsStation[ vd ] == false )
        //    minD = vertexExternal[ vd ].leaderWeight() * _d_Beta/2.0;
        if ( g[ vd ].coordPtr->x() <= -( _half_width - minD ) ) nB++;
        if ( g[ vd ].coordPtr->x() >= ( _half_width - minD ) ) nB++;
        if ( g[ vd ].coordPtr->y() <= -( _half_height - minD ) ) nB++;
        if ( g[ vd ].coordPtr->y() >= ( _half_height - minD ) ) nB++;
    }
#endif  // OCTILINEAR_BOUNDARY
    _output.resize( _nConstrs + nB + 2*nVE );
    _output << Eigen::VectorXd::Zero( _nConstrs + nB + 2*nVE );

    _updateEdgeCurAngle();
    //_setTargetAngle();
    // Regular edge length
    BGL_FORALL_EDGES( edge, g, BoundaryGraph )
    {
        BoundaryGraph::vertex_descriptor vdS = source( edge, g );
        BoundaryGraph::vertex_descriptor vdT = target( edge, g );
        //Coord2 vi = vertexSmooth[ vdS ];
        //Coord2 vj = vertexSmooth[ vdT ];
        Coord2 vi, vj;
        if( g[ vdS ].id < g[ vdT ].id ){
            vi = *g[ vdS ].coordPtr;
            vj = *g[ vdT ].coordPtr;
        }
        else{
            vi = *g[ vdT ].coordPtr;
            vj = *g[ vdS ].coordPtr;
        }
        Coord2 vji = vi - vj;
        double angle = g[ edge ].angle;
        double theta = g[ edge ].targetAngle - angle;
        double cosTheta = cos( theta ), sinTheta = sin( theta );
        double s = 1.0;
        //double s = edgeWeight[ edge ] / vji.norm();

#ifdef  DEBUG
        cerr << "vji = " << vji;
        if( vertexID[ vdS ] == 4 && vertexID[ vdT ] == 5 )
            cerr << "( " << vertexID[ vdS ] << ", " << vertexID[ vdT ] << ") angle = " << angle << " theta = " << theta << endl;
#endif  // DEBUG

        // x
        _output( nRows, 0 ) = _w_octilinear * s * ( cosTheta * vji.x() - sinTheta * vji.y() );
        nRows++;

        // y
        _output( nRows, 0 ) = _w_octilinear * s * ( sinTheta * vji.x() + cosTheta * vji.y() );
        nRows++;
    }

    // Positional constraints
    BGL_FORALL_VERTICES( vertex, g, BoundaryGraph ){

        // x
        _output( nRows, 0 ) = oldOutput( nRows, 0 );
        nRows++;

        // y
        _output( nRows, 0 ) = oldOutput( nRows, 0 );
        nRows++;
    }

#ifdef  OCTILINEAR_BOUNDARY
    // boundary constraints
    BGL_FORALL_VERTICES( vd, g, BoundaryGraph ){

        double minD = _d_Beta/2.0;
        //if( vertexIsStation[ vd ] == false )
        //    minD = vertexExternal[ vd ].leaderWeight() * _d_Beta/2.0;

        if ( g[ vd ].coordPtr->x() <= -( _half_width - minD ) ) {
            _output( nRows, 0 ) = _w_boundary * -( _half_width - minD );
            nRows++;
        }
        if ( g[ vd ].coordPtr->x() >= ( _half_width - minD ) ) {
            _output( nRows, 0 ) = _w_boundary * ( _half_width - minD );
            nRows++;
        }
        if ( g[ vd ].coordPtr->y() <= -( _half_height - minD ) ) {
            _output( nRows, 0 ) = _w_boundary * -( _half_height - minD );
            nRows++;
        }
        if ( g[ vd ].coordPtr->y() >= ( _half_height - minD ) ) {
            _output( nRows, 0 ) = _w_boundary * ( _half_height - minD );
            nRows++;
        }
    }
#endif  // OCTILINEAR_BOUNDARY

#ifdef  OCTILINEAR_CONFLICT
    // copy conflict coefficient
    unsigned int countVE = 0;
    for ( VEMap::iterator it = _boundary->VEconflict().begin();
          it != _boundary->VEconflict().end(); ++it ) {
        BoundaryGraph::vertex_descriptor vdV = it->second.first;
        BoundaryGraph::edge_descriptor ed = it->second.second;
        BoundaryGraph::vertex_descriptor vdS = source( ed, g );
        BoundaryGraph::vertex_descriptor vdT = target( ed, g );
        unsigned int idV = g[ vdV ].id;
        unsigned int idS = g[ vdS ].id;
        unsigned int idT = g[ vdT ].id;
        double r = ratioR[ countVE ];

        Coord2 v = *g[ vdV ].geoPtr;
        Coord2 p = r * *g[ vdS ].geoPtr + ( 1.0-r ) * *g[ vdT ].geoPtr;
        double minD = _d_Beta/2.0;
        //if( vertexIsStation[ vdV ] == false )
        //    minD = vertexExternal[ vdV ].leaderWeight() * _d_Beta/2.0;
        double delta = minD / ( v - p ).norm();

        // x
        _output( nRows, 0 ) = _w_crossing * delta * ( v - p ).x();
        nRows++;

        // y
        _output( nRows, 0 ) = _w_crossing * delta * ( v - p ).y();
        nRows++;
    }
#endif  // OCTILINEAR_CONFLICT

#ifdef  DEBUG
    cerr << "_updatedOutput:" << endl;
    cerr << _output << endl;
#endif  // DEBUG
}

//
//  Octilinear::LeastSquare --        optimization
//
//  Inputs
//      none
//
//  Outputs
//      err:
//
double Octilinear::LeastSquare( unsigned int iter )
{
    double mse = 0.0;
    for( int i = 0; i < iter; i++ ) {

        Eigen::VectorXd last_var = _var;

#ifdef  DEBUG_TIME
        clock_t start_time = 0;
        float time1 = 0, time2 = 0, time3 = 0, time4 = 0;
        start_time = clock();
#endif  // DEBUG_TIME

        // ### optimization ###
        _var = ( ( _coef.transpose() * _coef ) ).inverse() * _coef.transpose() * _output;
#ifdef  DEBUG_TIME
        time1 = clock() - start_time;
        start_time = clock();
#endif  // DEBUG_TIME

        // ### retrieve the result ###
        retrieve();
#ifdef  DEBUG_TIME
        time2 = clock() - start_time;
        start_time = clock();
#endif  // DEBUG_TIME

        // ### update coefficient matrix ###
        _updateCoefs();
#ifdef  DEBUG_TIME
        time3 = clock() - start_time;
        start_time = clock();
#endif  // DEBUG_TIME

        // ### update target values ###
        _updateOutputs();
#ifdef  DEBUG_TIME
        time4 = clock() - start_time;
#endif  // DEBUG_TIME

        // node movement 
        Eigen::VectorXd err = last_var - _var;
        mse = err.adjoint() * err;

        if( (i+1) % 100 == 0 )
            cerr << setprecision( 10 ) << "Loop(" << i << ") mse = " << mse << endl;
#ifdef  DEBUG_TIME
            cerr << "time1 = " << time1 << " time2 = " << time2 << " time3 = " << time3 << " time4 = " << time4 << endl;
#endif  // DEBUG_TIME
        if( ( mse ) < 5.0e-4 ) break;
    }

#ifdef  DEBUG
    cerr << "_var:" << endl;
    cerr << _var << endl;
#endif  // DEBUG
    return mse;
}

//
//  Octilinear::ConjugateGradient --        optimization
//
//  Inputs
//      none
//
//  Outputs
//      err
//
double Octilinear::ConjugateGradient( unsigned int iter )
{
    // initialization, prepare the square matrix
    Eigen::MatrixXd A;
    Eigen::VectorXd b, Ap;
    A = _coef.transpose() * _coef;
    b = _coef.transpose() * _output;

    // initialization
    Eigen::VectorXd err = b - A * _var;
    Eigen::VectorXd p = err;
    double rsold = err.adjoint() * err;

    // main algorithm
    for( int i = 0; i < iter; i++ ) {

        // prepare the square matrix
        A = _coef.transpose() * _coef;
        b = _coef.transpose() * _output;
        Ap = A * p;

        double alpha = (double)( p.transpose() * err ) / (double)( p.transpose() * Ap );
        _var = _var + alpha * p;
        err = b - A * _var;

        if ( sqrt( err.adjoint() * err ) < 1e-10 ) {
            cerr << "sqrterror(" << i << ") = " << sqrt( err.adjoint() * err ) << endl;
            break;
        }
        else {
            double beta = -1.0 * (double)( err.transpose() * Ap ) / (double)( p.transpose() * Ap );
            p = err + beta * p;
        }

        // update
        retrieve();
        _updateCoefs();
        _updateOutputs();
    }

    // cerr << "sqrterror = " << sqrt( err.adjoint() * err ) << endl;
    return sqrt( err.adjoint() * err );
}

//
//  Octilinear::retrieve --        retrieve the result
//
//  Inputs
//      none
//
//  Outputs
//      none
//
void Octilinear::retrieve( void )
{
    BoundaryGraph & g           = _boundary->boundary();
    unsigned int nVertices      = _boundary->nVertices();

    // find the vertex that is too close to an edge
    vector< BoundaryGraph::vertex_descriptor > vdVec;
    for ( VEMap::iterator it = _boundary->VEconflict().begin();
          it != _boundary->VEconflict().end(); ++it ) {
        BoundaryGraph::vertex_descriptor vdV = it->second.first;
        vdVec.push_back( vdV );
    }

    unsigned int nRows = 0;
    // update coordinates
    // but freeze the vertex that is too close to an edge
    BGL_FORALL_VERTICES( vertex, g, BoundaryGraph ){

        bool doClose = false;
        for( unsigned int i = 0; i < vdVec.size(); i++ ){
            if( vertex == vdVec[i] ) doClose = true;
        }

        if( _boundary->VEconflict().size() > 0 ){
            Coord2 downscale;
            downscale.x() = ( _var( nRows, 0 ) - g[ vertex ].coordPtr->x() )/2.0 + g[ vertex ].coordPtr->x();
            downscale.y() = ( _var( nRows + nVertices, 0 ) - g[ vertex ].coordPtr->y() )/2.0 + g[ vertex ].coordPtr->y();
            g[ vertex ].coordPtr->x() = downscale.x();
            g[ vertex ].coordPtr->y() = downscale.y();
        }
        else{
            g[ vertex ].coordPtr->x() = _var( nRows, 0 );
            g[ vertex ].coordPtr->y() = _var( nRows + nVertices, 0 );
        }
        nRows++;
    }

    // check possible conflict
    _boundary->checkVEConflicts();

#ifdef  DEBUG
    cerr << "retrieve:" << endl;
    BGL_FORALL_VERTICES( vertex, g, BoundaryGraph ){
        cerr << "V(" << g[ vertex ].id << ") = " << vertexSmooth[ vertex ];
    }
    BGL_FORALL_EDGES( edge, g, BoundaryGraph ){
        cerr << "E(" << g[ edge ].id << ") : smoAngle= " << edgeSmoAngle[ edge ] << endl;
    }
#endif  // DEBUG
}


//
//  Octilinear::clear --        memory management
//
//  Inputs
//      none
//
//  Outputs
//      none
//
void Octilinear::clear( void )
{
}

//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------

//
//  Octilinear::Octilinear -- default constructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
Octilinear::Octilinear( void )
{   
}

//
//  Octilinear::Octilinear -- copy constructor
//
//  Inputs
//  obj : object of this class
//
//  Outputs
//  none
//
Octilinear::Octilinear( const Octilinear & obj )
{
}


//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------

//
//  Octilinear::~Octilinear --    destructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
Octilinear::~Octilinear( void )
{
}

// end of header file
// Do not add any stuff under this line.

