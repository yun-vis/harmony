// Octilinear.cpp
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

#include "Octilinear.h"

//------------------------------------------------------------------------------
//	Protected functions
//------------------------------------------------------------------------------
//
//  Octilinear::_init --        initialize the constrained optimization problem
//
//  Inputs
//      __metro : pointer to metro
//
//  Outputs
//      none
//
void Octilinear::_init( Metro * __metro, double __half_width, double __half_height )
{
    _metro                      = __metro;
    Graph        & g            = _metro->g();
    unsigned int nVertices      = _metro->nStations();
    unsigned int nEdges         = _metro->nEdges();
    _d_Alpha                    = _metro->dAlpha();
    _d_Beta                     = _metro->dBeta();

    VertexIDMap         vertexID        = get( vertex_myid, g );
    VertexGeoMap        vertexGeo       = get( vertex_mygeo, g );
    EdgeWeightMap       edgeWeight      = get( edge_weight, g );

    // initialization
    _nVars = _nConstrs = 0;
    _w_octilinear = sqrt( 10.0 );
    _w_position = sqrt( 0.00001 );
    _w_boundary = sqrt( 20.0 );
    _w_crossing = sqrt( 20.0 );
    _half_width = __half_width;
    _half_height = __half_height;

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
#endif  // DEBUG

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#ifdef  DEBUG
    cerr << " nVertices = " << nVertices << " nEdges = " << nEdges << endl;
    cerr << " nVars = " << _nVars << " nConstrs = " << _nConstrs << endl;
    cerr << "Finished initializing the linear system" << endl;
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
    Graph        & g            = _metro->g();
    unsigned int nVertices      = _metro->nStations();
    //unsigned int nEdges         = _metro->nEdges();

    VertexIDMap         vertexID        = get( vertex_myid, g );
    EdgeIDMap           edgeID          = get( edge_myid, g );
    EdgeGeoAngleMap     edgeGeoAngle    = get( edge_mygeoangle, g );
    EdgeSmoAngleMap     edgeSmoAngle    = get( edge_mysmoangle, g );

    // initialization
    unsigned int nRows = 0;
    _coef.resize( _nConstrs, _nVars );
    _coef << Eigen::MatrixXd::Zero( _nConstrs, _nVars );

    // Regular edge octilinearty
    BGL_FORALL_EDGES( edge, g, Graph )
    {
        VertexDescriptor vdS = source( edge, g );
        VertexDescriptor vdT = target( edge, g );
        unsigned int idS = MIN2( vertexID[ vdS ], vertexID[ vdT ] );
        unsigned int idT = MAX2( vertexID[ vdS ], vertexID[ vdT ] );

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
    BGL_FORALL_VERTICES( vertex, g, Graph ){

        unsigned int id = vertexID[ vertex ];

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
    Graph        & g            = _metro->g();
    unsigned int nVertices      = _metro->nStations();

    //VertexIDMap         vertexID        = get( vertex_myid, g );
    VertexCoordMap          vertexCoord     = get( vertex_mycoord, g );

    // initialization
    _var.resize( _nVars );

    unsigned int nRows = 0;
    BGL_FORALL_VERTICES( vertex, g, Graph ){

        _var( nRows, 0 ) = vertexCoord[ vertex ].x();
        _var( nRows + nVertices, 0 ) = vertexCoord[ vertex ].y();
        nRows++;
    }

#ifdef  DEBUG
    cerr << "_initvar:" << endl;
    cerr << _var << endl; 
#endif  // DEBUG
}


void Octilinear::_updateEdgeCurAngle( void )
{
    Graph        & g            = _metro->g();

    VertexIDMap         vertexID        = get( vertex_myid, g );
    VertexCoordMap      vertexCoord     = get( vertex_mycoord, g );
    EdgeIDMap           edgeID          = get( edge_myid, g );
    EdgeCurAngleMap     edgeCurAngle    = get( edge_mycurangle, g );

    // initialization
    BGL_FORALL_EDGES( edge, g, Graph ){

        VertexDescriptor vS = source( edge, g );
        VertexDescriptor vT = target( edge, g );
        Coord2 vi, vj;
        if( vertexID[ vS ] < vertexID[ vT ] ){
            vi = vertexCoord[ vS ];
            vj = vertexCoord[ vT ];
        }
        else{
            vi = vertexCoord[ vT ];
            vj = vertexCoord[ vS ];
        }
        Coord2 vji = vj - vi;

        double angle = atan2( vji.y(), vji.x() );
        edgeCurAngle[ edge ] = angle;
    }

#ifdef  DEBUG
    BGL_FORALL_EDGES( edge, g, Graph ){
        cerr << "EID = " << edgeID[ edge ] 
             << ", edgeCurAngle = " << edgeCurAngle[ edge ] << endl;
    }
#endif  // DEBUG
}

void Octilinear::_setTargetAngle( void )
{
    Graph        & g            = _metro->g();

    VertexIDMap         vertexID        = get( vertex_myid, g );
    VertexCoordMap      vertexCoord     = get( vertex_mycoord, g );
    EdgeIDMap           edgeID          = get( edge_myid, g );
    EdgeCurAngleMap     edgeCurAngle    = get( edge_mycurangle, g );
    EdgeTargetMap       edgeTarget      = get( edge_mytarget, g );

    double sector[ 9 ] = { -M_PI, -3.0*M_PI/4.0, -M_PI/2.0, -M_PI/4.0, 0.0,
                           M_PI/4.0, M_PI/2.0, 3.0*M_PI/4.0, M_PI };

    vector< vector< VertexDescriptor > > vdVec( OCTILINEAR_SECTOR );
    BGL_FORALL_VERTICES( vertex, g, Graph ){
        DegreeSizeType degrees = out_degree( vertex, g );
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
    BGL_FORALL_EDGES( edge, g, Graph ){
        edgeTarget[ edge ] = 2.0*M_PI;
    }

    // set target angles
    for( int i = 0; i < vdVec.size(); i++ ){
        for( int j = 0; j < vdVec[ i ].size(); j++ ){

            map< double, EdgeDescriptor > circM;
            // sort the angle
            OutEdgeIterator e, e_end;
            for ( tie( e, e_end ) = out_edges( vdVec[i][j], g ); e != e_end; ++e ) {
                EdgeDescriptor ed = *e;
                VertexDescriptor vS = source( ed, g );
                VertexDescriptor vT = target( ed, g );
                double angle = edgeCurAngle[ ed ];

                if ( vertexID[ vS ] > vertexID[ vT ] ) {
                    if ( angle > 0 ) angle = -M_PI + edgeCurAngle[ ed ];
                    else angle = M_PI + edgeCurAngle[ ed ];
                }
                circM.insert( pair< double, EdgeDescriptor >( angle, ed ) );
                //cerr << "eID = " << edgeID[ ed ] << " angle = " << angle << " curAngle = " << edgeCurAngle[ ed ] << endl;
                // cerr << "sID = " << vertexID[ vS ] << " tID = " << vertexID[ vT ] << endl;
            }

            // assign the sector
            int index = 0;
            for ( map< double, EdgeDescriptor >::iterator it = circM.begin();
                  it != circM.end(); it++ ) {

                EdgeDescriptor ed = it->second;
                VertexDescriptor vS = source( ed, g );
                VertexDescriptor vT = target( ed, g );

                if( edgeTarget[ ed ] == 2.0*M_PI ){
                    double angle = edgeCurAngle[ ed ];
                    if ( vertexID[ vS ] > vertexID[ vT ] ) {
                        if ( angle > 0 ) angle = -M_PI + edgeCurAngle[ ed ];
                        else angle = M_PI + edgeCurAngle[ ed ];
                    }
                    double target = 0.0, minDist = M_PI;
/*
                    if( index == 9 ){
                        double dist = fabs( sector[ 0 ] - angle );
                        if( minDist > dist ) {
                            minDist = dist;
                            target = sector[ 0 ];
                            index = 0;
                        }
                    }
*/
//                    else{

                        for ( int k = index; k < 9 ; k++ ) {

                            double dist = fabs( sector[ k ] - angle );
                            if( minDist > dist ) {
                                minDist = dist;
                                target = sector[ k ];
                                index = k+1;
                            }
                        }
//                    }
                    if ( vertexID[ vS ] > vertexID[ vT ] ) {
                        if ( target > 0 ) edgeTarget[ ed ] = -M_PI + target;
                        else edgeTarget[ ed ] = M_PI + target;
                    }
                    else{
                        edgeTarget[ ed ] = target;
                    }
                    //cerr << "EID = " << edgeID[ ed ] << ", index = " << index << ", target = " << edgeTarget[ ed ] << endl;
                    //cerr << "EID = " << edgeID[ ed ] << ", angle = " << edgeCurAngle[ ed ] << ", target = " << edgeTarget[ ed ] << endl;
                }
                else{
                    double target = edgeTarget[ ed ];

                    if ( vertexID[ vS ] > vertexID[ vT ] ) {
                        if ( target > 0 ) target = -M_PI + edgeTarget[ ed ];
                        else target = M_PI + edgeTarget[ ed ];
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
    BGL_FORALL_EDGES( edge, g, Graph ){
        cerr << "eid = "<< edgeID[ edge ] << " angle = " << edgeCurAngle[ edge ] 
             << " target = " << edgeTarget[ edge ] + edgeCurAngle[ edge ] 
             << " rotate = " << edgeTarget[ edge ] << endl;
    }
#endif  // DEBUG
}

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
    Graph        & g            = _metro->g();

    VertexIDMap             vertexID            = get( vertex_myid, g );
    VertexSmoothMap         vertexSmooth        = get( vertex_mysmooth, g );
    VertexSelectMagMap      vertexSelectMag     = get( vertex_myselectmag, g );
    EdgeIDMap               edgeID              = get( edge_myid, g );
    EdgeWeightMap           edgeWeight          = get( edge_weight, g );
    EdgeCurAngleMap         edgeCurAngle        = get( edge_mycurangle, g );
    EdgeTargetMap           edgeTarget          = get( edge_mytarget, g );

    // initialization
    unsigned int nRows = 0;
    _output.resize( _nConstrs );
    _output << Eigen::VectorXd::Zero( _nConstrs );

    _updateEdgeCurAngle();
    _setTargetAngle();
    // Regular edge octilinearty
    BGL_FORALL_EDGES( edge, g, Graph )
    {
        VertexDescriptor vdS = source( edge, g );
        VertexDescriptor vdT = target( edge, g );
        Coord2 vi, vj;
        if( vertexID[ vdS ] < vertexID[ vdT ] ){
            vi = vertexSmooth[ vdS ];
            vj = vertexSmooth[ vdT ];
        }
        else{
            vi = vertexSmooth[ vdT ];
            vj = vertexSmooth[ vdS ];
        }
        Coord2 vji = vi - vj;
#ifdef  DEBUG
        cerr << "vji = " << vji;
#endif  // DEBUG
        double angle = edgeCurAngle[ edge ];
        double theta = edgeTarget[ edge ] - angle;
#ifdef  DEBUG
        cerr << "eid = " << edgeID[ edge ] << " ";
#endif  // DEBUG
        double cosTheta = cos( theta ), sinTheta = sin( theta );
        //double s = 1.0;
        double s = _d_Beta * edgeWeight[ edge ] / vji.norm();
        //double s = edgeWeight[ edge ] / vji.norm();

        // x
        _output( nRows, 0 ) = _w_octilinear * s * ( cosTheta * vji.x() - sinTheta * vji.y() );
        nRows++;

        // y
        _output( nRows, 0 ) = _w_octilinear * s * ( sinTheta * vji.x() + cosTheta * vji.y() );
        nRows++;
    }


    // Positional constraints
    BGL_FORALL_VERTICES( vertex, g, Graph ){

        _output( nRows, 0 ) = _w_position * vertexSmooth[ vertex ].x();
        nRows++;
        _output( nRows, 0 ) = _w_position * vertexSmooth[ vertex ].y();
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
    Graph               & g             = _metro->g();
    unsigned int        nVertices       = _metro->nStations();
    unsigned int        nVE             = 0;
    unsigned int        nB              = 0;
    vector< double >    ratioR          = _metro->ratioR();

    VertexIDMap         vertexID        = get( vertex_myid, g );
    VertexCoordMap      vertexCoord     = get( vertex_mycoord, g );
    VertexIsStationMap  vertexIsStation = get( vertex_myisstation, g );
    //VertexExternalMap   vertexExternal  = get( vertex_myexternal, g );
    EdgeIDMap           edgeID          = get( edge_myid, g );

    Eigen::MatrixXd     oldCoef;
    oldCoef = _coef.block( 0, 0, _nConstrs, _nVars );

#ifdef OCTILINEAR_CONFLICT
    nVE = _metro->VEconflict().size();
#endif // OCTILINEAR_CONFLICT
#ifdef OCTILINEAR_BOUNDARY
    BGL_FORALL_VERTICES( vd, g, Graph )
    {
        double minD = _d_Beta/2.0;
        //if( vertexIsStation[ vd ] == false )
        //    minD = vertexExternal[ vd ].leaderWeight() * _d_Beta/2.0;
        if ( vertexCoord[ vd ].x() <= -( _half_width - minD ) ) nB++;
        if ( vertexCoord[ vd ].x() >= ( _half_width - minD ) ) nB++;
        if ( vertexCoord[ vd ].y() <= -( _half_height - minD ) ) nB++;
        if ( vertexCoord[ vd ].y() >= ( _half_height - minD ) ) nB++;
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
    BGL_FORALL_VERTICES( vd, g, Graph ){

        unsigned int id = vertexID[ vd ];
        double minD = _d_Beta/2.0;
        //if( vertexIsStation[ vd ] == false )
        //    minD = vertexExternal[ vd ].leaderWeight() * _d_Beta/2.0;

        if ( vertexCoord[ vd ].x() <= -( _half_width - minD ) ) {
            _coef( nRows, id ) = _w_boundary;
            nRows++;
        }
        if ( vertexCoord[ vd ].x() >= ( _half_width - minD ) ) {
            _coef( nRows, id ) = _w_boundary;
            nRows++;
        }
        if ( vertexCoord[ vd ].y() <= -( _half_height - minD ) ) {
            _coef( nRows, id + nVertices ) = _w_boundary;
            nRows++;
        }
        if ( vertexCoord[ vd ].y() >= ( _half_height - minD ) ) {
            _coef( nRows, id + nVertices ) = _w_boundary;
            nRows++;
        }
    }
#endif  // OCTILINEAR_BOUNDARY

#ifdef  OCTILINEAR_CONFLICT
    // copy conflict coefficient
    unsigned int countVE = 0;
    for ( VEMap::iterator it = _metro->VEconflict().begin();
          it != _metro->VEconflict().end(); ++it ) {
        VertexDescriptor vdV = it->second.first;
        EdgeDescriptor ed = it->second.second;
        VertexDescriptor vdS = source( ed, g );
        VertexDescriptor vdT = target( ed, g );
        unsigned int idV = vertexID[ vdV ];
        unsigned int idS = vertexID[ vdS ];
        unsigned int idT = vertexID[ vdT ];
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
    Graph               & g             = _metro->g();
    unsigned int        nVE             = 0;
    unsigned int        nB              = 0;
    vector< double >    ratioR          = _metro->ratioR();

    VertexIDMap             vertexID            = get( vertex_myid, g );
    //VertexSmoothMap     vertexSmooth    = get( vertex_mysmooth, g );
    VertexCoordMap          vertexCoord         = get( vertex_mycoord, g );
    VertexHomeMap           vertexHome          = get( vertex_myhome, g );
    VertexSelectMagMap      vertexSelectMag     = get( vertex_myselectmag, g );
    VertexIsStationMap      vertexIsStation     = get( vertex_myisstation, g );
    //VertexExternalMap       vertexExternal      = get( vertex_myexternal, g );
    EdgeIDMap               edgeID              = get( edge_myid, g );
    EdgeWeightMap           edgeWeight          = get( edge_weight, g );
    EdgeCurAngleMap         edgeCurAngle        = get( edge_mycurangle, g );
    EdgeTargetMap           edgeTarget          = get( edge_mytarget, g );

    unsigned int nRows = 0;
    Eigen::VectorXd     oldOutput;
    oldOutput = _output;
#ifdef  OCTILINEAR_CONFLICT
    nVE = _metro->VEconflict().size();
#endif  // OCTILINEAR_CONFLICT
#ifdef  OCTILINEAR_BOUNDARY
    BGL_FORALL_VERTICES( vd, g, Graph )
    {
        double minD = _d_Beta/2.0;
        //if( vertexIsStation[ vd ] == false )
        //    minD = vertexExternal[ vd ].leaderWeight() * _d_Beta/2.0;
        if ( vertexCoord[ vd ].x() <= -( _half_width - minD ) ) nB++;
        if ( vertexCoord[ vd ].x() >= ( _half_width - minD ) ) nB++;
        if ( vertexCoord[ vd ].y() <= -( _half_height - minD ) ) nB++;
        if ( vertexCoord[ vd ].y() >= ( _half_height - minD ) ) nB++;
    }
#endif  // OCTILINEAR_BOUNDARY
    _output.resize( _nConstrs + nB + 2*nVE );
    _output << Eigen::VectorXd::Zero( _nConstrs + nB + 2*nVE );

    _updateEdgeCurAngle();
    //_setTargetAngle();
    // Regular edge length
    BGL_FORALL_EDGES( edge, g, Graph )
    {
        VertexDescriptor vdS = source( edge, g );
        VertexDescriptor vdT = target( edge, g );
        //Coord2 vi = vertexSmooth[ vdS ];
        //Coord2 vj = vertexSmooth[ vdT ];
        Coord2 vi, vj;
        if( vertexID[ vdS ] < vertexID[ vdT ] ){
            vi = vertexCoord[ vdS ];
            vj = vertexCoord[ vdT ];
        }
        else{
            vi = vertexCoord[ vdT ];
            vj = vertexCoord[ vdS ];
        }
        Coord2 vji = vi - vj;
        double angle = edgeCurAngle[ edge ];
        double theta = edgeTarget[ edge ] - angle;
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
    BGL_FORALL_VERTICES( vertex, g, Graph ){

        // x
        _output( nRows, 0 ) = oldOutput( nRows, 0 );
        nRows++;

        // y
        _output( nRows, 0 ) = oldOutput( nRows, 0 );
        nRows++;
    }

#ifdef  OCTILINEAR_BOUNDARY
    // boundary constraints
    BGL_FORALL_VERTICES( vd, g, Graph ){

        double minD = _d_Beta/2.0;
        //if( vertexIsStation[ vd ] == false )
        //    minD = vertexExternal[ vd ].leaderWeight() * _d_Beta/2.0;

        if ( vertexCoord[ vd ].x() <= -( _half_width - minD ) ) {
            _output( nRows, 0 ) = _w_boundary * -( _half_width - minD );
            nRows++;
        }
        if ( vertexCoord[ vd ].x() >= ( _half_width - minD ) ) {
            _output( nRows, 0 ) = _w_boundary * ( _half_width - minD );
            nRows++;
        }
        if ( vertexCoord[ vd ].y() <= -( _half_height - minD ) ) {
            _output( nRows, 0 ) = _w_boundary * -( _half_height - minD );
            nRows++;
        }
        if ( vertexCoord[ vd ].y() >= ( _half_height - minD ) ) {
            _output( nRows, 0 ) = _w_boundary * ( _half_height - minD );
            nRows++;
        }
    }
#endif  // OCTILINEAR_BOUNDARY

#ifdef  OCTILINEAR_CONFLICT
    // copy conflict coefficient
    unsigned int countVE = 0;
    for ( VEMap::iterator it = _metro->VEconflict().begin();
          it != _metro->VEconflict().end(); ++it ) {
        VertexDescriptor vdV = it->second.first;
        EdgeDescriptor ed = it->second.second;
        VertexDescriptor vdS = source( ed, g );
        VertexDescriptor vdT = target( ed, g );
        unsigned int idV = vertexID[ vdV ];
        unsigned int idS = vertexID[ vdS ];
        unsigned int idT = vertexID[ vdT ];
        double r = ratioR[ countVE ];

        Coord2 v = vertexHome[ vdV ];
        Coord2 p = r * vertexHome[ vdS ] + ( 1.0-r ) * vertexHome[ vdT ];
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
    Graph        & g            = _metro->g();
    unsigned int nVertices      = _metro->nStations();

    VertexCoordMap      vertexCoord     = get( vertex_mycoord, g );

    // find the vertex that is too close to an edge
    vector< VertexDescriptor > vdVec;
    for ( VEMap::iterator it = _metro->VEconflict().begin();
          it != _metro->VEconflict().end(); ++it ) {
        VertexDescriptor vdV = it->second.first;
        vdVec.push_back( vdV );
    }

    unsigned int nRows = 0;
    // update coordinates
    // but freeze the vertex that is too close to an edge
    BGL_FORALL_VERTICES( vertex, g, Graph ){

        bool doClose = false;
        for( unsigned int i = 0; i < vdVec.size(); i++ ){
            if( vertex == vdVec[i] ) doClose = true;
        }

        //if( doClose ){
        if( _metro->VEconflict().size() > 0 ){
            Coord2 downscale;
            downscale.x() = ( _var( nRows, 0 ) - vertexCoord[ vertex ].x() )/2.0 + vertexCoord[ vertex ].x();
            downscale.y() = ( _var( nRows + nVertices, 0 ) - vertexCoord[ vertex ].y() )/2.0 + vertexCoord[ vertex ].y();
            vertexCoord[ vertex ].x() = downscale.x();
            vertexCoord[ vertex ].y() = downscale.y();
        }
        else{
            vertexCoord[ vertex ].x() = _var( nRows, 0 );
            vertexCoord[ vertex ].y() = _var( nRows + nVertices, 0 );
        }
        nRows++;
    }

    // check possible conflict
    _metro->checkVEConflicts();

#ifdef  DEBUG
    cerr << "retrieve:" << endl;
    BGL_FORALL_VERTICES( vertex, g, Graph ){
        cerr << "V(" << vertexID[ vertex ] << ") = " << vertexSmooth[ vertex ];
    }
    BGL_FORALL_EDGES( edge, g, Graph ){
        cerr << "E(" << edgeID[ edge ] << ") : smoAngle= " << edgeSmoAngle[ edge ] << endl;
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

