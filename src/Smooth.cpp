// Smooth.cpp
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

#include "Smooth.h"

//------------------------------------------------------------------------------
//	Protected functions
//------------------------------------------------------------------------------
//
//  Smooth::_init --        initialize the constrained optimization problem
//
//  Inputs
//      __metro : pointer to metro
//
//  Outputs
//      none
//
void Smooth::_init( Metro * __metro, double __half_width, double __half_height )
{
    _metro                      = __metro;
    Graph        & g            = _metro->g();
    unsigned int nVertices      = _metro->nStations();
    unsigned int nEdges         = _metro->nEdges();
    _d_Alpha                    = _metro->dAlpha();
    _d_Beta                     = _metro->dBeta();

    VertexIDMap         vertexID        = get( vertex_myid, g );
    VertexGeoMap        vertexGeo       = get( vertex_mygeo, g );
    EdgeIDMap           edgeID          = get( edge_myid, g );
    EdgeWeightMap       edgeWeight      = get( edge_weight, g );
    EdgeIsLineMap       edgeIsLine      = get( edge_myisline, g );

    // initialization
    _nVars = _nConstrs = 0;
    _w_focuslength      = sqrt(  5.0  ); // <- 5.0
    _w_contextlength    = sqrt(  2.5  ); // <- 5.0
    _w_angle        = sqrt(  1.0  ); // <- 1.0
    _w_position     = sqrt(  0.025 ); // <- 0.05
    _w_boundary     = sqrt( 20.0  ); // <- 15.0
    _w_crossing     = sqrt( 60.0  ); // <- 15.0
    //_w_labelangle   = sqrt( 20.0  ); // <- 1.0
    _half_width = __half_width;
    _half_height = __half_height;

#ifdef  DEBUG
    cout << " smooth: numStations = " << nVertices << " num_vertices = " << num_vertices( g ) << endl;
    cout << " smooth: numEdges = " << nEdges << " num_edges = " << num_edges( g ) << endl;
    cerr << "nAlpha = " << nAlpha << " nBeta = " << nBeta << " nLabels = " << nLabels << " nEdges = " << nEdges << endl;
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

    // Maximal angles of incident edges
    BGL_FORALL_VERTICES( vertex, g, Graph )
    {
        DegreeSizeType degrees = out_degree( vertex, g );
        if( degrees > 1 ) _nConstrs += 2 * degrees;
    }
    //cerr << "degreeConstrs = " << _nConstrs << endl;

    // Positional constraints
    _nConstrs += 2 * nVertices;

    _initVars();
    _initCoefs();
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
//  Smooth::_initCoefs --        initialize the coefficient
//
//  Inputs
//      none
//
//  Outputs
//      none
//
void Smooth::_initCoefs( void )
{
    Graph        & g            = _metro->g();
    unsigned int nVertices      = _metro->nStations();

    VertexIDMap         vertexID        = get( vertex_myid, g );
    VertexNameMap       vertexName      = get( vertex_myname, g );
    VertexGeoMap        vertexGeo       = get( vertex_mygeo, g );
    VertexCoordMap      vertexCoord     = get( vertex_mycoord, g );
    VertexIsStationMap  vertexIsStation = get( vertex_myisstation, g );
    VertexSelectMagMap  vertexSelectMag = get( vertex_myselectmag, g );
    EdgeIDMap           edgeID          = get( edge_myid, g );
    EdgeGeoAngleMap     edgeGeoAngle    = get( edge_mygeoangle, g );
    EdgeSmoAngleMap     edgeSmoAngle    = get( edge_mysmoangle, g );
    EdgeIsLeaderMap     edgeIsLeader    = get( edge_myisleader, g );

    //cerr<< "nVertices = " << nVertices << endl;

    // initialization
    unsigned int nRows = 0;
    _coef.resize( _nConstrs, _nVars );
    _coef << Eigen::MatrixXd::Zero( _nConstrs, _nVars );

    // Regular edge length
    BGL_FORALL_EDGES( edge, g, Graph )
    {
        VertexDescriptor vdS = source( edge, g );
        VertexDescriptor vdT = target( edge, g );
        unsigned int idS = vertexID[ vdS ];
        unsigned int idT = vertexID[ vdT ];

        double wL = _w_contextlength;
        //if( edgeIsLeader[ edge ] == true ) wL = _w_leaderlength;
        if( vertexSelectMag[ vdS ] == true && vertexSelectMag[ vdT ] == true ) wL = _w_focuslength;

        // x
        _coef( nRows, idS ) = wL; 
        _coef( nRows, idT ) = -wL;
        nRows++;        
        
        // y
        _coef( nRows, idS + nVertices ) = wL;
        _coef( nRows, idT + nVertices ) = -wL;
        nRows++;
    }

    // Maximal angles of incident edges
    BGL_FORALL_VERTICES( vertex, g, Graph ){

        DegreeSizeType degrees = out_degree( vertex, g );
#ifdef  DEBUG
        cerr << "V(" << vertexID[ vertex ] << ") degrees = " << degrees << endl;
#endif  // DEBUG
    
        if( degrees > 1 ){

            // sort the embedding
            map< double, EdgeDescriptor > circM;
            OutEdgeIterator e, e_end;
            for ( tie( e, e_end ) = out_edges( vertex, g ); e != e_end; ++e ) {
                EdgeDescriptor ed = *e;
                VertexDescriptor vS = source( ed, g );
                VertexDescriptor vT = target( ed, g );
                double angle = edgeGeoAngle[ ed ];

                if ( vertexID[ vS ] > vertexID[ vT ] ) {
                    if ( angle > 0 ) angle = -M_PI + edgeGeoAngle[ ed ];
                    else angle = M_PI + edgeGeoAngle[ ed ];
                }
#ifdef  DEBUG
                cerr << vertexID[ vS ] << endl;
                cerr << vertexID[ vT ] << ", " 
                     << vertexCoord[ vT ] << ", " 
                     << vertexGeo[ vT ] << endl;
                cerr << "EIA = " << edgeID[ ed ] << setw(10) << " angle = " << angle << endl;
#endif  // DEBUG
                circM.insert( pair< double, EdgeDescriptor >( angle, ed ) );
            }
#ifdef  DEBUG
            cerr << "seq: ";
            for ( map< double, EdgeDescriptor >::iterator it = circM.begin();
                  it != circM.end(); it++ ) {
                EdgeDescriptor ed = it->second;
                VertexDescriptor vS = source( ed, g );
                VertexDescriptor vT = target( ed, g );
                cerr << " " << vertexID[ vT ];
            }
            cerr << endl;
#endif  // DEBUG


            // set coefficient
            double tanTheta = tan( (double)( degrees - 2 ) * M_PI / 2.0 / (double) degrees );
#ifdef  DEBUG
            if( vertexIsStation[ vertex ] != true )
                cerr << "id = " << vertexID[ vertex ] << " degrees = " << degrees << " tanTheta = " << tanTheta << endl;
#endif  // DEBUG
        
            map< double, EdgeDescriptor >::iterator itN = circM.begin();
            itN++;
            for ( map< double, EdgeDescriptor >::iterator it = circM.begin();
                  it != circM.end(); it++ ) {

                EdgeDescriptor edC = it->second;
                EdgeDescriptor edN = itN->second;
                VertexDescriptor vS = source( edC, g );
                VertexDescriptor vTC = target( edC, g );
                VertexDescriptor vTN = target( edN, g );
                unsigned int idS = vertexID[ vS ];
                unsigned int idTC = vertexID[ vTC ];
                unsigned int idTN = vertexID[ vTN ];

                // x
                _coef( nRows, idS ) = _w_angle;   
                _coef( nRows, idTC ) = -0.5 * _w_angle;   
                _coef( nRows, idTN ) = -0.5 * _w_angle;   
                _coef( nRows, idTC + nVertices ) = -0.5 * _w_angle * tanTheta;   
                _coef( nRows, idTN + nVertices ) =  0.5 * _w_angle * tanTheta;   
                nRows++;

                // y
                _coef( nRows, idS + nVertices ) = _w_angle;   
                _coef( nRows, idTC + nVertices ) = -0.5 * _w_angle;   
                _coef( nRows, idTN + nVertices ) = -0.5 * _w_angle;   
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

#ifdef  SKIP
    cerr << "after angle" << endl;
    cerr << _nConstrs << " == " << nRows + 2*nVertices << endl;
    cerr << " nRows = " << nRows << endl;
#endif  // SKIP

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
//  Smooth::_initVars --        initialize the variables
//
//  Inputs
//      none
//
//  Outputs
//      none
//
void Smooth::_initVars( void )
{
    Graph        & g            = _metro->g();
    unsigned int nVertices      = _metro->nStations();

    //VertexIDMap         vertexID        = get( vertex_myid, g );
    //VertexGeoMap            vertexGeo       = get( vertex_mygeo, g );
    VertexSmoothMap      vertexSmooth     = get( vertex_mysmooth, g );

    // initialization
    _var.resize( _nVars );

    unsigned int nRows = 0;
    BGL_FORALL_VERTICES( vertex, g, Graph ){

        _var( nRows, 0 ) = vertexSmooth[ vertex ].x();
        _var( nRows + nVertices, 0 ) = vertexSmooth[ vertex ].y();
        nRows++;
    }

    assert( _nVars == 2*nRows );
#ifdef  DEBUG
    cerr << "_initvar:" << endl;
    cerr << _var << endl; 
#endif  // DEBUG
}

//
//  Smooth::_initOutputs --        initialize the output
//
//  Inputs
//      none
//
//  Outputs
//      none
//
void Smooth::_initOutputs( void )
{
    Graph        & g            = _metro->g();

    //VertexIDMap         vertexID        = get( vertex_myid, g );
    //VertexSmoothMap      vertexSmooth     = get( vertex_mysmooth, g );
    VertexGeoMap            vertexGeo       = get( vertex_mygeo, g );
    VertexSelectMagMap      vertexSelectMag     = get( vertex_myselectmag, g );
    EdgeIDMap               edgeID              = get( edge_myid, g );
    EdgeWeightMap           edgeWeight          = get( edge_weight, g );
    EdgeSelectCtrlMap       edgeSelectCtrl      = get( edge_myselectctrl, g );
    EdgeIsLeaderMap         edgeIsLeader        = get( edge_myisleader, g );

    // initialization
    unsigned int nRows = 0;
    _output.resize( _nConstrs );
    _output << Eigen::VectorXd::Zero( _nConstrs );

    // Regular edge length
    BGL_FORALL_EDGES( edge, g, Graph )
    {
        VertexDescriptor vdS = source( edge, g );
        VertexDescriptor vdT = target( edge, g );
        Coord2 vi = vertexGeo[ vdS ];
        Coord2 vj = vertexGeo[ vdT ];
        Coord2 vji = vi - vj;
        double s = _d_Beta * edgeWeight[ edge ] / vji.norm();
        double cosTheta = 1.0, sinTheta = 0.0;
        //double magnification = 1.0;
        //if( vertexSelectShift[ vdS ] == true || vertexSelectShift[ vdT ] == true ) magnification *= 2.0;
        //if( edgeSelectCtrl[ edge ] == true ) magnification = 2.0;

#ifdef  DEBUG
        cerr << "EID = " << edgeID[ edge ] << ", weight = " << edgeWeight[ edge ] << endl;
#endif  // DEBUG

        double wL = _w_contextlength;
        if( vertexSelectMag[ vdS ] == true && vertexSelectMag[ vdT ] == true ) wL = _w_focuslength;
        //double wL = _w_length;
        //if( edgeIsLeader[ edge ] == true ) wL = _w_leaderlength;

        // x
        _output( nRows, 0 ) = wL * s * ( cosTheta * vji.x() - sinTheta * vji.y() );
        nRows++;

        // y
        _output( nRows, 0 ) = wL * s * ( sinTheta * vji.x() + cosTheta * vji.y() );
        nRows++;
    }

    // Maximal angles of incident edges
    BGL_FORALL_VERTICES( vertex, g, Graph ){

        DegreeSizeType degrees = out_degree( vertex, g );

        if( degrees > 1 ){

            //double tanTheta = tan( (double)( degrees - 2 ) * M_PI / 2.0 / (double) degrees );
            OutEdgeIterator eo_cur, eo_nxt, eo_end;
            for( tie( eo_cur, eo_end ) = out_edges( vertex, g ); eo_cur != eo_end; ++eo_cur ){

                // x
                _output( nRows, 0 ) = 0.0;
                nRows++;

                // y
                _output( nRows, 0 ) = 0.0;
                nRows++;
            }
        }
    }

    // Positional constraints
    BGL_FORALL_VERTICES( vertex, g, Graph ){

        _output( nRows, 0 ) = _w_position * vertexGeo[ vertex ].x();
        nRows++;
        _output( nRows, 0 ) = _w_position * vertexGeo[ vertex ].y();
        nRows++;
    }

#ifdef  DEBUG
    cerr << "_initOutput:" << endl;
    cerr << _output << endl;
#endif  // DEBUG
}


//
//  Smooth::_updateCoefs --        update the coefs
//
//  Inputs
//      none
//
//  Outputs
//      none
//
void Smooth::_updateCoefs( void )
{
    Graph               & g             = _metro->g();
    unsigned int        nVertices       = _metro->nStations();
    unsigned int        nVE             = 0;
    unsigned int        nB              = 0;
    vector< double >    ratioR          = _metro->ratioR();

    VertexIDMap         vertexID        = get( vertex_myid, g );
    VertexSmoothMap     vertexSmooth    = get( vertex_mysmooth, g );
    VertexIsStationMap  vertexIsStation = get( vertex_myisstation, g );
    //VertexExternalMap   vertexExternal  = get( vertex_myexternal, g );
    EdgeIDMap           edgeID          = get( edge_myid, g );

    Eigen::MatrixXd     oldCoef;
    oldCoef = _coef.block( 0, 0, _nConstrs, _nVars );
#ifdef  SMOOTH_CONFLICT
    nVE = _metro->VEconflict().size();
#endif  // SMOOTH_CONFLICT
#ifdef  SMOOTH_BOUNDARY
    BGL_FORALL_VERTICES( vd, g, Graph ) 
    {
        double minD = _d_Beta/2.0;
        //if( vertexIsStation[ vd ] == false )
            //minD = vertexExternal[ vd ].leaderWeight() * _d_Beta/2.0;
        if ( vertexSmooth[ vd ].x() <= -( _half_width - minD ) ) nB++;
        if ( vertexSmooth[ vd ].x() >= ( _half_width - minD ) ) nB++;
        if ( vertexSmooth[ vd ].y() <= -( _half_height - minD ) ) nB++;
        if ( vertexSmooth[ vd ].y() >= ( _half_height - minD ) ) nB++;
    }
#endif  // SMOOTH_BOUNDARY
    _coef.resize( _nConstrs + nB + 2*nVE, _nVars );
    // _coef << Eigen::MatrixXd::Zero( _nConstrs + nB + 2*nVE, _nVars );

    // copy old coefficient
    _coef << oldCoef, Eigen::MatrixXd::Zero( nB + 2*nVE, _nVars );

    //if( nVE != 0 || nB != 0 )
    //    cerr << "nVE = " << nVE << " nB = " << nB << endl;

    unsigned int nRows = _nConstrs;

#ifdef  SMOOTH_BOUNDARY
    // add boundary coefficient
    BGL_FORALL_VERTICES( vd, g, Graph ){

        unsigned int id = vertexID[ vd ];
        double minD = _d_Beta/2.0;
        //if( vertexIsStation[ vd ] == false )
        //    minD = vertexExternal[ vd ].leaderWeight() * _d_Beta/2.0;

        if ( vertexSmooth[ vd ].x() <= -( _half_width - minD ) ) {
            _coef( nRows, id ) = _w_boundary;
            nRows++;
        }
        if ( vertexSmooth[ vd ].x() >= ( _half_width - minD ) ) {
            _coef( nRows, id ) = _w_boundary;
            nRows++;
        }
        if ( vertexSmooth[ vd ].y() <= -( _half_height - minD ) ) {
            _coef( nRows, id + nVertices ) = _w_boundary;
            nRows++;
        }
        if ( vertexSmooth[ vd ].y() >= ( _half_height - minD ) ) {
            _coef( nRows, id + nVertices ) = _w_boundary;
            nRows++;
        }
    }
#endif  // SMOOTH_BOUNDARY

    // cerr << "_coef.rows = " << _coef.rows() << endl;
    // cerr << "_coef.cols = " << _coef.cols() << endl;

#ifdef  SMOOTH_CONFLICT
    // add conflict coefficient
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
#endif  // SMOOTH_CONFLICT

#ifdef  DEBUG
    cerr << "###############" << endl;
    cerr << "newCoef:" << endl;
    cerr << _coef << endl;
    cerr << "###############" << endl;
#endif  // DEBUG
}

//
//  Smooth::_updateOutputs --        update the output
//
//  Inputs
//      none
//
//  Outputs
//      none
//
void Smooth::_updateOutputs( void )
{
    Graph               & g             = _metro->g();
    unsigned int        nVertices       = _metro->nStations();
    unsigned int        nVE             = 0;
    unsigned int        nB              = 0;
    vector< double >    ratioR          = _metro->ratioR();

    VertexIDMap             vertexID            = get( vertex_myid, g );
    VertexSmoothMap         vertexSmooth        = get( vertex_mysmooth, g );
    VertexGeoMap            vertexGeo           = get( vertex_mygeo, g );
    VertexSelectMagMap      vertexSelectMag     = get( vertex_myselectmag, g );
    VertexIsStationMap      vertexIsStation     = get( vertex_myisstation, g );
    //VertexExternalMap       vertexExternal      = get( vertex_myexternal, g );
    EdgeIDMap               edgeID              = get( edge_myid, g );
    EdgeWeightMap           edgeWeight          = get( edge_weight, g );
    EdgeSelectCtrlMap       edgeSelectCtrl      = get( edge_myselectctrl, g );
    EdgeIsLeaderMap         edgeIsLeader        = get( edge_myisleader, g );

    unsigned int nRows = 0;
    Eigen::VectorXd     oldOutput;
    oldOutput = _output;
#ifdef  SMOOTH_CONFLICT
    nVE = _metro->VEconflict().size();
#endif  // SMOOTH_CONFLICT
#ifdef  SMOOTH_BOUNDARY
    BGL_FORALL_VERTICES( vd, g, Graph ) 
    {
        double minD = _d_Beta/2.0;
        //if( vertexIsStation[ vd ] == false )
        //    minD = vertexExternal[ vd ].leaderWeight() * _d_Beta/2.0;
        if ( vertexSmooth[ vd ].x() <= -( _half_width - minD ) ) nB++;
        if ( vertexSmooth[ vd ].x() >= ( _half_width - minD ) ) nB++;
        if ( vertexSmooth[ vd ].y() <= -( _half_height - minD ) ) nB++;
        if ( vertexSmooth[ vd ].y() >= ( _half_height - minD ) ) nB++;
    }
#endif  // SMOOTH_BOUNDARY
    _output.resize( _nConstrs + nB + 2*nVE );
    _output << Eigen::VectorXd::Zero( _nConstrs + nB + 2*nVE );
    //cerr << "_output.rows = " << _output.rows() << endl;

    // Regular edge length
    BGL_FORALL_EDGES( edge, g, Graph )
    {
        VertexDescriptor vdS = source( edge, g );
        VertexDescriptor vdT = target( edge, g );
        Coord2 vi = vertexGeo[ vdS ];
        Coord2 vj = vertexGeo[ vdT ];
        Coord2 vji = vi - vj;
        Coord2 pi = vertexSmooth[ vdS ];
        Coord2 pj = vertexSmooth[ vdT ];
        Coord2 pji = pi - pj;
        double s = _d_Beta * edgeWeight[ edge ] / vji.norm();
        double angleV = atan2( vji.y(), vji.x() );
        double angleP = atan2( pji.y(), pji.x() );
        double diffTheta = angleP - angleV;
        double cosTheta = cos( diffTheta ); 
        double sinTheta = sin( diffTheta );
#ifdef  DEBUG
        cerr << "idS = " << vertexID[ vdS ] << " idT = " << vertexID[ vdT ] << endl;
        cerr << "cosTheta = " << cosTheta << " sinTheta = " << sinTheta << endl;
        cerr << "angleP = " << angleP << " angleV = " << angleV << endl;
        cerr << "EID = " << edgeID[edge] << " angleP = " << angleP << " angleV = " << angleV << " diffTheta = " << diffTheta << endl;
        if( vertexID[ vdS ] == 3 && vertexID[ vdT ] == 15 ) 
            cerr << "( " << vertexID[ vdS ] << ", " << vertexID[ vdT ] << ") angleP = " << angleP << " angleV = " << angleV << " diffTheta = " << diffTheta << endl;
#endif  // DEBUG

        //double magnification = 1.0;
        //if( vertexSelectShift[ vdS ] == true || vertexSelectShift[ vdT ] == true ) magnification *= 2.0;
        //if( edgeSelectCtrl[ edge ] == true ) magnification = 2.0;

        double wL = _w_contextlength;
        if( vertexSelectMag[ vdS ] == true && vertexSelectMag[ vdT ] == true ) wL = _w_focuslength;
        //double wL = _w_length;
        //if( edgeIsLeader[ edge ] == true ) wL = _w_leaderlength;

        // x
        _output( nRows, 0 ) = wL * s * ( cosTheta * vji.x() - sinTheta * vji.y() );
        nRows++;

        // y
        _output( nRows, 0 ) = wL * s * ( sinTheta * vji.x() + cosTheta * vji.y() );
        nRows++;
    }


    // Maximal angles of incident edges
    BGL_FORALL_VERTICES( vertex, g, Graph ){

        DegreeSizeType degrees = out_degree( vertex, g );

        if( degrees > 1 ){

            double tanTheta = tan( (double)( degrees - 2 ) * M_PI / 2.0 / (double) degrees );
            OutEdgeIterator eo_cur, eo_nxt, eo_end;
            for( tie( eo_cur, eo_end ) = out_edges( vertex, g ); eo_cur != eo_end; ++eo_cur ){

                // x
                _output( nRows, 0 ) = oldOutput( nRows, 0 );
                nRows++;

                // y
                _output( nRows, 0 ) = oldOutput( nRows, 0 );
                nRows++;
            }
        }
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

#ifdef  SMOOTH_BOUNDARY
    // boundary constraints
    BGL_FORALL_VERTICES( vd, g, Graph ){

        double minD = _d_Beta/2.0;
        //if( vertexIsStation[ vd ] == false )
        //    minD = vertexExternal[ vd ].leaderWeight() * _d_Beta/2.0;

        if ( vertexSmooth[ vd ].x() <= -( _half_width - minD ) ) {
            _output( nRows, 0 ) = _w_boundary * -( _half_width - minD );
            nRows++;
        }
        if ( vertexSmooth[ vd ].x() >= ( _half_width - minD ) ) {
            _output( nRows, 0 ) = _w_boundary * ( _half_width - minD );
            nRows++;
        }
        if ( vertexSmooth[ vd ].y() <= -( _half_height - minD ) ) {
            _output( nRows, 0 ) = _w_boundary * -( _half_height - minD );
            nRows++;
        }
        if ( vertexSmooth[ vd ].y() >= ( _half_height - minD ) ) {
            _output( nRows, 0 ) = _w_boundary * ( _half_height - minD );
            nRows++;
        }
    }
#endif  // SMOOTH_BOUNDARY

    // cerr << "nRows = " << nRows << endl;

#ifdef  SMOOTH_CONFLICT
    // conflict constraints
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

        Coord2 v = vertexGeo[ vdV ];
        Coord2 p = r * vertexGeo[ vdS ] + ( 1.0-r ) * vertexGeo[ vdT ];
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
#endif  // SMOOTH_CONFLICT

#ifdef  DEBUG
    cerr << "_updatedOutput:" << endl;
    cerr << _output << endl;
#endif  // DEBUG
}

//
//  Smooth::LeastSquare --        optimization
//
//  Inputs
//      none
//
//  Outputs
//      none
//
double Smooth::LeastSquare( unsigned int iter )
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
//  Smooth::ConjugateGradient --        optimization
//
//  Inputs
//      none
//
//  Outputs
//      none
//
double Smooth::ConjugateGradient( unsigned int iter )
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
//  Smooth::retrieve --        retrieve the result
//
//  Inputs
//      none
//
//  Outputs
//      none
//
void Smooth::retrieve( void )
{
    Graph        & g            = _metro->g();
    unsigned int nVertices      = _metro->nStations();

    VertexIDMap         vertexID        = get( vertex_myid, g );
    VertexCoordMap      vertexCoord     = get( vertex_mycoord, g );
    VertexSmoothMap     vertexSmooth    = get( vertex_mysmooth, g );
    VertexTempMap       vertexTemp      = get( vertex_mytemp, g );
    //VertexExternalMap   vertexExternal  = get( vertex_myexternal, g );
    VertexExtstateMap   vertexExtistate = get( vertex_myextstate, g );
    EdgeIDMap           edgeID          = get( edge_myid, g );
    EdgeGeoAngleMap     edgeGeoAngle    = get( edge_mygeoangle, g );
    EdgeSmoAngleMap     edgeSmoAngle    = get( edge_mysmoangle, g );
    EdgeCurAngleMap     edgeCurAngle    = get( edge_mycurangle, g );

    vector< vector< VertexDescriptor > > vdMatrix;
    // find the vertex that is too close to an edge
    for ( VEMap::iterator it = _metro->VEconflict().begin();
          it != _metro->VEconflict().end(); ++it ) {

        vector< VertexDescriptor > vdVec;
        VertexDescriptor vdV = it->second.first;
        EdgeDescriptor ed = it->second.second;
        VertexDescriptor vdS = source( ed, g );
        VertexDescriptor vdT = target( ed, g );
        vdVec.push_back( vdV );
        vdVec.push_back( vdS );
        vdVec.push_back( vdT );

        vdMatrix.push_back( vdVec );
        //cerr << "V = " << vertexID[ vdV ] << " S = " << vertexID[ vdS ] << " T = " << vertexID[ vdT ] << endl;
    }
    //cerr << endl;

    unsigned int nRows = 0;
    double scale = 1.0;
    // update coordinates
    // but freeze the vertex that is too close to an edge
    BGL_FORALL_VERTICES( vertex, g, Graph ){

        vector< int > rowVec;
        for( unsigned int i = 0; i < vdMatrix.size(); i++ ){
            if( vertex == vdMatrix[i][0] ) rowVec.push_back( i );
        }
        if( rowVec.size() > 0 ){

            Coord2 curCoord = vertexSmooth[ vertex ];
            Coord2 newCoord( _var( nRows, 0 ), _var( nRows + nVertices, 0 ) );
            for( unsigned int i = 0; i < rowVec.size(); i++ ){

                VertexDescriptor vdS = vdMatrix[ rowVec[i] ][ 1 ];
                VertexDescriptor vdT = vdMatrix[ rowVec[i] ][ 2 ];
                //cerr << "V = " << vertexID[ vertex ] << " S = " << vertexID[ vdS ] << " T = " << vertexID[ vdT ]
                //     << " dist = " << (newCoord - curCoord).norm() << endl;
                Coord2 coordS = vertexSmooth[ vdS ];
                Coord2 coordT = vertexSmooth[ vdT ];
                double m = ( coordS.y() - coordT.y() ) / ( coordS.x() - coordT.x() );
                double k = coordS.y() - m * coordS.x();
                double tmpS = 1.0;
                while( ( curCoord.y() - m * curCoord.x() - k ) * ( newCoord.y() - m * newCoord.x() - k ) < 0 ) {
                    //cerr << ( curCoord.y() - m * curCoord.x() - k ) << ", " << newCoord.y() - m * newCoord.x() - k << endl;
                    Coord2 diff = newCoord - curCoord;
                    //cerr << "diff = " << diff << "new = " << newCoord << "cur = " << curCoord << endl;
                    newCoord = diff/2.0 + curCoord;
                    tmpS = tmpS/2.0;
                    //cerr << vertexID[ vertex ] << " == ( " << vertexID[ vdS ] << ", " << vertexID[ vdT ] << " )" << endl;
                }
                if( tmpS < scale ) scale = tmpS;
            }
        }
    }
    //cerr << "scale = " << scale << endl;
    BGL_FORALL_VERTICES( vertex, g, Graph ){
        Coord2 curCoord = vertexSmooth[ vertex ];
        //vertexCoord[ vertex ].x() = vertexSmooth[ vertex ].x() = scale * ( _var( nRows, 0 ) - curCoord.x() ) + curCoord.x();
        //vertexCoord[ vertex ].y() = vertexSmooth[ vertex ].y() = scale * ( _var( nRows + nVertices, 0 ) - curCoord.y() ) + curCoord.y();
        vertexTemp[ vertex ].x() = vertexCoord[ vertex ].x();
        vertexTemp[ vertex ].y() = vertexCoord[ vertex ].y();
        vertexCoord[ vertex ].x() = vertexSmooth[ vertex ].x() = _var( nRows, 0 );
        vertexCoord[ vertex ].y() = vertexSmooth[ vertex ].y() = _var( nRows + nVertices, 0 );
        assert( vertexID[ vertex ] == nRows );
        nRows++;
    }

#ifdef  SKIP
    unsigned int nRows = 0;
    // update coordinates
    // but freeze the vertex that is too close to an edge
    BGL_FORALL_VERTICES( vertex, g, Graph ){

        vector< int > rowVec;
        for( unsigned int i = 0; i < vdMatrix.size(); i++ ){
            if( vertex == vdMatrix[i][0] ) rowVec.push_back( i );
        }

        if( rowVec.size() > 0 ){

            Coord2 curCoord = vertexSmooth[ vertex ];
            Coord2 newCoord( _var( nRows, 0 ), _var( nRows + nVertices, 0 ) );
            for( unsigned int i = 0; i < rowVec.size(); i++ ){

                VertexDescriptor vdS = vdMatrix[ rowVec[i] ][ 1 ];
                VertexDescriptor vdT = vdMatrix[ rowVec[i] ][ 2 ];
                //cerr << "V = " << vertexID[ vertex ] << " S = " << vertexID[ vdS ] << " T = " << vertexID[ vdT ] 
                //     << " dist = " << (newCoord - curCoord).norm() << endl;
                Coord2 coordS = vertexSmooth[ vdS ];
                Coord2 coordT = vertexSmooth[ vdT ];
                double m = ( coordS.y() - coordT.y() ) / ( coordS.x() - coordT.x() );
                double k = coordS.y() - m * coordS.x();
                while( ( curCoord.y() - m * curCoord.x() - k ) * ( newCoord.y() - m * newCoord.x() - k ) < 0 ) {
                    Coord2 diff = newCoord - curCoord;
                    newCoord = diff/2.0 + curCoord;
                    //cerr << "diff = " << diff << endl;
                    //cerr << vertexID[ vertex ] << " == " << vertexID[ vdMatrix[mRow][0] ] << endl;
                }
            }

            vertexCoord[ vertex ].x() = vertexSmooth[ vertex ].x() = newCoord.x();
            vertexCoord[ vertex ].y() = vertexSmooth[ vertex ].y() = newCoord.y();
        }
        else{
            vertexCoord[ vertex ].x() = vertexSmooth[ vertex ].x() = _var( nRows, 0 );
            vertexCoord[ vertex ].y() = vertexSmooth[ vertex ].y() = _var( nRows + nVertices, 0 );
        }
        assert( vertexID[ vertex ] == nRows );
        nRows++;
    }
#endif  // SKIP
    // update smooth angle
    BGL_FORALL_EDGES( edge, g, Graph ){

        VertexDescriptor vdS = source( edge, g );
        VertexDescriptor vdT = target( edge, g );

        Coord2 coordO;
        Coord2 coordD;
        if( vertexID[ vdS ] < vertexID[ vdT ] ){
            coordO = vertexSmooth[ vdS ];
            coordD = vertexSmooth[ vdT ];
        }
        else{
            coordO = vertexSmooth[ vdT ];
            coordD = vertexSmooth[ vdS ];
        }
        double diffX = coordD.x() - coordO.x();
        double diffY = coordD.y() - coordO.y();
        double angle = atan2( diffY, diffX );

        edgeSmoAngle[ edge ] = angle;
        edgeCurAngle[ edge ] = angle;

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
//  Smooth::clear --        memory management
//
//  Inputs
//      none
//
//  Outputs
//      none
//
void Smooth::clear( void )
{
}

//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------

//
//  Smooth::Smooth -- default constructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
Smooth::Smooth( void )
{   
}

//
//  Smooth::Smooth -- copy constructor
//
//  Inputs
//  obj : object of this class
//
//  Outputs
//  none
//
Smooth::Smooth( const Smooth & obj )
{
}


//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------

//
//  Smooth::~Smooth --    destructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
Smooth::~Smooth( void )
{
}

// end of header file
// Do not add any stuff under this line.

