// Stress.cpp
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

#include "optimization/Stress.h"

//------------------------------------------------------------------------------
//	Protected functions
//------------------------------------------------------------------------------
//
//  Stress::_init --        initialize the constrained optimization problem
//
//  Inputs
//      __boundary : pointer to boundary
//
//  Outputs
//      none
//
void Stress::_init( ForceGraph     *__forceGraphPtr, double __half_width, double __half_height )
{
    _forceGraphPtr      = __forceGraphPtr;
    ForceGraph &g       = *_forceGraphPtr;
    _nVertices          = num_vertices( g );
    _nEdges             = num_edges( g );

    // initialization
    _nVars          = 0;
    _nConstrs       = 0;
    _half_width     = __half_width;
    _half_height    = __half_height;

    string configFilePath = "../configs/stress.conf";

    //read config file
    Base::Config conf( configFilePath );

    if ( conf.has( "w_contextlength" ) ){
        string paramContextlength = conf.gets( "w_contextlength" );
        _w_contextlength = sqrt( stringToDouble( paramContextlength ) );
    }

    if ( conf.has( "w_angle" ) ){
        string paramAngle = conf.gets( "w_angle" );
        _w_angle = sqrt( stringToDouble( paramAngle ) );
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

#ifdef  DEBUG
    cout << " stress: numStations = " << _nVertices << endl;
    cout << " stress: numEdges = " << _nEdges << endl;
    //cerr << "nAlpha = " << nAlpha << " nBeta = " << nBeta << " nLabels = " << nLabels << " nEdges = " << nEdges << endl;
    //cerr << "_d_Alpha = " << _d_Alpha << " _d_Beta = " << _d_Beta << endl;
    cerr << "_w_contextlength = " << _w_contextlength << endl
         << "_w_angle = " << _w_angle << endl
         << "_w_position = " << _w_position << endl
         << "_w_boundary = " << _w_boundary << endl
         << "_w_crossing = " << _w_crossing << endl;
#endif  // DEBUG

    // Maximal angles of incident edges
    _unit_length    = 0.0;
    BGL_FORALL_EDGES( ed, g, ForceGraph )
    {
        ForceGraph::vertex_descriptor vdS = source( ed, g );
        ForceGraph::vertex_descriptor vdT = target( ed, g );

        double length = ( *g[ vdS ].coordPtr - *g[ vdT ].coordPtr ).norm();
        _unit_length += length;
    }
    _unit_length = _unit_length / (double) num_edges( g );

//------------------------------------------------------------------------------
//      Total number of linear variables
//------------------------------------------------------------------------------
    _nVars = 2 * _nVertices;

//------------------------------------------------------------------------------
//      Total number of linear constraints
//------------------------------------------------------------------------------
    // Regular edge length
    _nConstrs += 2 * _nEdges;

#ifdef SKIP
    // Maximal angles of incident edges
    BGL_FORALL_VERTICES( vd, g, ForceGraph )
    {
        ForceGraph::degree_size_type degrees = out_degree( vd, g );
        if( degrees > 1 ) _nConstrs += 2 * degrees;
    }
    //cerr << "degreeConstrs = " << _nConstrs << endl;
#endif // SKIP

    // Positional constraints
    _nConstrs += 2 * _nVertices;

    _initVars();
    _initCoefs();
    _initOutputs();
    _updateCoefs();
    _updateOutputs();

#ifdef  DEBUG
    printGraph( g );
    cerr << " nVertices = " << nVertices << " nEdges = " << nEdges << endl;
    cerr << " nVars = " << _nVars << " nConstrs = " << _nConstrs << endl;
    cerr << "Finished initializing the linear system" << endl;
#endif  // DEBUG
}


//
//  Stress::_initCoefs --        initialize the coefficient
//
//  Inputs
//      none
//
//  Outputs
//      none
//
void Stress::_initCoefs( void )
{
    ForceGraph  & g            = *_forceGraphPtr;

    // initialization
    unsigned int nRows = 0;
    _coef.resize( _nConstrs, _nVars );
    _coef << Eigen::MatrixXd::Zero( _nConstrs, _nVars );

    // Regular edge length
    BGL_FORALL_EDGES( ed, g, ForceGraph )
    {
        ForceGraph::vertex_descriptor vdS = source( ed, g );
        ForceGraph::vertex_descriptor vdT = target( ed, g );
        unsigned int idS = g[ vdS ].id;
        unsigned int idT = g[ vdT ].id;

        double wL = _w_contextlength;

        // x
        _coef( nRows, idS ) = wL;
        _coef( nRows, idT ) = -wL;
        nRows++;

        // y
        _coef( nRows, idS + _nVertices ) = wL;
        _coef( nRows, idT + _nVertices ) = -wL;
        nRows++;
    }

#ifdef SKIP
    // Maximal angles of incident edges
    BGL_FORALL_VERTICES( vd, g, ForceGraph ){

        ForceGraph::degree_size_type degrees = out_degree( vd, g );
#ifdef  DEBUG
        cerr << "V(" << vertexID[ vd ] << ") degrees = " << degrees << endl;
#endif  // DEBUG

        if( degrees > 1 ){

            // sort the embedding
            map< double, ForceGraph::edge_descriptor > circM;
            ForceGraph::out_edge_iterator e, e_end;
            for ( tie( e, e_end ) = out_edges( vd, g ); e != e_end; ++e ) {
                ForceGraph::edge_descriptor ed = *e;
                ForceGraph::vertex_descriptor vS = source( ed, g );
                ForceGraph::vertex_descriptor vT = target( ed, g );
                double angle = g[ ed ].geoAngle;

                if ( g[ vS ].id > g[ vT ].id ) {
                    if ( angle > 0 ) angle = -M_PI + g[ ed ].geoAngle;
                    else angle = M_PI + g[ ed ].geoAngle;
                }
#ifdef  DEBUG
                cerr << vertexID[ vS ] << endl;
                cerr << vertexID[ vT ] << ", "
                     << vertexCoord[ vT ] << ", "
                     << vertexGeo[ vT ] << endl;
                cerr << "EIA = " << edgeID[ ed ] << setw(10) << " angle = " << angle << endl;
#endif  // DEBUG
                circM.insert( pair< double, ForceGraph::edge_descriptor >( angle, ed ) );
            }
#ifdef  DEBUG
            cerr << "seq: ";
            for ( map< double, ForceGraph::edge_descriptor >::iterator it = circM.begin();
                  it != circM.end(); it++ ) {
                ForceGraph::edge_descriptor ed = it->second;
                ForceGraph::vertex_descriptor vS = source( ed, g );
                ForceGraph::vertex_descriptor vT = target( ed, g );
                cerr << " " << vertexID[ vT ];
            }
            cerr << endl;
#endif // DEBUG

            // set coefficient
            double tanTheta = tan( (double)( degrees - 2 ) * M_PI / 2.0 / (double) degrees );
#ifdef  DEBUG
            if( vertexIsStation[ vd ] != true )
                cerr << "id = " << vertexID[ vd ] << " degrees = " << degrees << " tanTheta = " << tanTheta << endl;
#endif  // DEBUG

            map< double, ForceGraph::edge_descriptor >::iterator itN = circM.begin();
            itN++;
            for ( map< double, ForceGraph::edge_descriptor >::iterator it = circM.begin();
                  it != circM.end(); it++ ) {

                ForceGraph::edge_descriptor edC = it->second;
                ForceGraph::edge_descriptor edN = itN->second;
                ForceGraph::vertex_descriptor vS = source( edC, g );
                ForceGraph::vertex_descriptor vTC = target( edC, g );
                ForceGraph::vertex_descriptor vTN = target( edN, g );
                unsigned int idS = g[ vS ].id;
                unsigned int idTC = g[ vTC ].id;
                unsigned int idTN = g[ vTN ].id;

                // x
                _coef( nRows, idS ) = _w_angle;
                _coef( nRows, idTC ) = -0.5 * _w_angle;
                _coef( nRows, idTN ) = -0.5 * _w_angle;
                _coef( nRows, idTC + _nVertices ) = -0.5 * _w_angle * tanTheta;
                _coef( nRows, idTN + _nVertices ) =  0.5 * _w_angle * tanTheta;
                nRows++;

                // y
                _coef( nRows, idS + _nVertices ) = _w_angle;
                _coef( nRows, idTC + _nVertices ) = -0.5 * _w_angle;
                _coef( nRows, idTN + _nVertices ) = -0.5 * _w_angle;
                _coef( nRows, idTC ) =  0.5 * _w_angle * tanTheta;
                _coef( nRows, idTN ) = -0.5 * _w_angle * tanTheta;
                nRows++;

                itN++;
                if( itN == circM.end() ) {
                    itN = circM.begin();
                }
            }
        }
    }
#endif // SKIP

#ifdef  SKIP
    cerr << "after angle" << endl;
    cerr << _nConstrs << " == " << nRows + 2*nVertices << endl;
    cerr << " nRows = " << nRows << endl;
#endif  // SKIP

    // Positional constraints
    BGL_FORALL_VERTICES( vd, g, ForceGraph ){

        unsigned int id = g[ vd ].id;

        // x
        _coef( nRows, id ) = _w_position;
        nRows++;

        // y
        _coef( nRows, id + _nVertices ) = _w_position;
        nRows++;
    }

//#ifdef  DEBUG
    cerr << "_coef:" << endl;
    cerr << _coef << endl;
//vv#endif  // DEBUG
}


//
//  Stress::_initVars --        initialize the variables
//
//  Inputs
//      none
//
//  Outputs
//      none
//
void Stress::_initVars( void )
{
    ForceGraph & g           = *_forceGraphPtr;

    // initialization
    _var.resize( _nVars );

    unsigned int nRows = 0;
    BGL_FORALL_VERTICES( vd, g, ForceGraph ){

        _var( nRows, 0 )              = g[ vd ].coordPtr->x();
        _var( nRows + _nVertices, 0 ) = g[ vd ].coordPtr->y();
        nRows++;
    }

    assert( _nVars == 2*nRows );
//#ifdef  DEBUG
    cerr << "_initvar:" << endl;
    cerr << _var << endl;
//#endif  // DEBUG
}

//
//  Stress::_initOutputs --        initialize the output
//
//  Inputs
//      none
//
//  Outputs
//      none
//
void Stress::_initOutputs( void )
{
    ForceGraph & g           = *_forceGraphPtr;

    // initialization
    unsigned int nRows = 0;
    _output.resize( _nConstrs );
    _output << Eigen::VectorXd::Zero( _nConstrs );

    // Regular edge length
    BGL_FORALL_EDGES( ed, g, ForceGraph )
    {
        ForceGraph::vertex_descriptor vdS = source( ed, g );
        ForceGraph::vertex_descriptor vdT = target( ed, g );
        Coord2 vi = *g[ vdS ].coordPtr;
        Coord2 vj = *g[ vdT ].coordPtr;
        Coord2 vji = vi - vj;
        double s = _unit_length * g[ ed ].weight / vji.norm();
        double cosTheta = 1.0, sinTheta = 0.0;
        //double magnification = 1.0;
        //if( vertexSelectShift[ vdS ] == true || vertexSelectShift[ vdT ] == true ) magnification *= 2.0;
        //if( edgeSelectCtrl[ ed ] == true ) magnification = 2.0;

#ifdef  DEBUG
        cerr << "EID = " << edgeID[ ed ] << ", weight = " << edgeWeight[ ed ] << endl;
#endif  // DEBUG

        double wL = _w_contextlength;

        // x
        _output( nRows, 0 ) = wL * s * ( cosTheta * vji.x() - sinTheta * vji.y() );
        nRows++;

        // y
        _output( nRows, 0 ) = wL * s * ( sinTheta * vji.x() + cosTheta * vji.y() );
        nRows++;
    }

#ifdef SKIP
    // Maximal angles of incident edges
    BGL_FORALL_VERTICES( vd, g, ForceGraph ){

        ForceGraph::degree_size_type degrees = out_degree( vd, g );

        if( degrees > 1 ){

            //double tanTheta = tan( (double)( degrees - 2 ) * M_PI / 2.0 / (double) degrees );
            ForceGraph::out_edge_iterator eo_cur, eo_nxt, eo_end;
            for( tie( eo_cur, eo_end ) = out_edges( vd, g ); eo_cur != eo_end; ++eo_cur ){

                // x
                _output( nRows, 0 ) = 0.0;
                nRows++;

                // y
                _output( nRows, 0 ) = 0.0;
                nRows++;
            }
        }
    }
#endif // SKIP

    // Positional constraints
    BGL_FORALL_VERTICES( vd, g, ForceGraph ){

        _output( nRows, 0 ) = _w_position * g[ vd ].coordPtr->x();
        nRows++;
        _output( nRows, 0 ) = _w_position * g[ vd ].coordPtr->y();
        nRows++;
    }

//#ifdef  DEBUG
    cerr << "_initOutput:" << endl;
    cerr << _output << endl;
//#endif  // DEBUG
}


//
//  Stress::_updateCoefs --        update the coefs
//
//  Inputs
//      none
//
//  Outputs
//      none
//
void Stress::_updateCoefs( void )
{
    ForceGraph & g                  = *_forceGraphPtr;
    unsigned int        nVE         = 0;
    unsigned int        nB          = 0;
#ifdef SKIP
    vector< double >    ratioR      = _boundary->ratioR();
#endif // SKIP

    Eigen::MatrixXd     oldCoef;
    oldCoef = _coef.block( 0, 0, _nConstrs, _nVars );
#ifdef  STRESS_CONFLICT
    nVE = _boundary->VEconflict().size();
#endif  // STRESS_CONFLICT
#ifdef  STRESS_BOUNDARY
    BGL_FORALL_VERTICES( vd, g, ForceGraph )
    {
        double minD = 1.0/2.0;
        //if( vertexIsStation[ vd ] == false )
            //minD = vertexExternal[ vd ].leaderWeight() * _d_Beta/2.0;
        if ( g[ vd ].stressPtr->x() <= -( _half_width - minD ) ) nB++;
        if ( g[ vd ].stressPtr->x() >= ( _half_width - minD ) ) nB++;
        if ( g[ vd ].stressPtr->y() <= -( _half_height - minD ) ) nB++;
        if ( g[ vd ].stressPtr->y() >= ( _half_height - minD ) ) nB++;
    }
#endif  // STRESS_BOUNDARY
    _coef.resize( _nConstrs + nB + 2*nVE, _nVars );
    // _coef << Eigen::MatrixXd::Zero( _nConstrs + nB + 2*nVE, _nVars );

    // copy old coefficient
    _coef << oldCoef, Eigen::MatrixXd::Zero( nB + 2*nVE, _nVars );

    //if( nVE != 0 || nB != 0 )
    //    cerr << "nVE = " << nVE << " nB = " << nB << endl;

    unsigned int nRows = _nConstrs;

#ifdef  STRESS_BOUNDARY
    // add boundary coefficient
    BGL_FORALL_VERTICES( vd, g, ForceGraph ){

        unsigned int id = g[ vd ].id;
        double minD = _d_Beta/2.0;
        //if( vertexIsStation[ vd ] == false )
        //    minD = vertexExternal[ vd ].leaderWeight() * _d_Beta/2.0;

        if ( g[ vd ].stressPtr->x() <= -( _half_width - minD ) ) {
            _coef( nRows, id ) = _w_boundary;
            nRows++;
        }
        if ( g[ vd ].stressPtr->x() >= ( _half_width - minD ) ) {
            _coef( nRows, id ) = _w_boundary;
            nRows++;
        }
        if ( g[ vd ].stressPtr->y() <= -( _half_height - minD ) ) {
            _coef( nRows, id + nVertices ) = _w_boundary;
            nRows++;
        }
        if ( g[ vd ].stressPtr->y() >= ( _half_height - minD ) ) {
            _coef( nRows, id + nVertices ) = _w_boundary;
            nRows++;
        }
    }
#endif  // STRESS_BOUNDARY

    // cerr << "_coef.rows = " << _coef.rows() << endl;
    // cerr << "_coef.cols = " << _coef.cols() << endl;

#ifdef  STRESS_CONFLICT
    // add conflict coefficient
    unsigned int countVE = 0;
    for ( VEMap::iterator it = _boundary->VEconflict().begin();
          it != _boundary->VEconflict().end(); ++it ) {
        ForceGraph::vertex_descriptor vdV = it->second.first;
        ForceGraph::edge_descriptor ed = it->second.second;
        ForceGraph::vertex_descriptor vdS = source( ed, g );
        ForceGraph::vertex_descriptor vdT = target( ed, g );
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
#endif  // STRESS_CONFLICT

//#ifdef  DEBUG
    cerr << "###############" << endl;
    cerr << "newCoef:" << endl;
    cerr << _coef << endl;
    cerr << "###############" << endl;
//#endif  // DEBUG
}

//
//  Stress::_updateOutputs --        update the output
//
//  Inputs
//      none
//
//  Outputs
//      none
//
void Stress::_updateOutputs( void )
{
    ForceGraph          & g             = *_forceGraphPtr;
    unsigned int        nVE             = 0;
    unsigned int        nB              = 0;
#ifdef SKIP
    vector< double >    ratioR          = _boundary->ratioR();
#endif // SKIP

    unsigned int nRows = 0;
    Eigen::VectorXd     oldOutput;
    oldOutput = _output;
#ifdef  STRESS_CONFLICT
    nVE = _boundary->VEconflict().size();
#endif  // STRESS_CONFLICT
#ifdef  STRESS_BOUNDARY
    BGL_FORALL_VERTICES( vd, g, ForceGraph )
    {
        double minD = _d_Beta/2.0;
        //if( vertexIsStation[ vd ] == false )
        //    minD = vertexExternal[ vd ].leaderWeight() * _d_Beta/2.0;
        if ( g[ vd ].stressPtr->x() <= -( _half_width - minD ) ) nB++;
        if ( g[ vd ].stressPtr->x() >= ( _half_width - minD ) ) nB++;
        if ( g[ vd ].stressPtr->y() <= -( _half_height - minD ) ) nB++;
        if ( g[ vd ].stressPtr->y() >= ( _half_height - minD ) ) nB++;
    }
#endif  // STRESS_BOUNDARY
    _output.resize( _nConstrs + nB + 2*nVE );
    _output << Eigen::VectorXd::Zero( _nConstrs + nB + 2*nVE );
    //cerr << "_output.rows = " << _output.rows() << endl;

    // Regular edge length
    BGL_FORALL_EDGES( ed, g, ForceGraph )
    {
        ForceGraph::vertex_descriptor vdS = source( ed, g );
        ForceGraph::vertex_descriptor vdT = target( ed, g );
        Coord2 vi = *g[ vdS ].coordPtr;
        Coord2 vj = *g[ vdT ].coordPtr;
        Coord2 vji = vi - vj;
        Coord2 pi = *g[ vdS ].forcePtr;
        Coord2 pj = *g[ vdT ].forcePtr;
        Coord2 pji = pi - pj;
        double s = _unit_length * g[ ed ].weight / vji.norm();
        double angleV = atan2( vji.y(), vji.x() );
        double angleP = atan2( pji.y(), pji.x() );
        double diffTheta = angleP - angleV;
        double cosTheta = cos( diffTheta );
        double sinTheta = sin( diffTheta );
#ifdef  DEBUG
        cerr << "idS = " << vertexID[ vdS ] << " idT = " << vertexID[ vdT ] << endl;
        cerr << "cosTheta = " << cosTheta << " sinTheta = " << sinTheta << endl;
        cerr << "angleP = " << angleP << " angleV = " << angleV << endl;
        cerr << "EID = " << edgeID[ed] << " angleP = " << angleP << " angleV = " << angleV << " diffTheta = " << diffTheta << endl;
        if( vertexID[ vdS ] == 3 && vertexID[ vdT ] == 15 )
            cerr << "( " << vertexID[ vdS ] << ", " << vertexID[ vdT ] << ") angleP = " << angleP << " angleV = " << angleV << " diffTheta = " << diffTheta << endl;
#endif  // DEBUG

        //double magnification = 1.0;
        //if( vertexSelectShift[ vdS ] == true || vertexSelectShift[ vdT ] == true ) magnification *= 2.0;
        //if( edgeSelectCtrl[ ed ] == true ) magnification = 2.0;

        double wL = _w_contextlength;

        // x
        _output( nRows, 0 ) = wL * s * ( cosTheta * vji.x() - sinTheta * vji.y() );
        nRows++;

        // y
        _output( nRows, 0 ) = wL * s * ( sinTheta * vji.x() + cosTheta * vji.y() );
        nRows++;
    }

#ifdef SKIP
    // Maximal angles of incident edges
    BGL_FORALL_VERTICES( vd, g, ForceGraph ){

        ForceGraph::degree_size_type degrees = out_degree( vd, g );

        if( degrees > 1 ){

            double tanTheta = tan( (double)( degrees - 2 ) * M_PI / 2.0 / (double) degrees );
            ForceGraph::out_edge_iterator eo_cur, eo_nxt, eo_end;
            for( tie( eo_cur, eo_end ) = out_edges( vd, g ); eo_cur != eo_end; ++eo_cur ){

                // x
                _output( nRows, 0 ) = oldOutput( nRows, 0 );
                nRows++;

                // y
                _output( nRows, 0 ) = oldOutput( nRows, 0 );
                nRows++;
            }
        }
    }
#endif // SKIP

    // Positional constraints
    BGL_FORALL_VERTICES( vd, g, ForceGraph ){

        // x
        _output( nRows, 0 ) = oldOutput( nRows, 0 );
        nRows++;

        // y
        _output( nRows, 0 ) = oldOutput( nRows, 0 );
        nRows++;
    }

#ifdef  STRESS_BOUNDARY
    // boundary constraints
    BGL_FORALL_VERTICES( vd, g, ForceGraph ){

        double minD = _d_Beta/2.0;
        //if( vertexIsStation[ vd ] == false )
        //    minD = vertexExternal[ vd ].leaderWeight() * _d_Beta/2.0;

        if ( g[ vd ].stressPtr->x() <= -( _half_width - minD ) ) {
            _output( nRows, 0 ) = _w_boundary * -( _half_width - minD );
            nRows++;
        }
        if ( g[ vd ].stressPtr->x() >= ( _half_width - minD ) ) {
            _output( nRows, 0 ) = _w_boundary * ( _half_width - minD );
            nRows++;
        }
        if ( g[ vd ].stressPtr->y() <= -( _half_height - minD ) ) {
            _output( nRows, 0 ) = _w_boundary * -( _half_height - minD );
            nRows++;
        }
        if ( g[ vd ].stressPtr->y() >= ( _half_height - minD ) ) {
            _output( nRows, 0 ) = _w_boundary * ( _half_height - minD );
            nRows++;
        }
    }
#endif  // STRESS_BOUNDARY

    // cerr << "nRows = " << nRows << endl;

#ifdef  STRESS_CONFLICT
    // conflict constraints
    unsigned int countVE = 0;
    for ( VEMap::iterator it = _boundary->VEconflict().begin();
          it != _boundary->VEconflict().end(); ++it ) {
        ForceGraph::vertex_descriptor vdV = it->second.first;
        ForceGraph::edge_descriptor ed = it->second.second;
        ForceGraph::vertex_descriptor vdS = source( ed, g );
        ForceGraph::vertex_descriptor vdT = target( ed, g );
        unsigned int idV = g[ vdV ].id;
        unsigned int idS = g[ vdS ].id;
        unsigned int idT = g[ vdT ].id;
        double r = ratioR[ countVE ];

        Coord2 v = *g[ vdV ].geoPtr;
        Coord2 p = r * *g[ vdS ].geoPtr + ( 1.0-r ) * *g[ vdT ].geoPtr;
        double minD = _d_Beta;
        //if( vertexIsStation[ vdV ] == false )
        //    minD = vertexExternal[ vdV ].leaderWeight() * _d_Beta;
        double delta = minD / ( v - p ).norm();

        // x
        _output( nRows, 0 ) = _w_crossing * delta * ( v - p ).x();
        nRows++;

        // y
        _output( nRows, 0 ) = _w_crossing * delta * ( v - p ).y();
        nRows++;
    }
#endif  // STRESS_CONFLICT

#ifdef  DEBUG
    cerr << "_updatedOutput:" << endl;
    cerr << _output << endl;
#endif  // DEBUG
}

//
//  Stress::LeastSquare --        optimization
//
//  Inputs
//      none
//
//  Outputs
//      none
//
double Stress::LeastSquare( unsigned long iter )
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

#ifdef  DEBUG_TIME
        if( ( i+1 ) % 100 == 0 )
            cerr << setprecision( 10 ) << "Loop(" << i << ") mse = " << mse << endl;
        cerr << "time1 = " << time1 << " time2 = " << time2 << " time3 = " << time3 << " time4 = " << time4 << endl;
#endif  // DEBUG_TIME
        if( ( mse ) < 1.0e-1 ) break;
    }

#ifdef  DEBUG
    cerr << "_var:" << endl;
    cerr << _var << endl;
    cerr << "_var:" << endl;
    for( int i = 0; i < _var.rows; i++ ){
        cerr << setw( 10 ) << setprecision( 3 ) << _var.at< double >( i, 0 ) << " ";
        // if( i % 2 == 1 ) cerr << endl;
    }
    cerr << endl;
#endif  // DEBUG
    return mse;
}

//
//  Stress::ConjugateGradient --        optimization
//
//  Inputs
//      none
//
//  Outputs
//      none
//
double Stress::ConjugateGradient( unsigned long iter )
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
//  Stress::retrieve --        retrieve the result
//
//  Inputs
//      none
//
//  Outputs
//      none
//
void Stress::retrieve( void )
{
    ForceGraph        & g    = *_forceGraphPtr;
    vector< vector< ForceGraph::vertex_descriptor > > vdMatrix;

#ifdef SKIP
    // find the vertex that is too close to an edge
    for ( VEMap::iterator it = _boundary->VEconflict().begin();
          it != _boundary->VEconflict().end(); ++it ) {

        vector< ForceGraph::vertex_descriptor > vdVec;
        ForceGraph::vertex_descriptor vdV = it->second.first;
        ForceGraph::edge_descriptor ed = it->second.second;
        ForceGraph::vertex_descriptor vdS = source( ed, g );
        ForceGraph::vertex_descriptor vdT = target( ed, g );
        vdVec.push_back( vdV );
        vdVec.push_back( vdS );
        vdVec.push_back( vdT );

        vdMatrix.push_back( vdVec );
        //cerr << "V = " << vertexID[ vdV ] << " S = " << vertexID[ vdS ] << " T = " << vertexID[ vdT ] << endl;
    }
    //cerr << endl;
#endif // SKIP

    unsigned int nRows = 0;
    double scale = 1.0;
    // update coordinates
    // but freeze the vertex that is too close to an edge
    BGL_FORALL_VERTICES( vd, g, ForceGraph ){

        vector< int > rowVec;
        for( unsigned int i = 0; i < vdMatrix.size(); i++ ){
            if( vd == vdMatrix[i][0] ) rowVec.push_back( i );
        }
        if( rowVec.size() > 0 ){

            Coord2 curCoord = *g[ vd ].forcePtr;
            Coord2 newCoord( _var( nRows, 0 ), _var( nRows + _nVertices, 0 ) );
            for( unsigned int i = 0; i < rowVec.size(); i++ ){

                ForceGraph::vertex_descriptor vdS = vdMatrix[ rowVec[i] ][ 1 ];
                ForceGraph::vertex_descriptor vdT = vdMatrix[ rowVec[i] ][ 2 ];
                //cerr << "V = " << vertexID[ vd ] << " S = " << vertexID[ vdS ] << " T = " << vertexID[ vdT ]
                //     << " dist = " << (newCoord - curCoord).norm() << endl;
                Coord2 coordS = *g[ vdS ].forcePtr;
                Coord2 coordT = *g[ vdT ].forcePtr;
                double m = ( coordS.y() - coordT.y() ) / ( coordS.x() - coordT.x() );
                double k = coordS.y() - m * coordS.x();
                double tmpS = 1.0;
                while( ( curCoord.y() - m * curCoord.x() - k ) * ( newCoord.y() - m * newCoord.x() - k ) < 0 ) {
                    //cerr << ( curCoord.y() - m * curCoord.x() - k ) << ", " << newCoord.y() - m * newCoord.x() - k << endl;
                    Coord2 diff = newCoord - curCoord;
                    //cerr << "diff = " << diff << "new = " << newCoord << "cur = " << curCoord << endl;
                    newCoord = diff/2.0 + curCoord;
                    tmpS = tmpS/2.0;
                    //cerr << vertexID[ vd ] << " == ( " << vertexID[ vdS ] << ", " << vertexID[ vdT ] << " )" << endl;
                }
                if( tmpS < scale ) scale = tmpS;
            }
        }
    }
    //cerr << "scale = " << scale << endl;
    BGL_FORALL_VERTICES( vd, g, ForceGraph ){
        Coord2 curCoord = *g[ vd ].forcePtr;
        g[ vd ].coordPtr->x() = g[ vd ].forcePtr->x() = _var( nRows, 0 );
        g[ vd ].coordPtr->y() = g[ vd ].forcePtr->y() = _var( nRows + _nVertices, 0 );
        assert( g[ vd ].id == nRows );
        nRows++;
    }

#ifdef SKIP
    // update stress angle
    BGL_FORALL_EDGES( ed, g, ForceGraph ){

        ForceGraph::vertex_descriptor vdS = source( ed, g );
        ForceGraph::vertex_descriptor vdT = target( ed, g );

        Coord2 coordO;
        Coord2 coordD;
        if( g[ vdS ].id < g[ vdT ].id ){
            coordO = *g[ vdS ].stressPtr;
            coordD = *g[ vdT ].stressPtr;
        }
        else{
            coordO = *g[ vdT ].stressPtr;
            coordD = *g[ vdS ].stressPtr;
        }
        double diffX = coordD.x() - coordO.x();
        double diffY = coordD.y() - coordO.y();
        double angle = atan2( diffY, diffX );

        g[ ed ].stressAngle = angle;
        g[ ed ].angle = angle;
    }
#endif // SKIP

#ifdef SKIP
    // check possible conflict
    _boundary->checkVEConflicts();
#endif // SKIP

    cerr << "retrieve:" << endl;
    BGL_FORALL_VERTICES( vd, g, ForceGraph ){
        cerr << "V(" << g[ vd ].id << ") = " << *g[ vd ].coordPtr;
    }
#ifdef  DEBUG
    BGL_FORALL_EDGES( ed, g, ForceGraph ){
        cerr << "E(" << edgeID[ ed ] << ") : smoAngle= " << edgeSmoAngle[ ed ] << endl;
    }
#endif  // DEBUG
}


//
//  Stress::clear --        memory management
//
//  Inputs
//      none
//
//  Outputs
//      none
//
void Stress::clear( void )
{
}

//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------

//
//  Stress::Stress -- default constructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
Stress::Stress( void )
{   
}

//
//  Stress::Stress -- copy constructor
//
//  Inputs
//  obj : object of this class
//
//  Outputs
//  none
//
Stress::Stress( const Stress & obj )
{
}


//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------

//
//  Stress::~Stress --    destructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
Stress::~Stress( void )
{
}

// end of header file
// Do not add any stuff under this line.

