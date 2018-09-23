// Voronoi.cpp
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
#include "voronoi/Voronoi.h"

//------------------------------------------------------------------------------
//	Protected functions
//------------------------------------------------------------------------------
//
//  Voronoi::clear --        memory management
//
//  Inputs
//      none
//
//  Outputs
//      none
//
void Voronoi::clear( void )
{
}

//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------
//
//  Voronoi::create --        create voronoi diagram
//
//  Inputs
//      none
//
//  Outputs
//      none
//
void Voronoi::createVoronoiDiagram( void )
{
    vector< RT2::Weighted_point > wpoints;

    // initialization
    _polyVec2D.clear();
    _polygonVecPtr->clear();
    wpoints.clear();

    // copy points
    //cerr << "points:" << endl;
    for( unsigned int i = 0; i < _seedVecPtr->size(); i++ ){

        wpoints.push_back( RT2::Weighted_point( K::Point_2( (*_seedVecPtr)[i].x(), (*_seedVecPtr)[i].y() ), 0 ) );
        //cerr << (*_seedVecPtr)[i];
    }

    //Find the bounding box of the points. This will be used to crop the Voronoi
    //diagram later.
    //const K::Iso_rectangle_2 bbox = CGAL::bounding_box( wpoints.begin(), wpoints.end() );
    const K::Iso_rectangle_2 bbox( K::Point_2( -(int)_width/2, -(int)_height/2 ), K::Point_2( (int)_width/2, (int)_height/2 ) );
    //cerr << "bbox = " << bbox << endl;

    //Create a Regular Triangulation from the points
    RT2 rt( wpoints.begin(), wpoints.end() );
    rt.is_valid();

    //Wrap the triangulation with a Voronoi diagram adaptor. This is necessary to
    //get the Voronoi faces.
    VD vd( rt );

    //CGAL often returns objects that are either segments or rays. This converts
    //these objects into segments. If the object would have resolved into a ray,
    //that ray is intersected with the bounding box defined above and returned as
    //a segment.
    const auto ConvertToSeg = [&](const CGAL::Object seg_obj, bool outgoing) -> K::Segment_2 {
        //One of these will succeed and one will have a NULL pointer
        const K::Segment_2 *dseg = CGAL::object_cast<K::Segment_2>(&seg_obj);
        const K::Ray_2     *dray = CGAL::object_cast<K::Ray_2>(&seg_obj);
        if (dseg) { //Okay, we have a segment
            return *dseg;
        } else {    //Must be a ray
            const auto &source = dray->source();
            const auto dsx     = source.x();
            const auto dsy     = source.y();
            const auto &dir    = dray->direction();
            const auto tpoint  = K::Point_2(dsx+RAY_LENGTH*dir.dx(),dsy+RAY_LENGTH*dir.dy());
            if( outgoing )
                return K::Segment_2( dray->source(), tpoint );
            else
                return K::Segment_2( tpoint, dray->source() );
        }
    };

    //Loop over the faces of the Voronoi diagram in some arbitrary order (does not match with seed order)
    int fnum = 0;
    for( VD::Face_iterator fit = vd.faces_begin(); fit!=vd.faces_end(); ++fit, fnum++ ){

        CGAL::Polygon_2< K > pgon;

        //Edge circulators traverse endlessly around a face. Make a note of the
        //starting point so we know when to quit.
        VD::Face::Ccb_halfedge_circulator ec_start = fit->ccb();

        //Find a bounded edge to start on
        //for(;ec_start->is_unbounded();ec_start++){}


        //Current location of the edge circulator
        VD::Face::Ccb_halfedge_circulator ec = ec_start;


        do {
            //A half edge circulator representing a ray doesn't carry direction
            //information. To get it, we take the dual of the dual of the half-edge.
            //The dual of a half-edge circulator is the edge of a Delaunay triangle.
            //The dual of the edge of Delaunay triangle is either a segment or a ray.
            // const CGAL::Object seg_dual = rt.dual(ec->dual());
            const CGAL::Object seg_dual = vd.dual().dual(ec->dual());

            //Convert the segment/ray into a segment
            const auto this_seg = ConvertToSeg(seg_dual, ec->has_target());

            pgon.push_back( this_seg.source() );

            //If the segment has no target, it's a ray. This means that the next
            //segment will also be a ray. We need to connect those two rays with a
            //segment. The following accomplishes this.
            if(!ec->has_target()){
                const CGAL::Object nseg_dual = vd.dual().dual(ec->next()->dual());
                const auto next_seg = ConvertToSeg(nseg_dual, ec->next()->has_target());
                pgon.push_back(next_seg.target());
            }

        } while ( ++ec != ec_start ); //Loop until we get back to the beginning

        //In order to crop the Voronoi diagram, we need to convert the bounding box
        //into a polygon. You'd think there'd be an easy way to do this. But there
        //isn't (or I haven't found it).
        CGAL::Polygon_2< K > bpoly;
        bpoly.push_back( K::Point_2( bbox.xmin(), bbox.ymin() ));
        bpoly.push_back( K::Point_2( bbox.xmax(), bbox.ymin() ));
        bpoly.push_back( K::Point_2( bbox.xmax(), bbox.ymax() ));
        bpoly.push_back( K::Point_2( bbox.xmin(), bbox.ymax() ));

        //Perform the intersection. Since CGAL is very general, it believes the
        //result might be multiple polygons with holes.
        std::list<CGAL::Polygon_with_holes_2< K >> isect;
        CGAL::intersection( pgon, bpoly, std::back_inserter(isect) );

        //But we know better. The intersection of a convex polygon and a box is
        //always a single polygon without holes. Let's assert this.
        assert( isect.size()==1 );

        //And recover the polygon of interest
        auto &poly_w_holes = isect.front();
        auto &poly_outer   = poly_w_holes.outer_boundary();

        //Print the polygon as a WKT polygon
        vector< K::Point_2 > p;
        //cout << fnum << ", ""\"POLYGON ((" ;
        for( auto v=poly_outer.vertices_begin(); v!=poly_outer.vertices_end(); v++ ){

            // cout << v->x() << " " << v->y() << ", ";
            ostringstream ostr;

            // x
            ostr.str( "" );
            ostr << v->x();
            double x = boost::lexical_cast< double >( ostr.str().c_str() );

            // y
            ostr.str( "" );
            ostr << v->y();
            double y = boost::lexical_cast< double >( ostr.str().c_str() );

            // cerr << setprecision(20) << "x = " << x << " y = " << y << endl;
            p.push_back( K::Point_2( x, y ) );
        }

        // add the end points to generate an enclosed polygon
        p.push_back( p[0] );
        p.push_back( p[0] );
        // std::cout<<poly_outer.vertices_begin()->x()<<" "<<poly_outer.vertices_begin()->y()<<"))\"\n";
        _polyVec2D.push_back( p );

    }
    mapSeedsandPolygons();
}

//
//  Voronoi::mapSeedsandPolygons -- map seeds and polygons
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Voronoi::mapSeedsandPolygons( void )
{
    for( unsigned int i = 0; i < _seedVecPtr->size(); i++ ){

        K::Point_2 pt( (*_seedVecPtr)[i].x(), (*_seedVecPtr)[i].y() );

        // find appropriate polygon boundary
        for( unsigned int m = 0; m < _polyVec2D.size(); m++ ) {

            K::Point_2 *points = new K::Point_2[ _polyVec2D[m].size() ];
            vector< Coord2 > coords;
            for( unsigned int n = 0; n < _polyVec2D[m].size(); n++ ) {

                // copy to points
                points[n] = K::Point_2( _polyVec2D[m][n].x(), _polyVec2D[m][n].y() );

                // copy to coords
                ostringstream ostr;
                // x
                ostr.str( "" );
                ostr << _polyVec2D[m][n].x();
                double x = boost::lexical_cast< double >( ostr.str().c_str() );

                // y
                ostr.str( "" );
                ostr << _polyVec2D[m][n].y();
                double y = boost::lexical_cast< double >( ostr.str().c_str() );

                coords.push_back( Coord2( x, y ) );
            }

            CGAL::Bounded_side bside = CGAL::bounded_side_2( points, points+_polyVec2D[m].size(), pt );

            if (bside == CGAL::ON_BOUNDED_SIDE) {
                Polygon2 poly( coords );
                (*_polygonVecPtr).insert( pair< unsigned int, Polygon2 >( i, poly ) );
                // cout << " is inside the polygon.\n";
                // point inside
            } else if (bside == CGAL::ON_BOUNDARY) {
                //cout << " is on the polygon boundary.\n";
                // point on the border
            } else if (bside == CGAL::ON_UNBOUNDED_SIDE) {
                //cout << " is outside the polygon.\n";
                // point outside
            }

            delete [] points;
        }
    }
}

//
//  Voronoi::Voronoi -- default constructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
Voronoi::Voronoi( void )
{   
}

//
//  Voronoi::Voronoi -- copy constructor
//
//  Inputs
//  obj : object of this class
//
//  Outputs
//  none
//
Voronoi::Voronoi( const Voronoi & obj )
{
}


//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------

//
//  Voronoi::~Voronoi --    destructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
Voronoi::~Voronoi( void )
{
}

// end of header file
// Do not add any stuff under this line.

