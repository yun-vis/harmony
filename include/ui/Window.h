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

    //void createActions( void );
    //void createMenus( void );

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

    // level high
    void selectLevelHighBuildBoundary( void );
    void buildLevelHighBoundaryGraph( void );
    void updateLevelHighPolygonComplex( void );

    void selectLevelHighSmooth( void );
    void selectLevelHighOctilinear( void );
    void selectLevelHighStress( void );

    // simplification
    void selectCloneGraph( void );
    void selectLevelHighMinDistance( void );
    void selectLevelHighMovebackSmooth( void );
    void selectLevelHighMovebackOctilinear( void );
    void selectLevelHighMovebackStress( void );

    void selectLevelHighSmoothSmall( void );
    void selectLevelHighOctilinearSmall( void );
    void selectLevelHighStressSmall( void );

    // level middle
    void selectLevelMiddleBuildBoundary( void );
    void buildLevelMiddleBoundaryGraph( void );
    void updateLevelMiddlePolygonComplex( void );
    void selectLevelMiddleOctilinear( void );


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
