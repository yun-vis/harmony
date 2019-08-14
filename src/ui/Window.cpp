#include "ui/Window.h"

//----------------------------------------------------------
// Window
//----------------------------------------------------------
Window::Window( QWidget *parent )
    : QMainWindow( parent )
{
    _gv = new GraphicsView( this );
    setCentralWidget( _gv );

    setGeometry( 0, 0, _gv->width(), _gv->height() );
    _levelType = LEVEL_HIGH;

#ifdef SKIP
    createActions();
    createMenus();
#endif // SKIP

#ifdef RECORD_VIDEO
    _timerVideoStart();
#endif // RECORD_VIDEO
}

Window::~Window()
{
}

Window::Window( const Window & obj )
{
    _gv = obj._gv;
    _levelhighPtr = obj._levelhighPtr;
    _boundaryVecPtr = obj._boundaryVecPtr;

    // cells of subgraphs
    _cellPtr = obj._cellPtr;
    _roadPtr = obj._roadPtr;
    _lanePtr = obj._lanePtr;

    // display
    _content_width = obj._content_width;
    _content_height = obj._content_height;
}

void Window::_init( void )
{
    _content_width = width() - LEFTRIGHT_MARGIN;
    _content_height = height() - TOPBOTTOM_MARGIN;

    // initialization of boundary
    _levelhighPtr = new LevelHigh;

    // initialization of region data
    _boundaryVecPtr = new vector< Boundary >;
    //_simplifiedBoundaryPtr = new Boundary;
    _cellPtr = new Cell;
    _roadPtr = new Road;
    _lanePtr = new vector< Road >;

    _cellPtr->setPathwayData( _pathway, *_pathway->width(), *_pathway->height() );
    _roadPtr->setPathwayData( _pathway, *_pathway->width(), *_pathway->height() );

    _gv->setPathwayData( _pathway, *_pathway->width(), *_pathway->height() );
    _gv->setRegionData( _levelhighPtr, _boundaryVecPtr,
                        _cellPtr, _roadPtr, _lanePtr );
}


void Window::simulateKey( Qt::Key key )
{
    QKeyEvent eventP( QEvent::KeyPress, key, Qt::NoModifier );
    QApplication::sendEvent( this, &eventP );
    QKeyEvent eventR( QEvent::KeyRelease, key, Qt::NoModifier );
    QApplication::sendEvent( this, &eventR );
}

void Window::postLoad( void )
{
    vector< Boundary > &boundaryVec = *_boundaryVecPtr;

    for( unsigned int i = 0; i < boundaryVec.size(); i++ ){

        boundaryVec[i].adjustsize( _content_width/2, _content_height/2 );
        boundaryVec[i].clearConflicts();
    }

    redrawAllScene();
}

void Window::redrawAllScene( void )
{
    _pathway->pathwayMutex().lock();
    _gv->initSceneItems();
    _pathway->pathwayMutex().unlock();

    _gv->update();
    _gv->scene()->update();
    update();
    repaint();

#ifdef DEBUG
    cerr << "_is_polygonFlag = " << _gv->isPolygonFlag() << endl;
    cerr << "_is_polygonComplexFlag = " << _gv->isPolygonComplexFlag() << endl;
#endif // DEBUG

    QCoreApplication::processEvents();
}


void Window::selectLevelHighBuildBoundary( void )
{
    //clearGraph( _simplifiedBoundaryPtr->boundary() );
    buildLevelHighBoundaryGraph();
    _gv->isSimplifiedFlag() = false;
    _gv->isBoundaryFlag() = true;
    redrawAllScene();
}

#ifdef RECORD_VIDEO
void Window::_timerVideoStart( void )
{
    _timerVideo.start( 3000, this );
    _timerVideoID = _timerVideo.timerId();
    cerr << "_timerVideo.timerId = " << _timerVideo.timerId() << endl;
}

void Window::_timerVideoStop( void )
{
    _timerVideo.stop();
}

void Window::timerVideo( void )
{
    cerr << "processing events..." << endl;
    QCoreApplication::processEvents();
    simulateKey( Qt::Key_E );
}
#endif // RECORD_VIDEO


void Window::threadBoundaryForce( void )
{
    // rendering setting
    _gv->isPolygonFlag() = true;

    // initialization
    _levelhighPtr->forceBone().init( &_levelhighPtr->bone(), &_contour, LEVEL_HIGH, "../configs/boundary.conf" );
    ctpl::thread_pool pool( _gv->maxThread() ); // limited thread number in the pool

    // create a new thread
    ThreadLevelHigh tlh;
    //vector < unsigned int > indexVec;

    tlh.setPathwayData( _pathway, *_pathway->width(), *_pathway->height() );
    tlh.setRegionData( _levelhighPtr, _boundaryVecPtr,
                       _cellPtr, _roadPtr, _lanePtr );
    tlh.init( THREAD_BOUNDARY, _gv->energyType(), 0, 0 );

    pool.push([]( int id, ThreadLevelHigh* tlh ){ tlh->run( id ); }, &tlh );
    //string name = "w";
    //pool.push([&]( int id, ThreadLevelHigh* tlh ){ tlh->run( id, name ); }, &tlh );
    //cerr << "pool.size() = " << pool.size() << endl;


    // wait for all computing threads to finish and stop all threads
    // pool.stop();
    // cerr << "after name = " << name << endl;
    // cv.wait_until( p.n_idle() != _gv->maxThread() );

    // rendering
    while( pool.n_idle() != _gv->maxThread() ){

        cerr << "pool.n_idle() = " << pool.n_idle() << endl;
        this_thread::sleep_for( chrono::milliseconds( 500 ) );
        redrawAllScene();
    }

    // examples from CTPL
    // pool.push(first); // function
    // pool.push(third, "additional_param");
    // pool.push( [] (int id){ std::cout << "hello from " << id << '\n'; }); // lambda
    // pool.push(std::ref(second)); // functor, reference
    // pool.push(const_cast<const Second &>(second)); // functor, copy ctor
    // pool.push(std::move(second)); // functor, move ctor

    //simulateKey( Qt::Key_2 );
}

void Window::threadCenterForce( void )
{
// initialization
    ctpl::thread_pool pool( _gv->maxThread() ); // limited thread number in the pool

    // cerr << "size = " << _cellPtr->cellVec().size() << endl;
    vector< ThreadLevelCenter * > tlc;
    tlc.resize( _cellPtr->centerVec().size() );

    //for( unsigned int i = 0; i < 2; i++ ){
    for( unsigned int i = 0; i < tlc.size(); i++ ){

        // create a new thread
        tlc[i] = new ThreadLevelCenter;

        tlc[i]->setPathwayData( _pathway, *_pathway->width(), *_pathway->height() );
        tlc[i]->setRegionData( _levelhighPtr, _boundaryVecPtr,
                               _cellPtr, _roadPtr, _lanePtr );

        tlc[i]->init( THREAD_CENTER, _gv->energyType(), i, 0 );
        pool.push([]( int id, ThreadLevelCenter* t ){ t->run( id ); }, tlc[i] );
    }

    // rendering
    while( pool.n_idle() != _gv->maxThread() ){

        //cerr << "pool.n_idle() = " << pool.n_idle() << endl;
        this_thread::sleep_for( chrono::milliseconds( 500 ) );
        redrawAllScene();
    }
    // cerr << "pool.n_idle() = " << pool.n_idle() << endl;

    // clear the memory
    for( unsigned int i = 0; i < _cellPtr->centerVec().size(); i++ ){
        delete tlc[i];
    }

}

void Window::threadCellForce( void )
{
    // initialization
    ctpl::thread_pool pool( _gv->maxThread() ); // limited thread number in the pool

    // cerr << "size = " << _cellPtr->cellVec().size() << endl;
    vector< ThreadLevelMiddle * > tlm;
    tlm.resize( _cellPtr->cellVec().size() );

    //for( unsigned int i = 0; i < 2; i++ ){
    for( unsigned int i = 0; i < tlm.size(); i++ ){

        // create a new thread
        tlm[i] = new ThreadLevelMiddle;
        //vector < unsigned int > indexVec;
        //indexVec.push_back( i );

        tlm[i]->setPathwayData( _pathway, *_pathway->width(), *_pathway->height() );
        tlm[i]->setRegionData( _levelhighPtr, _boundaryVecPtr,
                           _cellPtr, _roadPtr, _lanePtr );

        tlm[i]->init( THREAD_CELL, _gv->energyType(), i, 0 );
        pool.push([]( int id, ThreadLevelMiddle* t ){ t->run( id ); }, tlm[i] );
    }

    // rendering
    while( pool.n_idle() != _gv->maxThread() ){

         //cerr << "pool.n_idle() = " << pool.n_idle() << endl;
        this_thread::sleep_for( chrono::milliseconds( 500 ) );
        redrawAllScene();
    }
    // cerr << "pool.n_idle() = " << pool.n_idle() << endl;

    // clear the memory
    for( unsigned int i = 0; i < _cellPtr->cellVec().size(); i++ ){
        delete tlm[i];
    }

    //simulateKey( Qt::Key_W );
}

/*
void Window::threadBoneForce( void )
{
    _gv->isCellPolygonFlag() = false;
    _gv->isCellPolygonComplexFlag() = true;
    _gv->isMCLPolygonFlag() = true;
    _gv->isPathwayPolygonFlag() = false;

    // initialization
    ctpl::thread_pool pool( _gv->maxThread() ); // limited thread number in the pool

    vector< multimap< int, CellComponent > > & cellComponentVec = _cellPtr->cellComponentVec();

    // initialization
    vector< vector< ThreadLevelLow * > > tll;
    tll.resize( cellComponentVec.size() );

    for( unsigned int i = 0; i < cellComponentVec.size(); i++ ){
        multimap< int, CellComponent > &cellComponentMap = cellComponentVec[ i ];
        tll[i].resize( cellComponentMap.size() );
    }

    for( unsigned int i = 0; i < cellComponentVec.size(); i++ ){

        multimap< int, CellComponent > &cellComponentMap = cellComponentVec[ i ];

        for( unsigned int j = 0; j < cellComponentMap.size(); j++ ){

            multimap< int, CellComponent >::iterator itC = cellComponentMap.begin();
            advance( itC, j );
            CellComponent &c = itC->second;

            // create mcl force
            c.mcl.forceBone().init( &itC->second.mcl.bone(), &itC->second.contour, LEVEL_LOW, "../configs/mcl.conf" );
            c.mcl.forceBone().id() = 0;

            tll[i][j] = new ThreadLevelLow;
            tll[i][j]->setPathwayData( _pathway, *_pathway->width(), *_pathway->height() );
            tll[i][j]->setRegionData( _levelhighPtr, _boundaryVecPtr,
                                      _cellPtr, _roadPtr, _lanePtr );

            //vector < unsigned int > indexVec;
            //indexVec.push_back( i );
            //indexVec.push_back( j );
            tll[i][j]->init( THREAD_BONE, _gv->energyType(), i, j );
            pool.push([]( int id, ThreadLevelLow* t ){ t->run( id ); }, tll[i][j] );
        }
    }

    // rendering
    while( pool.n_idle() != _gv->maxThread() ){

        //cerr << "pool.n_idle() = " << pool.n_idle() << endl;
        this_thread::sleep_for( chrono::milliseconds( 500 ) );
        redrawAllScene();
    }

    // clear the memory
    for( unsigned int i = 0; i < cellComponentVec.size(); i++ ){

        multimap< int, CellComponent > &cellComponentMap = cellComponentVec[ i ];

        for( unsigned int j = 0; j < cellComponentMap.size(); j++ ) {
            delete tll[i][j];
        }
    }
}
*/

void Window::threadPathwayForce( void )
{
    _gv->isCellPolygonComplexFlag() = false;
    _gv->isPathwayPolygonFlag() = true;

    // initialization
    vector< multimap< int, CellComponent > > &cellComponentVec = _cellPtr->cellComponentVec();
    ctpl::thread_pool pool( _gv->maxThread() ); // limited thread number in the pool
    vector< vector< ThreadLevelDetailed * > > tld;

    vector< vector< unsigned int > > idMat;
    idMat.resize( cellComponentVec.size() );
    tld.resize( idMat.size() );
    unsigned int idD = 0;
    unsigned int threadNo = 0;
    for( unsigned int i = 0; i < idMat.size(); i++ ){

        multimap< int, CellComponent > &cellComponentMap = cellComponentVec[i];
        idMat[i].resize( cellComponentMap.size() );
        tld[i].resize( idMat[i].size() );
        for( unsigned int j = 0; j < idMat[i].size(); j++ ){

            idMat[i][j] = idD;
            idD++;
            threadNo++;
        }
        cerr << "no. of component = " << idMat[i].size() << endl;
    }
    cerr << "no. of threads = " << threadNo << endl;

    // cerr << "idD = " << idD << endl;
    // cerr << "From main thread: " << QThread::currentThreadId() << endl;
    unsigned int idC = 0;
    //for( unsigned int i = 0; i < 1; i++ ){
    for( unsigned int i = 0; i < cellComponentVec.size(); i++ ){


        multimap< int, CellComponent > &cellComponentMap = cellComponentVec[i];

        for( unsigned int j = 0; j < cellComponentMap.size(); j++ ){

            multimap< int, CellComponent >::iterator itC = cellComponentMap.begin();
            advance( itC, j );
            CellComponent &c = itC->second;

            c.detail.forceBone().init( &c.detail.bone(), &c.contour.contour(), LEVEL_DETAIL, "../configs/pathway.conf" );
            c.detail.forceBone().id() = idC;

            cerr << endl << endl << "enable a thread... i = " << i << " j = " << j << endl;

            tld[i][j] = new ThreadLevelDetailed;
            tld[i][j]->setPathwayData( _pathway, *_pathway->width(), *_pathway->height() );
            tld[i][j]->setRegionData( _levelhighPtr, _boundaryVecPtr,
                                   _cellPtr, _roadPtr, _lanePtr );
            tld[i][j]->init( THREAD_PATHWAY, _gv->energyType(), i, j );
            pool.push([]( int id, ThreadLevelDetailed* t ){ t->run( id ); }, tld[i][j] );

            idC++;
        }
    }

    // rendering
    while( pool.n_idle() != _gv->maxThread() ){

        cerr << "pool.n_idle() = " << pool.n_idle() << endl;
        this_thread::sleep_for( chrono::milliseconds( 500 ) );
        redrawAllScene();
    }
}

void Window::steinertree( void )
{
    // select a target metabolite
    MetaboliteGraph             &g         = _pathway->g();

    // highlight
    BGL_FORALL_VERTICES( vd, g, MetaboliteGraph ) {
        if( g[ vd ].type == "metabolite" ){
            //if( *g[ vd ].namePtr == "Glucose" ){
            //if( *g[ vd ].namePtr == "glu_L[c]" ){
            if( *g[ vd ].namePtr == "glu_L[c]" ){
            //if( *g[ vd ].namePtr == "Soy_Sauce" ){
                *g[ vd ].isSelectedPtr = true;
            }
        }
    }

    // compute steiner tree
    _roadPtr->initRoad( _cellPtr );
    // _roadPtr->initRoad( _cellPtr->cellComponentVec() );
    _roadPtr->buildRoad();
    // cerr << "road built..." << endl;

    _lanePtr->clear();
    _lanePtr->resize( 1 );
    (*_lanePtr)[0].setPathwayData( _pathway, *_pathway->width(), *_pathway->height() );
    (*_lanePtr)[0].initSteinerNet( _cellPtr->cellComponentVec() );
    (*_lanePtr)[0].steinerTree();


#ifdef SKIP
    // compute steiner tree
    _roadPtr->initRoad( _cellPtr );
    // _roadPtr->initRoad( _cellPtr->cellComponentVec() );
    _roadPtr->buildRoad();
    // cerr << "road built..." << endl;
    _gv->isRoadFlag() = true;

    _lanePtr->clear();
    _lanePtr->resize( 1 );
    for( unsigned int i = 0; i < _lanePtr->size(); i++ ){
        vector < Highway > &highwayVec = _roadPtr->highwayMat()[i];
        (*_lanePtr)[0].setPathwayData( _pathway, *_pathway->width(), *_pathway->height() );
        (*_lanePtr)[0].initLane( 0, _cellPtr->cellComponentVec()[0], &highwayVec );
        (*_lanePtr)[0].steinerTree();
    }
#endif // SKIP
}

#ifdef SKIP
void Window::steinertree( void )
{


    // compute steiner tree
    _roadPtr->initRoad( _cellPtr );
    // _roadPtr->initRoad( _cellPtr->cellComponentVec() );
    _roadPtr->buildRoad();
    // cerr << "road built..." << endl;
    _gv->isRoadFlag() = true;

    cerr << "/***************" << endl;
    cerr << "/* Steiner tree" << endl;
    cerr << "/***************" << endl;

#ifdef DEBUG
    for( unsigned int i = 0; i < _roadPtr->highwayMat().size(); i++ ){
        for( unsigned int j = 0; j < _roadPtr->highwayMat()[i].size(); j++ ){

            map< MetaboliteGraph::vertex_descriptor,
                    MetaboliteGraph::vertex_descriptor > &common = _roadPtr->highwayMat()[i][j].common;

            map< MetaboliteGraph::vertex_descriptor,
                    MetaboliteGraph::vertex_descriptor >::iterator it;
            for( it = common.begin(); it != common.end(); it++ ){
                    cerr << _pathway->subG()[i][ it->first ].id << endl;
                }
                cerr << endl;
        }
    }
#endif // DEBUG

    _lanePtr->clear();
    _lanePtr->resize( _pathway->nSubsys() );
    for( unsigned int i = 0; i < _lanePtr->size(); i++ ){
        vector < Highway > &highwayVec = _roadPtr->highwayMat()[i];
        (*_lanePtr)[i].setPathwayData( _pathway, *_pathway->width(), *_pathway->height() );
        (*_lanePtr)[i].initLane( i, _cellPtr->cellComponentVec()[i], &highwayVec );
        (*_lanePtr)[i].steinerTree();
    }
    _gv->isLaneFlag() = true;

}
#endif // SKIP


void Window::threadOctilinearBoundary( void )
{
    // initialization
    const int iter = 200;
    ctpl::thread_pool pool( _gv->maxThread() ); // limited thread number in the pool

    vector< Boundary > &boundaryVec = *_boundaryVecPtr;
    vector< ThreadOctilinearBoundary * > tob;
    tob.resize( boundaryVec.size() );

    _octilinearVecPtr->clear();
    _octilinearVecPtr->resize( boundaryVec.size() );

    // cerr << "boundaryVec.size() = " << boundaryVec.size() << endl;
    for( unsigned int i = 0; i < boundaryVec.size(); i++ ) {

        cerr << "select octilinear ..."
             << " boundaryVec[i].nVertices() = " << boundaryVec[i].nVertices()
             << " iter = " << iter << endl;

        // initialization
        (*_octilinearVecPtr)[i] = new Octilinear;
        (*_octilinearVecPtr)[i]->prepare( &boundaryVec[i],
                                         _content_width/2.0, _content_height/2.0 );

        // create a new thread
        tob[i] = new ThreadOctilinearBoundary;
        tob[i]->setRegionData( _levelhighPtr, _boundaryVecPtr,
                               _cellPtr, _roadPtr, _lanePtr );
        tob[i]->init( (*_octilinearVecPtr)[i], iter, (*_octilinearVecPtr)[i]->opttype(), 10 );

        pool.push([]( int id, ThreadOctilinearBoundary *t ){ t->run( id ); }, tob[i] );
    }

    // rendering
    while( pool.n_idle() != _gv->maxThread() ){

        //cerr << "pool.n_idle() = " << pool.n_idle() << endl;
        this_thread::sleep_for( chrono::milliseconds( 500 ) );
        redrawAllScene();
    }
    // cerr << "pool.n_idle() = " << pool.n_idle() << endl;

    // clear the memory
    for( unsigned int i = 0; i < boundaryVec.size(); i++ ) {
        delete tob[i];
    }

    //simulateKey( Qt::Key_O );
}

//
//  Window::buildPackageGraph --    build the boundary from the voronoi cell
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Window::buildLevelHighBoundaryGraph( void )
{
    // initialization
    _levelhighPtr->polygonComplexVD().clear();
    _boundaryVecPtr->clear();
    _boundaryVecPtr->resize(1);

    vector< Boundary > &bVec = *_boundaryVecPtr;
    BoundaryGraph &bg = bVec[0].boundary();
    unsigned int &nVertices = bVec[0].nVertices();
    unsigned int &nEdges = bVec[0].nEdges();
    unsigned int &nLines = bVec[0].nLines();

#ifdef EXAMPLE
    nVertices = 0;
    for( unsigned int i = 0; i < 5; i++ ){
        BoundaryGraph::vertex_descriptor curVD5 = add_vertex( bg );
        double x5 = 1.0, y5 = 1.0;
        if( i == 0 ){
            x5 = 1.0;
            y5 = 1.0;
        }
        if( i == 1 ){
            x5 = -0.5;
            y5 = 1.6;
        }
        if( i == 2 ){
            x5 = -1.2;
            y5 = -0.2;
        }
        if( i == 3 ){
            x5 = -0.2;
            y5 = -1.0;
        }
        if( i == 4 ){
            x5 = 1.0;
            y5 = -1.0;
        }
        bg[ curVD5 ].geoPtr       = new Coord2( x5, y5 );
        bg[ curVD5 ].smoothPtr    = new Coord2( x5, y5);
        bg[ curVD5 ].coordPtr     = new Coord2( x5, y5 );
        bg[ curVD5 ].id = bg[ curVD5 ].initID = nVertices;
        bg[ curVD5 ].namePtr = new string( to_string( bg[ curVD5 ].id ) );
        bg[ curVD5 ].weight = 1.0;
        nVertices++;
    }

    nEdges = 0;
    for( unsigned int i = 0; i < 5; i++ ) {

        BoundaryGraph::vertex_descriptor curVDS = vertex( i, bg );
        BoundaryGraph::vertex_descriptor curVDT = vertex( (i+1)%5, bg );

        // handle fore edge
        pair<BoundaryGraph::edge_descriptor, unsigned int> foreE = add_edge( curVDS, curVDT, bg );
        BoundaryGraph::edge_descriptor foreED = foreE.first;

        // calculate geographical angle
        Coord2 coordO;
        Coord2 coordD;
        if( bg[ curVDS ].initID < bg[ curVDT ].initID ){
            coordO = *bg[ curVDS ].coordPtr;
            coordD = *bg[ curVDT ].coordPtr;
        }
        else{
            coordO = *bg[ curVDT ].coordPtr;
            coordD = *bg[ curVDS ].coordPtr;
        }
        double diffX = coordD.x() - coordO.x();
        double diffY = coordD.y() - coordO.y();
        double angle = atan2( diffY, diffX );

        bg[ foreED ].initID = bg[ foreED ].id = nEdges;
        bg[ foreED ].weight = 1.0;
        bg[ foreED ].geoAngle = angle;
        bg[ foreED ].smoothAngle = angle;
        bg[ foreED ].angle = angle;
        bg[ foreED ].lineID.push_back( nLines );
        bg[ foreED ].visitedTimes = 0;

        nEdges++;
    }
#endif // EXAMPLE

    // create boundary graph
    for( unsigned int i = 0; i < _levelhighPtr->polygonComplex().size(); i++ ){

        Polygon2 &polygon = _levelhighPtr->polygonComplex()[i];
        vector< ForceGraph::vertex_descriptor > vdVec;

        unsigned int size = polygon.elements().size();
        for( unsigned int j = 1; j < size+1; j++ ){

            //Coord2 &coordS = polygon.elements()[j-1];
            //Coord2 &coordT = polygon.elements()[j%(int)polygon.elements().size()];

            // Check if the station exists or not
            BoundaryGraph::vertex_descriptor curVDS = NULL;
            BoundaryGraph::vertex_descriptor curVDT = NULL;

            // add vertices
            for( unsigned int k = 0; k < 2; k++ ){

                BoundaryGraph::vertex_descriptor curVD = NULL;

                // Check if the station exists or not
                BGL_FORALL_VERTICES( vd, bg, BoundaryGraph )
                    {
                        Coord2 &c = *bg[ vd ].coordPtr;
                        if( fabs( ( polygon.elements()[ (j-1+k)%(int)size]-c ).norm() ) < 1e-2 ){

#ifdef DEBUG
                            cerr << "The node has been in the database." << endl;
#endif  // DEBUG
                            if( k == 0 ) curVD = curVDS = vd;
                            if( k == 1 ) curVD = curVDT = vd;
                            break;
                        }
                    }

                if ( curVD == NULL ){

                    curVD = add_vertex( bg );
                    //vector< unsigned int > lineID = bg[ curVD ].lineID;
                    //lineID.push_back( nLines );

                    double x = polygon.elements()[j-1+k].x();
                    double y = polygon.elements()[j-1+k].y();
                    bg[ curVD ].geoPtr       = new Coord2( x, y );
                    bg[ curVD ].smoothPtr    = new Coord2( x, y );
                    bg[ curVD ].coordPtr     = new Coord2( x, y );

                    bg[ curVD ].id = bg[ curVD ].initID = nVertices;
                    bg[ curVD ].namePtr = new string( to_string( bg[ curVD ].id ) );
                    bg[ curVD ].weight = 1.0;
                    //bg[ curVD ].lineID.push_back( nLines );

                    if( k == 0 ) curVDS = curVD;
                    if( k == 1 ) curVDT = curVD;
                    nVertices++;
                }
                else{
                    bg[ curVD ].lineID.push_back( nLines );
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
            tie( oldED, found ) = edge( curVDS, curVDT, bg );


            if ( found == true ) {

                bg[ oldED ].lineID.push_back( nLines );
                bg[ oldED ].visitedTimes += 1;
                //eachline.push_back( oldED );
                //bool test = false;
                //tie( oldED, test ) = edge( oldVT, oldVS, _boundary );
            }
            else{

                //BoundaryGraph::vertex_descriptor src = vertex( indexS, _boundary );
                //BoundaryGraph::vertex_descriptor tar = vertex( indexT, _boundary );

                // handle fore edge
                pair<BoundaryGraph::edge_descriptor, unsigned int> foreE = add_edge( curVDS, curVDT, bg );
                BoundaryGraph::edge_descriptor foreED = foreE.first;

                // calculate geographical angle
                Coord2 coordO;
                Coord2 coordD;
                if( bg[ curVDS ].initID < bg[ curVDT ].initID ){
                    coordO = *bg[ curVDS ].coordPtr;
                    coordD = *bg[ curVDT ].coordPtr;
                }
                else{
                    coordO = *bg[ curVDT ].coordPtr;
                    coordD = *bg[ curVDS ].coordPtr;
                }
                double diffX = coordD.x() - coordO.x();
                double diffY = coordD.y() - coordO.y();
                double angle = atan2( diffY, diffX );

                bg[ foreED ].initID = bg[ foreED ].id = nEdges;
                bg[ foreED ].weight = 1.0;
                bg[ foreED ].geoAngle = angle;
                bg[ foreED ].smoothAngle = angle;
                bg[ foreED ].angle = angle;
                bg[ foreED ].lineID.push_back( nLines );
                bg[ foreED ].visitedTimes = 0;

                //eachline.push_back( foreED );
#ifdef  DEBUG
                cout << "nEdges = " << _nEdges << " angle = " << angle << endl;
#endif  // DEBUG
                nEdges++;
            }
        }
        _levelhighPtr->polygonComplexVD().insert( pair< unsigned int, vector< ForceGraph::vertex_descriptor > >( i, vdVec ) );
    }

    // update the fixed flag
    BGL_FORALL_EDGES( ed, bg, BoundaryGraph )
    {
        if( bg[ ed ].visitedTimes == 0 ){

            BoundaryGraph::vertex_descriptor vdS = source( ed, bg );
            BoundaryGraph::vertex_descriptor vdT = target( ed, bg );

            bg[ vdS ].isFixed = true;
            bg[ vdT ].isFixed = true;

            // cerr << "eid = " << bg[ ed ].id << " node = " << bg[ vdS ]. id << " ," << bg[ vdT ].id << endl;
        }
    }

    // create lines
    // _line.resize( _polygonComplexVD.size() );
    map< unsigned int, vector< BoundaryGraph::vertex_descriptor > >::iterator itS;
    // map< unsigned int, vector< BoundaryGraph::vertex_descriptor > >::itereator itT;
    for( itS = _levelhighPtr->polygonComplexVD().begin(); itS != _levelhighPtr->polygonComplexVD().end(); itS++ ){
        vector< BoundaryGraph::vertex_descriptor > &vdVec = itS->second;
        bVec[0].lineSta().push_back( vdVec );
    }
    nLines = _levelhighPtr->polygonComplexVD().size();

#ifdef DEBUG
    cerr << "nV = " << nVertices << " nE = " << nEdges
         << " nL = " << nLines << endl;
#endif // DEBUG
}


//
//  Window::selectLevelMiddleBuildBoundary --    build the boundary from the voronoi cell
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Window::selectLevelMiddleBuildBoundary( void )
{
    //clearGraph( _simplifiedBoundaryPtr->boundary() );

    buildLevelMiddleBoundaryGraph();
    _gv->isSimplifiedFlag() = false;
    redrawAllScene();
}

//
//  Window::buildLevelMiddleBoundaryGraph --    build the boundary from the voronoi cell
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Window::buildLevelMiddleBoundaryGraph( void )
{
    // initialization
    _boundaryVecPtr->clear();
    _boundaryVecPtr->resize( _cellPtr->cellComponentVec().size() );
    vector< Boundary > &bVec = *_boundaryVecPtr;

    cerr << "cellComponentVec.size() = " <<  _cellPtr->cellComponentVec().size() << endl;
    cerr << "_boundaryVecPtr->size() = " << _boundaryVecPtr->size() << endl;
    for( unsigned int i = 0; i < bVec.size(); i++ ) {

        BoundaryGraph &bg = bVec[i].boundary();
        unsigned int &nVertices = bVec[i].nVertices();
        unsigned int &nEdges = bVec[i].nEdges();
        unsigned int &nLines = bVec[i].nLines();

        // create boundary graph
        resetVisitedTimes( bg );

        multimap< int, CellComponent > &cellComponent = _cellPtr->cellComponentVec()[i];
        unsigned int id = 0;
        for( multimap< int, CellComponent >::iterator itC = cellComponent.begin();
             itC != cellComponent.end(); itC++ ){

            Polygon2 &polygon = itC->second.contour.contour();
            vector< ForceGraph::vertex_descriptor > vdVec;
            unsigned int size = polygon.elements().size();

            for( unsigned int j = 1; j < size+1; j++ ){

                // Check if the station exists or not
                BoundaryGraph::vertex_descriptor curVDS = NULL;
                BoundaryGraph::vertex_descriptor curVDT = NULL;

                // add vertices
                for( unsigned int k = 0; k < 2; k++ ){

                    BoundaryGraph::vertex_descriptor curVD = NULL;

                    // Check if the station exists or not
                    BGL_FORALL_VERTICES( vd, bg, BoundaryGraph )
                        {
                            Coord2 &c = *bg[ vd ].coordPtr;
                            if( fabs( ( polygon.elements()[ (j-1+k)%(int)size]-c ).norm() ) < 1e-2 ){

#ifdef DEBUG
                                cerr << "The node has been in the database." << endl;
#endif  // DEBUG
                                if( k == 0 ) curVD = curVDS = vd;
                                if( k == 1 ) curVD = curVDT = vd;
                                break;
                            }
                        }

                    if ( curVD == NULL ){

                        curVD = add_vertex( bg );
                        //vector< unsigned int > lineID = bg[ curVD ].lineID;
                        //lineID.push_back( nLines );

                        double x = polygon.elements()[j-1+k].x();
                        double y = polygon.elements()[j-1+k].y();
                        bg[ curVD ].geoPtr       = new Coord2( x, y );
                        bg[ curVD ].smoothPtr    = new Coord2( x, y );
                        bg[ curVD ].coordPtr     = new Coord2( x, y );

                        bg[ curVD ].id = bg[ curVD ].initID = nVertices;
                        bg[ curVD ].namePtr = new string( to_string( bg[ curVD ].id ) );
                        bg[ curVD ].weight = 1.0;
                        //bg[ curVD ].lineID.push_back( nLines );

                        if( k == 0 ) curVDS = curVD;
                        if( k == 1 ) curVDT = curVD;
                        nVertices++;
                    }
                    else{
                        bg[ curVD ].lineID.push_back( nLines );
#ifdef DEBUG
                        cerr << "[Existing] curV : lineID = " << endl;
        for ( unsigned int k = 0; k < vertexLineID[ curVD ].size(); ++k )
            cerr << "lineID[ " << setw( 3 ) << k << " ] = " << vertexLineID[ curVD ][ k ] << endl;
#endif  // DEBUG
                    }
                }
                vdVec.push_back( curVDS );
                // cerr << _boundary[ curVDS ].id << " ";
                // cerr << "( " << _boundary[ curVDS ].id << ", " << _boundary[ curVDT ].id << " )" << endl;

                // add edges
                // search previous edge
                bool found = false;
                BoundaryGraph::edge_descriptor oldED;
                //BoundaryGraph::vertex_descriptor oldVS = ptrSta[ origID ];
                //BoundaryGraph::vertex_descriptor oldVT = ptrSta[ destID ];
                //unsigned int indexS = _boundary[ curVDS ].initID;
                //unsigned int indexT = _boundary[ curVDT ].initID;
                tie( oldED, found ) = edge( curVDS, curVDT, bg );

                if ( found == true ) {

                    bg[ oldED ].lineID.push_back( nLines );
                    bg[ oldED ].visitedTimes += 1;
                    //eachline.push_back( oldED );
                    //bool test = false;
                    //tie( oldED, test ) = edge( oldVT, oldVS, _boundary );
                }
                else{

                    // handle fore edge
                    pair<BoundaryGraph::edge_descriptor, unsigned int> foreE = add_edge( curVDS, curVDT, bg );
                    BoundaryGraph::edge_descriptor foreED = foreE.first;

                    // calculate geographical angle
                    Coord2 coordO;
                    Coord2 coordD;
                    if( bg[ curVDS ].initID < bg[ curVDT ].initID ){
                        coordO = *bg[ curVDS ].coordPtr;
                        coordD = *bg[ curVDT ].coordPtr;
                    }
                    else{
                        coordO = *bg[ curVDT ].coordPtr;
                        coordD = *bg[ curVDS ].coordPtr;
                    }
                    double diffX = coordD.x() - coordO.x();
                    double diffY = coordD.y() - coordO.y();
                    double angle = atan2( diffY, diffX );

                    bg[ foreED ].initID = bg[ foreED ].id = nEdges;
                    bg[ foreED ].weight = 1.0;
                    bg[ foreED ].geoAngle = angle;
                    bg[ foreED ].smoothAngle = angle;
                    bg[ foreED ].angle = angle;
                    bg[ foreED ].lineID.push_back( nLines );
                    bg[ foreED ].visitedTimes = 0;

                    //eachline.push_back( foreED );
#ifdef  DEBUG
                    cout << "nEdges = " << _nEdges << " angle = " << angle << endl;
#endif  // DEBUG
                    nEdges++;
                }
            }
            itC->second.polygonComplexVD = vdVec;
            id++;
        }

        // update the fixed flag
        BGL_FORALL_EDGES( ed, bg, BoundaryGraph )
            {
                if( bg[ ed ].visitedTimes == 0 ){

                    BoundaryGraph::vertex_descriptor vdS = source( ed, bg );
                    BoundaryGraph::vertex_descriptor vdT = target( ed, bg );

                    bg[ vdS ].isFixed = true;
                    bg[ vdT ].isFixed = true;

                    // cerr << "eid = " << bg[ ed ].id << " node = " << bg[ vdS ]. id << " ," << bg[ vdT ].id << endl;
                }
            }
        // cerr << endl;

        cerr << "MiddleBuildBoundary::nV = " << num_vertices( bVec[i].boundary() ) << endl;

#ifdef SKIP
        // create lines
    // _line.resize( _polygonComplexVD.size() );
    map< unsigned int, vector< BoundaryGraph::vertex_descriptor > >::iterator itS;
    // map< unsigned int, vector< BoundaryGraph::vertex_descriptor > >::itereator itT;
    for( itS = _levelhighPtr->polygonComplexVD().begin(); itS != cellVec[ m ].polygonComplexVD().end(); itS++ ){
        vector< BoundaryGraph::vertex_descriptor > &vdVec = itS->second;
        _boundaryPtr->lineSta().push_back( vdVec );
    }
    nLines = cellVec[ m ].polygonComplexVD().size();
#endif // SKIP

#ifdef DEBUG
        cerr << "nV = " << _nVertices << " nE = " << _nEdges
     << " nL = " << _nLines << endl;
#endif // DEBUG

    }

    cerr << "finishing building the middle graph..." << endl;
}

//
//  Window::selectLevelDetailBuildBoundary --    build the boundary from the voronoi cell
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Window::selectLevelDetailBuildBoundary( void )
{
    buildLevelDetailBoundaryGraph();
    _gv->isSimplifiedFlag() = false;
    _gv->isBoundaryFlag() = true;
    redrawAllScene();
}

//
//  Window::buildLevelDetailBoundaryGraph --    build the boundary from the voronoi cell
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Window::buildLevelDetailBoundaryGraph( void ) {

    vector< multimap <int, CellComponent> > &cellCVec = _cellPtr->cellComponentVec();

    // initialization
    _levelhighPtr->polygonComplexVD().clear();
    _boundaryVecPtr->clear();
    //_boundaryVecPtr->resize(cellCVec.size());
    //vector<Boundary> &bVec = *_boundaryVecPtr;

    //for (unsigned int m = 0; m < 1; m++) {
    unsigned int index = 0;
    for (unsigned int m = 0; m < cellCVec.size(); m++) {

        multimap< int, CellComponent > &componentMap = cellCVec[m];
        multimap< int, CellComponent >::iterator itC = componentMap.begin();

        index += componentMap.size();
    }
    _boundaryVecPtr->resize( index );

    index = 0;
    vector<Boundary> &bVec = *_boundaryVecPtr;
    for (unsigned int m = 0; m < cellCVec.size(); m++) {

        multimap< int, CellComponent > &componentMap = cellCVec[m];
        multimap< int, CellComponent >::iterator itC = componentMap.begin();

        //for (unsigned int n = 0; n < 1; n++) {
        for( ; itC != componentMap.end(); itC++ ){

            //Boundary b;
            //_boundaryVecPtr->push_back( b );

            BoundaryGraph &bg = bVec[index].boundary();
            unsigned int &nVertices = bVec[index].nVertices();
            unsigned int &nEdges = bVec[index].nEdges();
            unsigned int &nLines = bVec[index].nLines();
            index++;

            // initialization
            bg.clear();
            nVertices = nEdges = nLines = 0;
            resetVisitedTimes( bg );
            Force &f = itC->second.detail.forceBone();

            // draw polygons
            vector< Seed > &seedVec = *f.voronoi().seedVec();
            for( unsigned int i = 0; i < seedVec.size(); i++ ) {

                Polygon2 &polygon = seedVec[i].cellPolygon;
                vector< ForceGraph::vertex_descriptor > vdVec;

                unsigned int size = polygon.elements().size();
                for( unsigned int j = 1; j < size+1; j++ ){

                    // Check if the station exists or not
                    BoundaryGraph::vertex_descriptor curVDS = NULL;
                    BoundaryGraph::vertex_descriptor curVDT = NULL;

                    // add vertices
                    for( unsigned int k = 0; k < 2; k++ ){

                        BoundaryGraph::vertex_descriptor curVD = NULL;

                        // Check if the station exists or not
                        BGL_FORALL_VERTICES( vd, bg, BoundaryGraph )
                        {
                            Coord2 &c = *bg[ vd ].coordPtr;
                            if( fabs( ( polygon.elements()[ (j-1+k)%(int)size]-c ).norm() ) < 1e-2 ){

#ifdef DEBUG
                                cerr << "The node has been in the database." << endl;
#endif  // DEBUG
                                if( k == 0 ) curVD = curVDS = vd;
                                if( k == 1 ) curVD = curVDT = vd;
                                break;
                            }
                        }

                        if ( curVD == NULL ){

                            curVD = add_vertex( bg );
                            //vector< unsigned int > lineID = bg[ curVD ].lineID;
                            //lineID.push_back( nLines );

                            double x = polygon.elements()[j-1+k].x();
                            double y = polygon.elements()[j-1+k].y();
                            bg[ curVD ].geoPtr       = new Coord2( x, y );
                            bg[ curVD ].smoothPtr    = new Coord2( x, y );
                            bg[ curVD ].coordPtr     = new Coord2( x, y );

                            bg[ curVD ].id = bg[ curVD ].initID = nVertices;
                            bg[ curVD ].namePtr = new string( to_string( bg[ curVD ].id ) );
                            bg[ curVD ].weight = 1.0;
                            //bg[ curVD ].lineID.push_back( nLines );

                            if( k == 0 ) curVDS = curVD;
                            if( k == 1 ) curVDT = curVD;
                            nVertices++;
                        }
                        else{
                            bg[ curVD ].lineID.push_back( nLines );
#ifdef DEBUG
                            cerr << "[Existing] curV : lineID = " << endl;
                            for ( unsigned int k = 0; k < vertexLineID[ curVD ].size(); ++k )
                                cerr << "lineID[ " << setw( 3 ) << k << " ] = " << vertexLineID[ curVD ][ k ] << endl;
#endif  // DEBUG
                        }
                    }
                    vdVec.push_back( curVDS );
                    // cerr << _boundary[ curVDS ].id << " ";
                    // cerr << "( " << _boundary[ curVDS ].id << ", " << _boundary[ curVDT ].id << " )" << endl;

                    // add edges
                    // search previous edge
                    bool found = false;
                    BoundaryGraph::edge_descriptor oldED;
                    //BoundaryGraph::vertex_descriptor oldVS = ptrSta[ origID ];
                    //BoundaryGraph::vertex_descriptor oldVT = ptrSta[ destID ];
                    //unsigned int indexS = _boundary[ curVDS ].initID;
                    //unsigned int indexT = _boundary[ curVDT ].initID;
                    tie( oldED, found ) = edge( curVDS, curVDT, bg );


                    if ( found == true ) {

                        bg[ oldED ].lineID.push_back( nLines );
                        bg[ oldED ].visitedTimes += 1;
                        //eachline.push_back( oldED );
                        //bool test = false;
                        //tie( oldED, test ) = edge( oldVT, oldVS, _boundary );
                    }
                    else{

                        //BoundaryGraph::vertex_descriptor src = vertex( indexS, _boundary );
                        //BoundaryGraph::vertex_descriptor tar = vertex( indexT, _boundary );

                        // handle fore edge
                        pair<BoundaryGraph::edge_descriptor, unsigned int> foreE = add_edge( curVDS, curVDT, bg );
                        BoundaryGraph::edge_descriptor foreED = foreE.first;

                        // calculate geographical angle
                        Coord2 coordO;
                        Coord2 coordD;
                        if( bg[ curVDS ].initID < bg[ curVDT ].initID ){
                            coordO = *bg[ curVDS ].coordPtr;
                            coordD = *bg[ curVDT ].coordPtr;
                        }
                        else{
                            coordO = *bg[ curVDT ].coordPtr;
                            coordD = *bg[ curVDS ].coordPtr;
                        }
                        double diffX = coordD.x() - coordO.x();
                        double diffY = coordD.y() - coordO.y();
                        double angle = atan2( diffY, diffX );

                        bg[ foreED ].initID = bg[ foreED ].id = nEdges;
                        bg[ foreED ].weight = 1.0;
                        bg[ foreED ].geoAngle = angle;
                        bg[ foreED ].smoothAngle = angle;
                        bg[ foreED ].angle = angle;
                        bg[ foreED ].lineID.push_back( nLines );
                        bg[ foreED ].visitedTimes = 0;

                        //eachline.push_back( foreED );
#ifdef  DEBUG
                        cout << "nEdges = " << _nEdges << " angle = " << angle << endl;
#endif  // DEBUG
                        nEdges++;
                    }
                }
                //map< unsigned int, vector< BoundaryGraph::vertex_descriptor > >::iterator itP;
                itC->second.detail.polygonComplexVD().insert( pair< unsigned int, vector< BoundaryGraph::vertex_descriptor > >
                        ( i, vdVec ) );
                cerr << i << ", " << _boundaryVecPtr->size() << endl;
                cerr << " vdVec.size() = " << vdVec.size() << " ?= " << size << endl;
                for( unsigned int q = 0; q < vdVec.size(); q++ ){
                    cerr << "id = " << bg[vdVec[q]].id << endl;
                }
                //id++;
            }

            // update the fixed flag
            BGL_FORALL_EDGES( ed, bg, BoundaryGraph )
            {
                if( bg[ ed ].visitedTimes == 0 ){

                    BoundaryGraph::vertex_descriptor vdS = source( ed, bg );
                    BoundaryGraph::vertex_descriptor vdT = target( ed, bg );

                    bg[ vdS ].isFixed = true;
                    bg[ vdT ].isFixed = true;

                    // cerr << "eid = " << bg[ ed ].id << " node = " << bg[ vdS ]. id << " ," << bg[ vdT ].id << endl;
                }
            }

            printGraph( bg );
            cerr << endl;
            cerr << "DetailBuildBoundary::nV = " << num_vertices(bVec[ bVec.size()-1 ].boundary()) << endl;
        }
    }

    cerr << "index = " << index << endl;
    cerr << "finishing building the detailed graph..." << endl;
}

//
//  Window::updatePolygonComplex --    update the ploygon complex
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Window::updateLevelHighPolygonComplex( void )
{
    cerr << "updating high polygonComplex after optimization ..." << endl;

    vector< Boundary > &bVec = *_boundaryVecPtr;
    BoundaryGraph &bg = bVec[0].boundary();
    map< unsigned int, vector< ForceGraph::vertex_descriptor > >::iterator itP;
    map< unsigned int, Polygon2 >::iterator itC = _levelhighPtr->polygonComplex().begin();
    for( itP = _levelhighPtr->polygonComplexVD().begin(); itP != _levelhighPtr->polygonComplexVD().end(); itP++ ){

        vector< ForceGraph::vertex_descriptor > &p = itP->second;
        for( unsigned int i = 0; i < p.size(); i++ ){
            itC->second.elements()[i].x() = bg[ p[i] ].coordPtr->x();
            itC->second.elements()[i].y() = bg[ p[i] ].coordPtr->y();

            // cerr << "i = " << i << " " << itC->second.elements()[i];
        }
        itC++;
    }
}

//
//  Window::updateLevelMiddlePolygonComplex --    update the ploygon complex
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Window::updateLevelMiddlePolygonComplex( void )
{
    cerr << "updating middle polygonComplex after optimization ..." << endl;

    vector< Boundary > &bVec = *_boundaryVecPtr;
    for( unsigned int i = 0; i < bVec.size(); i++ ) {

        BoundaryGraph &bg = bVec[i].boundary();

        multimap< int, CellComponent > &cellComponent = _cellPtr->cellComponentVec()[i];
        for( multimap< int, CellComponent >::iterator itC = _cellPtr->cellComponentVec()[i].begin();
             itC != _cellPtr->cellComponentVec()[i].end(); itC++ ){

            vector< ForceGraph::vertex_descriptor > &polygonComplexVD = itC->second.polygonComplexVD;
            for( unsigned int j = 0; j < polygonComplexVD.size(); j++ ){
                itC->second.contour.contour().elements()[j].x() = bg[ polygonComplexVD[j] ].coordPtr->x();
                itC->second.contour.contour().elements()[j].y() = bg[ polygonComplexVD[j] ].coordPtr->y();

                // cerr << "j = " << j << " " << itC->second.contour.elements()[j];
            }
        }
    }

    // clean contour
    _cellPtr->cleanPolygonComplex();

    // compute curvy contour
    vector< multimap< int, CellComponent > > &cellCVec    = _cellPtr->cellComponentVec();
    for( unsigned int k = 0; k < cellCVec.size(); k++ ){

        multimap< int, CellComponent > &componentMap = cellCVec[k];
        multimap< int, CellComponent >::iterator itC = componentMap.begin();
        for( ; itC != componentMap.end(); itC++ ){

            itC->second.contour.computeChaikinCurve( 5, 50 );
        }
    }
}

//
//  Window::updateLevelDetailPolygonComplex --    update the ploygon complex
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Window::updateLevelDetailPolygonComplex( void )
{
    cerr << "updating detail polygonComplex after optimization ..." << endl;
    vector< multimap <int, CellComponent> > &cellCVec = _cellPtr->cellComponentVec();
    vector< Boundary > &bVec = *_boundaryVecPtr;
    // cerr << "bVec.size() = " << bVec.size() << endl;

    unsigned int index = 0;
    for (unsigned int m = 0; m < cellCVec.size(); m++) {

        multimap< int, CellComponent > &componentMap = cellCVec[m];
        multimap< int, CellComponent >::iterator itC = componentMap.begin();

        //for (unsigned int n = 0; n < 1; n++) {
        for( ; itC != componentMap.end(); itC++ ){

            //cerr << "index = " << index << endl;
            Boundary &b = bVec[ index ];
            BoundaryGraph &bg = b.boundary();

            // printGraph( bg );
            // cerr << endl << endl << endl;

            Bone &detail = itC->second.detail;
            Force &force = detail.forceBone();
            vector< Seed > &seedVec = *force.voronoi().seedVec();
            for( unsigned int i = 0; i < seedVec.size(); i++ ) {

                map< unsigned int, vector< BoundaryGraph::vertex_descriptor > >::iterator itP = detail.polygonComplexVD().begin();
                advance( itP, i );

                Polygon2 &polygon = seedVec[i].cellPolygon;
                // cerr << "polygon.size() = " << polygon.elements().size() << endl;
                // cerr << "vd.size() = " << itP->second.size() << endl;
                for( unsigned int j = 0; j < polygon.elements().size(); j++ ){
                    // cerr << "i = " << i << " j = " << j << endl;
                    // cerr << " " << polygon.elements()[j];
                    // cerr << "id = " << bg[ itP->second[j] ].id << endl;
                    polygon.elements()[j].x() = bg[ itP->second[j] ].coordPtr->x();
                    polygon.elements()[j].y() = bg[ itP->second[j] ].coordPtr->y();
                }
                polygon.updateCentroid();
                ForceGraph::vertex_descriptor vd = vertex( i, detail.bone() );
                detail.bone()[vd].coordPtr->x() = polygon.centroid().x();
                detail.bone()[vd].coordPtr->y() = polygon.centroid().y();
            }
            index++;
        }
    }

    // collect cell boundary contour polygons
    for( unsigned int i = 0; i < cellCVec.size(); i++ ){

        multimap< int, CellComponent > & cellComponentMap = cellCVec[i];
        multimap< int, CellComponent >::iterator itC;

        // cerr << "i = " << i << " size = " << cellComponentMap.size() << endl;
        for( itC = cellComponentMap.begin(); itC != cellComponentMap.end(); itC++ ){

            CellComponent &component = itC->second;
            unsigned int subsysID = component.groupID;
            Polygon2 &c = component.contour.contour();

            if( subsysID == 2 ) cerr << "testc0 = " << c << endl;
        }
    }

    // update cell contour
    // _cellPtr->createPolygonComplexFromDetailGraph();

    // update contour
    // _cellPtr->updatePolygonComplexFromDetailGraph();
}


double computeCV( vector< double > data )
{
    double sum = 0.0, mean, standardDeviation = 0.0;

    for( unsigned int i = 0; i < data.size(); i++ ) {
        sum += data[i];
    }
    mean = sum/(double)data.size();

    for( unsigned int i = 0; i < data.size(); i++ ){
        standardDeviation += pow( data[i] - mean, 2 );
    }

    return sqrt( standardDeviation / (double)data.size() )/mean;
}

#ifdef GMAP
void Window::spaceCoverage( void )
{
    UndirectedPropertyGraph g;
    //_pathway->loadDot( g, "../dot/gmap-pathway.dot" );
    //_pathway->loadDot( g, "../dot/gmap-recipe.dot" );
    _pathway->loadDot( g, "../dot/gmap-metabolic.dot" );

    VertexIndexMap              vertexIndex     = get( vertex_index, g );
    VertexPosMap                vertexPos       = get( vertex_mypos, g );
    VertexLabelMap              vertexLabel     = get( vertex_mylabel, g );
    VertexColorMap              vertexColor     = get( vertex_mycolor, g );
    VertexXMap                  vertexX         = get( vertex_myx, g );
    VertexYMap                  vertexY         = get( vertex_myy, g );
    EdgeIndexMap                edgeIndex       = get( edge_index, g );
    EdgeWeightMap               edgeWeight      = get( edge_weight, g );


    int neighborNo = 5;
    vector< double > neighbor;
    vector< double > area;
    Polygon2 contour;

    double minX = INFINITY, maxX = -INFINITY, minY = INFINITY, maxY = -INFINITY;
    BGL_FORALL_VERTICES( vd, g, UndirectedPropertyGraph ) {
        if( minX > vertexX[vd] ) minX = vertexX[vd];
        if( minY > vertexY[vd] ) minY = vertexY[vd];
        if( maxX < vertexX[vd] ) maxX = vertexX[vd];
        if( maxY < vertexY[vd] ) maxY = vertexY[vd];
    }
    cerr << "mixX = " << minX << " minY = " << minY << endl;
    cerr << "maxX = " << maxX << " maxY = " << maxY << endl;
    // pathway
    //contour.elements().push_back( Coord2( -51.067, -65.033 ) );
    //contour.elements().push_back( Coord2( 2236.6, -65.033 ) );
    //contour.elements().push_back( Coord2( 2236.6, 1616.8 ) );
    //contour.elements().push_back( Coord2( -51.067, 1616.8 ) );
    // recipe
    //contour.elements().push_back( Coord2( -66.788, -85.048 ) );
    //contour.elements().push_back( Coord2( 5404.7, -85.048 ) );
    //contour.elements().push_back( Coord2( 5404.7, 4966.6 ) );
    //contour.elements().push_back( Coord2( -66.788, 4966.6 ) );
    // metabolic
    // "-99.785,-68.388,4445.4,4205.1"
    contour.elements().push_back( Coord2( -99.785, -68.388 ) );
    contour.elements().push_back( Coord2( 4445.4 , -68.388 ) );
    contour.elements().push_back( Coord2( 4445.4, 4205.1 ) );
    contour.elements().push_back( Coord2( -99.785 , 4205.1) );

    // voronoi
    Voronoi v;
    vector< Seed > seedVec;
    BGL_FORALL_VERTICES( vd, g, UndirectedPropertyGraph ) {

        Seed seed;
        seed.id = vertexIndex[vd];
        seed.weight = 1.0;
        seed.coord = Coord2( vertexX[vd], vertexY[vd] );
        seedVec.push_back( seed );
    }
    v.init( seedVec, contour );
    v.id() = 0;
    v.createVoronoiDiagram( false );  // true: weighted, false: uniformed

    for( unsigned int i = 0; i < seedVec.size(); i++ ){
        Polygon2 &p = seedVec[i].cellPolygon;
        p.updateCentroid();
        area.push_back( p.area() );
        //cerr << "area = " << p.area() << endl;
    }

    // neighbor
    vector< Coord2 > coordVec;
    BGL_FORALL_VERTICES( vd, g, UndirectedPropertyGraph ) {

        coordVec.push_back( Coord2( vertexX[vd], vertexY[vd] ) );
    }

    for( unsigned int i = 0; i < coordVec.size(); i++ ){

        multimap< double, double > sortedDist;
        for( unsigned int j = 0; j < coordVec.size(); j++ ){

            double dist = (coordVec[i] - coordVec[j]).norm();
            sortedDist.insert( pair< double, double >( dist, dist ) );
        }
        double sum = 0;
        for( unsigned int j = 0; j < neighborNo; j++ ){
            multimap< double, double >::iterator it = sortedDist.begin();
            advance( it, j );
            sum += it->first;
        }
        neighbor.push_back( sum );
    }

    cerr << "neighbor CV = " << computeCV( neighbor ) << endl;
    cerr << "area CV = " << computeCV( area ) << endl;
}
#endif // GMAP


void Window::spaceCoverage( void )
{
    int neighborNo = 5;
    vector< double > neighbor;
    vector< double > area;
    vector< ForceGraph > &lsubg =  _pathway->lsubG();
    Polygon2 contour;

    contour.elements().push_back( Coord2( - 0.5*_content_width, - 0.5*_content_height ) );
    contour.elements().push_back( Coord2( + 0.5*_content_width, - 0.5*_content_height ) );
    contour.elements().push_back( Coord2( + 0.5*_content_width, + 0.5*_content_height ) );
    contour.elements().push_back( Coord2( - 0.5*_content_width, + 0.5*_content_height ) );
    cerr << "content_width = " << _content_width << " content_height = " << _content_height << endl;

    // voronoi
    Voronoi v;
    vector< Seed > seedVec;
    unsigned int index;
    for( unsigned int i = 0; i < lsubg.size(); i++ ){

        BGL_FORALL_VERTICES( vd, lsubg[i], ForceGraph ) {

            Seed seed;
            seed.id = lsubg[i][vd].id+index;
            seed.weight = 1.0;
            seed.coord = *lsubg[i][vd].coordPtr;
            seedVec.push_back( seed );
        }
        index += num_vertices( lsubg[i] );
    }
    v.init( seedVec, contour );
    v.id() = 0;
    v.createVoronoiDiagram( false );  // true: weighted, false: uniformed

    for( unsigned int i = 0; i < seedVec.size(); i++ ){
        Polygon2 &p = seedVec[i].cellPolygon;
        p.updateCentroid();
        area.push_back( p.area() );
        //cerr << "area = " << p.area() << endl;
    }

    // neighbor
    vector< Coord2 > coordVec;
    for( unsigned int i = 0; i < lsubg.size(); i++ ){

        BGL_FORALL_VERTICES( vd, lsubg[i], ForceGraph ) {

            coordVec.push_back( *lsubg[i][vd].coordPtr );
        }
    }
    for( unsigned int i = 0; i < coordVec.size(); i++ ){

        multimap< double, double > sortedDist;
        for( unsigned int j = 0; j < coordVec.size(); j++ ){

            double dist = (coordVec[i] - coordVec[j]).norm();
            sortedDist.insert( pair< double, double >( dist, dist ) );
        }
        double sum = 0;
        for( unsigned int j = 0; j < neighborNo; j++ ){
            multimap< double, double >::iterator it = sortedDist.begin();
            advance( it, j );
            sum += it->first;
        }
        neighbor.push_back( sum );
    }

    cerr << "neighbor CV = " << computeCV( neighbor ) << endl;
    cerr << "area CV = " << computeCV( area ) << endl;
}


void Window::timerEvent( QTimerEvent *event )
{
    Q_UNUSED( event );

#ifdef RECORD_VIDEO
    if( event->timerId() == _timerVideoID ){
        // cerr << "event->timerId() = " << event->timerId() << endl;
        timerVideo();
    }
#endif // RECORD_VIDEO

    redrawAllScene();
}


void Window::keyPressEvent( QKeyEvent *event )
{
    switch( event->key() ) {

        case Qt::Key_1:
        {
            checkInETime();
            cerr << "*********** Starting Execution Time = " << checkOutETime() << endl;
            checkInCPUTime();
            cerr << "*********** Starting CPU Time = " << checkOutCPUTime() << endl;

            //****************************************
            // initialization
            //****************************************

            //****************************************
            // optimization
            //****************************************
            threadBoundaryForce();

            //****************************************
            // rendering
            //****************************************
            redrawAllScene();

            break;
        }
        case Qt::Key_2:
        {
            //****************************************
            // initialization
            //****************************************
            simulateKey( Qt::Key_B );
            //simulateKey( Qt::Key_E );

            //****************************************
            // optimization
            //****************************************
            _levelType = LEVEL_HIGH;
            threadOctilinearBoundary();

            simulateKey( Qt::Key_O );
            simulateKey( Qt::Key_L );

            //****************************************
            // rendering
            //****************************************
            redrawAllScene();
            simulateKey( Qt::Key_E );

            cerr << "[Force-Directed] Finished Execution Time = " << checkOutETime() << endl;
            cerr << "[Force-Directed] Finished CPU Time = " << checkOutCPUTime() << endl;

            break;
        }
        case Qt::Key_Q:
        {
            checkInETime();
            cerr << "*********** Starting Execution Time = " << checkOutETime() << endl;
            checkInCPUTime();
            cerr << "*********** Starting CPU Time = " << checkOutCPUTime() << endl;

            //----------------------------------------
            // initialization
            //----------------------------------------
            _gv->isCenterPolygonFlag() = true;
            _gv->isCenterFlag() = true;

            //----------------------------------------
            // optimization
            //----------------------------------------
            threadCenterForce();

            //----------------------------------------
            // rendering
            //----------------------------------------
            redrawAllScene();
            simulateKey( Qt::Key_E );

            break;
        }
        case Qt::Key_W:
        {
            //----------------------------------------
            // initialization
            //----------------------------------------
            _gv->isCenterPolygonFlag() = false;
            _gv->isCenterFlag() = false;
            _gv->isCellFlag() = true;
            _gv->isCompositeFlag() = false;
            _gv->isPolygonFlag() = false;
            _gv->isCellPolygonComplexFlag() = true;
            // _gv->isBoundaryFlag() = false;

            //----------------------------------------
            // optimization
            //----------------------------------------
            // update initial center position
            _cellPtr->updateCenterCoords();

            //----------------------------------------
            // rendering
            //----------------------------------------
            redrawAllScene();
            simulateKey( Qt::Key_E );

            cerr << "[Force-Directed] Finished Execution Time = " << checkOutETime() << endl;
            cerr << "[Force-Directed] Finished CPU Time = " << checkOutCPUTime() << endl;

            break;
        }
        case Qt::Key_A:
        {
            checkInETime();
            cerr << "*********** Starting Execution Time = " << checkOutETime() << endl;
            checkInCPUTime();
            cerr << "*********** Starting CPU Time = " << checkOutCPUTime() << endl;

            //----------------------------------------
            // initialization
            //----------------------------------------
            _gv->isCellPolygonFlag() = true;
            _gv->isCellFlag() = true;

            // simulateKey( Qt::Key_E );

            //----------------------------------------
            // optimization
            //----------------------------------------
            threadCellForce();

            //----------------------------------------
            // rendering
            //----------------------------------------
            redrawAllScene();
            simulateKey( Qt::Key_E );

            break;
        }
        case Qt::Key_S:
        {
            //----------------------------------------
            // initialization
            //----------------------------------------
            _gv->isCellPolygonFlag() = false;
            _gv->isCellFlag() = false;
            _gv->isCellPolygonComplexFlag() = true;
            _cellPtr->createPolygonComplex();

            //----------------------------------------
            // optimization
            //----------------------------------------
            // initialization and build the boundary
            selectLevelMiddleBuildBoundary();

            _levelType = LEVEL_MIDDLE;
            threadOctilinearBoundary();
            simulateKey( Qt::Key_O );

            //----------------------------------------
            // rendering
            //----------------------------------------
            redrawAllScene();
            simulateKey( Qt::Key_E );

            cerr << "[Force-Directed] Finished Execution Time = " << checkOutETime() << endl;
            cerr << "[Force-Directed] Finished CPU Time = " << checkOutCPUTime() << endl;

            break;
        }
        case Qt::Key_Z:
        {
            checkInETime();
            cerr << "*********** Starting Execution Time = " << checkOutETime() << endl;
            checkInCPUTime();
            cerr << "*********** Starting CPU Time = " << checkOutCPUTime() << endl;

            //----------------------------------------
            // initialization
            //----------------------------------------
            _gv->isCellPolygonComplexFlag() = false;
            _gv->isPathwayPolygonFlag() = true;
            //_gv->isMCLPolygonFlag() = false;
            _gv->isSubPathwayFlag() = true;
            _levelType = LEVEL_LOW;
            _cellPtr->updatePathwayCoords();

            //----------------------------------------
            // optimization
            //----------------------------------------
            threadPathwayForce();

            //----------------------------------------
            // rendering
            //----------------------------------------
            redrawAllScene();

            break;
        }
        case Qt::Key_X:
        {
            //----------------------------------------
            // initialization
            //----------------------------------------
            // initialization and build the boundary
            selectLevelDetailBuildBoundary();
            _levelType = LEVEL_DETAIL;

            //----------------------------------------
            // optimization
            //----------------------------------------
            threadOctilinearBoundary();
            //simulateKey( Qt::Key_E );

            simulateKey( Qt::Key_O );

            //----------------------------------------
            // rendering
            //----------------------------------------
            redrawAllScene();

            cerr << "[Force-Directed] Finished Execution Time = " << checkOutETime() << endl;
            cerr << "[Force-Directed] Finished CPU Time = " << checkOutCPUTime() << endl;
            break;
        }
        case Qt::Key_R:
        {
            _gv->isBoundaryFlag() = false;
            _gv->isRoadFlag() = true;
            _gv->isPathwayPolygonFlag() = false;
            _gv->isLaneFlag() = true;
            _gv->isCellPolygonComplexFlag() = true;

            // steiner tree
            steinertree();
            simulateKey( Qt::Key_E );
            break;
        }
        case Qt::Key_D:
        {
            _pathway->exportDot();
            _pathway->exportEdges();
            break;
        }
        case Qt::Key_3:
        {
            _gv->isPathwayPolygonContourFlag() = !_gv->isPathwayPolygonContourFlag() ;
            redrawAllScene();
            break;
        }
        case Qt::Key_4:
        {
            _gv->isSkeletonFlag() = !_gv->isSkeletonFlag();
            redrawAllScene();
            break;
        }
        case Qt::Key_5:
        {
            _gv->isCompositeFlag() = !_gv->isCompositeFlag();
            redrawAllScene();
            break;
        }
        case Qt::Key_6:
        {
            _gv->isPolygonFlag() = !_gv->isPolygonFlag();
            redrawAllScene();
            break;
        }
        case Qt::Key_7:
        {
            _gv->isPolygonComplexFlag() = !_gv->isPolygonComplexFlag();
            redrawAllScene();
            break;
        }
        case Qt::Key_8:
        {
            _gv->isBoundaryFlag() = !_gv->isBoundaryFlag();
            redrawAllScene();
            break;
        }
        case Qt::Key_9:
        {
            _gv->isCellPolygonFlag() = !_gv->isCellPolygonFlag();
            _gv->isCellFlag() = !_gv->isCellFlag();
            redrawAllScene();
            break;
        }
        case Qt::Key_0:
        {
            _gv->isSubPathwayFlag() = !_gv->isSubPathwayFlag();
            redrawAllScene();
            break;
        }
        case Qt::Key_L:
        {
            _gv->isCenterFlag() = true;

            // load setting
            _pathway->initLayout( _levelhighPtr->polygonComplex() );

            // initialize cell
            _cellPtr->clear();
            _cellPtr->init( &_gv->veCoverage(), &_gv->veRatio(), &_levelhighPtr->polygonComplex() );

            break;
        }
        case Qt::Key_O:
        {
            if( _levelType == LEVEL_HIGH ) {

                updateLevelHighPolygonComplex();

                _gv->isPolygonFlag() = false;
                _gv->isCompositeFlag() = false;
                _gv->isPolygonComplexFlag() = true;
                _gv->isBoundaryFlag() = true;
            }
            else if( _levelType == LEVEL_MIDDLE ) {

                updateLevelMiddlePolygonComplex();

                _gv->isBoundaryFlag() = true;
                _gv->isPolygonComplexFlag() = false;
            }
            else if( _levelType == LEVEL_LOW ) {
                ;
            }
            else if( _levelType == LEVEL_DETAIL ) {

                updateLevelDetailPolygonComplex();
            }
            else{
                cerr << "sth is wrong here... at " << __LINE__ << " in " << __FILE__ << endl;
            }

            redrawAllScene();
            break;
        }
        case Qt::Key_B:
        {

            // create boundary
            _levelhighPtr->createPolygonComplex( num_vertices( _levelhighPtr->skeleton() ) );
            selectLevelHighBuildBoundary();

#ifdef SKIP
            _levelhighPtr->writePolygonComplex();
#endif // SKIP
            break;
        }
        case Qt::Key_V:
        {
            // read and initialize the data
            _pathway->init( _gv->inputPath(), _gv->tmpPath(),
                            _gv->fileFreq(), _gv->fileType(),
                            _gv->cloneThreshold() );

            _pathway->generate();
            _gv->veCoverage() = _pathway->nVertices() + _gv->veRatio() * _pathway->nEdges();

            // canvasArea: content width and height
            // labelArea: total area of text labels
            // double canvasArea = width() * height();
            double labelArea = 0.0;
            map< string, Subdomain * > &sub = _pathway->subsys();
            for( map< string, Subdomain * >::iterator it = sub.begin();
                 it != sub.end(); it++ ){
                labelArea += it->second->idealArea;
            }

#ifdef DEBUG
            cerr << "canvas = " << canvasArea << " labelArea = " << labelArea << endl;
            cerr << "nV = " << _pathway->nVertices() << " nE = " << _pathway->nEdges()
                 << " veCoverage = " << _gv->veCoverage() << endl;
#endif // DEBUG

            double ratio = (double)width()/(double)height();
            double x = sqrt( labelArea * _gv->veCoverage() / (double)_pathway->nVertices() / ratio );
            _content_width = ratio * x;
            _content_height = x;

#ifdef DEBUG
            cerr << "veCoverage = " << _gv->veCoverage()
                 << " _content_width = " << _content_width << " _content_height = " << _content_height << endl;
            if( 2.0 * _content_width < width() ){
                _content_width = width()/2.0;
                _content_height = height()/2.0;
            }
#endif // DEBUG

            _contour.elements().push_back( Coord2( - 0.5*_content_width, - 0.5*_content_height ) );
            _contour.elements().push_back( Coord2( + 0.5*_content_width, - 0.5*_content_height ) );
            _contour.elements().push_back( Coord2( + 0.5*_content_width, + 0.5*_content_height ) );
            _contour.elements().push_back( Coord2( - 0.5*_content_width, + 0.5*_content_height ) );
            _contour.boundingBox() = Coord2( _content_width, _content_height );
            _contour.boxCenter().x() = 0.0;
            _contour.boxCenter().y() = 0.0;
            _contour.area() = _content_width * _content_height;
            _levelhighPtr->init( &_content_width, &_content_height, &_gv->veCoverage(),
                                 _pathway->skeletonG(), &_contour );
#ifdef DEBUG
            //cerr << "width x height = " << width() * height() << endl;
            //cerr << "label sum = " << sum << endl;
            //cerr << "ratio = " << width() * height() / sum << endl;
            cerr << "new_content_width = " << _content_width
                 << " new_content_height = " << _content_height << endl;
#endif // DEBUG
            _gv->setSceneRect( -( _content_width + LEFTRIGHT_MARGIN )/2.0, -( _content_height + TOPBOTTOM_MARGIN )/2.0,
                               _content_width + LEFTRIGHT_MARGIN, _content_height + TOPBOTTOM_MARGIN );

            _levelhighPtr->normalizeSkeleton( _content_width, _content_height );
            _levelhighPtr->decomposeSkeleton();
            _levelhighPtr->normalizeBone( _content_width, _content_height );

            // ui
            _gv->isCompositeFlag() = true;

            simulateKey( Qt::Key_E );
            redrawAllScene();
            break;
        }
        case Qt::Key_C:
        {
            spaceCoverage();
            break;
        }
        case Qt::Key_E:
        {
            redrawAllScene();
            // _gv->exportPNG( -width()/2.0, -height()/2.0, width(), height() );
            _gv->exportPNG( -( _content_width + LEFTRIGHT_MARGIN )/2.0, -( _content_height + TOPBOTTOM_MARGIN )/2.0,
                               _content_width + LEFTRIGHT_MARGIN, _content_height + TOPBOTTOM_MARGIN );
            break;
        }
        case Qt::Key_Minus:
        {
            _gv->isSimplifiedFlag() = false;
            _gv->isSkeletonFlag() = false;
            _gv->isCompositeFlag() = false;
            _gv->isPolygonFlag() = false;
            _gv->isPolygonComplexFlag() = false;
            _gv->isBoundaryFlag() = false;
            _gv->isCellFlag() = false;
            _gv->isCellPolygonFlag() = false;
            _gv->isCellPolygonComplexFlag() = false;
            //_gv->isMCLPolygonFlag() = false;
            _gv->isRoadFlag() = true;
            _gv->isLaneFlag() = true;
            _gv->isSubPathwayFlag() = true;
            _gv->isPathwayPolygonFlag() = true;
            cerr << "_gv->isSimplifiedFlag() = " << _gv->isSimplifiedFlag() << endl;
            cerr << "_gv->isSkeletonFlag() = " << _gv->isSkeletonFlag() << endl;
            cerr << "_gv->isCompositeFlag() = " << _gv->isCompositeFlag() << endl;
            cerr << "_gv->isPolygonFlag() = " << _gv->isPolygonFlag() << endl;
            cerr << "_gv->isPolygonComplexFlag() = " << _gv->isPolygonComplexFlag() << endl;
            cerr << "_gv->isBoundaryFlag() = " << _gv->isBoundaryFlag() << endl;
            cerr << "_gv->isCellFlag() = " << _gv->isCellFlag() << endl;
            cerr << "_gv->isCellPolygonFlag() = " << _gv->isCellPolygonFlag() << endl;
            cerr << "_gv->isCellPolygonComplexFlag() = " << _gv->isCellPolygonComplexFlag() << endl;
            //cerr << "_gv->isMCLPolygonFlag() = " << _gv->isMCLPolygonFlag() << endl;
            cerr << "_gv->isRoadFlag() = " << _gv->isRoadFlag() << endl;
            cerr << "_gv->isLaneFlag() = " << _gv->isLaneFlag() << endl;
            cerr << "_gv->isSubPathwayFlag() = " << _gv->isSubPathwayFlag() << endl;
            cerr << "_gv->isPathwayPolygonFlag() = " << _gv->isPathwayPolygonFlag() << endl;
            redrawAllScene();
        }
            break;
        case Qt::Key_Plus:
        {
            _gv->isSimplifiedFlag() = false;
            _gv->isSkeletonFlag() = false;
            _gv->isCompositeFlag() = false;
            _gv->isPolygonFlag() = false;
            _gv->isPolygonComplexFlag() = false;
            _gv->isBoundaryFlag() = true;
            _gv->isCellFlag() = false;
            _gv->isCellPolygonFlag() = false;
            _gv->isCellPolygonComplexFlag() = false;
            //_gv->isMCLPolygonFlag() = false;
            _gv->isRoadFlag() = true;
            _gv->isLaneFlag() = true;
            _gv->isSubPathwayFlag() = true;
            _gv->isPathwayPolygonFlag() = true;
            cerr << "_gv->isSimplifiedFlag() = " << _gv->isSimplifiedFlag() << endl;
            cerr << "_gv->isSkeletonFlag() = " << _gv->isSkeletonFlag() << endl;
            cerr << "_gv->isCompositeFlag() = " << _gv->isCompositeFlag() << endl;
            cerr << "_gv->isPolygonFlag() = " << _gv->isPolygonFlag() << endl;
            cerr << "_gv->isPolygonComplexFlag() = " << _gv->isPolygonComplexFlag() << endl;
            cerr << "_gv->isBoundaryFlag() = " << _gv->isBoundaryFlag() << endl;
            cerr << "_gv->isCellFlag() = " << _gv->isCellFlag() << endl;
            cerr << "_gv->isCellPolygonFlag() = " << _gv->isCellPolygonFlag() << endl;
            cerr << "_gv->isCellPolygonComplexFlag() = " << _gv->isCellPolygonComplexFlag() << endl;
            //cerr << "_gv->isMCLPolygonFlag() = " << _gv->isMCLPolygonFlag() << endl;
            cerr << "_gv->isRoadFlag() = " << _gv->isRoadFlag() << endl;
            cerr << "_gv->isLaneFlag() = " << _gv->isLaneFlag() << endl;
            cerr << "_gv->isSubPathwayFlag() = " << _gv->isSubPathwayFlag() << endl;
            cerr << "_gv->isPathwayPolygonFlag() = " << _gv->isPathwayPolygonFlag() << endl;
            redrawAllScene();
        }
            break;
        case Qt::Key_Escape:
            close();
            break;
        default:
            QWidget::keyPressEvent( event );
    }
}
