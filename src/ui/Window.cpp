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

#ifdef DEBUG
    cerr << "_is_polygonFlag = " << _gv->isPolygonFlag() << endl;
    cerr << "_is_polygonComplexFlag = " << _gv->isPolygonComplexFlag() << endl;
#endif // DEBUG

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
    cerr << "selectCloneGraph..." << endl;

    // initialization
    _simplifiedBoundaryPtr->cloneLayout( *_boundaryPtr );
    _simplifiedBoundaryPtr->clearConflicts();

#ifdef DEBUG
    cerr << "nOriGraph = " << num_vertices( _boundaryPtr->boundary() ) << endl;
    cerr << "nSmallGraph = " << num_vertices( _simplifiedBoundaryPtr->boundary() ) << endl;
#endif // DEBUG
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

void Window::selectSmoothSmall( void )
{
    // run coarse smooth optimization
    clock_t start_time = clock();
    double err = 0.0;

    _smoothPtr->prepare( _simplifiedBoundaryPtr, _content_width/2, _content_height/2 );
    err = _smoothPtr->ConjugateGradient( 3 * _simplifiedBoundaryPtr->nVertices() );
    _smoothPtr->retrieve();

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

        _smoothPtr->prepare( _simplifiedBoundaryPtr, _content_width/2, _content_height/2 );
        if( num_vertices( _simplifiedBoundaryPtr->boundary() ) == ( num_vertices( _boundaryPtr->boundary() ) ) ) {
            iter = MAX2( 2 * ( _boundaryPtr->nVertices() - _simplifiedBoundaryPtr->nVertices() ), 30 );
        }
        else{
            iter = MAX2( 2 * ( _boundaryPtr->nVertices() - _simplifiedBoundaryPtr->nVertices() ), 30 );
        }
        err = _smoothPtr->ConjugateGradient( iter );
        _smoothPtr->retrieve();

        cerr << "    time each loop = " << clock() - start << " err = " << err << " iter = " << iter << endl;
        // ofs << "    time each loop = " << clock() - start << " err = " << err << " iter = " << iter << endl;
    }
    //_simplifiedBoundaryPtr->adjustsize( width(), height() );
    _smoothPtr->clear();
    _boundaryPtr->cloneSmooth( *_simplifiedBoundaryPtr );

    // cerr << "Total Time CG = " << clock() - start_time << endl;
    // ofs << "Total Time CG = " << clock() - start_time << endl;

    redrawAllScene();
}

void Window::selectSmooth( void )
{
    OPTTYPE opttype = _smoothPtr->opttype();

    // run smooth optimization
    _smoothPtr->prepare( _boundaryPtr, _content_width/2, _content_height/2 );

    switch( opttype ){
        case LEAST_SQUARE:
        {
            int iter = _boundaryPtr->nVertices();
            _smoothPtr->LeastSquare( iter );
        }
            break;
        case CONJUGATE_GRADIENT:
        {
            int iter = _boundaryPtr->nVertices();
            _smoothPtr->ConjugateGradient( iter );
        }
            break;
        default:
            break;
    }
    _smoothPtr->retrieve();
    _smoothPtr->clear();

    redrawAllScene();
}

void Window::selectOctilinearSmall( void )
{
    // run coarse octilinear optimization
    double err = 0.0;

    _octilinearPtr->prepare( _simplifiedBoundaryPtr, _content_width/2, _content_height/2 );
    err = _octilinearPtr->ConjugateGradient( 5 * _simplifiedBoundaryPtr->nVertices() );
    _octilinearPtr->retrieve();
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

        _octilinearPtr->prepare( _simplifiedBoundaryPtr, _content_width/2, _content_height/2 );
        if( num_vertices( _simplifiedBoundaryPtr->boundary() ) == ( num_vertices( _boundaryPtr->boundary() ) ) ) {
            iter = MAX2( 2 * ( _boundaryPtr->nVertices() - _simplifiedBoundaryPtr->nVertices() ), 30 );
        }
        else{
            iter = MAX2( 2 * ( _boundaryPtr->nVertices() - _simplifiedBoundaryPtr->nVertices() ), 30 );
        }
        err = _octilinearPtr->ConjugateGradient( iter );
        _octilinearPtr->retrieve();

        // ofs << "    time each loop = " << clock() - start << " err = " << err << " iter = " << iter << endl;
    }

    _octilinearPtr->clear();
    _boundaryPtr->cloneOctilinear( *_simplifiedBoundaryPtr );

    redrawAllScene();
}

void Window::selectOctilinear( void )
{
    cerr << "select octilinear ..." << endl;

    // initialization
    int iter = _boundaryPtr->nVertices();
    _octilinearPtr->clear();
    _octilinearPtr->prepare( _simplifiedBoundaryPtr, _content_width/2.0, _content_height/2.0 );

    // run octilinear optimization
    OPTTYPE opttype = _octilinearPtr->opttype();
#ifdef DEBUG
    cerr << "oct::_content_width = " << _content_width << " half = " << _content_width/2 << endl;
    cerr << "iter = " << iter << endl;
#endif // DEBUG
    switch( opttype ) {
        case LEAST_SQUARE:
        {
            _octilinearPtr->LeastSquare( iter );
        }
            break;
        case CONJUGATE_GRADIENT:
        {
            _octilinearPtr->ConjugateGradient( iter );
        }
            break;
        default:
        {
            cerr << "something is wrong here... at " << __LINE__ << " in " << __FILE__ << endl;
        }
            break;
    }
    _octilinearPtr->retrieve();

    redrawAllScene();
}

void Window::selectStressSmall( void )
{
    // run coarse smooth optimization
    clock_t start_time = clock();
    double err = 0.0;

    _smoothPtr->prepare( _simplifiedBoundaryPtr, _content_width/2, _content_height/2 );
    err = _smoothPtr->ConjugateGradient( 3 * _simplifiedBoundaryPtr->nVertices() );
    _smoothPtr->retrieve();

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

        _smoothPtr->prepare( _simplifiedBoundaryPtr, _content_width/2, _content_height/2 );
        if( num_vertices( _simplifiedBoundaryPtr->boundary() ) == ( num_vertices( _boundaryPtr->boundary() ) ) ) {
            iter = MAX2( 2 * ( _boundaryPtr->nVertices() - _simplifiedBoundaryPtr->nVertices() ), 30 );
        }
        else{
            iter = MAX2( 2 * ( _boundaryPtr->nVertices() - _simplifiedBoundaryPtr->nVertices() ), 30 );
        }
        err = _smoothPtr->ConjugateGradient( iter );
        _smoothPtr->retrieve();

        cerr << "    time each loop = " << clock() - start << " err = " << err << " iter = " << iter << endl;
        // ofs << "    time each loop = " << clock() - start << " err = " << err << " iter = " << iter << endl;
    }
    //_simplifiedBoundaryPtr->adjustsize( width(), height() );
    _smoothPtr->clear();
    _boundaryPtr->cloneSmooth( *_simplifiedBoundaryPtr );

    // cerr << "Total Time CG = " << clock() - start_time << endl;
    // ofs << "Total Time CG = " << clock() - start_time << endl;

    redrawAllScene();
}

void Window::selectStress( void )
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
    Controller * conPtr = new Controller;
    conPtr->setPathwayData( _pathway, *_pathway->width(), *_pathway->height() );
    conPtr->setRegionData( _levelhighPtr, _boundaryPtr, _simplifiedBoundaryPtr,
                           _cellPtr, _roadPtr, _lanePtr );

    vector < unsigned int > indexVec;
    conPtr->init( indexVec, WORKER_BOUNDARY );
    controllers.push_back( conPtr );

    connect( conPtr, &Controller::update, this, &Window::redrawAllScene );
    connect( &conPtr->wt(), &QThread::finished, this, &Window::listenProcessBoundary );

    _gv->isPolygonFlag() = true;

    QString text = "processBoundary";
    Q_EMIT conPtr->operate( text );
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
            CellComponent &c = itC->second;

            // create mcl force
            c.mcl.forceBone().init( &itC->second.mcl.bone(), itC->second.contour, "../configs/mcl.conf" );
            c.mcl.forceBone().id() = 0;

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

            QString text = "processBone";
            Q_EMIT conPtr->operate( text );
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
    unsigned int idC = 0;
    for( unsigned int i = 0; i < cellComponentVec.size(); i++ ){

        multimap< int, CellComponent > &cellComponentMap = cellComponentVec[i];

        for( unsigned int j = 0; j < cellComponentMap.size(); j++ ){

            multimap< int, CellComponent >::iterator itC = cellComponentMap.begin();
            advance( itC, j );
            CellComponent &c = itC->second;

            c.detail.forceBone().init( &c.detail.bone(), c.contour, "../configs/pathway.conf" );
            c.detail.forceBone().id() = idC;

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

            idC++;
            QString text = "processDetailedPathway";
            Q_EMIT conPtr->operate( text );
        }
    }
}

void Window::steinertree( void )
{

    // compute steiner tree
    _roadPtr->initRoad( _cellPtr->cellComponentVec() );
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
    for( unsigned int i = 0; i < controllers.size(); i++ ) {
        controllers[ i ]->wt().quit();
        delete controllers[ i ];
    }
    controllers.clear();
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
    _boundaryPtr->clear();
    _simplifiedBoundaryPtr->clear();

    BoundaryGraph &bg = _boundaryPtr->boundary();
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
        _boundaryPtr->lineSta().push_back( vdVec );
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
    _boundaryPtr->clear();
    _simplifiedBoundaryPtr->clear();

    BoundaryGraph &bg = _boundaryPtr->boundary();
    unsigned int &nVertices = _boundaryPtr->nVertices();
    unsigned int &nEdges = _boundaryPtr->nEdges();
    unsigned int &nLines = _boundaryPtr->nLines();

    // create boundary graph
    for( unsigned int m = 0; m < _cellPtr->cellComponentVec().size(); m++ ){

        resetVisitedTimes( bg );
        multimap< int, CellComponent > &cellComponent = _cellPtr->cellComponentVec()[m];
        unsigned int id = 0;
        for( multimap< int, CellComponent >::iterator itC = _cellPtr->cellComponentVec()[m].begin();
                itC != _cellPtr->cellComponentVec()[m].end(); itC++ ){

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
void Window::buildLevelDetailBoundaryGraph( void )
{
    // initialization
    _boundaryPtr->clear();
    _simplifiedBoundaryPtr->clear();

    BoundaryGraph &bg = _boundaryPtr->boundary();
    unsigned int &nVertices = _boundaryPtr->nVertices();
    unsigned int &nEdges = _boundaryPtr->nEdges();
    unsigned int &nLines = _boundaryPtr->nLines();

    // create boundary graph
    vector< multimap< int, CellComponent > > &cellCVec    = _cellPtr->cellComponentVec();
    for( unsigned int k = 0; k < cellCVec.size(); k++ ){


        multimap< int, CellComponent > &componentMap = cellCVec[k];
        multimap< int, CellComponent >::iterator itC = componentMap.begin();

        for( ; itC != componentMap.end(); itC++ ){

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

    BoundaryGraph &bg = _boundaryPtr->boundary();
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

    vector< Bone >   &cellVec = _cellPtr->cellVec();
    BoundaryGraph &bg = _boundaryPtr->boundary();
    for( unsigned int m = 0; m < _cellPtr->cellComponentVec().size(); m++ ){

        multimap< int, CellComponent > &cellComponent = _cellPtr->cellComponentVec()[m];

        for( multimap< int, CellComponent >::iterator itC = _cellPtr->cellComponentVec()[m].begin();
             itC != _cellPtr->cellComponentVec()[m].end(); itC++ ){

            vector< ForceGraph::vertex_descriptor > &polygonComplexVD = itC->second.polygonComplexVD;
            for( unsigned int i = 0; i < polygonComplexVD.size(); i++ ){
                itC->second.contour.elements()[i].x() = bg[ polygonComplexVD[i] ].coordPtr->x();
                itC->second.contour.elements()[i].y() = bg[ polygonComplexVD[i] ].coordPtr->y();
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

    vector< Bone >   &cellVec = _cellPtr->cellVec();
    BoundaryGraph &bg = _boundaryPtr->boundary();

    for( unsigned int m = 0; m < _cellPtr->cellComponentVec().size(); m++ ){

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
            processBoundary();
            break;
        }
        case Qt::Key_2:
        {
            stopProcessBoundary();
            simulateKey( Qt::Key_O );
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
            //simulateKey( Qt::Key_P );
            cerr << "stopProcessCell..." << endl;
            stopProcessCell();
            cerr << "[Force-Directed] Finished Execution Time = " << checkOutETime() << endl;
            cerr << "[Force-Directed] Finished CPU Time = " << checkOutCPUTime() << endl;

            // initialization and build the boundary
            selectLevelMiddleBuildBoundary();

            // optimization
            selectCloneGraph();
            selectOctilinear();
            updateLevelMiddlePolygonComplex();
            //selectOctilinearSmallCboundary();

            _gv->isBoundaryFlag() = true;
            _gv->isPolygonComplexFlag() = false;

            redrawAllScene();
            break;
        }
        case Qt::Key_A:
        {
            checkInETime();
            cerr << "*********** Starting Execution Time = " << checkOutETime() << endl;
            checkInCPUTime();
            cerr << "*********** Starting CPU Time = " << checkOutCPUTime() << endl;
            processBone();
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
            //_timerPathwayStart();
            processDetailedPathway();
            _gv->isCellPolygonComplexFlag() = false;
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

            // initialization and build the boundary
            selectLevelDetailBuildBoundary();

            // boundary optimization
            selectCloneGraph();
            selectOctilinear();
            updateLevelDetailPolygonComplex();

            // steiner tree
            steinertree();

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
            selectSmooth();
            //selectSmoothSmallBoundary();
            break;
        }
        case Qt::Key_O:
        {
            selectCloneGraph();
            selectOctilinear();

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
            selectStress();
            //_levelhighPtr->updatePolygonComplex();
            //selectOctilinearSmallBoundary();

            _gv->isSkeletonFlag() = true;
            //_gv->isPolygonFlag() = true;
            redrawAllScene();
            break;
        }
        case Qt::Key_B:
        {
            // initialization
            _boundaryPtr->clear();
            _simplifiedBoundaryPtr->clear();

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
            double canvasArea = width() * height();
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

            double x = sqrt( labelArea * _gv->veCoverage() / (double)_pathway->nVertices() / 12.0 );
            _content_width = 4.0 * x;
            _content_height = 3.0 * x;
            if( 2.0 * _content_width < width() ){
                _content_width = width()/2.0;
                _content_height = height()/2.0;
            }

            Polygon2 contour;
            contour.elements().push_back( Coord2( - 0.5*_content_width, - 0.5*_content_height ) );
            contour.elements().push_back( Coord2( + 0.5*_content_width, - 0.5*_content_height ) );
            contour.elements().push_back( Coord2( + 0.5*_content_width, + 0.5*_content_height ) );
            contour.elements().push_back( Coord2( - 0.5*_content_width, + 0.5*_content_height ) );
            _levelhighPtr->init( &_content_width, &_content_height, &_gv->veCoverage(),
                                 _pathway->skeletonG(), contour );
#ifdef DEBUG
            cerr << "width x height = " << width() * height() << endl;
            cerr << "label sum = " << sum << endl;
            cerr << "ratio = " << width() * height() / sum << endl;
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
