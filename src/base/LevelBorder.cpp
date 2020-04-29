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
                         double *veCoveragePtr, Polygon2 *contourPtr,
                         SkeletonGraph &skeletonGraph ) {
	// initialize variables
	_content_widthPtr = widthPtr;
	_content_heightPtr = heightPtr;
	_veCoveragePtr = veCoveragePtr;
	
	RegionBase::init();
	clearGraph( _skeletonForceGraph );
	
	// initialize force
	_force.init( &_forceGraph, contourPtr, LEVEL_HIGH, "config/boundary.conf" );
	
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
	normalizeSkeleton();
	decomposeSkeleton();
	normalizeBone();
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
void LevelBorder::decomposeSkeleton( void ) {
	double labelArea = 0.0;
	unsigned int index = num_vertices( _skeletonForceGraph );
	
	// copy skeletonForceGraph to composite
	BGL_FORALL_VERTICES( vd, _skeletonForceGraph, ForceGraph ) {
			ForceGraph::vertex_descriptor vdNew = add_vertex( _forceGraph );
			_forceGraph[ vdNew ].id = _skeletonForceGraph[ vd ].id;
			_forceGraph[ vdNew ].initID = _skeletonForceGraph[ vd ].initID;
			
			_forceGraph[ vdNew ].coordPtr = new Coord2( _skeletonForceGraph[ vd ].coordPtr->x(),
			                                            _skeletonForceGraph[ vd ].coordPtr->y() );
			_forceGraph[ vdNew ].prevCoordPtr = new Coord2( _skeletonForceGraph[ vd ].coordPtr->x(),
			                                                _skeletonForceGraph[ vd ].coordPtr->y() );
			_forceGraph[ vdNew ].forcePtr = new Coord2( 0, 0 );
			_forceGraph[ vdNew ].placePtr = new Coord2( 0, 0 );
			_forceGraph[ vdNew ].shiftPtr = new Coord2( 0, 0 );
			_forceGraph[ vdNew ].weight = 0.0;
			
			_forceGraph[ vdNew ].widthPtr = new double( *_skeletonForceGraph[ vd ].widthPtr );
			_forceGraph[ vdNew ].heightPtr = new double( *_skeletonForceGraph[ vd ].heightPtr );
			_forceGraph[ vdNew ].areaPtr = new double( *_skeletonForceGraph[ vd ].areaPtr );
			
			// cerr << "bonearea = " << *_forceGraph[ vdNew ].areaPtr << endl;
			labelArea += *_forceGraph[ vdNew ].areaPtr;
		}
	BGL_FORALL_EDGES( ed, _skeletonForceGraph, ForceGraph ) {
			ForceGraph::vertex_descriptor vdS = source( ed, _skeletonForceGraph );
			ForceGraph::vertex_descriptor vdT = target( ed, _skeletonForceGraph );
			ForceGraph::vertex_descriptor vdCompoS = vertex( _skeletonForceGraph[ vdS ].id, _forceGraph );
			ForceGraph::vertex_descriptor vdCompoT = vertex( _skeletonForceGraph[ vdT ].id, _forceGraph );
			
			// add edge
			pair< ForceGraph::edge_descriptor, unsigned int > foreE = add_edge( vdCompoS, vdCompoT, _forceGraph );
			BoundaryGraph::edge_descriptor foreED = foreE.first;
			_forceGraph[ foreED ].id = _skeletonForceGraph[ ed ].id;
		}
	
	// split large vertices
	double unit = labelArea / 40.0;
	// double unit = 8000.0;

#ifdef DEBUG
	cerr << "labelArea = " << labelArea
		 << " veCoverage = " << *_veCoveragePtr << " unit = " << unit << endl;
	cerr << "nV = " << num_vertices( _forceGraph ) << " nE = " << num_edges( _forceGraph ) << endl;
#endif // DEBUG
	
	BGL_FORALL_VERTICES( vd, _forceGraph, ForceGraph ) {
			int mag = round( *_forceGraph[ vd ].areaPtr / unit );
			// int mag = round((*_forceGraph[ vd ].widthPtr) * (*_forceGraph[ vd ].heightPtr)/unit);
			
			// avoid from looping over the size of _skeletonForceGraph
			if( _forceGraph[ vd ].id >= num_vertices( _skeletonForceGraph ) ) break;

#ifdef DEBUG
			cerr << "area = " << *_forceGraph[ vd ].areaPtr << " mag = " << mag << " unit = " << unit << endl;
#endif // DEBUG
			
			vector< ForceGraph::edge_descriptor > removeEVec;
			// double radius = 25.0; // tiny
			double radius = 100.0;
			
			if( mag > 1 ) {
				
				ForceGraph::vertex_descriptor vdC = vd;
				ForceGraph::out_edge_iterator eo, eo_end;
				Coord2 origin = *_forceGraph[ vd ].coordPtr;
				vector< ForceGraph::vertex_descriptor > extendVD;
				
				// record the adjacent vertices for edge connection
				map< unsigned int, ForceGraph::vertex_descriptor > vdAdjacent;
				// cerr << "degree = " << out_degree( vd, _forceGraph ) << endl;
				for( tie( eo, eo_end ) = out_edges( vd, _forceGraph ); eo != eo_end; eo++ ) {
					
					ForceGraph::edge_descriptor ed = *eo;
					ForceGraph::vertex_descriptor vdT = target( ed, _forceGraph );
					vdAdjacent.insert(
							pair< unsigned int, ForceGraph::vertex_descriptor >( _forceGraph[ ed ].id, vdT ) );
					//remove_edge( *eo, _forceGraph );
					removeEVec.push_back( ed );
				}
				
				// add composite vertices
				for( int i = 0; i < mag; i++ ) {
					
					double cosTheta = 1.0 * cos( 2.0 * M_PI * ( double ) i / ( double ) mag );
					double sinTheta = 1.0 * sin( 2.0 * M_PI * ( double ) i / ( double ) mag );
					double x = origin.x() + radius * cosTheta;
					double y = origin.y() + radius * sinTheta;
					
					if( i == 0 ) {
						
						_forceGraph[ vd ].coordPtr->x() = x;
						_forceGraph[ vd ].coordPtr->y() = y;
						_forceGraph[ vd ].weight = *_forceGraph[ vd ].areaPtr;
						
						// post processing
						extendVD.push_back( vdC );
					}
					else {
						
						// cerr << "index = " << index << " area = " << *_forceGraph[ vd ].areaPtr << " mag = " << mag << " unit = " << unit << endl;
						
						// add vertex
						ForceGraph::vertex_descriptor vdNew = add_vertex( _forceGraph );
						_forceGraph[ vdNew ].id = index;
						_forceGraph[ vdNew ].initID = _forceGraph[ vd ].initID;
						
						_forceGraph[ vdNew ].coordPtr = new Coord2( x, y );
						_forceGraph[ vdNew ].prevCoordPtr = new Coord2( x, y );
						_forceGraph[ vdNew ].forcePtr = new Coord2( 0, 0 );
						_forceGraph[ vdNew ].placePtr = new Coord2( 0, 0 );
						_forceGraph[ vdNew ].shiftPtr = new Coord2( 0, 0 );
						_forceGraph[ vdNew ].weight = *_forceGraph[ vd ].areaPtr;
						
						_forceGraph[ vdNew ].widthPtr = new double( sqrt( unit ) );
						_forceGraph[ vdNew ].heightPtr = new double( sqrt( unit ) );
						_forceGraph[ vdNew ].areaPtr = new double( unit );
						
						// add edge
						pair< ForceGraph::edge_descriptor, unsigned int > foreE = add_edge( vdC, vdNew, _forceGraph );
						BoundaryGraph::edge_descriptor foreED = foreE.first;
						_forceGraph[ foreED ].id = num_edges( _forceGraph );
						_forceGraph[ foreED ].weight = 2.0 * ( *_forceGraph[ vd ].areaPtr );
						
						// add last edge to form a circle
						if( i == mag - 1 ) {
							pair< ForceGraph::edge_descriptor, unsigned int > foreE = add_edge( vd, vdNew,
							                                                                    _forceGraph );
							BoundaryGraph::edge_descriptor foreED = foreE.first;
							_forceGraph[ foreED ].id = num_edges( _forceGraph );
							_forceGraph[ foreED ].weight = 2.0 * ( *_forceGraph[ vd ].areaPtr );
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
				for( itA = vdAdjacent.begin(); itA != vdAdjacent.end(); itA++ ) {
					
					ForceGraph::vertex_descriptor cloestVD = extendVD[ 0 ];
					ForceGraph::vertex_descriptor vdT = itA->second;
					double minDist = ( *_forceGraph[ cloestVD ].coordPtr - *_forceGraph[ vdT ].coordPtr ).norm();
					for( unsigned int i = 1; i < extendVD.size(); i++ ) {
						
						ForceGraph::vertex_descriptor vdS = extendVD[ i ];
						double dist = ( *_forceGraph[ vdS ].coordPtr - *_forceGraph[ vdT ].coordPtr ).norm();
						
						if( dist < minDist ) {
							cloestVD = extendVD[ i ];
							minDist = dist;
						}
					}
					
					pair< ForceGraph::edge_descriptor, unsigned int > foreE = add_edge( vdT, cloestVD, _forceGraph );
					ForceGraph::edge_descriptor foreED = foreE.first;
					_forceGraph[ foreED ].id = itA->first;
					_forceGraph[ foreED ].weight = _forceGraph[ vdT ].weight + _forceGraph[ cloestVD ].weight;
				}
				
				// remove edges
				// cerr << "eSize = " << removeEVec.size() << endl;
				for( unsigned int i = 0; i < removeEVec.size(); i++ ) {
					// cerr << "eID = " << _forceGraph[removeEVec[i]].id << endl;
					remove_edge( removeEVec[ i ], _forceGraph );
				}
			}
			else {
				_forceGraph[ vd ].weight = *_forceGraph[ vd ].areaPtr;
			}
		}

#ifdef DEBUG
	BGL_FORALL_VERTICES( vd, _forceGraph, ForceGraph )
	{
		cerr << "vID = " << _forceGraph[vd].id << endl;
		cerr << "weight = " << _forceGraph[vd].weight << endl;
	}
	cerr << endl;
	BGL_FORALL_EDGES( ed, _forceGraph, ForceGraph )
	{
		cerr << "eID = " << _forceGraph[ed].id << endl;
		cerr << "weight = " << _forceGraph[ed].weight << endl;
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
void LevelBorder::normalizeSkeleton( void ) {
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
//  Package::normalizeBone --    normalize the forceGraph
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void LevelBorder::normalizeBone( void ) {
	// initialization
	double xMin = INFINITY;
	double xMax = -INFINITY;
	double yMin = INFINITY;
	double yMax = -INFINITY;
	double ratio = 1.0;
	// double ratio = 0.8;
	//cerr << "w = " << width << " h = " << height << endl;
	double width = *_content_widthPtr;
	double height = *_content_heightPtr;
	
	// Scan all the vertex coordinates first
	BGL_FORALL_VERTICES( vd, _forceGraph, ForceGraph ) {
			Coord2 coord = *_forceGraph[ vd ].coordPtr;
			if( coord.x() < xMin ) xMin = coord.x();
			if( coord.x() > xMax ) xMax = coord.x();
			if( coord.y() < yMin ) yMin = coord.y();
			if( coord.y() > yMax ) yMax = coord.y();
		}
	
	// Normalize the coordinates
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
	
	BGL_FORALL_VERTICES( vd, _forceGraph, ForceGraph ) {
			Coord2 coord;
			coord.x() = width * ( _forceGraph[ vd ].coordPtr->x() - xMin ) / ( xMax - xMin ) - 0.5 * width;
			coord.y() = height * ( _forceGraph[ vd ].coordPtr->y() - yMin ) / ( yMax - yMin ) - 0.5 * height;
			_forceGraph[ vd ].coordPtr->x() = coord.x();
			_forceGraph[ vd ].coordPtr->y() = coord.y();
			// cerr << coord;
		}
}


#ifdef SKIP
//
//  LevelBorder::writePolygonComplex --    export the ploygon complex
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void LevelBorder::writePolygonComplex( void )
{
	// graphml
	string filename = "data/LevelBorder.graphml";

	ofstream ofs( filename.c_str() );
	if ( !ofs ) {
		cerr << "Cannot open the target file : " << filename << endl;
		return;
	}

	UndirectedPropertyGraph pg;

#ifdef DEBUG
	cerr << "LevelBorder::writePolygonComplex: "
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
	string polygon_filename = "data/LevelBorder.txt";

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
//  LevelBorder::readPolygonComplex --    read the ploygon complex
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void LevelBorder::readPolygonComplex( void )
{
	string filename = "data/LevelBorder.graphml";

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
	string polygon_filename = "data/LevelBorder.txt";

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
	cerr << "LevelBorder::readPolygonComplex:"
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
#endif // SKIP

// end of header file
// Do not add any stuff under this line.
