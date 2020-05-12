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
void Stress::_initStress( ForceGraph *forceGraphPtr,
                          Polygon2 *contourPtr ) {
	
	BoundaryGraph &g = _boundaryGraph;
	unsigned int nVertices = num_vertices( g );
	unsigned int nEdges = num_edges( g );
	
	// initialization
	_contourPtr = contourPtr;
	_contourPtr->updateCentroid();
	_contourPtr->computeBoundingBox();
	_half_width = 0.5 * _contourPtr->boundingBox().x();
	_half_height = 0.5 * _contourPtr->boundingBox().y();

#ifdef DEBUG
	cerr << "num_vertices( *forceGraphPtr ) = " << num_vertices( *forceGraphPtr ) << endl;
	cerr << "_contour = " << _contour;
	cerr << "_half_width = " << _half_width << endl;
	cerr << "_half_height = " << _half_height << endl;
	cerr << "_contourPtr->area() = " << _contourPtr->area() << endl;
#endif // DEBUG
	
	// copy the graph
	BGL_FORALL_VERTICES( vd, *forceGraphPtr, ForceGraph ) {
			
			BoundaryGraph::vertex_descriptor vdNew = add_vertex( g );
			g[ vdNew ].id = ( *forceGraphPtr )[ vd ].id;
			g[ vdNew ].weight = ( *forceGraphPtr )[ vd ].weight;
			g[ vdNew ].coordPtr = ( *forceGraphPtr )[ vd ].coordPtr;
			g[ vdNew ].geoPtr = new Coord2( g[ vdNew ].coordPtr->x(), g[ vdNew ].coordPtr->y() );
			g[ vdNew ].smoothPtr = new Coord2( g[ vdNew ].coordPtr->x(), g[ vdNew ].coordPtr->y() );
			g[ vdNew ].centroidPtr = new Coord2( g[ vdNew ].coordPtr->x(), g[ vdNew ].coordPtr->y() );
			//g[ vdNew ].stressPtr = new Coord2( g[ vdNew ].coordPtr->x(), g[ vdNew ].coordPtr->y() );
			//g[ vdNew ].octilinearPtr = new Coord2( g[ vdNew ].coordPtr->x(), g[ vdNew ].coordPtr->y() );
		}
	BGL_FORALL_EDGES( ed, *forceGraphPtr, ForceGraph ) {
			
			ForceGraph::vertex_descriptor vdFS = source( ed, *forceGraphPtr );
			ForceGraph::vertex_descriptor vdFT = target( ed, *forceGraphPtr );
			BoundaryGraph::vertex_descriptor vdBS = vertex( ( *forceGraphPtr )[ vdFS ].id, g );
			BoundaryGraph::vertex_descriptor vdBT = vertex( ( *forceGraphPtr )[ vdFT ].id, g );
			
			pair< BoundaryGraph::edge_descriptor, unsigned int > addE = add_edge( vdBS, vdBT, g );
			BoundaryGraph::edge_descriptor addED = addE.first;
			
			// calculate geographical angle
			Coord2 coordO;
			Coord2 coordD;
			if( g[ vdBS ].initID < g[ vdBT ].initID ) {
				coordO = *g[ vdBS ].coordPtr;
				coordD = *g[ vdBT ].coordPtr;
			}
			else {
				coordO = *g[ vdBT ].coordPtr;
				coordD = *g[ vdBS ].coordPtr;
			}
			double diffX = coordD.x() - coordO.x();
			double diffY = coordD.y() - coordO.y();
			double angle = atan2( diffY, diffX );
			
			g[ addED ].initID = g[ addED ].id = ( *forceGraphPtr )[ ed ].id;
			g[ addED ].weight = 1.0;
			g[ addED ].geoAngle = angle;
			g[ addED ].smoothAngle = angle;
			g[ addED ].targetAngle = 0;
			g[ addED ].angle = angle;
			g[ addED ].visitedTimes = 0;
		}
	
	// initialization
	double aspectRatio = _half_width / _half_height;
	_d_Alpha = sqrt( _contourPtr->area() / num_vertices( g ) );
	//_d_Alpha                    = _octilinearBoundaryVec->dAlpha();
	//_d_Beta                     = _octilinearBoundaryVec->dBeta();
	
	// initialization
	_nVars = _nConstrs = 0;
	
	string configFilePath = "config/" + Common::getBatchStr() + "/stress.conf";
	
	//read config file
	Base::Config conf( configFilePath );
	
	if( conf.has( "w_contextlength" ) ) {
		string paramContextlength = conf.gets( "w_contextlength" );
		_w_contextlength = sqrt( Common::stringToDouble( paramContextlength ) );
	}
	
	if( conf.has( "w_angle" ) ) {
		string paramAngle = conf.gets( "w_angle" );
		_w_angle = sqrt( Common::stringToDouble( paramAngle ) );
	}
	
	if( conf.has( "w_position" ) ) {
		string paramPosition = conf.gets( "w_position" );
		_w_position = sqrt( Common::stringToDouble( paramPosition ) );
	}
	
	if( conf.has( "w_boundary" ) ) {
		string paramBoundary = conf.gets( "w_boundary" );
		_w_boundary = sqrt( Common::stringToDouble( paramBoundary ) );
	}
	
	if( conf.has( "w_crossing" ) ) {
		string paramCrossing = conf.gets( "w_crossing" );
		_w_crossing = sqrt( Common::stringToDouble( paramCrossing ) );
	}
	
	if( conf.has( "opttype" ) ) {
		string paramType = conf.gets( "opttype" );
		if( paramType == "LEAST_SQUARE" )
			_paramOptType = LEAST_SQUARE;
		if( paramType == "CONJUGATE_GRADIENT" )
			_paramOptType = CONJUGATE_GRADIENT;
	}
	
	if( conf.has( "voronoi_frequency" ) ) {
		string paramVoronoiFreq = conf.gets( "voronoi_frequency" );
		_paramVoronoiFreq = stoi( paramVoronoiFreq );
	}
	
	if( conf.has( "ratio_position" ) ) {
		string paramRatioPosition = conf.gets( "ratio_position" );
		_paramRatioPosition = Common::stringToDouble( paramRatioPosition );
	}
	
	if( conf.has( "ratio_voronoi" ) ) {
		string paramRatioVoronoi = conf.gets( "ratio_voronoi" );
		_paramRatioVoronoi = Common::stringToDouble( paramRatioVoronoi );
	}

#ifdef  DEBUG
	cout << " Stress: numStations = " << nVertices << " num_vertices = " << num_vertices( g ) << endl;
	cout << " Stress: numEdges = " << nEdges << " num_edges = " << num_edges( g ) << endl;
	//cerr << "nAlpha = " << nAlpha << " nBeta = " << nBeta << " nLabels = " << nLabels << " nEdges = " << nEdges << endl;
	cerr << "_d_Alpha = " << _d_Alpha << endl;
	cerr << "_w_contextlength = " << _w_contextlength << endl
	     << "_w_angle = " << _w_angle << endl
	     << "_w_position = " << _w_position << endl
	     << "_w_boundary = " << _w_boundary << endl
	     << "_w_crossing = " << _w_crossing << endl;
	cerr << "_opttype = " << _paramOptType << endl
	     << "_voronoi_frequency = " << _paramVoronoiFreq << endl
	     << "_ratio_position = " << _paramRatioPosition << endl
	     << "_ratio_voronoi = " << _paramRatioVoronoi << endl;
	cerr << "_contourPtr->size() = " << _contourPtr->elements().size() << endl;
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
	BGL_FORALL_VERTICES( vertex, g, BoundaryGraph ) {
			BoundaryGraph::degree_size_type degrees = out_degree( vertex, g );
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
//  Stress::_initCoefs --        initialize the coefficient
//
//  Inputs
//      none
//
//  Outputs
//      none
//
void Stress::_initCoefs( void ) {
	
	BoundaryGraph &g = _boundaryGraph;
	unsigned int nVertices = num_vertices( g );
	
	//cerr<< "nVertices = " << nVertices << endl;
	
	// initialization
	unsigned int nRows = 0;
	_coef.resize( _nConstrs, _nVars );
	_coef << Eigen::MatrixXd::Zero( _nConstrs, _nVars );

#ifdef DEBUG
	printGraph( g );
#endif // DEBUG
	
	// Regular edge length
	BGL_FORALL_EDGES( edge, g, BoundaryGraph ) {
			BoundaryGraph::vertex_descriptor vdS = source( edge, g );
			BoundaryGraph::vertex_descriptor vdT = target( edge, g );
			unsigned int idS = g[ vdS ].id;
			unsigned int idT = g[ vdT ].id;
			
			double wL = _w_contextlength;
			
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
	BGL_FORALL_VERTICES( vertex, g, BoundaryGraph ) {
			
			BoundaryGraph::degree_size_type degrees = out_degree( vertex, g );
#ifdef  DEBUG
			cerr << "V(" << vertexID[ vertex ] << ") degrees = " << degrees << endl;
#endif  // DEBUG
			
			if( degrees > 1 ) {
				
				// sort the embedding
				map< double, BoundaryGraph::edge_descriptor > circM;
				BoundaryGraph::out_edge_iterator e, e_end;
				for( tie( e, e_end ) = out_edges( vertex, g ); e != e_end; ++e ) {
					BoundaryGraph::edge_descriptor ed = *e;
					BoundaryGraph::vertex_descriptor vS = source( ed, g );
					BoundaryGraph::vertex_descriptor vT = target( ed, g );
					double angle = g[ ed ].geoAngle;
					
					if( g[ vS ].id > g[ vT ].id ) {
						if( angle > 0 ) angle = -M_PI + g[ ed ].geoAngle;
						else angle = M_PI + g[ ed ].geoAngle;
					}
#ifdef  DEBUG
					cerr << g[ vS ].id << endl;
					cerr << g[ vT ].id << ", "
						 << *g[ vT ].coordPtr << *g[ vT ].geoPtr << endl;
					cerr << "EIA = " << g[ ed ].id << setw(10) << " angle = " << angle << endl;
#endif  // DEBUG
					circM.insert( pair< double, BoundaryGraph::edge_descriptor >( angle, ed ) );
				}
#ifdef  DEBUG
				cerr << "seq: ";
				for ( map< double, BoundaryGraph::edge_descriptor >::iterator it = circM.begin();
					  it != circM.end(); it++ ) {
					BoundaryGraph::edge_descriptor ed = it->second;
					BoundaryGraph::vertex_descriptor vS = source( ed, g );
					BoundaryGraph::vertex_descriptor vT = target( ed, g );
					cerr << " " << vertexID[ vT ];
				}
				cerr << endl;
#endif  // DEBUG
				
				
				// set coefficient
				double tanTheta = tan( ( double ) ( degrees - 2 ) * M_PI / 2.0 / ( double ) degrees );
#ifdef  DEBUG
				if( vertexIsStation[ vertex ] != true )
					cerr << "id = " << vertexID[ vertex ] << " degrees = " << degrees << " tanTheta = " << tanTheta << endl;
#endif  // DEBUG
				
				map< double, BoundaryGraph::edge_descriptor >::iterator itN = circM.begin();
				itN++;
				for( map< double, BoundaryGraph::edge_descriptor >::iterator it = circM.begin();
				     it != circM.end(); it++ ) {
					
					BoundaryGraph::edge_descriptor edC = it->second;
					BoundaryGraph::edge_descriptor edN = itN->second;
					BoundaryGraph::vertex_descriptor vS = source( edC, g );
					BoundaryGraph::vertex_descriptor vTC = target( edC, g );
					BoundaryGraph::vertex_descriptor vTN = target( edN, g );
					unsigned int idS = g[ vS ].id;
					unsigned int idTC = g[ vTC ].id;
					unsigned int idTN = g[ vTN ].id;
					
					// x
					_coef( nRows, idS ) = _w_angle;
					_coef( nRows, idTC ) = -0.5 * _w_angle;
					_coef( nRows, idTN ) = -0.5 * _w_angle;
					_coef( nRows, idTC + nVertices ) = -0.5 * _w_angle * tanTheta;
					_coef( nRows, idTN + nVertices ) = 0.5 * _w_angle * tanTheta;
					nRows++;
					
					// y
					_coef( nRows, idS + nVertices ) = _w_angle;
					_coef( nRows, idTC + nVertices ) = -0.5 * _w_angle;
					_coef( nRows, idTN + nVertices ) = -0.5 * _w_angle;
					_coef( nRows, idTC ) = 0.5 * _w_angle * tanTheta;
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
	BGL_FORALL_VERTICES( vertex, g, BoundaryGraph ) {
			
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
	cerr << setprecision(5) << _coef << endl;
#endif  // DEBUG
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
void Stress::_initVars( void ) {
	
	BoundaryGraph &g = _boundaryGraph;
	unsigned int nVertices = num_vertices( g );
	
	// initialization
	_var.resize( _nVars );
	
	unsigned int nRows = 0;
	BGL_FORALL_VERTICES( vertex, g, BoundaryGraph ) {
			
			_var( nRows, 0 ) = g[ vertex ].smoothPtr->x();
			_var( nRows + nVertices, 0 ) = g[ vertex ].smoothPtr->y();
			nRows++;
		}
	
	assert( _nVars == 2 * nRows );
#ifdef  DEBUG
	cerr << "_initvar:" << endl;
	cerr << _var << endl;
#endif  // DEBUG
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
void Stress::_initOutputs( void ) {
	
	BoundaryGraph &g = _boundaryGraph;
	
	// initialization
	unsigned int nRows = 0;
	_output.resize( _nConstrs );
	_output << Eigen::VectorXd::Zero( _nConstrs );
	
	// Regular edge length
	BGL_FORALL_EDGES( edge, g, BoundaryGraph ) {
			BoundaryGraph::vertex_descriptor vdS = source( edge, g );
			BoundaryGraph::vertex_descriptor vdT = target( edge, g );
			Coord2 vi = *g[ vdS ].geoPtr;
			Coord2 vj = *g[ vdT ].geoPtr;
			Coord2 vji = vi - vj;
			double s = _d_Alpha * g[ edge ].weight / vji.norm();
			double cosTheta = 1.0, sinTheta = 0.0;
			//double magnification = 1.0;
			//if( vertexSelectShift[ vdS ] == true || vertexSelectShift[ vdT ] == true ) magnification *= 2.0;
			//if( edgeSelectCtrl[ edge ] == true ) magnification = 2.0;

#ifdef  DEBUG
			cerr << "EID = " << edgeID[ edge ] << ", weight = " << edgeWeight[ edge ] << endl;
#endif  // DEBUG
			
			double wL = _w_contextlength;
			
			// x
			_output( nRows, 0 ) = wL * s * ( cosTheta * vji.x() - sinTheta * vji.y() );
			nRows++;
			
			// y
			_output( nRows, 0 ) = wL * s * ( sinTheta * vji.x() + cosTheta * vji.y() );
			nRows++;
		}
	
	// Maximal angles of incident edges
	BGL_FORALL_VERTICES( vertex, g, BoundaryGraph ) {
			
			BoundaryGraph::degree_size_type degrees = out_degree( vertex, g );
			
			if( degrees > 1 ) {
				
				//double tanTheta = tan( (double)( degrees - 2 ) * M_PI / 2.0 / (double) degrees );
				BoundaryGraph::out_edge_iterator eo_cur, eo_nxt, eo_end;
				for( tie( eo_cur, eo_end ) = out_edges( vertex, g ); eo_cur != eo_end; ++eo_cur ) {
					
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
	BGL_FORALL_VERTICES( vertex, g, BoundaryGraph ) {
			
			_output( nRows, 0 ) = _w_position * g[ vertex ].geoPtr->x();
			nRows++;
			_output( nRows, 0 ) = _w_position * g[ vertex ].geoPtr->y();
			nRows++;
		}

#ifdef  DEBUG
	cerr << "_initOutput:" << endl;
	cerr << _output << endl;
#endif  // DEBUG
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
void Stress::_updateCoefs( void ) {
	
	BoundaryGraph &g = _boundaryGraph;
	unsigned int nVertices = num_vertices( g );
	unsigned int nVE = 0;
	unsigned int nB = 0;
	//vector< double > ratioR = _octilinearBoundaryVec->ratioR();
	
	Eigen::MatrixXd oldCoef;
	oldCoef = _coef.block( 0, 0, _nConstrs, _nVars );
#ifdef  STRESS_CONFLICT
	nVE = _octilinearBoundaryVec->VEconflict().size();
#endif  // Stress_CONFLICT


#ifdef  STRESS_BOUNDARY
	Coord2 &boxCenter = _contourPtr->boxCenter();
#ifdef DEBUG
	cerr << "_half_width = " << _half_width << " _half_height = " << _half_height << endl;
	cerr << "boxCenter = " << boxCenter;
#endif // DEBUG
	// double minD = _d_Alpha/2.0;
	double minD = 10.0;
	BGL_FORALL_VERTICES( vd, g, BoundaryGraph ) {
			if( !_contourPtr->inPolygon( *g[ vd ].smoothPtr ) ) nB++;

#ifdef SKIP
			for( unsigned int i = 0; i < _contourPtr->elements().size(); i++ ){
	
				Coord2 &coordA = _contourPtr->elements()[i];
				Coord2 &coordB = _contourPtr->elements()[ (i+1)%(int)_contourPtr->elements().size() ];
				double geoL = ( coordB.y() - coordA.y() ) * g[ vd ].geoPtr->x() - ( coordB.x() - coordA.x() ) * g[ vd ].geoPtr->y() - coordA.x()*coordB.y() + coordB.x()*coordA.y();
				double smoothL = ( coordB.y() - coordA.y() ) * g[ vd ].smoothPtr->x() - ( coordB.x() - coordA.x() ) * g[ vd ].smoothPtr->y() - coordA.x()*coordB.y() + coordB.x()*coordA.y();
	
				if ( geoL * smoothL <= 0.0 ) nB++;
			}
#endif // SKIP

#ifdef SKIP
			if ( g[ vd ].smoothPtr->x() <= boxCenter.x() - ( _half_width - minD ) ) nB++;
			if ( g[ vd ].smoothPtr->x() >= boxCenter.x() + ( _half_width - minD ) ) nB++;
			if ( g[ vd ].smoothPtr->y() <= boxCenter.y() - ( _half_height - minD ) ) nB++;
			if ( g[ vd ].smoothPtr->y() >= boxCenter.y() + ( _half_height - minD ) ) nB++;
#endif // SKIP
		}
#endif  // STRESS_BOUNDARY
	
	cerr << "_nConstrs = " << _nConstrs << " nB = " << nB << " nVE = " << nVE << " _nVars = " << _nVars << endl;
	
	_coef.resize( _nConstrs + nB + 2 * nVE, _nVars );
	// _coef << Eigen::MatrixXd::Zero( _nConstrs + nB + 2*nVE, _nVars );
	
	// copy old coefficient
	_coef << oldCoef, Eigen::MatrixXd::Zero( nB + 2 * nVE, _nVars );
	
	//if( nVE != 0 || nB != 0 )
	//    cerr << "nVE = " << nVE << " nB = " << nB << endl;
	
	unsigned int nRows = _nConstrs;

#ifdef  STRESS_BOUNDARY
	// add boundary coefficient
	BGL_FORALL_VERTICES( vd, g, BoundaryGraph ) {
			
			unsigned int id = g[ vd ].id;
			if( !_contourPtr->inPolygon( *g[ vd ].smoothPtr ) ) {
				cerr << "vid = " << id << endl;
				_coef( nRows, id ) = _w_boundary;
				nRows++;
			}
#ifdef SKIP
			for( unsigned int i = 0; i < _contourPtr->elements().size(); i++ ){
	
				Coord2 &coordA = _contourPtr->elements()[i];
				Coord2 &coordB = _contourPtr->elements()[ (i+1)%(int)_contourPtr->elements().size() ];
				double geoL = ( coordB.y() - coordA.y() ) * g[ vd ].geoPtr->x() - ( coordB.x() - coordA.x() ) * g[ vd ].geoPtr->y() - coordA.x()*coordB.y() + coordB.x()*coordA.y();
				double smoothL = ( coordB.y() - coordA.y() ) * g[ vd ].smoothPtr->x() - ( coordB.x() - coordA.x() ) * g[ vd ].smoothPtr->y() - coordA.x()*coordB.y() + coordB.x()*coordA.y();
	
				if ( geoL * smoothL <= 0.0 ) {
					cerr << "vid = " << id << endl;
					_coef( nRows, id ) = _w_boundary;
					nRows++;
				}
			}
#endif // SKIP
#ifdef SKIP
			if ( g[ vd ].smoothPtr->x() <= boxCenter.x() - ( _half_width - minD ) ) {
				_coef( nRows, id ) = _w_boundary;
				nRows++;
			}
			if ( g[ vd ].smoothPtr->x() >= boxCenter.x() + ( _half_width - minD ) ) {
				_coef( nRows, id ) = _w_boundary;
				nRows++;
			}
			if ( g[ vd ].smoothPtr->y() <= boxCenter.y() - ( _half_height - minD ) ) {
				_coef( nRows, id + nVertices ) = _w_boundary;
				nRows++;
			}
			if ( g[ vd ].smoothPtr->y() >= boxCenter.y() + ( _half_height - minD ) ) {
				_coef( nRows, id + nVertices ) = _w_boundary;
				nRows++;
			}
#endif // SKIP
		}
#endif  // STRESS_BOUNDARY
	
	// cerr << "_coef.rows = " << _coef.rows() << endl;
	// cerr << "_coef.cols = " << _coef.cols() << endl;

#ifdef  STRESS_CONFLICT
	// add conflict coefficient
	unsigned int countVE = 0;
	for ( VEMap::iterator it = _octilinearBoundaryVec->VEconflict().begin();
		  it != _octilinearBoundaryVec->VEconflict().end(); ++it ) {
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
#endif  // STRESS_CONFLICT

#ifdef  DEBUG
	cerr << "###############" << endl;
	cerr << "newCoef:" << endl;
	cerr << _coef << endl;
	cerr << "###############" << endl;
#endif  // DEBUG
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
void Stress::_updateOutputs( void ) {
	
	BoundaryGraph &g = _boundaryGraph;
	unsigned int nVertices = num_vertices( g );
	unsigned int nVE = 0;
	unsigned int nB = 0;
	//vector< double > ratioR = _octilinearBoundaryVec->ratioR();
	
	unsigned int nRows = 0;
	Eigen::VectorXd oldOutput;
	oldOutput = _output;
#ifdef  STRESS_CONFLICT
	nVE = _octilinearBoundaryVec->VEconflict().size();
#endif  // STRESS_CONFLICT
#ifdef  STRESS_BOUNDARY
	Coord2 &boxCenter = _contourPtr->boxCenter();
	// double minD = _d_Alpha/2.0;
	double minD = 10.0;
	BGL_FORALL_VERTICES( vd, g, BoundaryGraph ) {
			if( !_contourPtr->inPolygon( *g[ vd ].smoothPtr ) ) nB++;

#ifdef SKIP
			for( unsigned int i = 0; i < _contourPtr->elements().size(); i++ ){
	
				Coord2 &coordA = _contourPtr->elements()[i];
				Coord2 &coordB = _contourPtr->elements()[ (i+1)%(int)_contourPtr->elements().size() ];
				double geoL = ( coordB.y() - coordA.y() ) * g[ vd ].geoPtr->x() - ( coordB.x() - coordA.x() ) * g[ vd ].geoPtr->y() - coordA.x()*coordB.y() + coordB.x()*coordA.y();
				double smoothL = ( coordB.y() - coordA.y() ) * g[ vd ].smoothPtr->x() - ( coordB.x() - coordA.x() ) * g[ vd ].smoothPtr->y() - coordA.x()*coordB.y() + coordB.x()*coordA.y();
	
				if ( geoL * smoothL <= 0.0 ) nB++;
			}
#endif // SKIP
#ifdef SKIP
			if ( g[ vd ].smoothPtr->x() <= boxCenter.x() - ( _half_width - minD ) ) nB++;
			if ( g[ vd ].smoothPtr->x() >= boxCenter.x() + ( _half_width - minD ) ) nB++;
			if ( g[ vd ].smoothPtr->y() <= boxCenter.y() - ( _half_height - minD ) ) nB++;
			if ( g[ vd ].smoothPtr->y() >= boxCenter.y() + ( _half_height - minD ) ) nB++;
#endif // SKIP
		}
#endif  // STRESS_BOUNDARY
	_output.resize( _nConstrs + nB + 2 * nVE );
	_output << Eigen::VectorXd::Zero( _nConstrs + nB + 2 * nVE );
	//cerr << "_output.rows = " << _output.rows() << endl;
	
	// Regular edge length
	BGL_FORALL_EDGES( edge, g, BoundaryGraph ) {
			BoundaryGraph::vertex_descriptor vdS = source( edge, g );
			BoundaryGraph::vertex_descriptor vdT = target( edge, g );
			Coord2 vi = *g[ vdS ].geoPtr;
			Coord2 vj = *g[ vdT ].geoPtr;
			Coord2 vji = vi - vj;
			Coord2 pi = *g[ vdS ].smoothPtr;
			Coord2 pj = *g[ vdT ].smoothPtr;
			Coord2 pji = pi - pj;
			double s = _d_Alpha * g[ edge ].weight / vji.norm();
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
			
			// x
			_output( nRows, 0 ) = wL * s * ( cosTheta * vji.x() - sinTheta * vji.y() );
			nRows++;
			
			// y
			_output( nRows, 0 ) = wL * s * ( sinTheta * vji.x() + cosTheta * vji.y() );
			nRows++;
		}
	
	
	// Maximal angles of incident edges
	BGL_FORALL_VERTICES( vertex, g, BoundaryGraph ) {
			
			BoundaryGraph::degree_size_type degrees = out_degree( vertex, g );
			
			if( degrees > 1 ) {
				
				double tanTheta = tan( ( double ) ( degrees - 2 ) * M_PI / 2.0 / ( double ) degrees );
				BoundaryGraph::out_edge_iterator eo_cur, eo_nxt, eo_end;
				for( tie( eo_cur, eo_end ) = out_edges( vertex, g ); eo_cur != eo_end; ++eo_cur ) {
					
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
	double avgA = 0.5 * _contourPtr->area() / ( double ) num_vertices( g );
	// cerr << "avgA = " << avgA << endl;
	BGL_FORALL_VERTICES( vertex, g, BoundaryGraph ) {
			
			double area = 0.0;
			if( _seedVec.size() > 0 ) {
				area = _seedVec[ g[ vertex ].id ].voronoiCellPtr->area();
				// cerr << "area = " << area << " avgA = " << avgA << endl;
			}
			double ratio = area / avgA;
			//Coord2 vecVC = *g[ vertex ].centroidPtr - Coord2( oldOutput( nRows, 0 ), oldOutput( nRows+1, 0 ) );
			// x
			if( area > avgA ) {
				//_output( nRows, 0 ) = oldOutput( nRows, 0 ) + _paramRatioVoronoi* ( vecVC/vecVC.norm() ).x();
				_output( nRows, 0 ) =
						_paramRatioPosition * oldOutput( nRows, 0 ) + _paramRatioVoronoi * g[ vertex ].centroidPtr->x();
			}
			else
				_output( nRows, 0 ) = oldOutput( nRows, 0 );
			nRows++;
			
			// y
			if( area > avgA ) {
				//_output( nRows, 0 ) = oldOutput( nRows, 0 ) + _paramRatioVoronoi* ( vecVC/vecVC.norm() ).y();
				_output( nRows, 0 ) =
						_paramRatioPosition * oldOutput( nRows, 0 ) + _paramRatioVoronoi * g[ vertex ].centroidPtr->y();
			}
			else
				_output( nRows, 0 ) = oldOutput( nRows, 0 );
			nRows++;
		}

#ifdef  STRESS_BOUNDARY
	// boundary constraints
	BGL_FORALL_VERTICES( vd, g, BoundaryGraph ) {
			
			if( !_contourPtr->inPolygon( *g[ vd ].smoothPtr ) ) {
				
				_output( nRows, 0 ) = _w_boundary * _contourPtr->minDistToPolygon( *g[ vd ].smoothPtr );
				nRows++;
			}

#ifdef SKIP
			for( unsigned int i = 0; i < _contourPtr->elements().size(); i++ ){
	
				Coord2 &coordA = _contourPtr->elements()[i];
				Coord2 &coordB = _contourPtr->elements()[ (i+1)%(int)_contourPtr->elements().size() ];
				double geoL = ( coordB.y() - coordA.y() ) * g[ vd ].geoPtr->x() - ( coordB.x() - coordA.x() ) * g[ vd ].geoPtr->y() - coordA.x()*coordB.y() + coordB.x()*coordA.y();
				double smoothL = ( coordB.y() - coordA.y() ) * g[ vd ].smoothPtr->x() - ( coordB.x() - coordA.x() ) * g[ vd ].smoothPtr->y() - coordA.x()*coordB.y() + coordB.x()*coordA.y();
	
				if ( geoL * smoothL <= 0.0 ) {
					_output( nRows, 0 ) = _w_boundary * fabs( smoothL/(coordB-coordA).norm() );
					nRows++;
				}
			}
#endif // SKIP

#ifdef SKIP
			if ( g[ vd ].smoothPtr->x() <= boxCenter.x() - ( _half_width - minD ) ) {
				_output( nRows, 0 ) = _w_boundary * -( _half_width - minD );
				nRows++;
			}
			if ( g[ vd ].smoothPtr->x() >= boxCenter.x() + ( _half_width - minD ) ) {
				_output( nRows, 0 ) = _w_boundary * ( _half_width - minD );
				nRows++;
			}
			if ( g[ vd ].smoothPtr->y() <= boxCenter.y() - ( _half_height - minD ) ) {
				_output( nRows, 0 ) = _w_boundary * -( _half_height - minD );
				nRows++;
			}
			if ( g[ vd ].smoothPtr->y() >= boxCenter.y() + ( _half_height - minD ) ) {
				_output( nRows, 0 ) = _w_boundary * ( _half_height - minD );
				nRows++;
			}
#endif // SKIP
		}
#endif  // STRESS_BOUNDARY
	
	// cerr << "nRows = " << nRows << endl;

#ifdef  STRESS_CONFLICT
	// conflict constraints
	unsigned int countVE = 0;
	for ( VEMap::iterator it = _octilinearBoundaryVec->VEconflict().begin();
		  it != _octilinearBoundaryVec->VEconflict().end(); ++it ) {
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
		double minD = _d_Alpha;
		//if( vertexIsStation[ vdV ] == false )
		//    minD = vertexExternal[ vdV ].leaderWeight() * _d_Alpha;
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
double Stress::LeastSquare( unsigned int iter ) {
	
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
//  Stress::initConjugateGradient --        optimization
//
//  Inputs
//      none
//
//  Outputs
//      none
//
void Stress::initConjugateGradient( void ) {
	
	// initialization, prepareBoundary the square matrix
	_A = _coef.transpose() * _coef;
	_b = _coef.transpose() * _output;
	
	// initialization
	_err = _b - _A * _var;
	_p = _err;
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
double Stress::ConjugateGradient( unsigned int &iter ) {
	
	// main algorithm
	// prepareBoundary the square matrix
	_A = _coef.transpose() * _coef;
	_b = _coef.transpose() * _output;
	_Ap = _A * _p;
	
	double alpha = ( double ) ( _p.transpose() * _err ) / ( double ) ( _p.transpose() * _Ap );
	_var = _var + alpha * _p;
	_err = _b - _A * _var;
	
	double beta = -1.0 * ( double ) ( _err.transpose() * _Ap ) / ( double ) ( _p.transpose() * _Ap );
	_p = _err + beta * _p;
	
	// update
	retrieve();
	//if( ( iter % _paramVoronoiFreq == 1 ) ){
	//if( ( iter/_paramVoronoiFreq > 0 ) && ( iter % _paramVoronoiFreq == 0 ) ){
	//cerr << "computing Voronoi..." << endl;
	//cerr << "simpleContour = " << _contour;
	//if( _workerName != "WorkerLevelMiddle" )
	_computeVoronoi();
	//}
	_updateCoefs();
	_updateOutputs();
	
	// cerr << "iter = " << iter << endl;
	
	return sqrt( _err.adjoint() * _err );
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
void Stress::retrieve( void ) {
	
	BoundaryGraph &g = _boundaryGraph;
	unsigned int nVertices = num_vertices( g );
	
	vector< vector< BoundaryGraph::vertex_descriptor > > vdMatrix;

#ifdef  STRESS_CONFLICT
	// find the vertex that is too close to an edge
	for( VEMap::iterator it = _octilinearBoundaryVec->VEconflict().begin();
	     it != _octilinearBoundaryVec->VEconflict().end(); ++it ) {
		
		vector< BoundaryGraph::vertex_descriptor > vdVec;
		BoundaryGraph::vertex_descriptor vdV = it->second.first;
		BoundaryGraph::edge_descriptor ed = it->second.second;
		BoundaryGraph::vertex_descriptor vdS = source( ed, g );
		BoundaryGraph::vertex_descriptor vdT = target( ed, g );
		vdVec.push_back( vdV );
		vdVec.push_back( vdS );
		vdVec.push_back( vdT );
		
		vdMatrix.push_back( vdVec );
		//cerr << "V = " << vertexID[ vdV ] << " S = " << vertexID[ vdS ] << " T = " << vertexID[ vdT ] << endl;
	}
	//cerr << endl;
#endif  // STRESS_CONFLICT
	
	unsigned int nRows = 0;
	double scale = 1.0;
#ifdef SKIP
	// update coordinates
	// but freeze the vertex that is too close to an edge
	BGL_FORALL_VERTICES( vertex, g, BoundaryGraph ){

		vector< int > rowVec;
		for( unsigned int i = 0; i < vdMatrix.size(); i++ ){
			if( vertex == vdMatrix[i][0] ) rowVec.push_back( i );
		}
		if( rowVec.size() > 0 ){

			Coord2 curCoord = *g[ vertex ].smoothPtr;
			Coord2 newCoord( _var( nRows, 0 ), _var( nRows + nVertices, 0 ) );
			for( unsigned int i = 0; i < rowVec.size(); i++ ){

				BoundaryGraph::vertex_descriptor vdS = vdMatrix[ rowVec[i] ][ 1 ];
				BoundaryGraph::vertex_descriptor vdT = vdMatrix[ rowVec[i] ][ 2 ];
				//cerr << "V = " << vertexID[ vertex ] << " S = " << vertexID[ vdS ] << " T = " << vertexID[ vdT ]
				//     << " dist = " << (newCoord - curCoord).norm() << endl;
				Coord2 coordS = *g[ vdS ].smoothPtr;
				Coord2 coordT = *g[ vdT ].smoothPtr;
				double m = ( coordS.y() - coordT.y() ) / ( coordS.x() - coordT.x() );
				double k = coordS.y() - m * coordS.x();
				double tmpS = 1.0;
				while( ( curCoord.y() - m * curCoord.x() - k ) * ( newCoord.y() - m * newCoord.x() - k ) < 0 ) {
					//cerr << ( curCoord.y() - m * curCoord.x() - k ) << ", " << newCoord.y() - m * newCoord.x() - k << endl;
					Coord2 diff = newCoord - curCoord;
					//cerr << "diff = " << diff << "new = " << newCoord << "cur = " << curCoord << endl;
					newCoord = diff/2.0 + curCoord;
					tmpS = tmpS/2.0;
					//cerr << g[ vertex ].id << " == ( " << g[ vdS ].id << ", " << g[ vdT ].id << " )" << endl;
				}
				if( tmpS < scale ) scale = tmpS;
			}
		}
	}
#endif // SKIP
	
	//cerr << "scale = " << scale << endl;
	BGL_FORALL_VERTICES( vertex, g, BoundaryGraph ) {
			
			// Coord2 curCoord = *g[ vertex ].smoothPtr;
			double x = _var( nRows, 0 );
			double y = _var( nRows + nVertices, 0 );
			Coord2 c( x, y );
			if( isnan( x ) || isnan( y ) ) {
				cerr << "something is wrong here... at " << __LINE__ << " in " << __FILE__ << endl;
				assert( false );
			}
				//else {
			else if( _contourPtr->inPolygon( c ) ) { // update only if the coordinates are in the simpleContour
				g[ vertex ].coordPtr->x() = g[ vertex ].smoothPtr->x() = x;
				g[ vertex ].coordPtr->y() = g[ vertex ].smoothPtr->y() = y;
			}
			
			// cerr << g[ vertex ].id << " = " << *g[ vertex ].smoothPtr;
			assert( g[ vertex ].id == nRows );
			nRows++;
		}
	
	// update Stress angle
	BGL_FORALL_EDGES( edge, g, BoundaryGraph ) {
			
			BoundaryGraph::vertex_descriptor vdS = source( edge, g );
			BoundaryGraph::vertex_descriptor vdT = target( edge, g );
			
			Coord2 coordO;
			Coord2 coordD;
			if( g[ vdS ].id < g[ vdT ].id ) {
				coordO = *g[ vdS ].smoothPtr;
				coordD = *g[ vdT ].smoothPtr;
			}
			else {
				coordO = *g[ vdT ].smoothPtr;
				coordD = *g[ vdS ].smoothPtr;
			}
			double diffX = coordD.x() - coordO.x();
			double diffY = coordD.y() - coordO.y();
			double angle = atan2( diffY, diffX );
			
			g[ edge ].smoothAngle = angle;
			g[ edge ].angle = angle;
		}

#ifdef  STRESS_CONFLICT
	// check possible conflict
	_octilinearBoundaryVec->checkVEConflicts();
#endif  // STRESS_CONFLICT

#ifdef  DEBUG
	cerr << "retrieve:" << endl;
	BGL_FORALL_VERTICES( vertex, g, BoundaryGraph ){
		cerr << "V(" << vertexID[ vertex ] << ") = " << vertexStress[ vertex ];
	}
	BGL_FORALL_EDGES( edge, g, BoundaryGraph ){
		cerr << "E(" << edgeID[ edge ] << ") : smoAngle= " << edgeSmoAngle[ edge ] << endl;
	}
#endif  // DEBUG
}

//
//  Stress::initSeed --	init voronoi seed
//
//  Inputs
//	none
//
//  Outputs
//	none
//
void Stress::_initStressSeed( void ) {
	
	_seedVec.clear();
	BoundaryGraph &bg = _boundaryGraph;
	
	BGL_FORALL_VERTICES( vd, bg, BoundaryGraph ) {
			
			Seed seed;
			seed.id = bg[ vd ].id;
			seed.weight = bg[ vd ].weight;
			seed.coordPtr = bg[ vd ].coordPtr;
			//seed.voronoiCellPtr = bg[ vd ].cellPtr;
			_seedVec.push_back( seed );
			
			// cerr << seed.coord;
		}
}

//
//  Stress::_computeVoronoi --        compute voronoi
//
//  Inputs
//      none
//
//  Outputs
//      none
//
void Stress::_computeVoronoi( void ) {
	
	BoundaryGraph &bg = _boundaryGraph;
	
	// initialization
	_initStressSeed();
	
	_voronoi.id() = 0;
	_voronoi.init( _seedVec, *_contourPtr );
	
	//cerr << "_seedVec.size() = " << _voronoi.seedVec()->size() << endl;
	//cerr << "simpleContour = " << _contour;
	_voronoi.createVoronoiDiagram( false );  // true: weighted, false: uniformed
	
	BGL_FORALL_VERTICES( vd, bg, BoundaryGraph ) {
			
			// initialization
			bg[ vd ].centroidPtr->zero();
			
			// Find the average pixel coordinates of each vertex
			if( _seedVec[ bg[ vd ].id ].voronoiCellPtr->area() != 0 ) {
#ifdef DEBUG
				cerr << "vid = " << bg[vd].id
				 << " element = " << _seedVec[ bg[vd].id ].voronoiCellPtr->elements().size()
				 << " area = " << _seedVec[ bg[vd].id ].voronoiCellPtr->area()
				 << " center = " << _seedVec[ bg[vd].id ].voronoiCellPtr->center() << endl;
#endif // DEBUG
				//Coord2 dest = _seedVec[ bg[vd].id ].voronoiCellPtr->center();
				//*bg[ vd ].centroidPtr = dest - *bg[ vd ].coordPtr;
				*bg[ vd ].centroidPtr = _seedVec[ bg[ vd ].id ].voronoiCellPtr->center();
			}
			else {
				cerr << "%%%%%%%%%% Number of pixels vanishes!!!" << endl;
				bg[ vd ].centroidPtr->zero();
			}
		}
}

//
//  Stress::run --        run optimization
//
//  Inputs
//      none
//
//  Outputs
//      none
//
void Stress::run( void ) {
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
void Stress::_clear( void ) {
	
	_var.resize( 0 );           // x
	_output.resize( 0 );        // b
	_coef.resize( 0, 0 );       // A
	_half_width = 0.0;
	_half_height = 0.0;
	
	_nVars = 0.0;
	_nConstrs = 0.0;
	_w_angle = 0.0;
	_w_position = 0.0;
	_w_contextlength = 0.0;
	_w_boundary = 0.0;
	_w_crossing = 0.0;
	_w_labelangle = 0.0;
	_d_Alpha = 0.0;           // focus edge length
	
	_paramOptType = LEAST_SQUARE;
	_paramVoronoiFreq = 0;
	_paramRatioPosition = 0.0;
	_paramRatioVoronoi = 0.0;
	
	// Conjugate Gradient
	_A.resize( 0, 0 );
	_b.resize( 0 );
	_Ap.resize( 0 );
	_err.resize( 0 );
	_p.resize( 0 );
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
Stress::Stress( void ) {

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
Stress::Stress( const Stress &obj ) {
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
Stress::~Stress( void ) {
}

// end of header file
// Do not add any stuff under this line.

