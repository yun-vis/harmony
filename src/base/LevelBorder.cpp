//==============================================================================
// LevelBorder.cc
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

#include "base/LevelBorder.h"
#include "graph/UndirectedPropertyGraph.h"

//------------------------------------------------------------------------------
//	Private functions
//------------------------------------------------------------------------------
//  LevelBorder::_init -- initialization
//
//  Inputs
//      string
//
//  Outputs
//  double
//
//
void LevelBorder::_init( double *widthPtr, double *heightPtr,
                         double *veCoveragePtr, SkeletonGraph &skeletonGraph ) {
	
	// initialize variables
	_levelType = LEVEL_BORDER;
	_content_widthPtr = widthPtr;
	_content_heightPtr = heightPtr;
	//_veCoveragePtr = veCoveragePtr;
	
	clearGraph( _skeletonForceGraph );
	
	// copy skeletonForceGraph to skeletionGraph
	BGL_FORALL_VERTICES( vd, skeletonGraph, SkeletonGraph ) {
		
			ForceGraph::vertex_descriptor vdNew = add_vertex( _skeletonForceGraph );
			_skeletonForceGraph[ vdNew ].id = skeletonGraph[ vd ].id;
			_skeletonForceGraph[ vdNew ].groupID = skeletonGraph[ vd ].id;
			_skeletonForceGraph[ vdNew ].initID = skeletonGraph[ vd ].id;
			
			_skeletonForceGraph[ vdNew ].coordPtr = new Coord2( skeletonGraph[ vd ].coordPtr->x(),
			                                                    skeletonGraph[ vd ].coordPtr->y() );
			_skeletonForceGraph[ vdNew ].prevCoordPtr = new Coord2( skeletonGraph[ vd ].coordPtr->x(),
			                                                        skeletonGraph[ vd ].coordPtr->y() );
			_skeletonForceGraph[ vdNew ].forcePtr = new Coord2( 0, 0 );
			_skeletonForceGraph[ vdNew ].placePtr = new Coord2( 0, 0 );
			_skeletonForceGraph[ vdNew ].shiftPtr = new Coord2( 0, 0 );
			_skeletonForceGraph[ vdNew ].weight = 0.0;
			
			_skeletonForceGraph[ vdNew ].widthPtr = new double( *skeletonGraph[ vd ].widthPtr );
			_skeletonForceGraph[ vdNew ].heightPtr = new double( *skeletonGraph[ vd ].heightPtr );
			_skeletonForceGraph[ vdNew ].areaPtr = new double( *skeletonGraph[ vd ].areaPtr );
		}
	
	BGL_FORALL_EDGES( ed, skeletonGraph, SkeletonGraph ) {
			SkeletonGraph::vertex_descriptor vdS = source( ed, skeletonGraph );
			SkeletonGraph::vertex_descriptor vdT = target( ed, skeletonGraph );
			ForceGraph::vertex_descriptor vdCompoS = vertex( skeletonGraph[ vdS ].id, _skeletonForceGraph );
			ForceGraph::vertex_descriptor vdCompoT = vertex( skeletonGraph[ vdT ].id, _skeletonForceGraph );
			
			// add edge
			pair< ForceGraph::edge_descriptor, unsigned int > foreE = add_edge( vdCompoS, vdCompoT,
			                                                                    _skeletonForceGraph );
			BoundaryGraph::edge_descriptor foreED = foreE.first;
			_skeletonForceGraph[ foreED ].id = _skeletonForceGraph[ ed ].id;
		}
	
	
	// normalization
	_normalizeSkeleton();
	_decomposeSkeleton();
	_normalizeRegionBase();
	
	// initialize regionBase
	Polygon2 contour;
	double &content_width = *_content_widthPtr;
	double &content_height = *_content_heightPtr;
	contour.elements().push_back( Coord2( -0.5 * content_width, -0.5 * content_height ) );
	contour.elements().push_back( Coord2( +0.5 * content_width, -0.5 * content_height ) );
	contour.elements().push_back( Coord2( +0.5 * content_width, +0.5 * content_height ) );
	contour.elements().push_back( Coord2( -0.5 * content_width, +0.5 * content_height ) );
	contour.boundingBox() = Coord2( content_width, content_height );
	contour.boxCenter().x() = 0.0;
	contour.boxCenter().y() = 0.0;
	contour.area() = content_width * content_height;
	
	_regionBase.init( &_levelType, contour );
}


//
//  LevelBorder::_clear --    clear the current LevelBorder information
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void LevelBorder::_clear( void ) {
}

//------------------------------------------------------------------------------
//	Protected functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------
//
//  LevelBorder::LevelBorder -- default constructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
LevelBorder::LevelBorder( void ) {
	clearGraph( _skeletonForceGraph );
}

//
//  LevelBorder::LevelBorder -- copy constructor
//
//  Inputs
//  obj : object of this class
//
//  Outputs
//  none
//
LevelBorder::LevelBorder( const LevelBorder &obj ) {
}


//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------

//
//  LevelBorder::LevelBorder --    destructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
LevelBorder::~LevelBorder( void ) {
}

//
//  LevelBorder::decomposeSkeleton --    decompose the skeletonForceGraph
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void LevelBorder::_decomposeSkeleton( void ) {
	
	double labelArea = 0.0;
	unsigned int index = num_vertices( _skeletonForceGraph );
	ForceGraph &forceGraph = _regionBase.forceGraph();
	
	// copy skeletonForceGraph to composite
	BGL_FORALL_VERTICES( vd, _skeletonForceGraph, ForceGraph ) {
			ForceGraph::vertex_descriptor vdNew = add_vertex( forceGraph );
			forceGraph[ vdNew ].id = _skeletonForceGraph[ vd ].id;
			forceGraph[ vdNew ].initID = _skeletonForceGraph[ vd ].initID;
			
			forceGraph[ vdNew ].coordPtr = new Coord2( _skeletonForceGraph[ vd ].coordPtr->x(),
			                                           _skeletonForceGraph[ vd ].coordPtr->y() );
			forceGraph[ vdNew ].prevCoordPtr = new Coord2( _skeletonForceGraph[ vd ].coordPtr->x(),
			                                               _skeletonForceGraph[ vd ].coordPtr->y() );
			forceGraph[ vdNew ].forcePtr = new Coord2( 0, 0 );
			forceGraph[ vdNew ].placePtr = new Coord2( 0, 0 );
			forceGraph[ vdNew ].shiftPtr = new Coord2( 0, 0 );
			forceGraph[ vdNew ].weight = 0.0;
			
			forceGraph[ vdNew ].widthPtr = new double( *_skeletonForceGraph[ vd ].widthPtr );
			forceGraph[ vdNew ].heightPtr = new double( *_skeletonForceGraph[ vd ].heightPtr );
			forceGraph[ vdNew ].areaPtr = new double( *_skeletonForceGraph[ vd ].areaPtr );
			forceGraph[ vdNew ].cellPtr = new Polygon2;
			
			labelArea += *forceGraph[ vdNew ].areaPtr;
		}
	BGL_FORALL_EDGES( ed, _skeletonForceGraph, ForceGraph ) {
			ForceGraph::vertex_descriptor vdS = source( ed, _skeletonForceGraph );
			ForceGraph::vertex_descriptor vdT = target( ed, _skeletonForceGraph );
			ForceGraph::vertex_descriptor vdCompoS = vertex( _skeletonForceGraph[ vdS ].id, forceGraph );
			ForceGraph::vertex_descriptor vdCompoT = vertex( _skeletonForceGraph[ vdT ].id, forceGraph );
			
			// add edge
			pair< ForceGraph::edge_descriptor, unsigned int > foreE = add_edge( vdCompoS, vdCompoT, forceGraph );
			BoundaryGraph::edge_descriptor foreED = foreE.first;
			forceGraph[ foreED ].id = _skeletonForceGraph[ ed ].id;
		}
	
	// split large vertices
	double unit = labelArea / 40.0;
	// double unit = 8000.0;

#ifdef DEBUG
	cerr << "labelArea = " << labelArea
		 << " veCoverage = " << *_veCoveragePtr << " unit = " << unit << endl;
	cerr << "nV = " << num_vertices( forceGraph ) << " nE = " << num_edges( forceGraph ) << endl;
#endif // DEBUG
	
	BGL_FORALL_VERTICES( vd, forceGraph, ForceGraph ) {
		
			int mag = round( *forceGraph[ vd ].areaPtr / unit );
			// int mag = round((*forceGraph[ vd ].widthPtr) * (*forceGraph[ vd ].heightPtr)/unit);
			
			// avoid from looping over the size of _skeletonForceGraph
			if( forceGraph[ vd ].id >= num_vertices( _skeletonForceGraph ) ) break;

#ifdef DEBUG
			cerr << "area = " << *forceGraph[ vd ].areaPtr << " mag = " << mag << " unit = " << unit << endl;
#endif // DEBUG
			
			vector< ForceGraph::edge_descriptor > removeEVec;
			// double radius = 25.0; // tiny
			double radius = 100.0;
			
			if( mag > 1 ) {
				
				ForceGraph::vertex_descriptor vdC = vd;
				ForceGraph::out_edge_iterator eo, eo_end;
				Coord2 origin = *forceGraph[ vd ].coordPtr;
				vector< ForceGraph::vertex_descriptor > extendVD;
				
				// record the adjacent vertices for edge connection
				map< unsigned int, ForceGraph::vertex_descriptor > vdAdjacent;
				// cerr << "degree = " << out_degree( vd, forceGraph ) << endl;
				for( tie( eo, eo_end ) = out_edges( vd, forceGraph ); eo != eo_end; eo++ ) {
					
					ForceGraph::edge_descriptor ed = *eo;
					ForceGraph::vertex_descriptor vdT = target( ed, forceGraph );
					vdAdjacent.insert(
							pair< unsigned int, ForceGraph::vertex_descriptor >( forceGraph[ ed ].id, vdT ) );
					//remove_edge( *eo, forceGraph );
					removeEVec.push_back( ed );
				}
				
				// add composite vertices
				for( int i = 0; i < mag; i++ ) {
					
					double cosTheta = 1.0 * cos( 2.0 * M_PI * ( double ) i / ( double ) mag );
					double sinTheta = 1.0 * sin( 2.0 * M_PI * ( double ) i / ( double ) mag );
					double x = origin.x() + radius * cosTheta;
					double y = origin.y() + radius * sinTheta;
					
					if( i == 0 ) {
						
						forceGraph[ vd ].coordPtr->x() = x;
						forceGraph[ vd ].coordPtr->y() = y;
						forceGraph[ vd ].weight = *forceGraph[ vd ].areaPtr;
						
						// post processing
						extendVD.push_back( vdC );
					}
					else {
						
						// cerr << "index = " << index << " area = " << *forceGraph[ vd ].areaPtr << " mag = " << mag << " unit = " << unit << endl;
						
						// add vertex
						ForceGraph::vertex_descriptor vdNew = add_vertex( forceGraph );
						forceGraph[ vdNew ].id = index;
						forceGraph[ vdNew ].initID = forceGraph[ vd ].initID;
						
						forceGraph[ vdNew ].coordPtr = new Coord2( x, y );
						forceGraph[ vdNew ].prevCoordPtr = new Coord2( x, y );
						forceGraph[ vdNew ].forcePtr = new Coord2( 0, 0 );
						forceGraph[ vdNew ].placePtr = new Coord2( 0, 0 );
						forceGraph[ vdNew ].shiftPtr = new Coord2( 0, 0 );
						forceGraph[ vdNew ].weight = *forceGraph[ vd ].areaPtr;
						
						forceGraph[ vdNew ].widthPtr = new double( sqrt( unit ) );
						forceGraph[ vdNew ].heightPtr = new double( sqrt( unit ) );
						forceGraph[ vdNew ].areaPtr = new double( unit );
						forceGraph[ vdNew ].cellPtr = new Polygon2;
						
						// add edge
						pair< ForceGraph::edge_descriptor, unsigned int > foreE = add_edge( vdC, vdNew, forceGraph );
						BoundaryGraph::edge_descriptor foreED = foreE.first;
						forceGraph[ foreED ].id = num_edges( forceGraph );
						forceGraph[ foreED ].weight = 2.0 * ( *forceGraph[ vd ].areaPtr );
						
						// add last edge to form a circle
						if( ( i == mag - 1 ) && ( mag != 2 ) ) {
							pair< ForceGraph::edge_descriptor, unsigned int > foreE = add_edge( vd, vdNew,
							                                                                    forceGraph );
							BoundaryGraph::edge_descriptor foreED = foreE.first;
							forceGraph[ foreED ].id = num_edges( forceGraph );
							forceGraph[ foreED ].weight = 2.0 * ( *forceGraph[ vd ].areaPtr );
						}
						
						// post processing
						extendVD.push_back( vdNew );
						vdC = vdNew;
						index++;
					}
				}
				
				// add composite edges
				// cerr << "size = " << vdAdjacent.size() << " ext = " << extendVD.size() << endl;
				map< unsigned int, ForceGraph::vertex_descriptor >::iterator itA;
				for( itA = vdAdjacent.begin(); itA != vdAdjacent.end(); itA++ ) {
					
					ForceGraph::vertex_descriptor cloestVD = extendVD[ 0 ];
					ForceGraph::vertex_descriptor vdT = itA->second;
					double minDist = ( *forceGraph[ cloestVD ].coordPtr - *forceGraph[ vdT ].coordPtr ).norm();
					for( unsigned int i = 1; i < extendVD.size(); i++ ) {
						
						ForceGraph::vertex_descriptor vdS = extendVD[ i ];
						double dist = ( *forceGraph[ vdS ].coordPtr - *forceGraph[ vdT ].coordPtr ).norm();
						
						if( dist < minDist ) {
							cloestVD = extendVD[ i ];
							minDist = dist;
						}
					}
					pair< ForceGraph::edge_descriptor, unsigned int > foreE = add_edge( vdT, cloestVD, forceGraph );
					ForceGraph::edge_descriptor foreED = foreE.first;
					forceGraph[ foreED ].id = itA->first;
					forceGraph[ foreED ].weight = forceGraph[ vdT ].weight + forceGraph[ cloestVD ].weight;
				}
				
				// remove edges
				// cerr << "eSize = " << removeEVec.size() << endl;
				for( unsigned int i = 0; i < removeEVec.size(); i++ ) {
					// cerr << "eID = " << forceGraph[removeEVec[i]].id << endl;
					remove_edge( removeEVec[ i ], forceGraph );
				}
			}
			else {
				forceGraph[ vd ].weight = *forceGraph[ vd ].areaPtr;
			}
		}

#ifdef DEBUG
	BGL_FORALL_VERTICES( vd, forceGraph, ForceGraph )
	{
		cerr << "vID = " << forceGraph[vd].id << endl;
		cerr << "weight = " << forceGraph[vd].weight << endl;
		cerr << "cell size = " << forceGraph[vd].cellPtr->elements().size() << endl;
	}
	cerr << endl;
	BGL_FORALL_EDGES( ed, forceGraph, ForceGraph )
	{
		cerr << "eID = " << forceGraph[ed].id << endl;
		cerr << "weight = " << forceGraph[ed].weight << endl;
	}
#endif // DEBUG
}

//
//  LevelBorder::normalizeSkeleton --    normalize the skeletonForceGraph
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void LevelBorder::_normalizeSkeleton( void ) {
	
	// initialization
	double xMin = INFINITY;
	double xMax = -INFINITY;
	double yMin = INFINITY;
	double yMax = -INFINITY;
	
	double width = *_content_widthPtr;
	double height = *_content_heightPtr;
	
	// Scan all the vertex coordinates first
	BGL_FORALL_VERTICES( vd, _skeletonForceGraph, ForceGraph ) {
			Coord2 coord = *_skeletonForceGraph[ vd ].coordPtr;
			if( coord.x() < xMin ) xMin = coord.x();
			if( coord.x() > xMax ) xMax = coord.x();
			if( coord.y() < yMin ) yMin = coord.y();
			if( coord.y() > yMax ) yMax = coord.y();
		}
	
	// Normalize the coordinates
	BGL_FORALL_VERTICES( vd, _skeletonForceGraph, ForceGraph ) {
			Coord2 coord;
			coord.x() = width * ( _skeletonForceGraph[ vd ].coordPtr->x() - xMin ) / ( xMax - xMin ) - 0.5 * width;
			coord.y() = height * ( _skeletonForceGraph[ vd ].coordPtr->y() - yMin ) / ( yMax - yMin ) - 0.5 * height;
			_skeletonForceGraph[ vd ].coordPtr->x() = coord.x();
			_skeletonForceGraph[ vd ].coordPtr->y() = coord.y();
			// cerr << coord;
		}
}

//
//  Package::normalizeRegionBase --    normalize the forceGraph
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void LevelBorder::_normalizeRegionBase( void ) {
	
	// initialization
	double xMin = INFINITY;
	double xMax = -INFINITY;
	double yMin = INFINITY;
	double yMax = -INFINITY;
	double ratio = 1.0;
	// double ratio = 0.8;
	//cerr << "w = " << width << " h = " << height << endl;
	double width = *_content_widthPtr * 0.95;
	double height = *_content_heightPtr * 0.95;
	
	ForceGraph &forceGraph = _regionBase.forceGraph();
	
	// Scan all the vertex coordinates first
	BGL_FORALL_VERTICES( vd, forceGraph, ForceGraph ) {
			Coord2 coord = *forceGraph[ vd ].coordPtr;
			if( coord.x() < xMin ) xMin = coord.x();
			if( coord.x() > xMax ) xMax = coord.x();
			if( coord.y() < yMin ) yMin = coord.y();
			if( coord.y() > yMax ) yMax = coord.y();
		}
	
	// Normalize skeleton
	BGL_FORALL_VERTICES( vd, _skeletonForceGraph, ForceGraph ) {
			Coord2 coord;
			coord.x() =
					ratio * width * ( _skeletonForceGraph[ vd ].coordPtr->x() - xMin ) / ( xMax - xMin ) - 0.5 * width;
			coord.y() = ratio * height * ( _skeletonForceGraph[ vd ].coordPtr->y() - yMin ) / ( yMax - yMin ) -
			            0.5 * height;
			_skeletonForceGraph[ vd ].coordPtr->x() = coord.x();
			_skeletonForceGraph[ vd ].coordPtr->y() = coord.y();
			// cerr << coord;
		}
	
	// normalize forceGraph
	BGL_FORALL_VERTICES( vd, forceGraph, ForceGraph ) {
			Coord2 coord;
			coord.x() = width * ( forceGraph[ vd ].coordPtr->x() - xMin ) / ( xMax - xMin ) - 0.5 * width;
			coord.y() = height * ( forceGraph[ vd ].coordPtr->y() - yMin ) / ( yMax - yMin ) - 0.5 * height;
			forceGraph[ vd ].coordPtr->x() = coord.x();
			forceGraph[ vd ].coordPtr->y() = coord.y();
			// cerr << coord;
		}
}

void LevelBorder::buildBoundaryGraph( void ) {
	
	// create boundary graph after the composite graph is lay out
	// build polygon complex information
	_regionBase.createPolygonComplex( num_vertices( _skeletonForceGraph ) );
	// build vd associted in the boundary graph
	_regionBase.polygonComplexVD().clear();
	
	// initialization
	_octilinearBoundaryVec.clear();
	_octilinearBoundaryVec.resize( 1 );
	
	_octilinearBoundaryVec[ 0 ] = new Octilinear;
	
	BoundaryGraph &bg = _octilinearBoundaryVec[ 0 ]->boundary();
	unsigned int nVertices = num_vertices( bg );
	unsigned int nEdges = num_edges( bg );
	
	// create boundary graph
	for( unsigned int i = 0; i < _regionBase.polygonComplex().size(); i++ ) {
		
		Polygon2 &polygon = _regionBase.polygonComplex()[ i ];
		vector< ForceGraph::vertex_descriptor > vdVec;
		
		unsigned int size = polygon.elements().size();
		for( unsigned int j = 1; j < size + 1; j++ ) {
			
			// Check if the station exists or not
			BoundaryGraph::vertex_descriptor curVDS = NULL;
			BoundaryGraph::vertex_descriptor curVDT = NULL;
			
			// add vertices
			for( unsigned int k = 0; k < 2; k++ ) {
				
				BoundaryGraph::vertex_descriptor curVD = NULL;
				
				// Check if the station exists or not
				BGL_FORALL_VERTICES( vd, bg, BoundaryGraph ) {
						Coord2 &c = *bg[ vd ].coordPtr;
						if( fabs( ( polygon.elements()[ ( j - 1 + k ) % ( int ) size ] - c ).norm() ) < 1e-2 ) {

#ifdef DEBUG
							cerr << "The node has been in the database." << endl;
#endif  // DEBUG
							if( k == 0 ) curVD = curVDS = vd;
							if( k == 1 ) curVD = curVDT = vd;
							break;
						}
					}
				
				if( curVD == NULL ) {
					
					curVD = add_vertex( bg );
					//vector< unsigned int > lineID = bg[ curVD ].lineID;
					//lineID.push_back( nLines );
					
					double x = polygon.elements()[ j - 1 + k ].x();
					double y = polygon.elements()[ j - 1 + k ].y();
					bg[ curVD ].geoPtr = new Coord2( x, y );
					bg[ curVD ].smoothPtr = new Coord2( x, y );
					bg[ curVD ].coordPtr = new Coord2( x, y );
					
					bg[ curVD ].id = bg[ curVD ].initID = nVertices;
					bg[ curVD ].namePtr = new string( to_string( bg[ curVD ].id ) );
					bg[ curVD ].weight = 1.0;
					//bg[ curVD ].lineID.push_back( nLines );
					
					if( k == 0 ) curVDS = curVD;
					if( k == 1 ) curVDT = curVD;
					nVertices++;
				}
				else {
					//bg[ curVD ].lineID.push_back( nLines );
#ifdef DEBUG
					cerr << "[Existing] curV : lineID = " << endl;
				for ( unsigned int k = 0; k < vertexLineID[ curVD ].size(); ++k )
					cerr << "lineID[ " << setw( 3 ) << k << " ] = " << vertexLineID[ curVD ][ k ] << endl;
#endif  // DEBUG
				}
			}
			vdVec.push_back( curVDS );
			// cerr << _octilinearBoundaryVec[ curVDS ].id << " ";
			
			//cerr << "( " << _octilinearBoundaryVec[ curVDS ].id << ", " << _octilinearBoundaryVec[ curVDT ].id << " )" << endl;
			
			// add edges
			// search previous edge
			bool found = false;
			BoundaryGraph::edge_descriptor oldED;
			//BoundaryGraph::vertex_descriptor oldVS = ptrSta[ origID ];
			//BoundaryGraph::vertex_descriptor oldVT = ptrSta[ destID ];
			//unsigned int indexS = _octilinearBoundaryVec[ curVDS ].initID;
			//unsigned int indexT = _octilinearBoundaryVec[ curVDT ].initID;
			tie( oldED, found ) = edge( curVDS, curVDT, bg );
			
			
			if( found == true ) {
				
				//bg[ oldED ].lineID.push_back( nLines );
				bg[ oldED ].visitedTimes += 1;
				//eachline.push_back( oldED );
				//bool test = false;
				//tie( oldED, test ) = edge( oldVT, oldVS, _octilinearBoundaryVec );
			}
			else {
				
				//BoundaryGraph::vertex_descriptor src = vertex( indexS, _octilinearBoundaryVec );
				//BoundaryGraph::vertex_descriptor tar = vertex( indexT, _octilinearBoundaryVec );
				
				// handle fore edge
				pair< BoundaryGraph::edge_descriptor, unsigned int > foreE = add_edge( curVDS, curVDT, bg );
				BoundaryGraph::edge_descriptor foreED = foreE.first;
				
				// calculate geographical angle
				Coord2 coordO;
				Coord2 coordD;
				if( bg[ curVDS ].initID < bg[ curVDT ].initID ) {
					coordO = *bg[ curVDS ].coordPtr;
					coordD = *bg[ curVDT ].coordPtr;
				}
				else {
					coordO = *bg[ curVDT ].coordPtr;
					coordD = *bg[ curVDS ].coordPtr;
				}
				double diffX = coordD.x() - coordO.x();
				double diffY = coordD.y() - coordO.y();
				double angle = atan2( diffY, diffX );
				
				bg[ foreED ].initID = bg[ foreED ].id = nEdges;
				bg[ foreED ].weight = 1.0;
				bg[ foreED ].geoAngle = angle;
				bg[ foreED ].smoothAngle = angle;
				bg[ foreED ].angle = angle;
				//bg[ foreED ].lineID.push_back( nLines );
				bg[ foreED ].visitedTimes = 0;
				
				//eachline.push_back( foreED );
#ifdef  DEBUG
				cout << "nEdges = " << _nEdges << " angle = " << angle << endl;
#endif  // DEBUG
				nEdges++;
			}
		}
		_regionBase.polygonComplexVD().insert(
				pair< unsigned int, vector< ForceGraph::vertex_descriptor > >( i, vdVec ) );
	}
	
	// update the fixed flag
	BGL_FORALL_EDGES( ed, bg, BoundaryGraph ) {
			if( bg[ ed ].visitedTimes == 0 ) {
				
				BoundaryGraph::vertex_descriptor vdS = source( ed, bg );
				BoundaryGraph::vertex_descriptor vdT = target( ed, bg );
				
				bg[ vdS ].isFixed = true;
				bg[ vdT ].isFixed = true;
				
				// cerr << "eid = " << bg[ ed ].id << " node = " << bg[ vdS ]. id << " ," << bg[ vdT ].id << endl;
			}
		}
	
	_octilinearBoundaryVec[ 0 ]->prepare( *_content_widthPtr / 2.0, *_content_heightPtr / 2.0 );
}

void LevelBorder::updatePolygonComplex( void ) {
	
	// cerr << "updating high polygonComplex after optimization ..." << endl;
	
	BoundaryGraph &bg = _octilinearBoundaryVec[ 0 ]->boundary();
	map< unsigned int, vector< ForceGraph::vertex_descriptor > >::iterator itP;
	map< unsigned int, Polygon2 >::iterator itC = _regionBase.polygonComplex().begin();
	unsigned int id = 0;
	for( itP = _regionBase.polygonComplexVD().begin(); itP != _regionBase.polygonComplexVD().end(); itP++ ) {
		
		if( id == 4 ) cerr << "blue thing:" << endl;
		vector< ForceGraph::vertex_descriptor > &p = itP->second;
		for( unsigned int i = 0; i < p.size(); i++ ) {
			itC->second.elements()[ i ].x() = bg[ p[ i ] ].coordPtr->x();
			itC->second.elements()[ i ].y() = bg[ p[ i ] ].coordPtr->y();
			
			if( id == 4 ) cerr << "i = " << bg[ p[ i ] ].id << " " << itC->second.elements()[i];
		}
		itC++;
		id++;
	}
}


// end of header file
// Do not add any stuff under this line.
