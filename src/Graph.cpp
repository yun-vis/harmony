//==============================================================================
// Graph.cpp
//      : program file for graph function 
//
//==============================================================================

#include <map>
#include <list>
#include "Graph.h"

using namespace std;

//------------------------------------------------------------------------------
//	Customized Vertex Functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Customized Edge Functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Customized Graph Functions
//------------------------------------------------------------------------------
//
//  Graph::resetGCoords -- reset current coordinate to home coordinate
//
//  Inputs
//  g   : object of Grpah
//
//  Outputs
//  none
//
void resetGCoords( Graph & g )
{
    VertexGeoMap        vertexGeo       = get( vertex_mygeo, g );
    VertexSmoothMap     vertexSmooth    = get( vertex_mysmooth, g );
    VertexCoordMap      vertexCoord     = get( vertex_mycoord, g );
    VertexSelectMagMap  vertexSelectMag = get( vertex_myselectmag, g );

    BGL_FORALL_VERTICES( vertex, g, Graph )
    {  
        vertexCoord[ vertex ].x() = vertexSmooth[ vertex ].x() = vertexGeo[ vertex ].x();
        vertexCoord[ vertex ].y() = vertexSmooth[ vertex ].y() = vertexGeo[ vertex ].y();
        vertexSelectMag[ vertex ] = false;
    }
}


//
//  Graph::printGraph -- print out the graph.
//
//  Inputs
//  g   : object of Grpah
//
//  Outputs
//  none
//
void printGraph( Graph & g )
{
    VertexIDMap         vertexID        = get( vertex_myid, g );
    VertexCoordMap      vertexCoord     = get( vertex_mycoord, g );
    VertexSmoothMap     vertexSmooth    = get( vertex_mysmooth, g );
    VertexGeoMap        vertexGeo       = get( vertex_mygeo, g );
    VertexHomeMap       vertexHome      = get( vertex_myhome, g );
    EdgeIDMap           edgeID          = get( edge_myid,g );
    EdgeWeightMap       edgeWeight      = get( edge_weight,g );
    EdgeCurAngleMap     edgeCurAngle    = get( edge_mycurangle,g );

#ifdef  SKIP
    cout << "Input station position: " << endl;
    BGL_FORALL_VERTICES( vertex, g, Graph )
    {
        cout << setprecision( 5 ) << "V(" << setw( 2 ) << vertexID[ vertex ] << "):"
             //<< setw( 10 ) << vertexExternal[ vertex ].leaderWeight() << endl;
             << setw( 10 ) << vertexHome[ vertex ].x() << setw( 10 ) << vertexHome[ vertex ].y() << endl;
    }

    cout << "Geographical station position: " << endl;
    BGL_FORALL_VERTICES( vertex, g, Graph )
    {
        cout << setprecision( 5 ) << "V(" << setw( 2 ) << vertexID[ vertex ] << "):"
             << setw( 10 ) << vertexGeo[ vertex ].x() << setw( 10 ) << vertexGeo[ vertex ].y() << endl;
    }

    cout << endl << "Smooth station position: " << endl;
    BGL_FORALL_VERTICES( vertex, g, Graph ) {
        cout << setprecision( 5 ) << "V(" << setw( 2 ) << vertexID[ vertex ] << "):"
             << setw( 10 ) << vertexSmooth[ vertex ].x() << setw( 10 ) << vertexSmooth[ vertex ].y() << endl;
    }

    cout << endl << "Octilinear station position: " << endl;
    BGL_FORALL_VERTICES( vertex, g, Graph ) {
        cout << setprecision( 5 ) << "V(" << setw( 2 ) << vertexID[ vertex ] << "):"
             << setw( 10 ) << vertexCoord[ vertex ].x() << setw( 10 ) << vertexCoord[ vertex ].y() << endl;
    }

#endif  // SKIP
    BGL_FORALL_EDGES( edge, g, Graph ) {

        VertexDescriptor vdS = source( edge, g );
        VertexDescriptor vdT = target( edge, g );
        unsigned int idS = vertexID[ vdS ];
        unsigned int idT = vertexID[ vdT ];
        cout << "E(" << setw( 2 ) << edgeID[ edge ] << ") = " 
             << setw( 5 ) << "(" << vertexID[ vdS ] << ", " << vertexID[ vdT ] << ")" 
             //<< " A = " << edgeCurAngle[ edge ] << endl;
             << " W = " << edgeWeight[ edge ] << endl;
    }
}

//
//  Graph::clearGraph -- clear the graph.
//
//  Inputs
//  g   : object of Grpah
//
//  Outputs
//  none
//
void clearGraph( Graph & g )
{
    // clear edges
    BGL_FORALL_EDGES( edge, g, Graph )
    {
        remove_edge( edge, g );
    }

    // clear vertices
    pair< VertexIterator, VertexIterator > vp;
    for ( vp = vertices( g ); vp.first != vp.second;  ) {
        VertexDescriptor vd = (*vp.first);
        ++vp.first;
        clear_vertex( vd, g );
        remove_vertex( vd, g );
    }

}

//
//  Graph::resetGraphCoord -- reset graph coord.
//
//  Inputs
//  g   : object of Grpah
//
//  Outputs
//  none
//
void resetGraphCoord( Graph & g )
{

    VertexHomeMap           vertexHome          = get( vertex_myhome, g );
    VertexGeoMap            vertexGeo           = get( vertex_mygeo, g );
    VertexSmoothMap         vertexSmooth        = get( vertex_mysmooth, g );
    VertexCoordMap          vertexCoord         = get( vertex_mycoord, g );


    BGL_FORALL_VERTICES( vertex, g, Graph )
    {
        // reset coord
        vertexCoord[ vertex ] = vertexSmooth[ vertex ] = vertexGeo[ vertex ];
    }
}

void resetGraphWeight( Graph & g )
{
    EdgeWeightMap       edgeWeight      = get( edge_weight, g );

    BGL_FORALL_EDGES( edge, g, Graph )
    {
        edgeWeight[ edge ] = 1.0;
    }
}

int _longestLabelNum( VertexDescriptor vd, vector< unsigned int > & visit, Graph & g )
{

    VertexIDMap             vertexID            = get( vertex_myid, g );
    VertexExtstateMap       vertexExtstate      = get( vertex_myextstate, g );
    VertexSelectMagMap      vertexSelectMag     = get( vertex_myselectmag, g );

    bool isVisit = false;
    for( unsigned int i = 0; i < visit.size(); i++ ){
        if( vertexID[ vd ] == visit[ i ] ) isVisit = true;
    }
    //cerr << " " << vertexID[ vd ] << " " << isVisit;
    if( vertexExtstate[ vd ] == false || vertexSelectMag[ vd ] == false || isVisit == true ) {
        //cerr << endl;
        visit.push_back( vertexID[ vd ] );
        return 0;
    }
    else{
        OutEdgeIterator e, e_end;
        for ( tie( e, e_end ) = out_edges( vd, g ); e != e_end; ++e ) {
            EdgeDescriptor ed = *e;
            VertexDescriptor vA = source( ed, g );
            VertexDescriptor vB = target( ed, g );
            visit.push_back( vertexID[ vd ] );
            return _longestLabelNum( vB, visit, g ) + 1;
        }
    }

    visit.push_back( vertexID[ vd ] );
    return 0;
}

//
//  Graph::setGraphWeight -- set graph weight.
//
//  Inputs
//  g   : object of Grpah
//
//  Outputs
//  none
//
void setGraphWeight( double x, double y, double meanNodeSize, double length, Graph & g )
{  
    VertexScaleMap          vertexScale         = get( vertex_myscale, g );
    EdgeWeightMap           edgeWeight          = get( edge_weight, g );

    // set scale
    BGL_FORALL_VERTICES( vd, g, Graph )
    {
        vertexScale[ vd ] = 1.0;
    }

    // set weight
    BGL_FORALL_EDGES( ed, g, Graph )
    {
        edgeWeight[ ed ] = 1.0;
    }
}

//
//  Vertex::setTexture = --    set the input texture file name
//
//  Inputs
//      __comment :     input comments
//
//  Outputs
//      none
//
void setVertexTexName( Graph & g, VertexDescriptor vd, const string & str )
{
    VertexTexNameMap vertexTexName = get( vertex_mytexname, g );

    vertexTexName[ vd ] = str;

    for ( unsigned int k = 0; k <  vertexTexName[ vd ].length(); ++k ) {
        if (  vertexTexName[ vd ][ k ] == ' ' )  vertexTexName[ vd ][ k ] = '_';
        else if (  vertexTexName[ vd ][ k ] == '\\' )  vertexTexName[ vd ].erase( k, 1 );
    }
}


//  Graph::shortestPath --      find the shortest path between the specified end
//                              vertices using the dijkstra algorithm
//
//  Inputs
//      source  : source vertex VD
//      target  : target vertex VD
//
//  Outputs
//      none
//
void shortestPath( VertexDescriptor vdS, VertexDescriptor vdT, Graph & g )
{
    vector< VertexDescriptor > path;
    VertexIDMap         vertexID        = get( vertex_myid, g );
    EdgeSelectShiftMap  edgeSelectShift = get( edge_myselectshift, g );
    EdgeSelectCtrlMap   edgeSelectCtrl  = get( edge_myselectctrl, g );
    EdgeWeightMap       edgeWeight      = get( edge_weight, g );

    unsigned int nVertices = num_vertices( g );
    vector< VertexDescriptor > parent( nVertices );
    vector< double > distance( nVertices );
    dijkstra_shortest_paths( g, vdS,
                             predecessor_map( make_iterator_property_map( parent.begin(),
                                                                          get( vertex_index, g ) ) ).
                             distance_map( make_iterator_property_map( distance.begin(),
                                                                       get( vertex_index, g ) ) ) );

    VertexDescriptor cur = vdT;
    while ( cur != vdS ) {
        path.push_back( cur );
        cur = parent[ vertexID[ cur ] ];
    }
    path.push_back( vdS );

    // update selected edges along the path
    BGL_FORALL_EDGES( ed, g, Graph ) {
        edgeSelectShift[ ed ] = false;
        edgeSelectCtrl[ ed ] = false;
        edgeWeight[ ed ] = 1.0;
    }
    for( int i = 1; i < path.size(); i++ ){

        VertexDescriptor vdS = path[ i-1 ];
        VertexDescriptor vdT = path[ i ];
        EdgeDescriptor ed;

        bool found = false;
        tie( ed, found ) = edge( vdS, vdT, g );

        if( found == false ) 
            cerr << "Errors occurred here at " << __LINE__ << " in " << __FILE__ << endl;
        else{ 
            edgeSelectCtrl[ ed ] = true;
            edgeWeight[ ed ] = 2.0;
        }
#ifdef  DEBUG
        cerr << " " << vertexID[ path[ i ] ];
#endif  // DEBUG
    }
}


//
//  geodesicDistance --  set distances from the selected vertex
//
//  Inputs
//      g       : reference to the graph
//
//  Outputs
//      none
//
void geodesicDistance( VertexDescriptor focusVD, Graph & g )
{
    VertexIDMap         vertexID        = get( vertex_myid, g );
    VertexSelectMagMap  vertexSelectMag = get( vertex_myselectmag, g );
    VertexSelectTopMap  vertexSelectTop = get( vertex_myselecttop, g );
    VertexGeodesicMap   vertexGeodesic  = get( vertex_mygeodesic, g );
    VertexZoneMap       vertexZone      = get( vertex_myzone, g );
    EdgeIDMap           edgeID          = get( edge_myid, g );
    EdgeWeightMap       edgeWeight      = get( edge_weight, g );
    vector< double >    unitWeight( num_edges( g ) );

//------------------------------------------------------------------------------
//      Initialize vertex geodesic distances
//------------------------------------------------------------------------------
    // Inititialization
    BGL_FORALL_EDGES( ed, g, Graph ) {
        unitWeight[ edgeID[ ed ] ] = 1.0;
    }
    BGL_FORALL_VERTICES( vd, g, Graph ) {
        put( vertex_mygeodesic, g, vd, 0.0 );
    }

//------------------------------------------------------------------------------
//      Calculate vertex geodesic distances
//------------------------------------------------------------------------------
    unsigned int nVertices = num_vertices( g );
    vector< VertexDescriptor > parent( nVertices );
    vector< double > distance( nVertices );
    dijkstra_shortest_paths( g, focusVD,
                             weight_map( make_iterator_property_map( unitWeight.begin(),
                                                                     get( edge_index, g ) ) ).
                             distance_map( make_iterator_property_map( distance.begin(),
                                                                       get( vertex_index, g ) ) ) );

    BGL_FORALL_VERTICES( vd, g, Graph ) {

        double oldV = vertexGeodesic[ vd ];
        double newV = oldV + distance[ vertexID[ vd ] ];
        vertexGeodesic[ vd ] = newV;
    }

    BGL_FORALL_VERTICES( vd, g, Graph ) {
        if( vertexGeodesic[ vd ] < 3 ) vertexSelectTop[ vd ] = true;
    }

//------------------------------------------------------------------------------
//      Calculate vertex zone
//------------------------------------------------------------------------------
    BGL_FORALL_VERTICES( vdI, g, Graph ) {
         if( vertexSelectMag[ vdI ] == true ) vertexZone[ vdI ] = 3;
         else if( vertexSelectTop[ vdI ] == true ) vertexZone[ vdI ] = 2;
         else vertexZone[ vdI ] = 1;
    }

    BGL_FORALL_EDGES( ed, g, Graph ) {
        VertexDescriptor vdS = source( ed, g );
        VertexDescriptor vdT = target( ed, g );
        double weight = MIN2( vertexZone[ vdS ], vertexZone[ vdT ] );
        //double weight = 4 - MAX2( vertexZone[ vdS ], vertexZone[ vdT ] );
        edgeWeight[ ed ] = weight;
    }

}

//
//  isEEOverlap --  check if the 2 edges are intersected 
//
//  Inputs
//      g       : reference to the graph
//
//  Outputs
//      none
//
bool isEEOverlap( const Coord2 & As, const Coord2 & At,
                  const Coord2 & Bs, const Coord2 & Bt )

{
    const double gap = 0.01;

    Coord2 Cs =   (1.0+gap) * As - gap * At;
    Coord2 Ct =  -gap * As + (1.0+gap) * At;
    Coord2 Ds =   (1.0+gap) * Bs - gap * Bt;
    Coord2 Dt =  -gap * Bs + (1.0+gap) * Bt;

#ifdef  SKIP
    cerr << " a = " << Cs
         << " b = " << Ct
         << " c = " << Ds
         << " d = " << Dt;
#endif  // SKIP

    if ( isSeparate( Cs, Ct, Ds, Dt ) ) return false;

    if ( isCollinear( Cs, Ct, Ds ) ||
         isCollinear( Cs, Ct, Dt ) ||
         isCollinear( Ds, Dt, Cs ) ||
         isCollinear( Ds, Dt, Ct ) ) return true;

    return ( ( doubleArea( Cs, Ct, Ds ) * doubleArea( Cs, Ct, Dt ) < 0.0 ) &&
             ( doubleArea( Ds, Dt, Cs ) * doubleArea( Ds, Dt, Ct ) < 0.0 ) );

}
