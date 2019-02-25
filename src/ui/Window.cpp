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
    _boundaryPtr = obj._boundaryPtr;
    _simplifiedBoundaryPtr = obj._simplifiedBoundaryPtr;

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
    _boundaryPtr = new Boundary;
    _simplifiedBoundaryPtr = new Boundary;
    _cellPtr = new Cell;
    _roadPtr = new Road;
    _lanePtr = new vector< Road >;

    _cellPtr->setPathwayData( _pathway, *_pathway->width(), *_pathway->height() );
    _roadPtr->setPathwayData( _pathway, *_pathway->width(), *_pathway->height() );

    _gv->setPathwayData( _pathway, *_pathway->width(), *_pathway->height() );
    _gv->setRegionData( _levelhighPtr,
                        _boundaryPtr, _simplifiedBoundaryPtr,
                        _cellPtr, _roadPtr, _lanePtr );
}

#ifdef SKIP
void Window::createActions( void )
{
    // simplification
    selCloneGraphAct = new QAction( tr("C&loneGraph"), this );
    connect( selCloneGraphAct, SIGNAL( triggered() ), this, SLOT( selectCloneGraph() ) );
    selMinDistanceAct = new QAction( tr("M&inDistance"), this );
    connect( selMinDistanceAct, SIGNAL( triggered() ), this, SLOT( selectMinDistance() ) );

    selMovebackSmoothAct = new QAction( tr("M&ovebackSmooth"), this );
    connect( selMovebackSmoothAct, SIGNAL( triggered() ), this, SLOT( selectMovebackSmooth() ) );

    selMovebackOctilinearAct = new QAction( tr("M&ovebackOctilinear"), this );
    connect( selMovebackOctilinearAct, SIGNAL( triggered() ), this, SLOT( selectMovebackOctilinear() ) );

    selMovebackStressAct = new QAction( tr("M&ovebackStress"), this );
    connect( selMovebackStressAct, SIGNAL( triggered() ), this, SLOT( selectMovebackStress() ) );

    // optimization
    // least square
    selSmoothLSAct = new QAction( tr("S&mooth Original (LS)"), this );
    connect( selSmoothLSAct, SIGNAL( triggered() ), this, SLOT( selectSmoothLS() ) );

    selOctilinearLSAct = new QAction( tr("O&ctilinear Original (LS)"), this );
    connect( selOctilinearLSAct, SIGNAL( triggered() ), this, SLOT( selectOctilinearLS() ) );

    selStressLSAct = new QAction( tr("S&tress Original (LS)"), this );
    connect( selStressLSAct, SIGNAL( triggered() ), this, SLOT( selectStressLS() ) );

    // conjugate gradient
    selSmoothSmallCGAct = new QAction( tr("S&mooth Small (CG)"), this );
    connect( selSmoothSmallCGAct, SIGNAL( triggered() ), this, SLOT( selectSmoothSmallCG() ) );
    selSmoothCGAct = new QAction( tr("S&mooth Original (CG)"), this );
    connect( selSmoothCGAct, SIGNAL( triggered() ), this, SLOT( selectSmoothCG() ) );

    selOctilinearSmallCGAct = new QAction( tr("O&ctilinear Small (CG)"), this );
    connect( selOctilinearSmallCGAct, SIGNAL( triggered() ), this, SLOT( selectOctilinearSmallCG() ) );
    selOctilinearCGAct = new QAction( tr("O&ctilinear Original (CG)"), this );
    connect( selOctilinearCGAct, SIGNAL( triggered() ), this, SLOT( selectOctilinearCG() ) );

    selStressSmallCGAct = new QAction( tr("S&tress Small (CG)"), this );
    connect( selStressSmallCGAct, SIGNAL( triggered() ), this, SLOT( selectStressSmallCG() ) );
    selStressCGAct = new QAction( tr("S&tress Original (CG)"), this );
    connect( selStressCGAct, SIGNAL( triggered() ), this, SLOT( selectStressCG() ) );
}

void Window::createMenus( void )
{
    // load
    // loadMenu = menuBar()->addMenu( tr("&Load") );
    // loadMenu->addAction( selDataAct );

    // simplification
    simMenu = menuBar()->addMenu( tr("&Simplification") );
    simMenu->addAction( selCloneGraphAct );
    simMenu->addAction( selMinDistanceAct );
    simMenu->addAction( selMovebackSmoothAct );
    simMenu->addAction( selMovebackOctilinearAct );
    simMenu->addAction( selMovebackStressAct );

    // optimization
    optMenu = menuBar()->addMenu( tr("&Optimization") );
    optMenu->addAction( selSmoothLSAct );
    optMenu->addAction( selSmoothSmallCGAct );
    optMenu->addAction( selSmoothCGAct );

    optMenu->addAction( selOctilinearLSAct );
    optMenu->addAction( selOctilinearSmallCGAct );
    optMenu->addAction( selOctilinearCGAct );

    optMenu->addAction( selStressLSAct );
    optMenu->addAction( selStressSmallCGAct );
    optMenu->addAction( selStressCGAct );
}
#endif // SKIP

void Window::simulateKey( Qt::Key key )
{
    QKeyEvent eventP( QEvent::KeyPress, key, Qt::NoModifier );
    QApplication::sendEvent( this, &eventP );
    QKeyEvent eventR( QEvent::KeyRelease, key, Qt::NoModifier );
    QApplication::sendEvent( this, &eventR );
}

void Window::postLoad( void )
{
    _boundaryPtr->adjustsize( _content_width/2, _content_height/2 );
    _boundaryPtr->clearConflicts();

    redrawAllScene();
}

void Window::redrawAllScene( void )
{
    _gv->initSceneItems();
    _gv->update();
    _gv->scene()->update();
    update();
    repaint();

    QCoreApplication::processEvents();
}

#ifdef SKIP
void Window::selectForce( void )
{
    simulateKey( Qt::Key_F );
    redrawAllScene();
}
#endif // SKIP

void Window::selectCloneGraph( void )
{
    // initialization
    _simplifiedBoundaryPtr->cloneLayout( *_boundaryPtr );
    _simplifiedBoundaryPtr->clearConflicts();

    redrawAllScene();
}

void Window::selectLevelHighMinDistance( void )
{
    _simplifiedBoundaryPtr->simplifyLayout();
    _gv->isSimplifiedFlag() = true;
    redrawAllScene();
}

void Window::selectLevelHighMovebackSmooth( void )
{
    bool isFinished = true;
    isFinished = _simplifiedBoundaryPtr->movebackNodes( *_boundaryPtr, TYPE_SMOOTH );
    // if( isFinished == true ) cerr << "All stations are moved back!!" << endl;
    redrawAllScene();
}

void Window::selectLevelHighMovebackOctilinear( void )
{
    bool isFinished = true;
    isFinished = _simplifiedBoundaryPtr->movebackNodes( *_boundaryPtr, TYPE_OCTILINEAR );
    // if( isFinished == true ) cerr << "All stations are moved back!!" << endl;
    redrawAllScene();
}

void Window::selectLevelHighMovebackStress( void )
{
    bool isFinished = true;
    isFinished = _simplifiedBoundaryPtr->movebackNodes( *_boundaryPtr, TYPE_STRESS );
    // if( isFinished == true ) cerr << "All stations are moved back!!" << endl;
    redrawAllScene();
}

void Window::selectLevelHighSmoothSmall( void )
{
    // run coarse smooth optimization
    clock_t start_time = clock();
    double err = 0.0;

    _levelhighPtr->smooth().prepare( _simplifiedBoundaryPtr, _content_width/2, _content_height/2 );
    err = _levelhighPtr->smooth().ConjugateGradient( 3 * _simplifiedBoundaryPtr->nVertices() );
    _levelhighPtr->smooth().retrieve();

    cerr << "simNStation = " << _simplifiedBoundaryPtr->nVertices() << endl;
    cerr << "nVertices = " << _boundaryPtr->nVertices() << endl;
    // ofs << "    Coarse CG: " << clock() - start_time << " err = " << err << " iter = " << 2 * _simplifiedBoundaryPtr->nStations() << endl;

    // run smooth optimization
    while( true ) {

        clock_t start = clock();
        int iter = 0;

        // check if all nodes are moved back
#ifdef  DEBUG
        cerr << " num_vertices( _simplifiedBoundaryPtr->boundary() ) = " << num_vertices( _simplifiedBoundaryPtr->boundary() ) << endl
             << " num_vertices( _boundaryPtr->boundary() ) = " << num_vertices( _boundaryPtr->boundary() ) << endl
             << " nLabels = " << nLabels << endl;
#endif  // DEBUG
        if( num_vertices( _simplifiedBoundaryPtr->boundary() ) == ( num_vertices( _boundaryPtr->boundary() ) ) ) {
            break;
        }
        else {
            for( int i = 0; i < REMOVEBACKNUM; i++ ){
                selectLevelHighMovebackSmooth();
            }
        }

        _levelhighPtr->smooth().prepare( _simplifiedBoundaryPtr, _content_width/2, _content_height/2 );
        if( num_vertices( _simplifiedBoundaryPtr->boundary() ) == ( num_vertices( _boundaryPtr->boundary() ) ) ) {
            iter = MAX2( 2 * ( _boundaryPtr->nVertices() - _simplifiedBoundaryPtr->nVertices() ), 30 );
        }
        else{
            iter = MAX2( 2 * ( _boundaryPtr->nVertices() - _simplifiedBoundaryPtr->nVertices() ), 30 );
        }
        err = _levelhighPtr->smooth().ConjugateGradient( iter );
        _levelhighPtr->smooth().retrieve();

        cerr << "    time each loop = " << clock() - start << " err = " << err << " iter = " << iter << endl;
        // ofs << "    time each loop = " << clock() - start << " err = " << err << " iter = " << iter << endl;
    }
    //_simplifiedBoundaryPtr->adjustsize( width(), height() );
    _levelhighPtr->smooth().clear();
    _boundaryPtr->cloneSmooth( *_simplifiedBoundaryPtr );

    // cerr << "Total Time CG = " << clock() - start_time << endl;
    // ofs << "Total Time CG = " << clock() - start_time << endl;

    redrawAllScene();
}

void Window::selectLevelHighSmooth( void )
{
    OPTTYPE opttype = _levelhighPtr->smooth().opttype();

    // run smooth optimization
    _levelhighPtr->smooth().prepare( _boundaryPtr, _content_width/2, _content_height/2 );

    switch( opttype ){
        case LEAST_SQUARE:
        {
            int iter = _boundaryPtr->nVertices();
            _levelhighPtr->smooth().LeastSquare( iter );
        }
            break;
        case CONJUGATE_GRADIENT:
        {
            int iter = _boundaryPtr->nVertices();
            _levelhighPtr->smooth().ConjugateGradient( iter );
        }
            break;
        default:
            break;
    }
    _levelhighPtr->smooth().retrieve();
    _levelhighPtr->smooth().clear();

    redrawAllScene();
}

void Window::selectLevelHighOctilinearSmall( void )
{
    // run coarse octilinear optimization
    double err = 0.0;

    _levelhighPtr->octilinear().prepare( _simplifiedBoundaryPtr, _content_width/2, _content_height/2 );
    err = _levelhighPtr->octilinear().ConjugateGradient( 5 * _simplifiedBoundaryPtr->nVertices() );
    _levelhighPtr->octilinear().retrieve();
    //ofs << "    Coarse CG: " << clock() - start_time << " err = " << err << " iter = " << 5 * _simplifiedBoundaryPtr->nVertices() << endl;

    // run octilinear optimization
    while( true ) {

        clock_t start = clock();
        int iter = 0;

        // check if all nodes are moved back
        if( num_vertices( _simplifiedBoundaryPtr->boundary() ) == ( num_vertices( _boundaryPtr->boundary() ) ) ) {
            break;
        }
        else {
            for( int i = 0; i < REMOVEBACKNUM; i++ ){
                selectLevelHighMovebackOctilinear();
            }
        }

        _levelhighPtr->octilinear().prepare( _simplifiedBoundaryPtr, _content_width/2, _content_height/2 );
        if( num_vertices( _simplifiedBoundaryPtr->boundary() ) == ( num_vertices( _boundaryPtr->boundary() ) ) ) {
            iter = MAX2( 2 * ( _boundaryPtr->nVertices() - _simplifiedBoundaryPtr->nVertices() ), 30 );
        }
        else{
            iter = MAX2( 2 * ( _boundaryPtr->nVertices() - _simplifiedBoundaryPtr->nVertices() ), 30 );
        }
        err = _levelhighPtr->octilinear().ConjugateGradient( iter );
        _levelhighPtr->octilinear().retrieve();

        // ofs << "    time each loop = " << clock() - start << " err = " << err << " iter = " << iter << endl;
    }

    _levelhighPtr->octilinear().clear();
    _boundaryPtr->cloneOctilinear( *_simplifiedBoundaryPtr );

    redrawAllScene();
}

void Window::selectLevelHighOctilinear( void )
{
    // initialization
    OPTTYPE opttype = _levelhighPtr->octilinear().opttype();
    int iter = _boundaryPtr->nVertices();

    // run octilinear optimization
    _levelhighPtr->octilinear().prepare( _boundaryPtr, _content_width/2, _content_height/2 );

    switch( opttype ) {
        case LEAST_SQUARE:
        {
            _levelhighPtr->octilinear().LeastSquare( iter );
        }
            break;
        case CONJUGATE_GRADIENT:
        {
            _levelhighPtr->octilinear().ConjugateGradient( iter );
        }
            break;
        default:
            break;
    }
    _levelhighPtr->octilinear().retrieve();
    _levelhighPtr->octilinear().clear();

    redrawAllScene();
}

void Window::selectLevelHighStressSmall( void )
{
    // run coarse smooth optimization
    clock_t start_time = clock();
    double err = 0.0;

    _levelhighPtr->smooth().prepare( _simplifiedBoundaryPtr, _content_width/2, _content_height/2 );
    err = _levelhighPtr->smooth().ConjugateGradient( 3 * _simplifiedBoundaryPtr->nVertices() );
    _levelhighPtr->smooth().retrieve();

    cerr << "simNStation = " << _simplifiedBoundaryPtr->nVertices() << endl;
    cerr << "nStation = " << _boundaryPtr->nVertices() << endl;
    // ofs << "    Coarse CG: " << clock() - start_time << " err = " << err << " iter = " << 2 * _simplifiedBoundaryPtr->nStations() << endl;

    // run smooth optimization
    while( true ) {

        clock_t start = clock();
        int iter = 0;

        // check if all nodes are moved back
#ifdef  DEBUG
        cerr << " num_vertices( _simplifiedBoundaryPtr->boundary() ) = " << num_vertices( _simplifiedBoundaryPtr->boundary() ) << endl
             << " num_vertices( _boundaryPtr->boundary() ) = " << num_vertices( _boundaryPtr->boundary() ) << endl
             << " nLabels = " << nLabels << endl;
#endif  // DEBUG
        if( num_vertices( _simplifiedBoundaryPtr->boundary() ) == ( num_vertices( _boundaryPtr->boundary() ) ) ) {
            break;
        }
        else {
            for( int i = 0; i < REMOVEBACKNUM; i++ ){
                selectLevelHighMovebackSmooth();
            }
        }

        _levelhighPtr->smooth().prepare( _simplifiedBoundaryPtr, _content_width/2, _content_height/2 );
        if( num_vertices( _simplifiedBoundaryPtr->boundary() ) == ( num_vertices( _boundaryPtr->boundary() ) ) ) {
            iter = MAX2( 2 * ( _boundaryPtr->nVertices() - _simplifiedBoundaryPtr->nVertices() ), 30 );
        }
        else{
            iter = MAX2( 2 * ( _boundaryPtr->nVertices() - _simplifiedBoundaryPtr->nVertices() ), 30 );
        }
        err = _levelhighPtr->smooth().ConjugateGradient( iter );
        _levelhighPtr->smooth().retrieve();

        cerr << "    time each loop = " << clock() - start << " err = " << err << " iter = " << iter << endl;
        // ofs << "    time each loop = " << clock() - start << " err = " << err << " iter = " << iter << endl;
    }
    //_simplifiedBoundaryPtr->adjustsize( width(), height() );
    _levelhighPtr->smooth().clear();
    _boundaryPtr->cloneSmooth( *_simplifiedBoundaryPtr );

    // cerr << "Total Time CG = " << clock() - start_time << endl;
    // ofs << "Total Time CG = " << clock() - start_time << endl;

    redrawAllScene();
}

void Window::selectLevelHighStress( void )
{
    OPTTYPE opttype = _levelhighPtr->stressBone().opttype();

    // run smooth optimization
    _levelhighPtr->stressBone().prepare( &_levelhighPtr->skeleton(), _content_width/2, _content_height/2 );

    switch( opttype ){
        case LEAST_SQUARE:
        {
            int iter = _boundaryPtr->nVertices();
            _levelhighPtr->stressBone().LeastSquare( iter );
            break;
        }
        case CONJUGATE_GRADIENT:
        {
            unsigned int long iter = _levelhighPtr->stressBone().nVertices();
            //_stressPtr->ConjugateGradient( iter );
            _levelhighPtr->stressBone().ConjugateGradient( 1 );
            break;
        }
        default:
            break;
    }
    _levelhighPtr->stressBone().retrieve();
    _levelhighPtr->stressBone().clear();

    redrawAllScene();
}


void Window::selectLevelHighBuildBoundary( void )
{
    //clearGraph( _simplifiedBoundaryPtr->boundary() );

    buildLevelHighBoundaryGraph();
    _gv->isSimplifiedFlag() = false;
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
    for( unsigned int i = 0; i < controllers.size(); i++ ) {
        allFinished = allFinished && controllers[ i ]->wt().isFinished();
        // cerr << "is controllers[" << i << "] finished ? "<< controllers[ i ]->wt().isFinished();
    }

    if( ( allFinished == true ) && ( controllers.size() != 0 ) ){
        //cerr << "Pressing Key_2 size = " << controllers.size() << endl;
        simulateKey( Qt::Key_2 );
    }
}

void Window::processBoundary( void )
{
    Polygon2 contour;
    contour.elements().push_back( Coord2( - 0.5*_content_width, - 0.5*_content_height ) );
    contour.elements().push_back( Coord2( + 0.5*_content_width, - 0.5*_content_height ) );
    contour.elements().push_back( Coord2( + 0.5*_content_width, + 0.5*_content_height ) );
    contour.elements().push_back( Coord2( - 0.5*_content_width, + 0.5*_content_height ) );

    _levelhighPtr->forceBone().init( &_levelhighPtr->bone(), contour, "../configs/boundary.conf" );

    _gv->isPolygonFlag() = true;

    Controller * conPtr = new Controller;
    conPtr->setPathwayData( _pathway, *_pathway->width(), *_pathway->height() );
    conPtr->setRegionData( _levelhighPtr, _boundaryPtr, _simplifiedBoundaryPtr,
                           _cellPtr, _roadPtr, _lanePtr );

    vector < unsigned int > indexVec;
    conPtr->init( indexVec, WORKER_BOUNDARY );
    controllers.push_back( conPtr );

    connect( conPtr, &Controller::update, this, &Window::redrawAllScene );
    connect( &conPtr->wt(), &QThread::finished, this, &Window::listenProcessBoundary );

    QString test;
    Q_EMIT conPtr->operate( test );
}

void Window::stopProcessBoundary( void )
{
    // quit all threads
    for( unsigned int i = 0; i < controllers.size(); i++ ) {
        controllers[ i ]->wt().quit();
        delete controllers[ i ];
    }
    controllers.clear();

    simulateKey( Qt::Key_B );

    _gv->isPolygonFlag() = true;
    _gv->isPolygonComplexFlag() = false;
    _gv->isBoundaryFlag() = false;
    _gv->isCompositeFlag() = true;
}

void Window::listenProcessCell( void )
{
    bool allFinished = true;

    //cerr << "before controllers size = " << controllers.size() << endl;
    // check if all threads are finished
    for( unsigned int i = 0; i < controllers.size(); i++ ) {
        allFinished = allFinished && controllers[ i ]->wt().isFinished();
        // cerr << "is controllers[" << i << "] finished ? "<< controllers[ i ]->wt().isFinished();
    }
    //cerr << "after controllers size = " << controllers.size() << endl;


    if( ( allFinished == true ) && ( controllers.size() != 0 ) ){
        simulateKey( Qt::Key_W );
    }
}

void Window::processCell( void )
{
    _gv->isCellPolygonFlag() = true;
    _gv->isCellFlag() = true;

    for( unsigned int i = 0; i < _cellPtr->cellVec().size(); i++ ){

        Controller * conPtr = new Controller;
        conPtr->setPathwayData( _pathway, *_pathway->width(), *_pathway->height() );
        conPtr->setRegionData( _levelhighPtr, _boundaryPtr, _simplifiedBoundaryPtr,
                               _cellPtr, _roadPtr, _lanePtr );
        vector < unsigned int > indexVec;
        indexVec.push_back( i );
        conPtr->init( indexVec, WORKER_CELL );
        controllers.push_back( conPtr );

        connect( conPtr, &Controller::update, this, &Window::redrawAllScene );
        connect( &conPtr->wt(), &QThread::finished, this, &Window::listenProcessCell );

        QString test;
        Q_EMIT conPtr->operate( test );
    }
}

void Window::stopProcessCell( void )
{
    // quit all threads
    for( unsigned int i = 0; i < controllers.size(); i++ ) {
        controllers[ i ]->wt().quit();
        delete controllers[ i ];
    }
    controllers.clear();

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
    for( unsigned int i = 0; i < controllers.size(); i++ ) {
        allFinished = allFinished && controllers[ i ]->wt().isFinished();
        // cerr << "is controllers[" << i << "] finished ? "<< controllers[ i ]->wt().isFinished();
    }

    if( ( allFinished == true ) && ( controllers.size() != 0 ) ){
        simulateKey( Qt::Key_S );
        //cerr << "Pressing Key_S size = " << controllers.size() << endl;
    }
}

void Window::processBone( void )
{
    vector< multimap< int, CellComponent > > & cellComponentVec = _cellPtr->cellComponentVec();

    for( unsigned int i = 0; i < cellComponentVec.size(); i++ ){

        multimap< int, CellComponent > &cellComponentMap = cellComponentVec[ i ];

        for( unsigned int j = 0; j < cellComponentMap.size(); j++ ){

            multimap< int, CellComponent >::iterator itC = cellComponentMap.begin();
            advance( itC, j );
            CellComponent &cell = itC->second;

            Controller * conPtr = new Controller;
            conPtr->setPathwayData( _pathway, *_pathway->width(), *_pathway->height() );
            conPtr->setRegionData( _levelhighPtr, _boundaryPtr, _simplifiedBoundaryPtr,
                                   _cellPtr, _roadPtr, _lanePtr );
            vector < unsigned int > indexVec;
            indexVec.push_back( i );
            indexVec.push_back( j );
            conPtr->init( indexVec, WORKER_BONE );
            controllers.push_back( conPtr );

            connect( conPtr, &Controller::update, this, &Window::redrawAllScene );
            connect( &conPtr->wt(), &QThread::finished, this, &Window::listenProcessBone );

            QString test;
            Q_EMIT conPtr->operate( test );
        }
    }
}

void Window::stopProcessBone( void )
{
    // quit all threads
    for( unsigned int i = 0; i < controllers.size(); i++ ) {
        controllers[ i ]->wt().quit();
        delete controllers[ i ];
    }
    controllers.clear();

    _gv->isSubPathwayFlag() = true;
}

void Window::listenProcessDetailedPathway( void )
{
    bool allFinished = true;

    // check if all threads are finished
    for( unsigned int i = 0; i < controllers.size(); i++ ) {
        allFinished = allFinished && controllers[ i ]->wt().isFinished();
        // cerr << "is controllers[" << i << "] finished ? "<< controllers[ i ]->wt().isFinished();
    }

    if( ( allFinished == true ) && ( controllers.size() != 0 ) ){
        simulateKey( Qt::Key_X );
        //cerr << "Pressing Key_X size = " << controllers.size() << endl;
    }
}

void Window::processDetailedPathway( void )
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
    for( unsigned int i = 0; i < cellComponentVec.size(); i++ ){

        const multimap< int, CellComponent > &cellComponentMap = cellComponentVec[i];

        for( unsigned int j = 0; j < cellComponentMap.size(); j++ ){

            // cerr << endl << endl << "enable a thread... i = " << i << " j = " << j << endl;
            Controller * conPtr = new Controller;
            conPtr->setPathwayData( _pathway, *_pathway->width(), *_pathway->height() );
            conPtr->setRegionData( _levelhighPtr, _boundaryPtr, _simplifiedBoundaryPtr,
                                   _cellPtr, _roadPtr, _lanePtr );
            vector < unsigned int > indexVec;
            indexVec.push_back( i );
            indexVec.push_back( j );
            conPtr->init( indexVec, WORKER_PATHWAY );
            controllers.push_back( conPtr );

            connect( conPtr, &Controller::update, this, &Window::redrawAllScene );
            connect( &conPtr->wt(), &QThread::finished, this, &Window::listenProcessDetailedPathway );

            QString test;
            Q_EMIT conPtr->operate( test );
        }
    }
}

void Window::stopProcessDetailedPathway( void )
{
    // quit all threads
    for( unsigned int i = 0; i < controllers.size(); i++ ) {
        controllers[ i ]->wt().quit();
        delete controllers[ i ];
    }
    controllers.clear();

    // compute steiner tree
    _roadPtr->initRoad( _cellPtr->cellComponentVec() );
    _roadPtr->buildRoad();
    // cerr << "road built..." << endl;
    _gv->isRoadFlag() = true;

    cerr << "/***************" << endl;
    cerr << "/* Steiner tree" << endl;
    cerr << "/***************" << endl;

    for( unsigned int i = 0; i < _roadPtr->highwayMat().size(); i++ ){
        for( unsigned int j = 0; j < _roadPtr->highwayMat()[i].size(); j++ ){

            map< MetaboliteGraph::vertex_descriptor,
                    MetaboliteGraph::vertex_descriptor > &common = _roadPtr->highwayMat()[i][j].common;

            map< MetaboliteGraph::vertex_descriptor,
                    MetaboliteGraph::vertex_descriptor >::iterator it;
#ifdef DEBUG
            for( it = common.begin(); it != common.end(); it++ ){
                    cerr << _pathway->subG()[i][ it->first ].id << endl;
                }
                cerr << endl;
#endif // DEBUG
        }
    }

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
    clearGraph( _boundaryPtr->boundary() );
    _boundaryPtr->nVertices() = 0;
    _boundaryPtr->nEdges() = 0;
    _boundaryPtr->nLines() = 0;
    unsigned int &nVertices = _boundaryPtr->nVertices();
    unsigned int &nEdges = _boundaryPtr->nEdges();
    unsigned int &nLines = _boundaryPtr->nLines();

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
                BGL_FORALL_VERTICES( vd, _boundaryPtr->boundary(), BoundaryGraph )
                    {
                        Coord2 &c = *_boundaryPtr->boundary()[ vd ].coordPtr;
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

                    curVD = add_vertex( _boundaryPtr->boundary() );
                    //vector< unsigned int > lineID = _boundaryPtr->boundary()[ curVD ].lineID;
                    //lineID.push_back( nLines );

                    double x = polygon.elements()[j-1+k].x();
                    double y = polygon.elements()[j-1+k].y();
                    _boundaryPtr->boundary()[ curVD ].geoPtr       = new Coord2( x, y );
                    _boundaryPtr->boundary()[ curVD ].smoothPtr    = new Coord2( x, y );
                    _boundaryPtr->boundary()[ curVD ].coordPtr     = new Coord2( x, y );

                    _boundaryPtr->boundary()[ curVD ].id = _boundaryPtr->boundary()[ curVD ].initID = nVertices;
                    _boundaryPtr->boundary()[ curVD ].namePtr = new string( to_string( _boundaryPtr->boundary()[ curVD ].id ) );
                    _boundaryPtr->boundary()[ curVD ].weight = 1.0;
                    //_boundaryPtr->boundary()[ curVD ].lineID.push_back( nLines );

                    if( k == 0 ) curVDS = curVD;
                    if( k == 1 ) curVDT = curVD;
                    nVertices++;
                }
                else{
                    _boundaryPtr->boundary()[ curVD ].lineID.push_back( nLines );
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
            tie( oldED, found ) = edge( curVDS, curVDT, _boundaryPtr->boundary() );


            if ( found == true ) {

                _boundaryPtr->boundary()[ oldED ].lineID.push_back( nLines );
                //eachline.push_back( oldED );
                //bool test = false;
                //tie( oldED, test ) = edge( oldVT, oldVS, _boundary );
            }
            else{

                //BoundaryGraph::vertex_descriptor src = vertex( indexS, _boundary );
                //BoundaryGraph::vertex_descriptor tar = vertex( indexT, _boundary );

                // handle fore edge
                pair<BoundaryGraph::edge_descriptor, unsigned int> foreE = add_edge( curVDS, curVDT, _boundaryPtr->boundary() );
                BoundaryGraph::edge_descriptor foreED = foreE.first;

                // calculate geographical angle
                Coord2 coordO;
                Coord2 coordD;
                if( _boundaryPtr->boundary()[ curVDS ].initID < _boundaryPtr->boundary()[ curVDT ].initID ){
                    coordO = *_boundaryPtr->boundary()[ curVDS ].coordPtr;
                    coordD = *_boundaryPtr->boundary()[ curVDT ].coordPtr;
                }
                else{
                    coordO = *_boundaryPtr->boundary()[ curVDT ].coordPtr;
                    coordD = *_boundaryPtr->boundary()[ curVDS ].coordPtr;
                }
                double diffX = coordD.x() - coordO.x();
                double diffY = coordD.y() - coordO.y();
                double angle = atan2( diffY, diffX );

                _boundaryPtr->boundary()[ foreED ].initID = _boundaryPtr->boundary()[ foreED ].id = nEdges;
                _boundaryPtr->boundary()[ foreED ].weight = 1.0;
                _boundaryPtr->boundary()[ foreED ].geoAngle = angle;
                _boundaryPtr->boundary()[ foreED ].smoothAngle = angle;
                _boundaryPtr->boundary()[ foreED ].angle = angle;
                _boundaryPtr->boundary()[ foreED ].lineID.push_back( nLines );

                //eachline.push_back( foreED );
#ifdef  DEBUG
                cout << "nEdges = " << _nEdges << " angle = " << angle << endl;
#endif  // DEBUG
                nEdges++;
            }
        }
        _levelhighPtr->polygonComplexVD().insert( pair< unsigned int, vector< ForceGraph::vertex_descriptor > >( i, vdVec ) );
    }

    // create lines
    // _line.resize( _polygonComplexVD.size() );
    map< unsigned int, vector< BoundaryGraph::vertex_descriptor > >::iterator itS;
    // map< unsigned int, vector< BoundaryGraph::vertex_descriptor > >::itereator itT;
    for( itS = _levelhighPtr->polygonComplexVD().begin(); itS != _levelhighPtr->polygonComplexVD().end(); itS++ ){
        vector< BoundaryGraph::vertex_descriptor > &vdVec = itS->second;
        _boundaryPtr->lineSta().push_back( vdVec );
    }
    nLines = _levelhighPtr->polygonComplexVD().size();

#ifdef DEBUG
    cerr << "nV = " << _nVertices << " nE = " << _nEdges
         << " nL = " << _nLines << endl;
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

    cerr << "MiddleBuildBoundary::nV = " << num_vertices( _boundaryPtr->boundary() ) << endl;
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
    vector< Package >   &cellVec = _cellPtr->cellVec();
    for( unsigned int i = 0; i < cellVec.size(); i++ ){
        cellVec[ i ].polygonComplexVD().clear();
    }
    clearGraph( _boundaryPtr->boundary() );
    _boundaryPtr->nVertices() = 0;
    _boundaryPtr->nEdges() = 0;
    _boundaryPtr->nLines() = 0;
    unsigned int &nVertices = _boundaryPtr->nVertices();
    unsigned int &nEdges = _boundaryPtr->nEdges();
    unsigned int &nLines = _boundaryPtr->nLines();

    // create boundary graph
    for( unsigned int m = 0; m < cellVec.size(); m++ ){
        for( unsigned int i = 0; i < cellVec[ m ].polygonComplex().size(); i++ ){

            Polygon2 &polygon = cellVec[ m ].polygonComplex()[i];
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
                    BGL_FORALL_VERTICES( vd, _boundaryPtr->boundary(), BoundaryGraph )
                        {
                            Coord2 &c = *_boundaryPtr->boundary()[ vd ].coordPtr;
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

                        curVD = add_vertex( _boundaryPtr->boundary() );
                        //vector< unsigned int > lineID = _boundaryPtr->boundary()[ curVD ].lineID;
                        //lineID.push_back( nLines );

                        double x = polygon.elements()[j-1+k].x();
                        double y = polygon.elements()[j-1+k].y();
                        _boundaryPtr->boundary()[ curVD ].geoPtr       = new Coord2( x, y );
                        _boundaryPtr->boundary()[ curVD ].smoothPtr    = new Coord2( x, y );
                        _boundaryPtr->boundary()[ curVD ].coordPtr     = new Coord2( x, y );

                        _boundaryPtr->boundary()[ curVD ].id = _boundaryPtr->boundary()[ curVD ].initID = nVertices;
                        _boundaryPtr->boundary()[ curVD ].namePtr = new string( to_string( _boundaryPtr->boundary()[ curVD ].id ) );
                        _boundaryPtr->boundary()[ curVD ].weight = 1.0;
                        //_boundaryPtr->boundary()[ curVD ].lineID.push_back( nLines );

                        if( k == 0 ) curVDS = curVD;
                        if( k == 1 ) curVDT = curVD;
                        nVertices++;
                    }
                    else{
                        _boundaryPtr->boundary()[ curVD ].lineID.push_back( nLines );
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
                tie( oldED, found ) = edge( curVDS, curVDT, _boundaryPtr->boundary() );


                if ( found == true ) {

                    _boundaryPtr->boundary()[ oldED ].lineID.push_back( nLines );
                    //eachline.push_back( oldED );
                    //bool test = false;
                    //tie( oldED, test ) = edge( oldVT, oldVS, _boundary );
                }
                else{

                    //BoundaryGraph::vertex_descriptor src = vertex( indexS, _boundary );
                    //BoundaryGraph::vertex_descriptor tar = vertex( indexT, _boundary );

                    // handle fore edge
                    pair<BoundaryGraph::edge_descriptor, unsigned int> foreE = add_edge( curVDS, curVDT, _boundaryPtr->boundary() );
                    BoundaryGraph::edge_descriptor foreED = foreE.first;

                    // calculate geographical angle
                    Coord2 coordO;
                    Coord2 coordD;
                    if( _boundaryPtr->boundary()[ curVDS ].initID < _boundaryPtr->boundary()[ curVDT ].initID ){
                        coordO = *_boundaryPtr->boundary()[ curVDS ].coordPtr;
                        coordD = *_boundaryPtr->boundary()[ curVDT ].coordPtr;
                    }
                    else{
                        coordO = *_boundaryPtr->boundary()[ curVDT ].coordPtr;
                        coordD = *_boundaryPtr->boundary()[ curVDS ].coordPtr;
                    }
                    double diffX = coordD.x() - coordO.x();
                    double diffY = coordD.y() - coordO.y();
                    double angle = atan2( diffY, diffX );

                    _boundaryPtr->boundary()[ foreED ].initID = _boundaryPtr->boundary()[ foreED ].id = nEdges;
                    _boundaryPtr->boundary()[ foreED ].weight = 1.0;
                    _boundaryPtr->boundary()[ foreED ].geoAngle = angle;
                    _boundaryPtr->boundary()[ foreED ].smoothAngle = angle;
                    _boundaryPtr->boundary()[ foreED ].angle = angle;
                    _boundaryPtr->boundary()[ foreED ].lineID.push_back( nLines );

                    //eachline.push_back( foreED );
#ifdef  DEBUG
                    cout << "nEdges = " << _nEdges << " angle = " << angle << endl;
#endif  // DEBUG
                    nEdges++;
                }
            }
            cellVec[ m ].polygonComplexVD().insert( pair< unsigned int, vector< ForceGraph::vertex_descriptor > >( i, vdVec ) );
        }

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
}

//
//  Window::updateLevelMiddlePolygonComplex --    build the boundary from the voronoi cell
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Window::updateLevelMiddlePolygonComplex( void )
{
    cerr << "updating polygonComplex after optimization ..." << endl;

    vector< Package >   &cellVec = _cellPtr->cellVec();
    for( unsigned int m = 0; m < cellVec.size(); m++ ){

        map< unsigned int, vector< ForceGraph::vertex_descriptor > >::iterator itP;
        map< unsigned int, Polygon2 >::iterator itC = cellVec[ m ].polygonComplex().begin();
        for( itP = cellVec[ m ].polygonComplexVD().begin(); itP != cellVec[ m ].polygonComplexVD().end(); itP++ ){
            vector< ForceGraph::vertex_descriptor > &p = itP->second;
            for( unsigned int i = 0; i < p.size(); i++ ){
                itC->second.elements()[i].x() = _boundaryPtr->boundary()[ p[i] ].coordPtr->x();
                itC->second.elements()[i].y() = _boundaryPtr->boundary()[ p[i] ].coordPtr->y();
            }
            itC++;
        }
    }
}


//
//  Window::selectLevelMiddleOctilinear --    build the boundary from the voronoi cell
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Window::selectLevelMiddleOctilinear( void )
{

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
    cerr << "updating polygonComplex after optimization ..." << endl;

    map< unsigned int, vector< ForceGraph::vertex_descriptor > >::iterator itP;
    map< unsigned int, Polygon2 >::iterator itC = _levelhighPtr->polygonComplex().begin();
    for( itP = _levelhighPtr->polygonComplexVD().begin(); itP != _levelhighPtr->polygonComplexVD().end(); itP++ ){
        vector< ForceGraph::vertex_descriptor > &p = itP->second;
        for( unsigned int i = 0; i < p.size(); i++ ){
            itC->second.elements()[i].x() = _boundaryPtr->boundary()[ p[i] ].coordPtr->x();
            itC->second.elements()[i].y() = _boundaryPtr->boundary()[ p[i] ].coordPtr->y();
        }
        itC++;
    }
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
            processBoundary();
            break;
        }
        case Qt::Key_2:
        {
            stopProcessBoundary();
            simulateKey( Qt::Key_O );
            // redrawAllScene();
            break;
        }
        case Qt::Key_Q:
        {
            simulateKey( Qt::Key_L );
            checkInETime();
            cerr << "*********** Starting Execution Time = " << checkOutETime() << endl;
            checkInCPUTime();
            cerr << "*********** Starting CPU Time = " << checkOutCPUTime() << endl;
            processCell();
            break;
        }
        case Qt::Key_W:
        {
            //cerr << "stop timer event" << endl;
            //_timerStop();
            //simulateKey( Qt::Key_P );
            stopProcessCell();
            //assert( _timerPtr->size() == 0 );

            selectLevelMiddleBuildBoundary();

            selectCloneGraph();
            selectLevelMiddleOctilinear();

            updateLevelMiddlePolygonComplex();
            //selectOctilinearSmallCboundary();

            //_gv->isPolygonFlag() = false;
            //_gv->isCompositeFlag() = false;
            //_gv->isPolygonComplexFlag() = true;
            _gv->isBoundaryFlag() = true;

            redrawAllScene();
            break;
        }
        case Qt::Key_A:
        {
            checkInETime();
            cerr << "*********** Starting Execution Time = " << checkOutETime() << endl;
            checkInCPUTime();
            cerr << "*********** Starting CPU Time = " << checkOutCPUTime() << endl;
            //_timerMCLStart();
            processBone();
            _gv->isCellPolygonFlag() = false;
            _gv->isMCLPolygonFlag() = true;
            break;
        }
        case Qt::Key_S:
        {
            cerr << "stopProcessBone..." << endl;
            stopProcessBone();
            _cellPtr->updatePathwayCoords();
            //_gv->isMCLPolygonFlag() = false;
            redrawAllScene();
            break;
        }
        case Qt::Key_Z:
        {
            checkInETime();
            cerr << "*********** Starting Execution Time = " << checkOutETime() << endl;
            checkInCPUTime();
            cerr << "*********** Starting CPU Time = " << checkOutCPUTime() << endl;
            //_timerPathwayStart();
            processDetailedPathway();
            _gv->isPathwayPolygonFlag() = true;
            _gv->isMCLPolygonFlag() = false;
            break;
        }
        case Qt::Key_X:
        {
            cerr << "stopProcessDetailedPathway..." << endl;
            stopProcessDetailedPathway();
            cerr << "[Force-Directed] Finished Execution Time = " << checkOutETime() << endl;
            cerr << "[Force-Directed] Finished CPU Time = " << checkOutCPUTime() << endl;
            redrawAllScene();
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
            _cellPtr->init( &_levelhighPtr->polygonComplex() );

            _gv->isCellFlag() = true;
            _gv->isCompositeFlag() = false;
            _gv->isPolygonFlag() = false;
            _gv->isPolygonComplexFlag() = true;
            _gv->isBoundaryFlag() = false;
            redrawAllScene();
            break;
        }
        case Qt::Key_P:
        {
            _cellPtr->createPolygonComplex();
            _cellPtr->updateMCLCoords();
            //_cellPtr->updatePathwayCoords();

            // _gv->isCellPolygonComplexFlag() = true;
            // _gv->isSubPathwayFlag() = true;
            redrawAllScene();
            break;
        }
        case Qt::Key_C:
        {
            selectCloneGraph();
            break;
        }
        case Qt::Key_M:
        {
            selectLevelHighMinDistance();
            break;
        }
        case Qt::Key_R:
        {
#ifdef SKIP
            _levelhighPtr->readPolygonComplex();
#endif // SKIP
            _gv->isPolygonComplexFlag() = true;
            _gv->isBoundaryFlag() = true;
            redrawAllScene();
            break;
        }
        case Qt::Key_D:
        {
            selectCloneGraph();
            selectLevelHighSmooth();
            //selectSmoothSmallCboundary();
            break;
        }
        case Qt::Key_O:
        {
            selectCloneGraph();
            selectLevelHighOctilinear();

            updateLevelHighPolygonComplex();
            //selectOctilinearSmallCboundary();

            _gv->isPolygonFlag() = false;
            _gv->isCompositeFlag() = false;
            _gv->isPolygonComplexFlag() = true;
            _gv->isBoundaryFlag() = true;
            redrawAllScene();
            break;
        }
        case Qt::Key_T:
        {
            selectCloneGraph();
            selectLevelHighStress();
            //_levelhighPtr->updatePolygonComplex();
            //selectOctilinearSmallCboundary();

            _gv->isSkeletonFlag() = true;
            //_gv->isPolygonFlag() = true;
            redrawAllScene();
            break;
        }
        case Qt::Key_B:
        {
            _levelhighPtr->createPolygonComplex( num_vertices( _levelhighPtr->skeleton() ) );
            selectLevelHighBuildBoundary();
#ifdef SKIP
            _levelhighPtr->writePolygonComplex();
#endif // SKIP
            break;
        }
        case Qt::Key_V:
        {
            _pathway->init( _gv->inputPath(), _gv->tmpPath(),
                            _gv->fileFreq(), _gv->fileType(),
                            _gv->cloneThreshold() );

            _pathway->generate();
            _pathway->exportEdges();

            _levelhighPtr->init( _pathway->skeletonG() );

            // update content width and height
#ifdef DEBUG
            cerr << "_content_width = " << _content_width
                 << " _content_height = " << _content_height << endl;
#endif // DEBUG
            double sum = 0.0;
            MetaboliteGraph             &g         = _pathway->g();
            BGL_FORALL_VERTICES( vd, g, MetaboliteGraph ) {
                QFontMetrics metrics( QFont( "Arial", _gv->fontSize(), QFont::Bold, false ) );
                double sx = metrics.width( QString::fromStdString( *g[vd].namePtr ) );
                double sy = metrics.height();
                sum += sx*sy;
            }

            double ratio = _gv->sizeRatio();
            double x = sqrt( sum * ratio / 12.0 );
            _content_width = 4.0 * x;
            _content_height = 3.0 * x;

#ifdef DEBUG
            cerr << "width x height = " << width() * height() << endl;
            cerr << "label sum = " << sum << endl;
            cerr << "ratio = " << width() * height() / sum << endl;
            cerr << "new_content_width = " << _content_width
                 << " new_content_height = " << _content_height << endl;
#endif // DEBUG
            _gv->setSceneRect( -( _content_width + LEFTRIGHT_MARGIN )/2.0, -( _content_height + TOPBOTTOM_MARGIN )/2.0,
                               _content_width + LEFTRIGHT_MARGIN, _content_height + TOPBOTTOM_MARGIN );

            _levelhighPtr->normalizeSkeleton( _content_width - LEFTRIGHT_MARGIN,
                                             _content_height - TOPBOTTOM_MARGIN );
            _levelhighPtr->decomposeSkeleton();
            _levelhighPtr->normalizeBone( _content_width - LEFTRIGHT_MARGIN,
                                         _content_height - TOPBOTTOM_MARGIN );

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
        case Qt::Key_Escape:
            close();
            break;
        default:
            QWidget::keyPressEvent( event );
    }
}
