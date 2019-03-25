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
    _gv->initSceneItems();
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


void Window::selectOctilinear( void )
{
    vector< Boundary > &boundaryVec = *_boundaryVecPtr;

    _octilinearVecPtr->clear();
    _octilinearVecPtr->resize( boundaryVec.size() );
    for( unsigned int i = 0; i < boundaryVec.size(); i++ ){

        cerr << "select octilinear ..." << " boundaryVec[i].nVertices() = "<< boundaryVec[i].nVertices() << endl;

        // initialization
        int iter = 200;
        //int iter = 2*_boundaryPtr->nVertices();
        // int iter = 2*sqrt( _boundaryPtr->nVertices() );
        //_octilinearVecPtr->prepare( _simplifiedBoundaryPtr, _content_width/2.0, _content_height/2.0 );
        (*_octilinearVecPtr)[i].prepare( &boundaryVec[i], _content_width/2.0, _content_height/2.0 );


        // run octilinear optimization
        OPTTYPE opttype = (*_octilinearVecPtr)[0].opttype();
#ifdef DEBUG
        cerr << "oct::_content_width = " << _content_width << " half = " << _content_width/2 << endl;
    cerr << "iter = " << iter << endl;
#endif // DEBUG
        switch( opttype ) {
            case LEAST_SQUARE:
            {
                (*_octilinearVecPtr)[i].LeastSquare( iter );
            }
                break;
            case CONJUGATE_GRADIENT:
            {
                (*_octilinearVecPtr)[i].ConjugateGradient( iter );
            }
                break;
            default:
            {
                cerr << "something is wrong here... at " << __LINE__ << " in " << __FILE__ << endl;
            }
                break;
        }
        (*_octilinearVecPtr)[i].retrieve();

        //cerr << "BoundaryGraph:" << endl;
        //printGraph( boundaryVec[i].boundary() );
    }

    redrawAllScene();
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
    _timerVideo.start( 1000, this );
    _timerVideoID = _timerVideo.timerId();
    cerr << "_timerVideo.timerId = " << _timerVideo.timerId() << endl;
}

void Window::_timerVideoStop( void )
{
    _timerVideo.stop();
}

void Window::timerVideo( void )
{
    simulateKey( Qt::Key_E );
}
#endif // RECORD_VIDEO

void Window::listenProcessBoundary( void )
{
    bool allFinished = true;

    // check if all threads are finished
    for( unsigned int i = 0; i < _controllers.size(); i++ ) {
        allFinished = allFinished && _controllers[ i ]->wt().isFinished();
        // cerr << "is _controllers[" << i << "] finished ? "<< _controllers[ i ]->wt().isFinished();
    }

    if( ( allFinished == true ) && ( _controllers.size() != 0 ) ){
        //cerr << "Pressing Key_2 size = " << _controllers.size() << endl;
        simulateKey( Qt::Key_2 );
    }
}

void Window::processBoundaryForce( void )
{
    // create a new thread
    _levelhighPtr->forceBone().init( &_levelhighPtr->bone(), &_contour, "../configs/boundary.conf" );

    Controller * conPtr = new Controller;
    conPtr->setPathwayData( _pathway, *_pathway->width(), *_pathway->height() );
    conPtr->setRegionData( _levelhighPtr, _boundaryVecPtr,
                           _cellPtr, _roadPtr, _lanePtr );

    vector < unsigned int > indexVec;
    conPtr->init( indexVec, WORKER_BOUNDARY );
    // set energy type
    conPtr->setEnergyType( _gv->energyType() );
    _controllers.push_back( conPtr );

    connect( conPtr, &Controller::update, this, &Window::redrawAllScene );
    connect( &conPtr->wt(), &QThread::finished, this, &Window::listenProcessBoundary );

    _gv->isPolygonFlag() = true;

    QString text = "processBoundaryForce";
    Q_EMIT conPtr->operate( text );
}

void Window::processBoundaryStress( void )
{
    _levelhighPtr->forceBone().prepare( &_levelhighPtr->bone(), &_contour );

    // create a new thread
    Controller * conPtr = new Controller;
    conPtr->setPathwayData( _pathway, *_pathway->width(), *_pathway->height() );
    conPtr->setRegionData( _levelhighPtr, _boundaryVecPtr,
                           _cellPtr, _roadPtr, _lanePtr );

    vector < unsigned int > indexVec;
    conPtr->init( indexVec, WORKER_BOUNDARY );
    // set energy type
    conPtr->setEnergyType( _gv->energyType() );
    _controllers.push_back( conPtr );

    connect( conPtr, &Controller::update, this, &Window::redrawAllScene );
    connect( &conPtr->wt(), &QThread::finished, this, &Window::listenProcessBoundary );

    _gv->isPolygonFlag() = true;

    QString text = "processBoundaryStress";
    Q_EMIT conPtr->operate( text );
}

void Window::stopProcessBoundary( void )
{
    // quit all threads
    for( unsigned int i = 0; i < _controllers.size(); i++ ) {
        _controllers[ i ]->wt().quit();
        delete _controllers[ i ];
    }
    _controllers.clear();

    simulateKey( Qt::Key_B );
}

void Window::listenProcessCell( void )
{
    bool allFinished = true;

    //cerr << "before _controllers size = " << _controllers.size() << endl;
    // check if all threads are finished
    for( unsigned int i = 0; i < _controllers.size(); i++ ) {
        allFinished = allFinished && _controllers[ i ]->wt().isFinished();
        // cerr << "is _controllers[" << i << "] finished ? "<< _controllers[ i ]->wt().isFinished();
    }
    //cerr << "after _controllers size = " << _controllers.size() << endl;


    if( ( allFinished == true ) && ( _controllers.size() != 0 ) ){
        //simulateKey( Qt::Key_W );
    }
}

void Window::processCellForce( void )
{
    _gv->isCellPolygonFlag() = true;
    _gv->isCellFlag() = true;

    //for( unsigned int i = 0; i < 2; i++ ){
    for( unsigned int i = 0; i < _cellPtr->cellVec().size(); i++ ){

        Controller * conPtr = new Controller;
        conPtr->setPathwayData( _pathway, *_pathway->width(), *_pathway->height() );
        conPtr->setRegionData( _levelhighPtr, _boundaryVecPtr,
                               _cellPtr, _roadPtr, _lanePtr );
        vector < unsigned int > indexVec;
        indexVec.push_back( i );
        conPtr->init( indexVec, WORKER_CELL );
        // set energy type
        conPtr->setEnergyType( _gv->energyType() );
        _controllers.push_back( conPtr );

        connect( conPtr, &Controller::update, this, &Window::redrawAllScene );
        connect( &conPtr->wt(), &QThread::finished, this, &Window::listenProcessCell );

        QString text = "processCellForce";
        Q_EMIT conPtr->operate( text );
    }
}

void Window::processCellStress( void )
{
    _gv->isCellPolygonFlag() = true;
    _gv->isCellFlag() = true;

    map< unsigned int, Polygon2 >  p = _levelhighPtr->polygonComplex();
    //for( unsigned int i = 0; i < 1; i++ ){
    for( unsigned int i = 0; i < _cellPtr->cellVec().size(); i++ ){

#ifdef DEBUG
        ForceGraph &g = _cellPtr->cellVec()[i].bone();
        unsigned int nEdges = num_edges( g );
        {
            ForceGraph::vertex_descriptor vdBS = vertex( 1, g );
            ForceGraph::vertex_descriptor vdBT = vertex( 2, g );

            pair< BoundaryGraph::edge_descriptor, unsigned int > addE = add_edge( vdBS, vdBT, g );
            BoundaryGraph::edge_descriptor addED = addE.first;

            g[ addED ].id = nEdges;
            g[ addED ].initID = nEdges;
            g[ addED ].weight = 1.0;
        }
#endif // DEBUG

        // for( unsigned int i = 0; i < _cellPtr->cellVec().size(); i++ ){

        map< unsigned int, Polygon2 >::iterator itP = p.begin();
        advance( itP, i );
        // cerr << "cell::contour = " << itP->second.elements().size() << endl;
        _cellPtr->cellVec()[i].forceBone().prepare( &_cellPtr->cellVec()[i].bone(), &itP->second );
        //cerr << "cell::contour = " << _cellPtr->cellVec()[i].forceBone().contour()->elements().size() << endl;

        Controller * conPtr = new Controller;
        conPtr->setPathwayData( _pathway, *_pathway->width(), *_pathway->height() );
        conPtr->setRegionData( _levelhighPtr, _boundaryVecPtr,
                               _cellPtr, _roadPtr, _lanePtr );
        vector < unsigned int > indexVec;
        indexVec.push_back( i );
        conPtr->init( indexVec, WORKER_CELL );
        // set energy type
        conPtr->setEnergyType( _gv->energyType() );
        _controllers.push_back( conPtr );

        connect( conPtr, &Controller::update, this, &Window::redrawAllScene );
        connect( &conPtr->wt(), &QThread::finished, this, &Window::listenProcessCell );

        QString text = "processCellForce";
        Q_EMIT conPtr->operate( text );
    }
}

void Window::stopProcessCell( void )
{
    // quit all threads
    for( unsigned int i = 0; i < _controllers.size(); i++ ) {
        _controllers[ i ]->wt().quit();
        delete _controllers[ i ];
    }
    _controllers.clear();

    _gv->isCellPolygonFlag() = false;
    _gv->isCellFlag() = false;
    _gv->isCellPolygonComplexFlag() = true;

    simulateKey( Qt::Key_P );
    //cerr << "Pressing Key_P" << endl;
}

void Window::listenProcessBone( void )
{
    bool allFinished = true;

    // check if all threads are finished
    for( unsigned int i = 0; i < _controllers.size(); i++ ) {
        allFinished = allFinished && _controllers[ i ]->wt().isFinished();
        // cerr << "is _controllers[" << i << "] finished ? "<< _controllers[ i ]->wt().isFinished();
    }

    if( ( allFinished == true ) && ( _controllers.size() != 0 ) ){
        simulateKey( Qt::Key_S );
        //cerr << "Pressing Key_S size = " << _controllers.size() << endl;
    }
}

void Window::processBoneForce( void )
{
    vector< multimap< int, CellComponent > > & cellComponentVec = _cellPtr->cellComponentVec();

    for( unsigned int i = 0; i < cellComponentVec.size(); i++ ){

        multimap< int, CellComponent > &cellComponentMap = cellComponentVec[ i ];

        for( unsigned int j = 0; j < cellComponentMap.size(); j++ ){

            multimap< int, CellComponent >::iterator itC = cellComponentMap.begin();
            advance( itC, j );
            CellComponent &c = itC->second;

            // create mcl force
            c.mcl.forceBone().init( &itC->second.mcl.bone(), &itC->second.contour, "../configs/mcl.conf" );
            c.mcl.forceBone().id() = 0;

            Controller * conPtr = new Controller;
            conPtr->setPathwayData( _pathway, *_pathway->width(), *_pathway->height() );
            conPtr->setRegionData( _levelhighPtr, _boundaryVecPtr,
                                   _cellPtr, _roadPtr, _lanePtr );
            vector < unsigned int > indexVec;
            indexVec.push_back( i );
            indexVec.push_back( j );
            conPtr->init( indexVec, WORKER_BONE );
            _controllers.push_back( conPtr );

            connect( conPtr, &Controller::update, this, &Window::redrawAllScene );
            connect( &conPtr->wt(), &QThread::finished, this, &Window::listenProcessBone );

            QString text = "processBoneForce";
            Q_EMIT conPtr->operate( text );
        }
    }
}

void Window::processBoneStress( void )
{
    vector< multimap< int, CellComponent > > & cellComponentVec = _cellPtr->cellComponentVec();

    for( unsigned int i = 0; i < cellComponentVec.size(); i++ ){

        multimap< int, CellComponent > &cellComponentMap = cellComponentVec[ i ];

        for( unsigned int j = 0; j < cellComponentMap.size(); j++ ){

            multimap< int, CellComponent >::iterator itC = cellComponentMap.begin();
            advance( itC, j );
            CellComponent &c = itC->second;

            if( num_vertices( itC->second.mcl.bone() ) > 0 ){

                // create mcl force
                c.mcl.forceBone().prepare( &itC->second.mcl.bone(), &itC->second.contour );
                c.mcl.forceBone().id() = 0;

                Controller * conPtr = new Controller;
                conPtr->setPathwayData( _pathway, *_pathway->width(), *_pathway->height() );
                conPtr->setRegionData( _levelhighPtr, _boundaryVecPtr,
                                       _cellPtr, _roadPtr, _lanePtr );
                vector < unsigned int > indexVec;
                indexVec.push_back( i );
                indexVec.push_back( j );
                conPtr->init( indexVec, WORKER_BONE );
                // set energy type
                conPtr->setEnergyType( _gv->energyType() );
                _controllers.push_back( conPtr );

                connect( conPtr, &Controller::update, this, &Window::redrawAllScene );
                connect( &conPtr->wt(), &QThread::finished, this, &Window::listenProcessBone );

                QString text = "processBoneStress";
                Q_EMIT conPtr->operate( text );
            }
        }
    }
}

void Window::stopProcessBone( void )
{
    // quit all threads
    for( unsigned int i = 0; i < _controllers.size(); i++ ) {
        _controllers[ i ]->wt().quit();
        delete _controllers[ i ];
    }
    _controllers.clear();

    _gv->isSubPathwayFlag() = true;
}

void Window::listenProcessDetailedPathway( void )
{
    bool allFinished = true;

    // check if all threads are finished
    for( unsigned int i = 0; i < _controllers.size(); i++ ) {
        allFinished = allFinished && _controllers[ i ]->wt().isFinished();
        // cerr << "is _controllers[" << i << "] finished ? "<< _controllers[ i ]->wt().isFinished();
    }

    if( ( allFinished == true ) && ( _controllers.size() != 0 ) ){
        simulateKey( Qt::Key_X );
        //cerr << "Pressing Key_X size = " << _controllers.size() << endl;
    }
}

void Window::processDetailedPathwayForce( void )
{
    vector< multimap< int, CellComponent > > &cellComponentVec = _cellPtr->cellComponentVec();
/*
    for( unsigned int i = 0; i < cellComponentVec.size(); i++ ){

        multimap< int, CellComponent > &cellComponentMap = cellComponentVec[ i ];

        for( unsigned int j = 0; j < cellComponentMap.size(); j++ ){

            multimap< int, CellComponent >::iterator itC = cellComponentMap.begin();
            advance( itC, j );
            CellComponent &c = itC->second;

            Boundary b;
            Smooth s;
            double err = 0.0;
            unsigned int &nVertices = b.nVertices();
            unsigned int &nEdges = b.nEdges();
            unsigned int &nLines = b.nLines();
            ForceGraph &cg = c.detail.bone();
            BoundaryGraph &bg = b.boundary();

            // copy vertices
            BGL_FORALL_VERTICES( vd, cg, ForceGraph )
            {
                BoundaryGraph::vertex_descriptor curVD = add_vertex( bg );

                bg[ curVD ].id          = cg[ vd ].id;
                bg[ curVD ].geoPtr      = new Coord2( cg[ vd ].coordPtr->x(), cg[ vd ].coordPtr->y() );
                bg[ curVD ].smoothPtr   = new Coord2( cg[ vd ].coordPtr->x(), cg[ vd ].coordPtr->y() );
                bg[ curVD ].coordPtr    = cg[ vd ].coordPtr;

#ifdef DEBUG
                if( i == 0 ){
                    cerr << "i = " << i << " vid = " << cg[ vd ].id << " cg[ vd ].coordPtr = " << *cg[ vd ].coordPtr;
                }
#endif // DEBUG

                bg[ curVD ].id = bg[ curVD ].initID = nVertices;
                bg[ curVD ].namePtr = new string( to_string( bg[ curVD ].id ) );
                bg[ curVD ].weight = 1.0;
                //bg[ curVD ].lineID.push_back( nLines );

                nVertices++;
            }

            // copy edges
            BGL_FORALL_EDGES( ed, cg, ForceGraph )
            {
                unsigned int idS = cg[ source( ed, cg ) ].id;
                unsigned int idT = cg[ target( ed, cg ) ].id;
                BoundaryGraph::vertex_descriptor vdS = vertex( idS, bg );
                BoundaryGraph::vertex_descriptor vdT = vertex( idT, bg );
                pair<BoundaryGraph::edge_descriptor, unsigned int> foreE = add_edge( vdS, vdT, bg );
                BoundaryGraph::edge_descriptor foreED = foreE.first;

                // calculate geographical angle
                Coord2 coordO;
                Coord2 coordD;
                if( bg[ vdS ].initID < bg[ vdT ].initID ){
                    coordO = *bg[ vdS ].coordPtr;
                    coordD = *bg[ vdT ].coordPtr;
                }
                else{
                    coordO = *bg[ vdT ].coordPtr;
                    coordD = *bg[ vdS ].coordPtr;
                }
                double diffX = coordD.x() - coordO.x();
                double diffY = coordD.y() - coordO.y();
                double angle = atan2( diffY, diffX );

                bg[ foreED ].initID = bg[ foreED ].id = cg[ ed ].id;
                bg[ foreED ].weight = 1.0;
                bg[ foreED ].geoAngle = angle;
                bg[ foreED ].smoothAngle = angle;
                bg[ foreED ].angle = angle;
                bg[ foreED ].lineID.push_back( nLines );
                bg[ foreED ].visitedTimes = 0;

                nEdges++;
            }

            // run smooth optimization
            s.prepare( &b, _content_width/2, _content_height/2 );
            err = s.ConjugateGradient( 3 * nVertices );
            s.retrieve();

            cerr << "simNStation = " << nVertices << endl;
            cerr << "nVertices = " << nVertices << endl;
            // ofs << "    Coarse CG: " << clock() - start_time << " err = " << err << " iter = " << 2 * _simplifiedBoundaryPtr->nStations() << endl;

            for( unsigned int k = 0; k < 2; k++ ) {
            // while( true ) {

                clock_t start = clock();
                int iter = 0;

                // check if all nodes are moved back
#ifdef  DEBUG
                cerr << " num_vertices( _simplifiedBoundaryPtr->boundary() ) = " << num_vertices( _simplifiedBoundaryPtr->boundary() ) << endl
             << " num_vertices( _boundaryPtr->boundary() ) = " << num_vertices( _boundaryPtr->boundary() ) << endl
             << " nLabels = " << nLabels << endl;
#endif  // DEBUG
                s.prepare( &b, _content_width/2, _content_height/2 );
                iter = MAX2( 2 * ( &b.nVertices() - &b.nVertices() ), 30 );
                err = s.ConjugateGradient( iter );
                s.retrieve();

                cerr << "i = " << i << "    time each loop = " << clock() - start << " err = " << err << " iter = " << iter << endl;
                // ofs << "    time each loop = " << clock() - start << " err = " << err << " iter = " << iter << endl;
            }
            //_simplifiedBoundaryPtr->adjustsize( width(), height() );
            s.clear();
            b.cloneSmooth( b );

            // cerr << "Total Time CG = " << clock() - start_time << endl;
            // ofs << "Total Time CG = " << clock() - start_time << endl;
        }
    }
*/
    vector< vector< unsigned int > > idMat;
    idMat.resize( cellComponentVec.size() );
    unsigned int idD = 0;
    for( unsigned int i = 0; i < idMat.size(); i++ ){

        multimap< int, CellComponent > &cellComponentMap = cellComponentVec[i];
        idMat[i].resize( cellComponentMap.size() );
        for( unsigned int j = 0; j < idMat[i].size(); j++ ){

            idMat[i][j] = idD;
            idD++;
        }
    }

    // cerr << "idD = " << idD << endl;
    // cerr << "From main thread: " << QThread::currentThreadId() << endl;
    unsigned int idC = 0;
    for( unsigned int i = 0; i < cellComponentVec.size(); i++ ){

        multimap< int, CellComponent > &cellComponentMap = cellComponentVec[i];

        for( unsigned int j = 0; j < cellComponentMap.size(); j++ ){

            multimap< int, CellComponent >::iterator itC = cellComponentMap.begin();
            advance( itC, j );
            CellComponent &c = itC->second;

            c.detail.forceBone().init( &c.detail.bone(), &c.contour, "../configs/pathway.conf" );
            c.detail.forceBone().id() = idC;

            // cerr << endl << endl << "enable a thread... i = " << i << " j = " << j << endl;
            Controller * conPtr = new Controller;
            conPtr->setPathwayData( _pathway, *_pathway->width(), *_pathway->height() );
            conPtr->setRegionData( _levelhighPtr, _boundaryVecPtr,
                                   _cellPtr, _roadPtr, _lanePtr );
            vector < unsigned int > indexVec;
            indexVec.push_back( i );
            indexVec.push_back( j );
            conPtr->init( indexVec, WORKER_PATHWAY );
            _controllers.push_back( conPtr );

            connect( conPtr, &Controller::update, this, &Window::redrawAllScene );
            connect( &conPtr->wt(), &QThread::finished, this, &Window::listenProcessDetailedPathway );

            idC++;
            QString text = "processDetailedPathwayForce";
            Q_EMIT conPtr->operate( text );
        }
    }
}

void Window::processDetailedPathwayStress( void )
{
    vector< multimap< int, CellComponent > > &cellComponentVec = _cellPtr->cellComponentVec();

    vector< vector< unsigned int > > idMat;
    idMat.resize( cellComponentVec.size() );
    unsigned int idD = 0;
    for( unsigned int i = 0; i < idMat.size(); i++ ){

        multimap< int, CellComponent > &cellComponentMap = cellComponentVec[i];
        idMat[i].resize( cellComponentMap.size() );
        for( unsigned int j = 0; j < idMat[i].size(); j++ ){

            idMat[i][j] = idD;
            idD++;
        }
    }

    // cerr << "idD = " << idD << endl;
    // cerr << "From main thread: " << QThread::currentThreadId() << endl;
    unsigned int idC = 0;
    for( unsigned int i = 0; i < cellComponentVec.size(); i++ ){

        multimap< int, CellComponent > &cellComponentMap = cellComponentVec[i];

        for( unsigned int j = 0; j < cellComponentMap.size(); j++ ){

            multimap< int, CellComponent >::iterator itC = cellComponentMap.begin();
            advance( itC, j );
            CellComponent &c = itC->second;

            c.detail.forceBone().prepare( &c.detail.bone(), &c.contour );
            c.detail.forceBone().id() = idC;

            // cerr << endl << endl << "enable a thread... i = " << i << " j = " << j << endl;
            Controller * conPtr = new Controller;
            conPtr->setPathwayData( _pathway, *_pathway->width(), *_pathway->height() );
            conPtr->setRegionData( _levelhighPtr, _boundaryVecPtr,
                                   _cellPtr, _roadPtr, _lanePtr );
            vector < unsigned int > indexVec;
            indexVec.push_back( i );
            indexVec.push_back( j );
            conPtr->init( indexVec, WORKER_PATHWAY );
            // set energy type
            conPtr->setEnergyType( _gv->energyType() );
            _controllers.push_back( conPtr );

            connect( conPtr, &Controller::update, this, &Window::redrawAllScene );
            connect( &conPtr->wt(), &QThread::finished, this, &Window::listenProcessDetailedPathway );

            idC++;
            QString text = "processDetailedPathwayStress";
            Q_EMIT conPtr->operate( text );
        }
    }
}


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

void Window::stopProcessDetailedPathway( void )
{
    // quit all threads
    for( unsigned int i = 0; i < _controllers.size(); i++ ) {
        _controllers[ i ]->wt().quit();
        delete _controllers[ i ];
    }
    _controllers.clear();
}

void Window::listenProcessOctilinearBoundary( void )
{
    bool allFinished = true;

    // check if all threads are finished
    for( unsigned int i = 0; i < _bControllers.size(); i++ ) {
        allFinished = allFinished && _bControllers[ i ]->wt().isFinished();
        // cerr << "is _bControllers[" << i << "] finished ? "<< _bControllers[ i ]->wt().isFinished();
    }

    if( ( allFinished == true ) && ( _bControllers.size() != 0 ) ){
        //cerr << "Pressing Key_O size = " << _bControllers.size() << endl;
        simulateKey( Qt::Key_O );
    }
}

void Window::processOctilinearBoundary( void )
{
    vector< Boundary > &boundaryVec = *_boundaryVecPtr;

    _octilinearVecPtr->clear();
    _octilinearVecPtr->resize( boundaryVec.size() );
    for( unsigned int i = 0; i < boundaryVec.size(); i++ ) {

        cerr << "select octilinear ..."
             << " boundaryVec[i].nVertices() = " << boundaryVec[i].nVertices() << endl;

        // initialization
        int iter = 200;
        //int iter = 2*_boundaryPtr->nVertices();
        // int iter = 2*sqrt( _boundaryPtr->nVertices() );
        (*_octilinearVecPtr)[i].prepare( &boundaryVec[i],
                                         _content_width/2.0, _content_height/2.0 );


        // create a new thread
        ControllerBoundary * conPtr = new ControllerBoundary;
        conPtr->setRegionData( _levelhighPtr, _boundaryVecPtr,
                               _cellPtr, _roadPtr, _lanePtr );

        conPtr->init(& (*_octilinearVecPtr)[i], iter, (*_octilinearVecPtr)[i].opttype() );
        _bControllers.push_back( conPtr );

        connect( conPtr, &ControllerBoundary::update, this, &Window::redrawAllScene );
        connect( &conPtr->wt(), &QThread::finished, this, &Window::listenProcessOctilinearBoundary );

        QString text = "processOctilinearBoundary";
        Q_EMIT conPtr->operate( text );
    }

#ifdef SKIP
    //usleep( 6000 );
    bool allFinished = true;
    while( true ){
        // check if all threads are finished
        for( unsigned int i = 0; i < _controllers.size(); i++ ) {
            allFinished = allFinished && _bControllers[ i ]->wt().isFinished();
            // cerr << "is _controllers[" << i << "] finished ? "<< _controllers[ i ]->wt().isFinished();
        }
        if( allFinished == true ) break;
    }
    for( unsigned int i = 0; i < _bControllers.size(); i++ ){
        _bControllers[i]->wait();
    }
#endif // SKIP
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
            //Coord2 &coordT = polygon.elements()[j%polygon.elements().size()];

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

    // cerr << "cellComponentVec.size() = " <<  _cellPtr->cellComponentVec().size() << endl;
    for( unsigned int i = 0; i < bVec.size(); i++ ) {

        cerr << "MiddleBuildBoundary::nV = " << num_vertices( bVec[i].boundary() ) << endl;

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

            Polygon2 &polygon = itC->second.contour;
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
    vector<multimap<int, CellComponent> > &cellCVec = _cellPtr->cellComponentVec();

    // initialization
    _levelhighPtr->polygonComplexVD().clear();
    _boundaryVecPtr->clear();
    //_boundaryVecPtr->resize(cellCVec.size());
    //vector<Boundary> &bVec = *_boundaryVecPtr;

    for (unsigned int m = 0; m < cellCVec.size(); m++) {

        multimap< int, CellComponent > &componentMap = cellCVec[m];
        multimap< int, CellComponent >::iterator itC = componentMap.begin();

        Boundary b;
        _boundaryVecPtr->push_back( b );
        vector<Boundary> &bVec = *_boundaryVecPtr;
        cerr << "DetailBuildBoundary::nV = " << num_vertices(bVec[ bVec.size()-1 ].boundary()) << endl;

        for( ; itC != componentMap.end(); itC++ ){


            BoundaryGraph &bg = bVec[m].boundary();
            unsigned int &nVertices = bVec[m].nVertices();
            unsigned int &nEdges = bVec[m].nEdges();
            unsigned int &nLines = bVec[m].nLines();

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
                map< unsigned int, vector< BoundaryGraph::vertex_descriptor > >::iterator itP;
                itC->second.detail.polygonComplexVD().insert( pair< unsigned int, vector< BoundaryGraph::vertex_descriptor > >
                        ( i, vdVec ) );
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
            //cerr << endl;
        }
    }
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
                itC->second.contour.elements()[j].x() = bg[ polygonComplexVD[j] ].coordPtr->x();
                itC->second.contour.elements()[j].y() = bg[ polygonComplexVD[j] ].coordPtr->y();

                // cerr << "j = " << j << " " << itC->second.contour.elements()[j];
            }
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

    vector< Boundary > &bVec = *_boundaryVecPtr;
    for( unsigned int m = 0; m < bVec.size(); m++ ) {

        BoundaryGraph &bg = bVec[m].boundary();

        multimap< int, CellComponent > &cellComponent = _cellPtr->cellComponentVec()[m];

        for( multimap< int, CellComponent >::iterator itC = _cellPtr->cellComponentVec()[m].begin();
             itC != _cellPtr->cellComponentVec()[m].end(); itC++ ){

            Bone &detail = itC->second.detail;
            Force &force = detail.forceBone();
            vector< Seed > &seedVec = *force.voronoi().seedVec();
            for( unsigned int i = 0; i < seedVec.size(); i++ ) {

                map< unsigned int, vector< BoundaryGraph::vertex_descriptor > >::iterator itP = detail.polygonComplexVD().begin();
                advance( itP, i );

                Polygon2 &polygon = seedVec[i].cellPolygon;
                for( unsigned int j = 0; j < polygon.elements().size(); j++ ){
                    polygon.elements()[j].x() = bg[ itP->second[j] ].coordPtr->x();
                    polygon.elements()[j].y() = bg[ itP->second[j] ].coordPtr->y();
                    // cerr << "i = " << i << " j = " << j << " " << polygon.elements()[j];
                }
            }
        }
    }

    // update cell contour
    _cellPtr->createPolygonComplexFromDetailGraph();

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
            if( _gv->energyType() == ENERGY_FORCE )
                processBoundaryForce();
            else if( _gv->energyType() == ENERGY_STRESS )
                processBoundaryStress();
            break;
        }
        case Qt::Key_2:
        {
            stopProcessBoundary();

            _levelType = LEVEL_HIGH;
            processOctilinearBoundary();

            //selectOctilinear();
            //simulateKey( Qt::Key_O );
            break;
        }
        case Qt::Key_Q:
        {
            simulateKey( Qt::Key_L );

            checkInETime();
            cerr << "*********** Starting Execution Time = " << checkOutETime() << endl;
            checkInCPUTime();
            cerr << "*********** Starting CPU Time = " << checkOutCPUTime() << endl;
            if( _gv->energyType() == ENERGY_FORCE )
                processCellForce();
            else if( _gv->energyType() == ENERGY_STRESS )
                processCellStress();

            break;
        }
        case Qt::Key_W:
        {
            cerr << "stopProcessCell..." << endl;
            stopProcessCell();
            cerr << "[Force-Directed] Finished Execution Time = " << checkOutETime() << endl;
            cerr << "[Force-Directed] Finished CPU Time = " << checkOutCPUTime() << endl;

            // initialization and build the boundary
            selectLevelMiddleBuildBoundary();

            _levelType = LEVEL_MIDDLE;
            processOctilinearBoundary();

            // optimization
            // selectOctilinear();
            // simulateKey( Qt::Key_O );

            break;
        }
        case Qt::Key_A:
        {
            checkInETime();
            cerr << "*********** Starting Execution Time = " << checkOutETime() << endl;
            checkInCPUTime();
            cerr << "*********** Starting CPU Time = " << checkOutCPUTime() << endl;
            if( _gv->energyType() == ENERGY_FORCE )
                processBoneForce();
            else if( _gv->energyType() == ENERGY_STRESS )
                processBoneStress();
            _gv->isCellPolygonFlag() = false;
            _gv->isCellPolygonComplexFlag() = true;
            _gv->isMCLPolygonFlag() = true;
            _gv->isPathwayPolygonFlag() = false;
            redrawAllScene();
            break;
        }
        case Qt::Key_S:
        {
            cerr << "stopProcessBone..." << endl;
            stopProcessBone();
            _cellPtr->updatePathwayCoords();
            redrawAllScene();
            break;
        }
        case Qt::Key_Z:
        {
            checkInETime();
            cerr << "*********** Starting Execution Time = " << checkOutETime() << endl;
            checkInCPUTime();
            cerr << "*********** Starting CPU Time = " << checkOutCPUTime() << endl;

            if( _gv->energyType() == ENERGY_FORCE )
                processDetailedPathwayForce();
            else if( _gv->energyType() == ENERGY_STRESS )
                processDetailedPathwayStress();
            _gv->isCellPolygonComplexFlag() = false;
            _gv->isPathwayPolygonFlag() = true;
            _gv->isMCLPolygonFlag() = false;
            redrawAllScene();
            break;
        }
        case Qt::Key_X:
        {
            cerr << "stopProcessDetailedPathway..." << endl;
            stopProcessDetailedPathway();
            cerr << "[Force-Directed] Finished Execution Time = " << checkOutETime() << endl;
            cerr << "[Force-Directed] Finished CPU Time = " << checkOutCPUTime() << endl;

            // initialization and build the boundary
            selectLevelDetailBuildBoundary();

            _levelType = LEVEL_DETAIL;
            processOctilinearBoundary();

            // optimization
            // selectOctilinear();
            // simulateKey( Qt::Key_O );

            break;
        }
        case Qt::Key_3:
        {
            _gv->isSimplifiedFlag() = !_gv->isSimplifiedFlag();
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
            // load setting
            _pathway->initLayout( _levelhighPtr->polygonComplex() );

            // initialize cell
            _cellPtr->clear();
            _cellPtr->init( &_gv->veCoverage(), &_levelhighPtr->polygonComplex() );

            _gv->isCellFlag() = true;
            _gv->isCompositeFlag() = false;
            _gv->isPolygonFlag() = false;
            _gv->isCellPolygonComplexFlag() = true;
            _gv->isBoundaryFlag() = false;
            redrawAllScene();
            break;
        }
        case Qt::Key_P:
        {
            _cellPtr->createPolygonComplex();
            _cellPtr->updateMCLCoords();
            // _cellPtr->updatePathwayCoords();
            // _gv->isCellPolygonComplexFlag() = true;
            // _gv->isSubPathwayFlag() = true;
            redrawAllScene();
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
            else if( _levelType == LEVEL_BONE ) {
                ;
            }
            else if( _levelType == LEVEL_DETAIL ) {

                updateLevelDetailPolygonComplex();

                // steiner tree
                steinertree();
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
            _pathway->exportEdges();
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

#ifdef SKIP
            if( 2.0 * _content_width < width() ){
                _content_width = width()/2.0;
                _content_height = height()/2.0;
            }
#endif // SKIP

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

            redrawAllScene();
            break;
        }
        case Qt::Key_E:
        {
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
            _gv->isMCLPolygonFlag() = false;
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
            cerr << "_gv->isMCLPolygonFlag() = " << _gv->isMCLPolygonFlag() << endl;
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
            _gv->isMCLPolygonFlag() = false;
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
            cerr << "_gv->isMCLPolygonFlag() = " << _gv->isMCLPolygonFlag() << endl;
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
