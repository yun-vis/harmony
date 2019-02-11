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
    vector< QBasicTimer	* > *_timerPtr;

    // rendering
    GraphicsView    *_gv;

    // threads
    vector< Controller * > controllers;

    // display
    int             _content_width;
    int             _content_height;

    // force target flag
    TIMERTYPE       _timerType;   // 0:TIMER_BOUNDARY, 1:TIMER_CELL, 2:TIMER_PATHWAY

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

    // optimization
    QMenu *optMenu;
    QAction *selSmoothLSAct;
    QAction *selOctilinearLSAct;
    QAction *selSmoothSmallCGAct;
    QAction *selSmoothCGAct;
    QAction *selOctilinearSmallCGAct;
    QAction *selOctilinearCGAct;

    void createActions( void );
    void createMenus( void );

    void postLoad( void );
    void simulateKey( Qt::Key key );

    void _init( void );

#ifdef RECORD_VIDEO
    void _timerVideoStart( void );
    void _timerVideoStop( void );
#endif //RECORD_VIDEO

    void _timerStop( void );
    void _timerBoundaryStart( void );
    void _timerBoundaryStop( void );
    void _timerPathwayCellStart( void );
    void _timerPathwayCellStop( void );
    void _timerMCLStart( void );
    void _timerMCLStop( void );
    void _timerPathwayStart( void );
    void _timerPathwayStop( void );

    bool _callTimerPathway( unsigned int id, unsigned int i, unsigned int j );

    // thread
    void processDetailedPathway( void );
    void stopProcessDetailedPathway( void );

public Q_SLOTS:

    // simplification
    void selectCloneGraph( void );
    void selectMinDistance( void );
    void selectMovebackSmooth( void );
    void selectMovebackOctilinear( void );

    // force
    void selectForce( void );

    // optimization
    void selectSmoothLS( void );
    void selectOctilinearLS( void );
    void selectSmoothSmallCG( void );
    void selectSmoothCG( void );
    void selectOctilinearSmallCG( void );
    void selectOctilinearCG( void );
    void selectSmooth( OPTTYPE opttype = CONJUGATE_GRADIENT );
    void selectOctilinear( OPTTYPE opttype = CONJUGATE_GRADIENT );

    // voronoi
    void selectBuildBoundary( void );

    // timer
#ifdef RECORD_VIDEO
    void timerVideo( void );
#endif //RECORD_VIDEO
    void timerBoundary( void );
    void timerPathwayCell( void );
    void timerMCL( void );
    void timerPathway( void );

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
