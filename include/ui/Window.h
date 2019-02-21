#ifndef WINDOW_H
#define WINDOW_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <map>
#include <list>
#include <algorithm>
#include <cmath>
#include <ctime>
#include <thread>
#include <cstdlib>

using namespace std;

#ifndef Q_MOC_RUN
#include "ui/GraphicsView.h"
#include "base/TimeComplexity.h"
#include "base/Controller.h"
#include "base/RegionData.h"
#endif // Q_MOC_RUN

#include <QtWidgets/QOpenGLWidget>      // qt should be included after boost to avoid conflict
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtGui/QMouseEvent>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QApplication>
#include <QtCore/QTimer>
#include <QtCore/QThread>

#define REMOVEBACKNUM   (15)
//#define RECORD_VIDEO

class Window : public QMainWindow, public PathwayData, public RegionData
{
    Q_OBJECT
private:

#ifdef RECORD_VIDEO
    unsigned int             _timerVideoID;
    QBasicTimer              _timerVideo;
#endif // RECORD_VIDEO

    // rendering
    GraphicsView    *_gv;

    // threads
    vector< Controller * > controllers;

    // display
    int             _content_width;
    int             _content_height;


    // menu
    // load
    QMenu *loadMenu;
    QAction *selDataAct;

    // simplification
    QMenu *simMenu;
    QAction *selCloneGraphAct;
    QAction *selMinDistanceAct;
    QAction *selMovebackSmoothAct;
    QAction *selMovebackOctilinearAct;
    QAction *selMovebackStressAct;

    // optimization
    QMenu *optMenu;
    QAction *selSmoothLSAct;
    QAction *selSmoothSmallCGAct;
    QAction *selSmoothCGAct;

    QAction *selOctilinearLSAct;
    QAction *selOctilinearSmallCGAct;
    QAction *selOctilinearCGAct;

    QAction *selStressLSAct;
    QAction *selStressSmallCGAct;
    QAction *selStressCGAct;

    void createActions( void );
    void createMenus( void );

    void postLoad( void );
    void simulateKey( Qt::Key key );

    void _init( void );

#ifdef RECORD_VIDEO
    void _timerVideoStart( void );
    void _timerVideoStop( void );
#endif //RECORD_VIDEO

    // thread controller
    // boundary
    void processBoundary( void );
    void stopProcessBoundary( void );
    // cell
    void processCell( void );
    void stopProcessCell( void );
    // bone
    void processBone( void );
    void stopProcessBone( void );
    // pathway
    void processDetailedPathway( void );
    void stopProcessDetailedPathway( void );

public Q_SLOTS:

    // simplification
    void selectCloneGraph( void );
    void selectMinDistance( void );
    void selectMovebackSmooth( void );
    void selectMovebackOctilinear( void );
    void selectMovebackStress( void );

    // force
    void selectForce( void );

    // optimization
    void selectSmoothLS( void );
    void selectOctilinearLS( void );
    void selectStressLS( void );

    void selectSmoothCG( void );
    void selectOctilinearCG( void );
    void selectStressCG( void );

    void selectSmoothSmallCG( void );
    void selectOctilinearSmallCG( void );
    void selectStressSmallCG( void );

    void selectSmooth( OPTTYPE opttype = CONJUGATE_GRADIENT );
    void selectOctilinear( OPTTYPE opttype = CONJUGATE_GRADIENT );
    void selectStress( OPTTYPE opttype = CONJUGATE_GRADIENT );

    // voronoi
    void selectBuildBoundary( void );

    // timer
#ifdef RECORD_VIDEO
    void timerVideo( void );
#endif //RECORD_VIDEO

    // thread controller
    // boundary
    void listenProcessBoundary( void );
    // cell
    void listenProcessCell( void );
    // bone
    void listenProcessBone( void );
    // pathway
    void listenProcessDetailedPathway( void );

    // display
    void redrawAllScene( void );

public:
    explicit Window( QWidget *parent = 0 );
    explicit Window( const Window & obj );
    ~Window();

    void init( void ){
        _init();
    }

protected:

    void keyPressEvent( QKeyEvent *event );
    void timerEvent( QTimerEvent *event );
};


#endif // WINDOW_H
