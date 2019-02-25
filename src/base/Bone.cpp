//==============================================================================
// Bone.cc
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

#include "base/Bone.h"


//------------------------------------------------------------------------------
//	Protected functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------
//
//  Bone::init -- initialize
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Bone::_init( void )
{
}

//
//  Bone::createPolygonComplex --    create the ploygon complex
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Bone::createPolygonComplex( unsigned int nV )
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
//  Bone::findVertexInComplex --    detect vertex-edge pair that is close to each other
//
//  Inputs
//  coord: coordinates of a point
//  complex: the graph
//  vd: vertex descriptor
//
//  Outputs
//  isFound: binary
//
bool Bone::findVertexInComplex( Coord2 &coord, ForceGraph &complex,
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
//  Bone::clear --    clear the current Bone information
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Bone::_clear( void )
{
    _forceBone.clear();
    _stressBone.clear();
}


//
//  Bone::Bone -- default constructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
Bone::Bone( void )
{
    _isForce = true;
}

//
//  Bone::Bone -- copy constructor
//
//  Inputs
//  obj : object of this class
//
//  Outputs
//  none
//
Bone::Bone( const Bone & obj )
{
}


//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------
//
//  Bone::~Bone --    destructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
Bone::~Bone( void )
{
}


// end of header file
// Do not add any stuff under this line.
