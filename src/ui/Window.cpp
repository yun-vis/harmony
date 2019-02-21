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

    createActions();
    createMenus();



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
    _boundaryPtr = obj._boundaryPtr;
    _simplifiedBoundaryPtr = obj._simplifiedBoundaryPtr;

    // cells of subgraphs
    _cellPtr = obj._cellPtr;
    _roadPtr = obj._roadPtr;
    _lanePtr = obj._lanePtr;

    _smoothPtr = obj._smoothPtr;
    _octilinearPtr = obj._octilinearPtr;
    _stressPtr = obj._stressPtr;

    // display
    _content_width = obj._content_width;
    _content_height = obj._content_height;
}

void Window::_init( void )
{
    _content_width = width() - LEFTRIGHT_MARGIN;
    _content_height = height() - TOPBOTTOM_MARGIN;

    // initialization of boundary 
    _boundaryPtr = new Boundary;
    _simplifiedBoundaryPtr = new Boundary;
    _smoothPtr = new Smooth;
    _octilinearPtr = new Octilinear;
    _stressPtr = new Stress;

    // initialization of region data
    _cellPtr = new Cell;
    _roadPtr = new Road;
    _lanePtr = new vector< Road >;

    _cellPtr->setPathwayData( _pathway, *_pathway->width(), *_pathway->height() );
    _roadPtr->setPathwayData( _pathway, *_pathway->width(), *_pathway->height() );

    _gv->setPathwayData( _pathway, *_pathway->width(), *_pathway->height() );
    _gv->setRegionData( _boundaryPtr, _simplifiedBoundaryPtr,
                        _cellPtr, _roadPtr, _lanePtr );
}

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

void Window::selectForce( void )
{
    simulateKey( Qt::Key_F );
    redrawAllScene();
}

void Window::selectCloneGraph( void )
{
    _simplifiedBoundaryPtr->cloneLayout( *_boundaryPtr );
    _simplifiedBoundaryPtr->clearConflicts();

    redrawAllScene();
}

void Window::selectMinDistance( void )
{
    _simplifiedBoundaryPtr->simplifyLayout();
    _gv->isSimplifiedFlag() = true;
    redrawAllScene();
}

void Window::selectMovebackSmooth( void )
{
    bool isFinished = true;
    isFinished = _simplifiedBoundaryPtr->movebackNodes( *_boundaryPtr, TYPE_SMOOTH );
    // if( isFinished == true ) cerr << "All stations are moved back!!" << endl;
    redrawAllScene();
}

void Window::selectMovebackOctilinear( void )
{
    bool isFinished = true;
    isFinished = _simplifiedBoundaryPtr->movebackNodes( *_boundaryPtr, TYPE_OCTILINEAR );
    // if( isFinished == true ) cerr << "All stations are moved back!!" << endl;
    redrawAllScene();
}

void Window::selectMovebackStress( void )
{
    bool isFinished = true;
    isFinished = _simplifiedBoundaryPtr->movebackNodes( *_boundaryPtr, TYPE_STRESS );
    // if( isFinished == true ) cerr << "All stations are moved back!!" << endl;
    redrawAllScene();
}

void Window::selectSmoothSmallCG( void )
{
    // run coarse smooth optimization
    clock_t start_time = clock();
    double err = 0.0;
    unsigned int nLabels = _simplifiedBoundaryPtr->nLabels();
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
        if( num_vertices( _simplifiedBoundaryPtr->boundary() ) == ( num_vertices( _boundaryPtr->boundary() ) + nLabels ) ) {
            break;
        }
        else {
            for( int i = 0; i < REMOVEBACKNUM; i++ ){
                selectMovebackSmooth();
            }
        }

        _smoothPtr->prepare( _simplifiedBoundaryPtr, _content_width/2, _content_height/2 );
        if( num_vertices( _simplifiedBoundaryPtr->boundary() ) == ( num_vertices( _boundaryPtr->boundary() ) + nLabels ) ) {
            iter = MAX2( 2 * ( _boundaryPtr->nVertices() + nLabels - _simplifiedBoundaryPtr->nVertices() ), 30 );
        }
        else{
            iter = MAX2( 2 * ( _boundaryPtr->nVertices() + nLabels - _simplifiedBoundaryPtr->nVertices() ), 30 );
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

void Window::selectSmooth( OPTTYPE opttype )
{
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

void Window::selectSmoothLS( void )
{
    selectSmooth( LEAST_SQUARE );
    redrawAllScene();
}

void Window::selectSmoothCG( void )
{
    selectSmooth( CONJUGATE_GRADIENT );
    redrawAllScene();
}

void Window::selectOctilinearSmallCG( void )
{
    // run coarse octilinear optimization
    double err = 0.0;
    unsigned int nLabels = _simplifiedBoundaryPtr->nLabels();

    _octilinearPtr->prepare( _simplifiedBoundaryPtr, _content_width/2, _content_height/2 );
    err = _octilinearPtr->ConjugateGradient( 5 * _simplifiedBoundaryPtr->nVertices() );
    _octilinearPtr->retrieve();
    //ofs << "    Coarse CG: " << clock() - start_time << " err = " << err << " iter = " << 5 * _simplifiedBoundaryPtr->nVertices() << endl;

    // run octilinear optimization
    while( true ) {

        clock_t start = clock();
        int iter = 0;

        // check if all nodes are moved back
        if( num_vertices( _simplifiedBoundaryPtr->boundary() ) == ( num_vertices( _boundaryPtr->boundary() ) + nLabels ) ) {
            break;
        }
        else {
            for( int i = 0; i < REMOVEBACKNUM; i++ ){
                selectMovebackOctilinear();
            }
        }

        _octilinearPtr->prepare( _simplifiedBoundaryPtr, _content_width/2, _content_height/2 );
        if( num_vertices( _simplifiedBoundaryPtr->boundary() ) == ( num_vertices( _boundaryPtr->boundary() ) + nLabels ) ) {
            iter = MAX2( 2 * ( _boundaryPtr->nVertices() + nLabels - _simplifiedBoundaryPtr->nVertices() ), 30 );
        }
        else{
            iter = MAX2( 2 * ( _boundaryPtr->nVertices() + nLabels - _simplifiedBoundaryPtr->nVertices() ), 30 );
        }
        err = _octilinearPtr->ConjugateGradient( iter );
        _octilinearPtr->retrieve();

        // ofs << "    time each loop = " << clock() - start << " err = " << err << " iter = " << iter << endl;
    }

    _octilinearPtr->clear();
    _boundaryPtr->cloneOctilinear( *_simplifiedBoundaryPtr );

    redrawAllScene();
}

void Window::selectOctilinear( OPTTYPE opttype )
{
    // run octilinear optimization
    _octilinearPtr->prepare( _boundaryPtr, _content_width/2, _content_height/2 );

    switch( opttype ) {
        case LEAST_SQUARE:
        {
            int iter = _boundaryPtr->nVertices();
            _octilinearPtr->LeastSquare( iter );
        }
            break;
        case CONJUGATE_GRADIENT:
        {
            int iter = _boundaryPtr->nVertices();
            _octilinearPtr->ConjugateGradient( iter );
        }
            break;
        default:
            break;
    }
    _octilinearPtr->retrieve();
    _octilinearPtr->clear();

    redrawAllScene();
}

void Window::selectOctilinearLS( void )
{
    selectOctilinear( LEAST_SQUARE );
    redrawAllScene();
}

void Window::selectOctilinearCG( void )
{
    selectOctilinear( CONJUGATE_GRADIENT );
    redrawAllScene();
}


void Window::selectStressSmallCG( void )
{
    // run coarse smooth optimization
    clock_t start_time = clock();
    double err = 0.0;
    unsigned int nLabels = _simplifiedBoundaryPtr->nLabels();
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
        if( num_vertices( _simplifiedBoundaryPtr->boundary() ) == ( num_vertices( _boundaryPtr->boundary() ) + nLabels ) ) {
            break;
        }
        else {
            for( int i = 0; i < REMOVEBACKNUM; i++ ){
                selectMovebackSmooth();
            }
        }

        _smoothPtr->prepare( _simplifiedBoundaryPtr, _content_width/2, _content_height/2 );
        if( num_vertices( _simplifiedBoundaryPtr->boundary() ) == ( num_vertices( _boundaryPtr->boundary() ) + nLabels ) ) {
            iter = MAX2( 2 * ( _boundaryPtr->nVertices() + nLabels - _simplifiedBoundaryPtr->nVertices() ), 30 );
        }
        else{
            iter = MAX2( 2 * ( _boundaryPtr->nVertices() + nLabels - _simplifiedBoundaryPtr->nVertices() ), 30 );
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

void Window::selectStress( OPTTYPE opttype )
{
    // run smooth optimization
    _stressPtr->prepare( &_boundaryPtr->skeleton(), _content_width/2, _content_height/2 );

    switch( opttype ){
        case LEAST_SQUARE:
        {
            int iter = _boundaryPtr->nVertices();
            _stressPtr->LeastSquare( iter );
            break;
        }
        case CONJUGATE_GRADIENT:
        {
            unsigned int long iter = _stressPtr->nVertices();
            //_stressPtr->ConjugateGradient( iter );
            _stressPtr->ConjugateGradient( 1 );
            break;
        }
        default:
            break;
    }
    _stressPtr->retrieve();
    _stressPtr->clear();

    redrawAllScene();
}

void Window::selectStressLS( void )
{
    selectStress( LEAST_SQUARE );
    redrawAllScene();
}

void Window::selectStressCG( void )
{
    selectStress( CONJUGATE_GRADIENT );
    redrawAllScene();
}

void Window::selectBuildBoundary( void )
{
    _boundaryPtr->buildBoundaryGraph();
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

    _boundaryPtr->forceBoundary().init( &_boundaryPtr->composite(), contour, "../configs/boundary.conf" );

    _gv->isPolygonFlag() = true;

    Controller * conPtr = new Controller;
    conPtr->setPathwayData( _pathway, *_pathway->width(), *_pathway->height() );
    conPtr->setRegionData( _boundaryPtr, _simplifiedBoundaryPtr,
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

    for( unsigned int i = 0; i < _cellPtr->forceCellVec().size(); i++ ){

        Controller * conPtr = new Controller;
        conPtr->setPathwayData( _pathway, *_pathway->width(), *_pathway->height() );
        conPtr->setRegionData( _boundaryPtr, _simplifiedBoundaryPtr,
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
            conPtr->setRegionData( _boundaryPtr, _simplifiedBoundaryPtr,
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
            conPtr->setRegionData( _boundaryPtr, _simplifiedBoundaryPtr,
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
            cerr << "test = " << (*_boundaryPtr->forceBoundary().voronoi().seedVec())[0].cellPolygon.elements().size() << endl;
            stopProcessBoundary();
            cerr << "test2 = " << (*_boundaryPtr->forceBoundary().voronoi().seedVec())[0].cellPolygon.elements().size() << endl;
            redrawAllScene();
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
            _pathway->initLayout( _boundaryPtr->polygonComplex() );

            // initialize cell
            _cellPtr->clear();
            _cellPtr->init( &_boundaryPtr->polygonComplex() );

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
            selectMinDistance();
            break;
        }
        case Qt::Key_R:
        {
            _boundaryPtr->readPolygonComplex();
            _gv->isPolygonComplexFlag() = true;
            _gv->isBoundaryFlag() = true;
            redrawAllScene();
            break;
        }
/*
        case Qt::Key_S:
        {
            selectCloneGraph();
            selectSmoothCG();
            //selectSmoothSmallCboundary();
            break;
        }
*/
        case Qt::Key_O:
        {
            selectCloneGraph();
            selectOctilinearCG();
            _boundaryPtr->updatePolygonComplex();
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
            selectStressCG();
            //_boundaryPtr->updatePolygonComplex();
            //selectOctilinearSmallCboundary();

            _gv->isSkeletonFlag() = true;
            //_gv->isPolygonFlag() = true;
            redrawAllScene();
            break;
        }
        case Qt::Key_B:
        {
            _boundaryPtr->createPolygonComplex();
            selectBuildBoundary();
            _boundaryPtr->writePolygonComplex();
            break;
        }
        case Qt::Key_V:
        {
            _pathway->init( _gv->inputPath(), _gv->tmpPath(),
                            _gv->fileFreq(), _gv->fileType(),
                            _gv->cloneThreshold() );

            _pathway->generate();
            _pathway->exportEdges();

            _boundaryPtr->init( _pathway->skeletonG() );

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

            _boundaryPtr->normalizeSkeleton( _content_width, _content_height );
            _boundaryPtr->decomposeSkeleton();
            _boundaryPtr->normalizeComposite( _content_width, _content_height );

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
