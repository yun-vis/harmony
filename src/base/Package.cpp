//==============================================================================
// Package.cc
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

#include "base/Package.h"


//------------------------------------------------------------------------------
//	Protected functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------
//
//  Package::init -- initialize
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Package::_init( void )
{
}

//
//  Package::createPolygonComplex --    create the ploygon complex
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Package::createPolygonComplex( unsigned int nV )
{
    vector< vector< Polygon2 > > _polygonMat;
    vector< Seed > &seedVec = *_forceBone.voronoi().seedVec();

    // initialization
    _polygonComplex.clear();

    // find the sets of the polygons of the same group
    // int nV = num_vertices( _skeleton );
    _polygonMat.resize( nV );
    assert( seedVec.size() == num_vertices( _bone ) );
    for( unsigned int i = 0; i < seedVec.size(); i++ ){

        int gid = _bone[ vertex( i, _bone ) ].initID;
        _polygonMat[ gid ].push_back( seedVec[i].cellPolygon );
    }

    for( unsigned int i = 0; i < _polygonMat.size(); i++ ){

        int cSize = _polygonMat[i].size();
        if( cSize > 1 ){

            Contour2 contour;
            vector< Polygon2 > pVec;
            for( unsigned int j = 0; j < _polygonMat[i].size(); j++ ) {
                Polygon2 &p = _polygonMat[i][j];
                pVec.push_back( p );
            }
            contour.init( i, pVec );
            contour.createContour();
            _polygonComplex.insert( pair< unsigned int, Polygon2 >( i, contour.contour() ) );
        }
        else{
            Polygon2 &p = _polygonMat[i][0];
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
//  Package::updatePolygonComplex --    update the ploygon complex
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Package::updatePolygonComplex( void )
{
    cerr << "updating polygonComplex after optimization ..." << endl;

    map< unsigned int, vector< ForceGraph::vertex_descriptor > >::iterator itP;
    map< unsigned int, Polygon2 >::iterator itC = _polygonComplex.begin();
    for( itP = _polygonComplexVD.begin(); itP != _polygonComplexVD.end(); itP++ ){
        vector< ForceGraph::vertex_descriptor > &p = itP->second;
        for( unsigned int i = 0; i < p.size(); i++ ){
            itC->second.elements()[i].x() = _boundary[ p[i] ].coordPtr->x();
            itC->second.elements()[i].y() = _boundary[ p[i] ].coordPtr->y();
        }
        itC++;
    }
}

//
//  Package::buildBoneGraph --    build the boundary from the voronoi cell
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Package::buildBoundaryGraph( void )
{
    // initialization
    _polygonComplexVD.clear();
    clearGraph( _boundary );
    _nVertices = 0;
    _nEdges = 0;
    _nLines = 0;

    // create boundary graph
    for( unsigned int i = 0; i < _polygonComplex.size(); i++ ){

        Polygon2 &polygon = _polygonComplex[i];
        vector< ForceGraph::vertex_descriptor > vdVec;

        unsigned int size = polygon.elements().size();
        for( unsigned int j = 1; j < size+1; j++ ){

            //Coord2 &coordS = polygon.elements()[j-1];
            //Coord2 &coordT = polygon.elements()[j%polygon.elements().size()];

            // Check if the station exists or not
            BoundaryGraph::vertex_descriptor curVDS = NULL;
            BoundaryGraph::vertex_descriptor curVDT = NULL;

            // add vertices
            for( unsigned int k = 0; k < 2; k++ ){

                BoundaryGraph::vertex_descriptor curVD = NULL;

                // Check if the station exists or not
                BGL_FORALL_VERTICES( vd, _boundary, BoundaryGraph )
                    {
                        Coord2 &c = *_boundary[ vd ].coordPtr;
                        if( fabs( ( polygon.elements()[ (j-1+k)%size]-c ).norm() ) < 1e-2 ){

#ifdef DEBUG
                            cerr << "The node has been in the database." << endl;
#endif  // DEBUG
                            if( k == 0 ) curVD = curVDS = vd;
                            if( k == 1 ) curVD = curVDT = vd;
                            break;
                        }
                    }

                if ( curVD == NULL ){

                    curVD = add_vertex( _boundary );
                    vector< unsigned int > lineID = _boundary[ curVD ].lineID;
                    lineID.push_back( _nLines );

                    double x = polygon.elements()[j-1+k].x();
                    double y = polygon.elements()[j-1+k].y();
                    _boundary[ curVD ].geoPtr       = new Coord2( x, y );
                    _boundary[ curVD ].smoothPtr    = new Coord2( x, y );
                    _boundary[ curVD ].coordPtr     = new Coord2( x, y );

                    _boundary[ curVD ].id = _boundary[ curVD ].initID = _nVertices;
                    _boundary[ curVD ].namePtr = new string( to_string( _boundary[ curVD ].id ) );
                    _boundary[ curVD ].weight = 1.0;
                    _boundary[ curVD ].lineID.push_back( _nLines );

                    if( k == 0 ) curVDS = curVD;
                    if( k == 1 ) curVDT = curVD;
                    _nVertices++;
                }
                else{
                    _boundary[ curVD ].lineID.push_back( _nLines );
#ifdef DEBUG
                    cerr << "[Existing] curV : lineID = " << endl;
                for ( unsigned int k = 0; k < vertexLineID[ curVD ].size(); ++k )
                    cerr << "lineID[ " << setw( 3 ) << k << " ] = " << vertexLineID[ curVD ][ k ] << endl;
#endif  // DEBUG
                }
            }
            vdVec.push_back( curVDS );
            // cerr << _boundary[ curVDS ].id << " ";

            //cerr << "( " << _boundary[ curVDS ].id << ", " << _boundary[ curVDT ].id << " )" << endl;

            // add edges
            // search previous edge
            bool found = false;
            BoundaryGraph::edge_descriptor oldED;
            //BoundaryGraph::vertex_descriptor oldVS = ptrSta[ origID ];
            //BoundaryGraph::vertex_descriptor oldVT = ptrSta[ destID ];
            //unsigned int indexS = _boundary[ curVDS ].initID;
            //unsigned int indexT = _boundary[ curVDT ].initID;
            tie( oldED, found ) = edge( curVDS, curVDT, _boundary );


            if ( found == true ) {

                _boundary[ oldED ].lineID.push_back( _nLines );
                //eachline.push_back( oldED );
                //bool test = false;
                //tie( oldED, test ) = edge( oldVT, oldVS, _boundary );
            }
            else{

                //BoundaryGraph::vertex_descriptor src = vertex( indexS, _boundary );
                //BoundaryGraph::vertex_descriptor tar = vertex( indexT, _boundary );

                // handle fore edge
                pair<BoundaryGraph::edge_descriptor, unsigned int> foreE = add_edge( curVDS, curVDT, _boundary );
                BoundaryGraph::edge_descriptor foreED = foreE.first;

                // calculate geographical angle
                Coord2 coordO;
                Coord2 coordD;
                if( _boundary[ curVDS ].initID < _boundary[ curVDT ].initID ){
                    coordO = *_boundary[ curVDS ].coordPtr;
                    coordD = *_boundary[ curVDT ].coordPtr;
                }
                else{
                    coordO = *_boundary[ curVDT ].coordPtr;
                    coordD = *_boundary[ curVDS ].coordPtr;
                }
                double diffX = coordD.x() - coordO.x();
                double diffY = coordD.y() - coordO.y();
                double angle = atan2( diffY, diffX );

                _boundary[ foreED ].initID = _boundary[ foreED ].id = _nEdges;
                _boundary[ foreED ].weight = 1.0;
                _boundary[ foreED ].geoAngle = angle;
                _boundary[ foreED ].smoothAngle = angle;
                _boundary[ foreED ].angle = angle;
                _boundary[ foreED ].lineID.push_back( _nLines );

                //eachline.push_back( foreED );
#ifdef  DEBUG
                cout << "nEdges = " << _nEdges << " angle = " << angle << endl;
#endif  // DEBUG
                _nEdges++;
            }
        }
        _polygonComplexVD.insert( pair< unsigned int, vector< ForceGraph::vertex_descriptor > >( i, vdVec ) );
    }

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
    cerr << "nV = " << _nVertices << " nE = " << _nEdges
         << " nL = " << _nLines << endl;
#endif // DEBUG
}

//
//  Package::findVertexInComplex --    detect vertex-edge pair that is close to each other
//
//  Inputs
//  coord: coordinates of a point
//  complex: the graph
//  vd: vertex descriptor
//
//  Outputs
//  isFound: binary
//
bool Package::findVertexInComplex( Coord2 &coord, ForceGraph &complex,
                                   ForceGraph::vertex_descriptor &target )
{
    bool isFound = false;

    BGL_FORALL_VERTICES( vd, complex, ForceGraph )
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

//
//  Package::clear --    clear the current Package information
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Package::_clear( void )
{
}


//
//  Package::Package -- default constructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
Package::Package( void )
{
}

//
//  Package::Package -- copy constructor
//
//  Inputs
//  obj : object of this class
//
//  Outputs
//  none
//
Package::Package( const Package & obj )
{
}


//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------
//
//  Package::~Package --    destructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
Package::~Package( void )
{
}


// end of header file
// Do not add any stuff under this line.
