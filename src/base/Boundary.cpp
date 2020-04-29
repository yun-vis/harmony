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
void Boundary::_init( void ) {
	double sumL = 0.0;
	
	// compute distanceBeta
	BGL_FORALL_EDGES( ed, _boundary, BoundaryGraph ) {
			BoundaryGraph::vertex_descriptor vdS = source( ed, _boundary );
			BoundaryGraph::vertex_descriptor vdT = target( ed, _boundary );
			
			double dist = ( *_boundary[ vdS ].coordPtr - *_boundary[ vdT ].coordPtr ).norm();
			sumL += dist;
		}
	
	_distAlpha = sumL / ( double ) num_edges( _boundary );
	
	_contour->area();
	
	_nVertices = num_vertices( _boundary );
	_nEdges = num_edges( _boundary );
}

//
//  Boundary::clear -- initialize
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Boundary::_clear( void ) {
	clearGraph( _boundary );
	
	//_shortestPathM.clear();
	_line.clear();
	_lineSta.clear();
	
	_nLines = 0;
	_nVertices = 0;
	_nEdges = 0;
	_meanVSize = 0.0;
	
	_removedVertices.clear();
	_removedEdges.clear();
	_removedWeights.clear();
	
	_VEconflict.clear();
	_ratioR.clear();
}

//
//  Boundary::adjustsize --  adjust size of the layout of the LevelBorder network
//
//  Inputs
//      width: window width
//      height: window height
//
//  Outputs
//      none
//
void Boundary::adjustsize( const int &width, const int &height ) {
	// for vertexGeo
	double xMin = INFINITY;
	double xMax = -INFINITY;
	double yMin = INFINITY;
	double yMax = -INFINITY;
	double aspect = ( double ) width / ( double ) height;
	
	// Scan all the vertex coordinates first
	BGL_FORALL_VERTICES( vertex, _boundary, BoundaryGraph ) {
			Coord2 coord = *_boundary[ vertex ].geoPtr;
			if( coord.x() < xMin ) xMin = coord.x();
			if( coord.x() > xMax ) xMax = coord.x();
			if( coord.y() < yMin ) yMin = coord.y();
			if( coord.y() > yMax ) yMax = coord.y();
		}
	
	// double range = 0.5 * MAX2( xMax - xMin, yMax - yMin );
	double xRange;
	double yRange;
	double xMid;
	double yMid;
	if( ( xMax - xMin ) / width > ( yMax - yMin ) / height ) {
		xRange = 0.5 * ( xMax - xMin );
		yRange = 0.5 * ( xMax - xMin ) * ( 1.0 / aspect );
	}
	else {
		xRange = 0.5 * ( yMax - yMin ) * aspect;
		yRange = 0.5 * ( yMax - yMin );
	}
	
	xRange *= 1.05;
	yRange *= 1.05;
	xMid = 0.5 * ( xMin + xMax );
	yMid = 0.5 * ( yMin + yMax );
	
	// Normalize the coordinates
	BGL_FORALL_VERTICES( vertex, _boundary, BoundaryGraph ) {
			Coord2 geo = *_boundary[ vertex ].geoPtr;
			Coord2 smooth = *_boundary[ vertex ].smoothPtr;
			Coord2 coord = *_boundary[ vertex ].coordPtr;
			
			geo.setX( width * ( geo.x() - xMid ) / xRange );
			geo.setY( height * ( geo.y() - yMid ) / yRange );
			smooth.setX( width * ( smooth.x() - xMid ) / xRange );
			smooth.setY( height * ( smooth.y() - yMid ) / yRange );
			coord.setX( width * ( coord.x() - xMid ) / xRange );
			coord.setY( height * ( coord.y() - yMid ) / yRange );
			
			_boundary[ vertex ].geoPtr->x() = geo.x();
			_boundary[ vertex ].geoPtr->y() = geo.y();
			_boundary[ vertex ].smoothPtr->x() = smooth.x();
			_boundary[ vertex ].smoothPtr->y() = smooth.y();
			_boundary[ vertex ].coordPtr->x() = coord.x();
			_boundary[ vertex ].coordPtr->y() = coord.y();
			
		}
	
	// compute the unit length of an edge (ratio)
	int nAlpha = 0;
	int nBeta = 0;
	double totallength = 0.0;
	BGL_FORALL_EDGES( edge, _boundary, BoundaryGraph ) {
			BoundaryGraph::vertex_descriptor vdS = source( edge, _boundary );
			BoundaryGraph::vertex_descriptor vdT = target( edge, _boundary );
			
			Coord2 coord = *_boundary[ vdT ].geoPtr - *_boundary[ vdS ].geoPtr;
			totallength += coord.norm();
			double w = _boundary[ edge ].weight;
			if( w == 1.0 ) nBeta++;
			else nAlpha++;
		}
	double magLength = 2.0;
	_distBeta = totallength / ( magLength * nAlpha + nBeta );
	_distAlpha = magLength * _distBeta;
	
	// cerr << "distBeta = " << _distBeta << endl;
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
bool Boundary::checkVEConflicts( void ) {
	bool doConflict = false;
	
	_VEconflict.clear();
	_VEconflict.clear();
	
	// find the vertex-edge pair that has smaller distance than the threshold
	BGL_FORALL_VERTICES( vertex, _boundary, BoundaryGraph ) {
			double distThreshold = _distBeta / 2.0;
			
			Coord2 coord = *_boundary[ vertex ].smoothPtr;
			unsigned idV = _boundary[ vertex ].id;
			
			BGL_FORALL_EDGES( edge, _boundary, BoundaryGraph ) {
					unsigned idE = _boundary[ edge ].id;
					BoundaryGraph::vertex_descriptor vdS = source( edge, _boundary );
					BoundaryGraph::vertex_descriptor vdT = target( edge, _boundary );
					
					if( ( vertex != vdS ) && ( vertex != vdT ) ) {
						
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
						if( fabs( p.y() - m * p.x() - k ) > 1.0e-8 ) {
							p = coord - dist * vec;
#ifdef  DEBUG
							cerr << "p.y() - m * p.x() + k = " << p.y() - m * p.x() - k << endl;
					cerr << "s.y() - m * s.x() + k = " << coordS.y() - m * coordS.x() - k << endl;
					cerr << "t.y() - m * t.x() + k = " << coordT.y() - m * coordT.x() - k << endl;
#endif  // DEBUG
						}
						double r = ( p - coordT ).x() / ( coordS - coordT ).x();
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
	for( VEMap::iterator it = _VEconflict.begin();
	     it != _VEconflict.end(); ++it ) {
		BoundaryGraph::vertex_descriptor vdV = it->second.first;
		BoundaryGraph::edge_descriptor ed = it->second.second;
		BoundaryGraph::vertex_descriptor vdS = source( ed, _boundary );
		BoundaryGraph::vertex_descriptor vdT = target( ed, _boundary );
		Coord2 &pointV = *_boundary[ vdV ].smoothPtr;
		Coord2 &pointS = *_boundary[ vdS ].smoothPtr;
		Coord2 &pointT = *_boundary[ vdT ].smoothPtr;
		
		double m = ( pointS.y() - pointT.y() ) / ( pointS.x() - pointT.x() );
		double k = pointS.y() - m * pointS.x();
		double dist = fabs( m * pointV.x() - pointV.y() + k ) / sqrt( SQUARE( m ) + 1.0 );
		Coord2 vec, p;
		vec.x() = ( pointS - pointT ).y();
		vec.y() = -( pointS - pointT ).x();
		vec = vec / vec.norm();
		p = pointV + dist * vec;
		if( fabs( p.y() - m * p.x() - k ) > 1.0e-8 ) {
			p = pointV - dist * vec;
		}
		double r = ( p - pointT ).x() / ( pointS - pointT ).x();
		
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
//  Boundary::Boundary -- default constructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
Boundary::Boundary( void ) {
	clearGraph( _boundary );
	_line.clear();
	_lineSta.clear();
	
	_nLines = 0;
	_nVertices = 0;
	_nEdges = 0;
	_meanVSize = 0.0;
	
	_removedVertices.clear();
	_removedEdges.clear();
	_removedWeights.clear();
	
	_VEconflict.clear();
	_ratioR.clear();
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
Boundary::Boundary( const Boundary &obj ) {
	_boundary = obj.boundary();
	
	//_shortestPathM = obj.spM();
	_line = obj.line();
	_lineSta = obj.lineSta();
	
	_nLines = obj._nLines;
	_nVertices = obj._nVertices;
	_nEdges = obj._nEdges;
	_meanVSize = obj._meanVSize;
	
	_removedVertices = obj.removedVertices();
	_removedEdges = obj.removedEdges();
	_removedWeights = obj.removedWeights();
	
	_VEconflict = obj.VEconflict();
	_ratioR = obj.ratioR();
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
Boundary::~Boundary( void ) {
	clearGraph( _boundary );
	
	//_shortestPathM.clear();
	_line.clear();
	_lineSta.clear();
	
	_nLines = 0;
	_nVertices = 0;
	_nEdges = 0;
	_meanVSize = 0.0;
	
	_removedVertices.clear();
	_removedEdges.clear();
	_removedWeights.clear();
	
	//_VEconflict.clear();
	_ratioR.clear();
}


// end of header file
// Do not add any stuff under this line.
