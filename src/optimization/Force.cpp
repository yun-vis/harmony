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
void Force::_init( Boundary * __boundary, int __width, int __height )
{
    // srand48( time( NULL ) );
    srand48( 3 );

    _boundaryPtr = __boundary;

    _center_x = 0.0;
    _center_y = 0.0;
    _width = __width;
    _height = __height;
    _mode = TYPE_FORCE;

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

    if ( conf.has( "ratio_force" ) ){
        string paramRatioForce = conf.gets( "ratio_force" );
        _paramRatioForce = stof( paramRatioForce );
    }

    if ( conf.has( "ratio_voronoi" ) ){
        string paramRatioVoronoi = conf.gets( "ratio_voronoi" );
        _paramRatioVoronoi = stof( paramRatioVoronoi );
    }

    if ( conf.has( "transformation_step" ) ){
        string paramTransformationStep = conf.gets( "transformation_step" );
        _paramTransformationStep = stof( paramTransformationStep );
    }

    if ( conf.has( "cell_ratio" ) ){
        string paramCellRatio = conf.gets( "cell_ratio" );
        _paramCellRatio = stof( paramCellRatio );
    }

    if ( conf.has( "displacement_limit" ) ){
        string paramDisplacementLimit = conf.gets( "displacement_limit" );
        _paramDisplacementLimit = stof( paramDisplacementLimit );
    }

    if ( conf.has( "final_epsilon" ) ){
        string paramFinalEpsilon = conf.gets( "final_epsilon" );
        _paramFinalEpsilon = stof( paramFinalEpsilon );
    }

    cerr << "filepath: " << configFilePath << endl;
    cerr << "ka: " << _paramKa << endl;
    cerr << "kr: " << _paramKr << endl;
    cerr << "ratio_force: " << _paramRatioForce << endl;
    cerr << "ratio_voronoi: " << _paramRatioVoronoi << endl;
    cerr << "transformation_step: " << _paramTransformationStep << endl;
    cerr << "cell_ratio: " << _paramCellRatio << endl;
    cerr << "displacement_limit: " << _paramDisplacementLimit << endl;
    cerr << "final_epsilon: " << _paramFinalEpsilon << endl;
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

    double side = _width * _height;
    double L = sqrt( side / ( double )max( 1.0, ( double )num_vertices( s ) ) );
    //double L = sqrt( SQUARE( 1.0 ) / ( double )max( 1.0, ( double )num_vertices( s ) ) );

    // cerr << "nV = " << num_vertices(s) << " nE = " << num_edges(s) << endl;
    BGL_FORALL_VERTICES( vdi, s, SkeletonGraph ) {

        // initialization
        s[ vdi ].force.x() = 0.0;
        s[ vdi ].force.y() = 0.0;

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
                    s[ vdi ].force += _paramKa * ( dist - L ) * unit;
                }
                // Replusive force by Couloum's power
                s[ vdi ].force -= ( _paramKr/SQUARE( dist ) ) * unit;
            }
        }
    }

#ifdef DEBUG
    BGL_FORALL_VERTICES( vd, s, SkeletonGraph ) {
        cerr << "id[" << s[vd].id << "] = " << s[ vd ].force;
    }
#endif // DEBUG
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

    SkeletonGraph & s = _boundaryPtr->skeleton();
    vector< Coord2 >	displace;
    double		err		= 0.0;

    // const double	scale		= _paramTransformationStep;
    const double	scale		= _paramTransformationStep*min(1.0,100.0/( double )num_vertices( s ));
    const double	force		= _paramCellRatio;

    // initialization
    displace.clear();

    // apply forces
    BGL_FORALL_VERTICES( vd, s, SkeletonGraph ) {

        Coord2 val;
        switch ( _mode ) {
            case TYPE_FORCE:        // force-directed
                val = s[vd].force;
                break;
            case TYPE_CENTROID:        // centroidal
                val = force * s[vd].place;
                break;
            case TYPE_HYBRID:
                val = _paramRatioForce * s[vd].force + force * _paramRatioVoronoi * s[vd].place;
                break;
        }
        assert( s[vd].id == displace.size() );
        displace.push_back( val );
    }

    // Scale the displacement of a node at each scale
    const double limit = _paramDisplacementLimit;
    BGL_FORALL_VERTICES( vd, s, SkeletonGraph ) {
        unsigned int idV = s[ vd ].id;
        s[ vd ].shift = scale * displace[ idV ];
        double norm = s[ vd ].shift.norm();
        if ( norm > limit ) {
            s[ vd ].shift *= ( limit/norm );
        }
    }

    // Move the entire layout while keeping its barycenter at the origin of the
    // screen coordinates
    Coord2 average;
    average.zero();
    BGL_FORALL_VERTICES( vd, s, SkeletonGraph ) {
        Coord2 newcoord = *s[ vd ].coordPtr + s[ vd ].shift;
        average += newcoord;
    }
    average /= ( double )num_vertices( s );

    BGL_FORALL_VERTICES( vd, s, SkeletonGraph ) {
        s[ vd ].shift -= average;
    }

    // Force the vertices stay within the screen
    double minX = _center_x - 0.5 * _width;
    double minY = _center_y - 0.5 * _height;
    double maxX = _center_x + 0.5 * _width;
    double maxY = _center_y + 0.5 * _height;

    BGL_FORALL_VERTICES( vd, s, SkeletonGraph ) {
        Coord2 newcoord = *s[ vd ].coordPtr + s[ vd ].shift;
        if ( newcoord.x() < minX )
        s[ vd ].shift.x() += minX - newcoord.x();
        if ( newcoord.x() > maxX )
        s[ vd ].shift.x() += maxX - newcoord.x();
        if ( newcoord.y() < minY )
        s[ vd ].shift.y() += minY - newcoord.y();
        if ( newcoord.y() > maxX )
        s[ vd ].shift.y() += maxY - newcoord.y();
    }

#ifdef DEBUG
    cerr << "_width = " << _width << " _height = " << _height << endl;
    cerr << "cx = " << _center_x << " cy = " << _center_y << endl;
    cerr << "[ " << minX << " , " << maxX << " ]  x [ " << minY << " , " << maxY << " ] " << endl;
    for( unsigned int i = 0; i < displace.size(); i++ ){
        cerr << i << ": " << displace[i];
    }
    BGL_FORALL_VERTICES( vd, s, SkeletonGraph ) {
        cerr << "id[" << s[vd].id << "] = " << s[ vd ].shift;
    }
#endif // DEBUG

    BGL_FORALL_VERTICES( vd, s, SkeletonGraph ) {
        *s[ vd ].coordPtr += s[ vd ].shift; //vertexCoord[ vd ] renew;
        err += s[ vd ].shift.squaredNorm();
    }

    return err/( double )num_vertices( s );
}



void Force::_onestep( void )
{
/*
    Graph &		g	= ( Graph & )*this;
    
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
//	Referring to members
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Assignment operators
//------------------------------------------------------------------------------
//
//  Force::operator = --	assignment
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
