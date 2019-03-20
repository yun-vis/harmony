//******************************************************************************
// Contour2.cc
//	: program file for 2D contour
//
//------------------------------------------------------------------------------
//
//	Ver 1.00		Date: Sun Sep 16 15:02:45 2012
//
//******************************************************************************

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------

#include <iostream>
#include <iomanip>
#include <cctype>
#include <cmath>
#include <algorithm>
using namespace std;

#include "base/Contour2.h"

//------------------------------------------------------------------------------
//	Macro Definitions
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
//	Protected Functions
//------------------------------------------------------------------------------
//
//  Contour2::_init --	initialization
//
//  Inputs
//  __elements
//
//  Outputs
//  none
//
void Contour2::_init( unsigned int __id, vector< Polygon2 > __polygons )
{
    _id = __id;
    _polygons = __polygons;

    // detect non-simple polygon and remove overlapped vertices
    for( unsigned int i = 0; i < _polygons.size(); i++ ){
        CGAL::Polygon_2< K > polygon;
        Polygon2 &p =  _polygons[i];
        for( unsigned int j = 0; j < p.elements().size(); j++ ){
            if( (p.elements()[j] - p.elements()[(j+1)%p.elements().size()]).norm() > 0.5 )
                polygon.push_back( K::Point_2( p.elements()[j].x(), p.elements()[j].y() ) );
        }

        p.elements().clear();
        for( unsigned int j = 0; j < polygon.size(); j++ ){
            p.elements().push_back( Coord2( CGAL::to_double( polygon[j].x() ),
                                            CGAL::to_double( polygon[j].y() ) ) );
        }

        bool isSimple = polygon.is_simple();

        if( polygon.is_simple() == false ){
            cerr << "i = " << i << " isSimple = " << polygon.is_simple() << endl;
            cerr << p << endl;
        }
    }
}

//
//  Contour2::_clear --	clear elements
//
//  Inputs
//
//
//  Outputs
//  none
//
void Contour2::_clear( void )
{
    _polygons.clear();
}

//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------
//
//  Contour2::Contour2 -- default constructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
Contour2::Contour2()
{
}

//
//  Contour2::Contour2 -- constructor
//
//  Inputs
//  __elements
//
//  Outputs
//  none
//
Contour2::Contour2( vector< Polygon2 > __polygons )
{
    _polygons = __polygons;
}

//
//  Contour2::Contour2 -- copy constructor
//
//  Inputs
//  polygon
//
//  Outputs
//  none
//
Contour2::Contour2( const Contour2 & v )
{
    _id = v._id;
    _contour = v._contour;
    _polygons = v._polygons;
}


//------------------------------------------------------------------------------
//	Assignment operators
//------------------------------------------------------------------------------

//
//  Contour2::operator = --	assignment
//
//  Inputs
//	v	: 2D coordinates
//
//  Outputs
//	reference to this object
//
Contour2 & Contour2::operator = ( const Contour2 & p )
{
    if ( this != &p ) {
        _id = p._id;
        _contour = p._contour;
        _polygons = p._polygons;
    } 
    return *this;
}

//------------------------------------------------------------------------------
//	Special functions
//------------------------------------------------------------------------------
bool Contour2::findVertexInComplex( const Coord2 &coord, UndirectedBaseGraph &complex,
                                    UndirectedBaseGraph::vertex_descriptor &target )
{
    bool isFound = false;

    BGL_FORALL_VERTICES( vd, complex, UndirectedBaseGraph )
    {
        //cerr << " vd " << *complex[vd].coordPtr << endl;
        if( ( coord - *complex[vd].coordPtr ).norm() < 1e-2 ){
            target = vd;
            isFound = true;
        }
    }

    //cerr << "isFound = " << isFound << endl;
    return isFound;
}

void Contour2::createContour( void ) {
    // copy cells to a graph
    UndirectedBaseGraph complex;
    unsigned int id = 0;
    for (unsigned int j = 0; j < _polygons.size(); j++) {

        Polygon2 &p = _polygons[j];
        vector<UndirectedBaseGraph::vertex_descriptor> vdVec;

        // add vertices
        for (unsigned int k = 0; k < p.elements().size(); k++) {
            UndirectedBaseGraph::vertex_descriptor vd = NULL;

            bool isFound = findVertexInComplex(p.elements()[k], complex, vd);
            if (isFound == true) {
                vdVec.push_back(vd);
            } else {
                vd = add_vertex(complex);
                complex[vd].id = id;
                complex[vd].coordPtr = new Coord2(p.elements()[k].x(), p.elements()[k].y());
                vdVec.push_back(vd);
                id++;
            }
        }

        // add edges
        unsigned int eid = 0;
        for (unsigned int k = 1; k < vdVec.size() + 1; k++) {

            UndirectedBaseGraph::vertex_descriptor vdS = vdVec[k - 1];
            UndirectedBaseGraph::vertex_descriptor vdT = vdVec[k % vdVec.size()];

            bool isFound = false;
            UndirectedBaseGraph::edge_descriptor oldED;
            tie(oldED, isFound) = edge(vdS, vdT, complex);

            if (isFound == true) {
                complex[oldED].weight += 1;
            } else {

                pair<UndirectedBaseGraph::edge_descriptor, unsigned int> foreE = add_edge(vdS, vdT, complex);
                UndirectedBaseGraph::edge_descriptor foreED = foreE.first;
                complex[foreED].id = eid;
                complex[foreED].weight = 0;
                eid++;
            }
        }
    }

    // remove inner edges
    vector<UndirectedBaseGraph::edge_descriptor> edVec;
    BGL_FORALL_EDGES(ed, complex, UndirectedBaseGraph) {
            if (complex[ed].weight > 0) {
                edVec.push_back(ed);
                //cerr << "w = " << complex[ed].weight << endl;
            }
        }

#ifdef DEBUG
    cerr << "bnV = " << num_vertices( complex ) << " ";
    cerr << "bnE = " << num_edges( complex ) << endl;
#endif // DEBUG

    for (unsigned int j = 0; j < edVec.size(); j++) {
        remove_edge(edVec[j], complex);
    }
    // reorder edge id
    unsigned int eid = 0;
    BGL_FORALL_EDGES(ed, complex, UndirectedBaseGraph) {
            complex[ed].id = eid;
            complex[ed].visit = false;
            eid++;
        }

    // find the vertex with degree > 0
#ifdef DEBUG
    cerr << "nV = " << num_vertices( complex ) << " ";
    cerr << "nE = " << num_edges( complex ) << endl;
#endif // DEBUG
    UndirectedBaseGraph::vertex_descriptor vdS;
    BGL_FORALL_VERTICES(vd, complex, UndirectedBaseGraph) {
#ifdef DEBUG
            if ( true ){
                cerr << " degree = " << out_degree( vd, complex );
            }
#endif // DEBUG
            if (out_degree(vd, complex) > 0) {
                vdS = vd;
                break;
            }
        }

    // find the contour
    UndirectedBaseGraph::out_edge_iterator eo, eo_end;
    UndirectedBaseGraph::vertex_descriptor vdC = vdS;
    //cerr << "idC = " << complex[vdC].id << " ";

    Polygon2 polygon;
    polygon.elements().push_back(Coord2(complex[vdS].coordPtr->x(), complex[vdS].coordPtr->y()));
    while (true) {

        UndirectedBaseGraph::vertex_descriptor vdT = NULL;
        for (tie(eo, eo_end) = out_edges(vdC, complex); eo != eo_end; eo++) {

            UndirectedBaseGraph::edge_descriptor ed = *eo;
            if (complex[ed].visit == false) {
                vdT = target(ed, complex);
                complex[ed].visit = true;
                break;
            }
            //cerr << "(" << complex[source( ed, complex )].id << "," << complex[target( ed, complex )].id << ")" << endl;
        }

        if (vdS == vdT) break;
        else
            assert(vdT != NULL);
        //cerr << complex[vdT].id << " ";
        polygon.elements().push_back(Coord2(complex[vdT].coordPtr->x(), complex[vdT].coordPtr->y()));
        vdC = vdT;
    }

    // simplify polygon
    //cerr << "size = " << polygon.elements().size() << endl;
    for (unsigned int i = 0; i < polygon.elements().size(); i++) {

        Coord2 prev;
        if (i == 0) prev = polygon.elements()[polygon.elements().size() - 1];
        else prev = polygon.elements()[i - 1];

        Coord2 curr = polygon.elements()[i];
        Coord2 next = polygon.elements()[(i + 1) % polygon.elements().size()];
        Coord2 vecA = prev - curr;
        Coord2 vecB = next - curr;

#ifdef DEBUG
        cerr << i << ": " << (i-1)%polygon.elements().size() << endl;
        cerr << "prev = " << prev;
        cerr << "next = " << next;
        cerr << "curr = " << curr;
        cerr << "vecA = " << vecA;
        cerr << "vecB = " << vecB;
#endif // DEBUG

        double cosTheta = vecA * vecB / (vecA.norm() * vecB.norm());
        if (fabs(M_PI - acos(cosTheta)) > 0.1) {
            //cerr << "acos = " << acos( cosTheta ) << endl;

            //Coord2 &prev = polygon.elements()[ (i-1+polygon.elements().size())%polygon.elements().size() ];
            //if( (prev-polygon.elements()[i]).norm() > 1 )
            _contour.elements().push_back(polygon.elements()[i]);
        }
    }

/*
    // clean contour
    // remove vertex on a line
    Polygon2 tmp;
    for( unsigned int i = 0; i < _contour.elements().size(); i++ ){
        int idP = (i-1+_contour.elements().size())%_contour.elements().size();
        int idC = i;
        int idN = (i+1)%_contour.elements().size();

        if( !Line2::isOnLine( _contour.elements()[i], _contour.elements()[idP], _contour.elements()[idN] ) ){
            tmp.elements().push_back( _contour.elements()[i] );
        }
        else{
            cerr << "removing... " << _contour.elements()[i];
            cerr << "idP = " << idP << " idC = " << idC << " idN = " << idN << endl;
        }
    }

    if( _contour.elements().size() != tmp.elements().size() ){
        cerr << "contour = " << _contour;
        cerr << "tmpContour = " << tmp;
    }
    _contour.elements().clear();
    _contour = tmp;
*/

    cerr << "(1)::_contour = " << _contour << endl;

    // detect non-simple polygon and remove overlapped vertices
{

    CGAL::Polygon_2<K> poly;
    for (unsigned int j = 0; j < _contour.elements().size(); j++) {
        if ((_contour.elements()[j] - _contour.elements()[(j + 1) % (int)_contour.elements().size()]).norm() > 0.5)
            poly.push_back(K::Point_2(_contour.elements()[j].x(), _contour.elements()[j].y()));
    }

    _contour.elements().clear();
    for (unsigned int j = 0; j < poly.size(); j++) {
        _contour.elements().push_back(Coord2(CGAL::to_double(poly[j].x()),
                                      CGAL::to_double(poly[j].y())));
    }

    bool isSimple = poly.is_simple();

    if (poly.is_simple() == false) {
        cerr << "isSimple = " << poly.is_simple() << endl;
        cerr << _contour << endl;
    }
}

    cerr << "(2)::_contour = " << _contour << endl;

    // remove picks
    Polygon2 tmp;
    tmp.elements() = _contour.elements();
    _contour.elements().clear();
    cerr << "size = " << tmp.elements().size() << endl;
    for( unsigned int i = 0; i < tmp.elements().size(); i++ ){
        int idP = (i-1+(int)tmp.elements().size())%(int)tmp.elements().size();
        int idN = (i+1)%(int)tmp.elements().size();
        Coord2 &coordP = tmp.elements()[idP];
        Coord2 &coordI = tmp.elements()[i];
        Coord2 &coordN = tmp.elements()[idN];
        Coord2 PI = coordP - coordI;
        Coord2 NI = coordN - coordI;
        Coord2 PN = coordP - coordN;
        //if( ( acos( (PI.squaredNorm() + NI.squaredNorm() - PN.squaredNorm())/(2.0*PI.norm()*NI.norm()) ) > 1e-5 ) ){
        if( ( acos( (PI.squaredNorm() + NI.squaredNorm() - PN.squaredNorm())/(2.0*PI.norm()*NI.norm()) ) > 1e-5 ) &&
             PN.norm()/NI.norm() > 1e-5  ){
            _contour.elements().push_back( tmp.elements()[i] );
        }
        else{
            cerr << "idP = " << idP << " i = " << i << " idN = " << idN << endl;
            cerr << "P = " << coordP;
            cerr << "I = " << coordI;
            cerr << "N = " << coordN;
            cerr << "PI = " << PI;
            cerr << "NI = " << NI;
            cerr << "PN =" << PN;
        }
    }

    cerr << "(3)::_contour = " << _contour << endl;

    // detect non-simple polygon and remove overlapped vertices
    CGAL::Polygon_2< K > poly;
    Polygon2 &p =  _contour;
    for( unsigned int j = 0; j < p.elements().size(); j++ ){
        if( (p.elements()[j] - p.elements()[(j+1)%(int)p.elements().size()]).norm() > 0.5 )
            poly.push_back( K::Point_2( p.elements()[j].x(), p.elements()[j].y() ) );
    }

    p.elements().clear();
    for( unsigned int j = 0; j < poly.size(); j++ ){
        p.elements().push_back( Coord2( CGAL::to_double( poly[j].x() ),
                                        CGAL::to_double( poly[j].y() ) ) );
    }

    bool isSimple = poly.is_simple();

    if( poly.is_simple() == false ){
        cerr << "isSimple = " << poly.is_simple() << endl;
        cerr << p << endl;
    }

    cerr << "(4)::_contour = " << _contour << endl;

/*
    // remove picks
    bool exist = false;
    unsigned int idS = 0, idT = 0;
    for( unsigned int i = 0; i < _contour.elements().size(); i++ ){
        for( unsigned int j = i+1; j < _contour.elements().size(); j++ ) {
            if( ( _contour.elements()[i] - _contour.elements()[j] ).norm() < 0.1 ){
                exist = true;
                idS = i;
                idT = j;
            }
        }
    }
    if( exist == true ){
        tmp.clear();
        for( unsigned int i = 0; i < _contour.elements().size(); i++ ){
            if( i <= idS || i > idT )
                tmp.elements().push_back( _contour.elements()[i] );
        }
        _contour.elements().clear();
        _contour = tmp;
    }
*/

    _contour.updateCentroid();
    _contour.updateOrientation();
#ifdef DEBUG
    cerr << "centroid = " << polygon.centroid() << endl;
    cerr << "p[" << i << "] = " << polygon << endl;
    cerr << "contour = " << _contour << endl;
#endif // DEBUG
}

bool Contour2::inContour( Coord2 &coord )
{
    return _contour.inPolygon( coord );
}

//------------------------------------------------------------------------------
//	Friend functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	I/O functions
//------------------------------------------------------------------------------
//
//  operator << --	output
//
//  Inputs
//	s	: reference to output stream
//	v	: 2D coordinates
//
//  Outputs
//	reference to output stream
//
ostream & operator << ( ostream & stream, const Contour2 & obj )
{
    int i;		// loop counter
    // set the output formatting
    //stream << setiosflags( ios::showpoint );
    //stream << setprecision( 8 );
    //int width = 16;
    // print out the elements
    for ( i = 0; i < obj._contour.elements().size(); i++ ) {
	    //stream << setw( width ) << obj._element[ i ];
    	stream << setw( 4 ) << obj._contour.elements()[ i ];
	    if ( i != 1 ) stream << "\t";
    }
    stream << endl;

    return stream;
}


//
//  operator >> --	input
//
//  Inputs
//	s	: reference to input stream
//	v	: 2D coordinates
//
//  Outputs
//	reference to input stream
//
istream & operator >> ( istream & stream, Contour2 & obj )
{
    int i;		// loop counter
    // reading the elements
    for ( i = 0; i < obj._contour.elements().size(); i++ )
	stream >> obj._contour.elements()[ i ];
    return stream;
}




