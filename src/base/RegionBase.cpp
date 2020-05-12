//==============================================================================
// RegionBase.cc
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

#include "base/RegionBase.h"


//------------------------------------------------------------------------------
//	Protected functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------
//
//  RegionBase::init -- initialize
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void RegionBase::_init( LEVELTYPE *__levelTypePtr, Polygon2 &__contour ) {
	
	_simpleInputContour = __contour;
	_levelTypePtr = __levelTypePtr;
	_force.init( &_forceGraph, &_simpleInputContour,
			_levelTypePtr, "config/" + Common::getBatchStr() + "/boundary.conf" );
}

//
//  RegionBase::clear --    clear the current RegionBase information
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void RegionBase::_clear( void ) {
	
	_polygonComplex.clear();
	_force.clear();
	_stress.clear();
}


//
//  RegionBase::createPolygonComplex --    create the ploygon complex
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void RegionBase::createPolygonComplex( unsigned int nCluster ) {

	vector< vector< Polygon2 > > _polygonMat;
	vector< Seed > &seedVec = *_force.voronoi().seedVec();
	
	// initialization
	// _polygonComplex.clear();
	
	// find the sets of the polygons of the same group
	_polygonMat.resize( nCluster );
	
#ifdef DEBUG
	cerr << "nCluster = " << nCluster << endl;
	cerr << "seedVec.size() = " << seedVec.size()
	     << " num_vertices( _forceGraph ) = " << num_vertices( _forceGraph ) << endl;
	assert( seedVec.size() == num_vertices( _forceGraph ) );
#endif // DEBUG
	
	for( unsigned int i = 0; i < seedVec.size(); i++ ) {
		
		int gid = _forceGraph[ vertex( i, _forceGraph ) ].initID;
		_polygonMat[ gid ].push_back( *seedVec[ i ].voronoiCellPtr );
	}
	
	for( unsigned int i = 0; i < _polygonMat.size(); i++ ) {
		
		int cSize = _polygonMat[ i ].size();
		if( cSize > 1 ) {
			
			Contour2 contour;
			vector< Polygon2 > pVec;
			for( unsigned int j = 0; j < _polygonMat[ i ].size(); j++ ) {
				Polygon2 &p = _polygonMat[ i ][ j ];
				pVec.push_back( p );
			}
			contour.init( i, pVec );
			contour.createContour();
			_polygonComplex.insert( pair< unsigned int, Polygon2 >( i, contour.contour() ) );
		}
		else {
			Polygon2 &p = _polygonMat[ i ][ 0 ];
			p.updateOrientation();
			_polygonComplex.insert( pair< unsigned int, Polygon2 >( i, p ) );
		}
	}

#ifdef DEBUG
	map< unsigned int, Polygon2 >::iterator itC = _polygonComplex.begin();
	for( ; itC != _polygonComplex.end(); itC++ ){
		Polygon2 &p = itC->second;
		for( unsigned int i = 0; i < p.elements().size(); i++ ){
			cerr << p.elements()[i] << " ";
		}
	}
	cerr << endl;
#endif // DEBUG
}


//
//  RegionBase::RegionBase -- default constructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
RegionBase::RegionBase( void ) {
	
	clearGraph( _forceGraph );
	_energyType = ENERGY_FORCE;
}

//
//  RegionBase::RegionBase -- copy constructor
//
//  Inputs
//  obj : object of this class
//
//  Outputs
//  none
//
RegionBase::RegionBase( const RegionBase &obj ) {
}


//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------
//
//  RegionBase::~RegionBase --    destructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
RegionBase::~RegionBase( void ) {
}


// end of header file
// Do not add any stuff under this line.
