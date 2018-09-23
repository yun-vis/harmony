//==============================================================================
// Force.cc
//	: program file for networks
//
//------------------------------------------------------------------------------
//
//				Date: Thu Sep  8 02:55:49 2016
//
//==============================================================================

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------

#include <ctime>

using namespace std;

#include <boost/format.hpp>
#include "base/Common.h"
#include "optimization/Force.h"

//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Private Functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Protected Functions
//------------------------------------------------------------------------------

//
//  Force::_init --	initialize the network (called once when necessary)
//
//  Inputs
//	none
//
//  Outputs
//	none
//
void Force::_init( Boundary * __boundary )
{
    // srand48( time( NULL ) );
    srand48( 3 );

    _boundaryPtr = __boundary;

    _center_x = 0.0;
    _center_y = 0.0;

    string configFilePath = "../configs/force.conf";
    int result;

    //read config file
    Base::Config conf( configFilePath );

    if ( conf.has( "ka" ) ){
        string paramKa = conf.gets( "ka" );
        _paramKa = stof( paramKa );
    }

    if ( conf.has( "kr" ) ){
        string paramKr = conf.gets( "kr" );
        _paramKr = stof( paramKr );
    }

    cerr << "filepath: " << configFilePath << endl;
    cerr << "ka: " << _paramKa << endl;
    cerr << "kr: " << _paramKr << endl;
}

//
//  Force::force --	compute the displacements exerted by the force-directed model
//
//  Inputs
//	none
//
//  Outputs
//	none
//
void Force::_force( void )
{
    SkeletonGraph & s = _boundaryPtr->skeleton();

    //double side = _window_side * SCREEN_SIDE;
    //double L = sqrt( SQUARE( side ) / ( double )max( 1.0, ( double )_nAnnotated ) );
    double L = sqrt( SQUARE( 1.0 ) / ( double )max( 1.0, ( double )num_vertices( s ) ) );

    BGL_FORALL_VERTICES( vdi, s, SkeletonGraph ) {

        Coord2 shift( 0.0, 0.0 );
        *s[ vdi ].forcePtr = shift;

        BGL_FORALL_VERTICES( vdj, s, SkeletonGraph ) {

            if ( vdi == vdj ) {
                ; // do nothing
            }
            else {

                Coord2 diff, unit;
                double dist;
                
                diff = *s[ vdj ].coordPtr - *s[ vdi ].coordPtr;
                dist = diff.norm();
                unit = diff.unit();
                
                SkeletonGraph::edge_descriptor	ed;
                bool isExisted;
                tie( ed, isExisted ) = edge( vdi, vdj, s );

                if ( isExisted ) {
                    // Drawing force by the spring
                    *s[ vdi ].forcePtr += _paramKa * ( dist - L ) * unit;
                }
                // Replusive force by Couloum's power
                *s[ vdi ].forcePtr -= ( _paramKr/SQUARE( dist ) ) * unit;
            }
        }
    }
}


//
//  Force::_gap --	return the convergence error in laying out the graph
//
//  Inputs
//	none
//
//  Outputs
//	error value
//
double Force::_gap( void )
{
    const char theName[] = "Force::_gap : ";
/*
    Graph &		g	= ( Graph & )*this;
    vector< Coord2 >	displace;
    double		err		= 0.0;
    // const double	scale		= TRANSFORMATION_STEP;
    const double	scale		= TRANSFORMATION_STEP*min(1.0,100.0/( double )num_vertices( *this ));
    const double	force		= CELL_RATIO;
    
    VertexIDMap		vertexID	= get( vertex_myid, g );
    VertexCoordMap	vertexCoord	= get( vertex_mycoord, g );
    VertexShiftMap	vertexShift	= get( vertex_myshift, g );
    VertexForceMap	vertexForce	= get( vertex_myforce, g );
    VertexPlaceMap	vertexPlace	= get( vertex_myplace, g );
    VertexSwitchMap	vertexSwitch	= get( vertex_myswitch, g );
    VertexOverlapMap	vertexOverlap	= get( vertex_myoverlap, g );
    VertexRatioMap	vertexRatio	= get( vertex_myratio, g );
    
    //cerr << "Force::_net->gp() = " << gp() << endl;
    //cerr << "Force::_net->clut() = " << clut() << endl;
    
    displace.clear();
    BGL_FORALL_VERTICES( vd, g, Graph ) {
        Coord2 val;
	switch ( _mode ) {
	  case 0:		// force-directed
	      val = vertexForce[ vd ];
	      break;
	  case 1:		// centroidal
	      val = force*vertexPlace[ vd ];
	      break;
	  case 2:		// hybrid simple & adaptive
	  case 3:
	      val = ratioF()*vertexForce[ vd ] + force*ratioV()*vertexPlace[ vd ];
	      break;
	  case 4:		// smoothing
	  case 5:
	      val = vertexRatio[ vd ]*vertexForce[ vd ] +
		  force*( 1.0 - vertexRatio[ vd ] )*vertexPlace[ vd ];
#ifdef DEBUG_SMOOTH
	      cerr << " force = " << vertexForce[ vd ];
	      cerr << " place = " << vertexPlace[ vd ];
	      cerr << " ratio = " << vertexRatio[ vd ] << " : " << 1.0 - vertexRatio[ vd ] << endl;
#endif	// DEBUG_SMOOTH
	      break;
	  default:
	      cerr << "Unexpected error mode" << endl;
	      break;
        }
	assert( vertexID[ vd ] == displace.size() );
        displace.push_back( val );
    }

    // Scale the displacement of a node at each scale
    const double limit = DISPLACEMENT_LIMIT;
    BGL_FORALL_VERTICES( vd, g, Graph ) {
	unsigned int idV = vertexID[ vd ];
	vertexShift[ vd ] = scale * displace[ idV ];
	double norm = vertexShift[ vd ].norm();
	if ( norm > limit ) {
	    vertexShift[ vd ] *= ( limit/norm );
	}
    }
    
    // Move the entire layout while keeping its barycenter at the origin of the
    // screen coordinates
    Coord2 average;
    average.zero();
    BGL_FORALL_VERTICES( vd, g, Graph ) {
        Coord2 newcoord = vertexCoord[ vd ] + vertexShift[ vd ];
        average += newcoord;
    }
    average /= ( double )num_vertices( g );
    
    BGL_FORALL_VERTICES( vd, g, Graph ) {
        vertexShift[ vd ] -= average;
    }
    
    // Force the vertices stay within the screen
    const double central = CONTENT_RATIO;
    // double central = 0.90;
    // double minX = -central * SCREEN_SIDE;
    // double minY = -central * SCREEN_SIDE;
    // double maxX =  central * SCREEN_SIDE;
    // double maxY =  central * SCREEN_SIDE;
    double minX = _center_x - central * _window_side * SCREEN_SIDE;
    double minY = _center_y - central * _window_side * SCREEN_SIDE;
    double maxX = _center_x + central * _window_side * SCREEN_SIDE;
    double maxY = _center_y + central * _window_side * SCREEN_SIDE;
    // cerr << "[ " << minX << " , " << maxX << " ]  x [ " << minY << " , " << maxY << " ] " << endl;
    BGL_FORALL_VERTICES( vd, g, Graph ) {
	if ( vertexSwitch[ vd ] ) {
	    Coord2 newcoord = vertexCoord[ vd ] + vertexShift[ vd ];
	    if ( newcoord.x() < minX )
		vertexShift[ vd ].x() += minX - newcoord.x();
	    if ( newcoord.x() > maxX )
		vertexShift[ vd ].x() += maxX - newcoord.x();
	    if ( newcoord.y() < minY )
		vertexShift[ vd ].y() += minY - newcoord.y();
	    if ( newcoord.y() > maxX )
		vertexShift[ vd ].y() += maxY - newcoord.y();
	}
    }

    BGL_FORALL_VERTICES( vd, g, Graph ) {
        vertexCoord[ vd ] += vertexShift[ vd ]; //vertexCoord[ vd ] renew;
        err += vertexShift[ vd ].squaredNorm();
    }

    return err/( double )num_vertices( g );
*/
}



void Force::_onestep( void )
{
/*
    Graph &		g	= ( Graph & )*this;
    VertexIDMap		vertexID	= get( vertex_myid, g );
    VertexCoordMap	vertexCoord	= get( vertex_mycoord, g );
    VertexShiftMap	vertexShift	= get( vertex_myshift, g );
    // VertexForceMap	vertexForce	= get( vertex_myforce, g );
    // VertexPlaceMap	vertexPlace	= get( vertex_myplace, g );
    VertexAspectMap     vertexAspect    = get( vertex_myaspect, g );
    VertexSwitchMap	vertexSwitch	= get( vertex_myswitch, g );
    VertexOverlapMap	vertexOverlap	= get( vertex_myoverlap, g );
    VertexRatioMap	vertexRatio	= get( vertex_myratio, g );
    vector< double >	postRatio;
    // vector< Coord2 >	displace;
    
    // const double	scale		= TRANSFORMATION_STEP;
    const double	scale		= TRANSFORMATION_STEP*min(1.0,100.0/( double )num_vertices( *this ));
    const double	force		= CELL_RATIO;
    unsigned int	nNeighbors = 0;
    multimap< double, VertexDescriptor > neighbor;

    // double Vnum =	num_vertices(*this);
    //double rad = 2.0*sqrt(2.0/Vnum);
    //double rad = 2.0*sqrt(2.0/Vnum)*window_side();
    // double rad = 2.0*sqrt(4.0/Vnum)*window_side();
    // cerr << "##############################" << "Force::_onestep" << endl;

    postRatio.clear();
    BGL_FORALL_VERTICES( vdi, g, Graph ) {
	double curRatio = 1.0;
	if ( vertexSwitch[ vdi ] ) {
	    nNeighbors = 0;
	    neighbor.clear();
	    BGL_FORALL_VERTICES( vdj, g, Graph ) {
		if ( vertexSwitch[ vdj ] && ( vdi != vdj ) ) {
#ifdef CHEBYSHEV_DISTANCE
		    // *** Chebychev distance
		    double aspect = vertexAspect[ vdi ];
		    Coord2 diff = vertexCoord[ vdj ] - vertexCoord[ vdi ];
		    double d = max( abs( diff.x() ), aspect * abs( diff.y() ) );
#else	// CHEBYSHEV_DISTANCE
		    // *** Euclidean distance
		    double d = ( vertexCoord[ vdi ] - vertexCoord[ vdj ] ).norm();
#endif	// CHEBYSHEV_DISTANCE
		    neighbor.insert( pair< double, VertexDescriptor >( d, vdj ) );
		    if ( nNeighbors < K_NEIGHBORS ) nNeighbors++;
		    else {
			multimap< double, VertexDescriptor >::iterator last = neighbor.end();
			last--;
			neighbor.erase( last );
		    }
		}
	    }
	    curRatio = 0.0;
	    // unsigned int counter = 0;
	    for ( multimap< double, VertexDescriptor >::iterator iter = neighbor.begin();
		  iter != neighbor.end(); ++iter ) {
		curRatio += vertexRatio[ iter->second ];
		// counter++;
	    }
	    // cerr << " nNeighbors = " << nNeighbors << " counter = " << counter << endl;
	    // assert( nNeighbors == counter );
#ifdef DEBUG_SMOOTH
	    cerr << " nAdj = " << nAdj;
#endif	// DEBUG_SMOOTH
	    if ( nNeighbors > 0 ) curRatio /= ( double )nNeighbors;
	    else {
#ifdef RESPONSIVE_CONTROL
		curRatio = 1.0; 
#else	// RESPONSIVE_CONTROL
		curRatio = vertexRatio[ vdi ];
#endif	// RESPONSIVE_CONTROL
	    }
	}
	postRatio.push_back( curRatio );
	assert( vertexID[ vdi ] == postRatio.size() - 1 );
    }

    BGL_FORALL_VERTICES( vd, g, Graph ) {
	if ( vertexSwitch[ vd ] ) {
	    vertexRatio[ vd ] = min( MAX_RATIO, max( MIN_RATIO, postRatio[ vertexID[ vd ] ] ) );
	}
    }
*/
}

//------------------------------------------------------------------------------
//	Public Functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------
//
//  Force::Force --	default constructor
//
//  Inputs
//	none
//
//  Outputs
//	none
//
Force::Force()
{
    //_init();
}


//
//  Force::Force --	copy constructor
//
//  Inputs
//	obj	: object of this class
//
//  Outputs
//	none
//
Force::Force( const Force & obj )
{
    //_init();
}


//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------

//
//  Force::~Force --	destructor
//
//  Inputs
//	none
//
//  Outputs
//	none
//
Force::~Force()
{

}


//------------------------------------------------------------------------------
//	Refering to members
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Assignment opereators
//------------------------------------------------------------------------------
//
//  Force::operator = --	assignement
//
//  Inputs
//	obj	: objects of this class
//
//  Outputs
//	this object
//
Force & Force::operator = ( const Force & obj )
{
    return *this;
}


//------------------------------------------------------------------------------
//	I/O functions
//------------------------------------------------------------------------------

//
//  operator << --	output
//
//  Argument
//	stream	: output stream
//	obj	: object of this class
//
//  Outputs
//	output stream
//
ostream & operator << ( ostream & stream, const Force & obj )
{
    // stream << ( const Graph & )obj;	
    return stream;
}


//
//  operator >> --	input
//
//  Inputs
//	stream	: input stream
//	obj	: object of this class
//
//  Outputs
//	input stream
//
istream & operator >> ( istream & stream, Force & obj )
{
    // stream >> ( Graph & )obj;
    return stream;
}


// end of header file
// Do not add any stuff under this line.
