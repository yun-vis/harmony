#ifndef WINDOW_H
#define WINDOW_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <map>
#include <list>
#include <algorithm>
#include <cmath>
#include <cstdlib>

using namespace std;

#include <QtWidgets/QOpenGLWidget>      // qt should be included after boost to avoid conflict
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtGui/QMouseEvent>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QApplication>
#include <QtCore/QTimer>
#include <QtCore/QThread>

#ifndef Q_MOC_RUN
#include "base/Timer.h"
#include "base/ThreadOctilinearBoundary.h"
#include "base/ThreadLevelBorder.h"
#include "base/ThreadLevelCellCenter.h"
#include "base/ThreadLevelCellComponent.h"
#include "base/ThreadLevelDetail.h"
#include "base/RegionData.h"
#include "../third_party/CTPL/ctpl.h"
#include "ui/GraphicsView.h"
#endif // Q_MOC_RUN

#define REMOVEBACKNUM   (15)
//#define RECORD_VIDEO

class Window : public QMainWindow, public PathwayData, public RegionData {

Q_OBJECT
private:

#ifdef RECORD_VIDEO
	unsigned int             _timerVideoID;
	QBasicTimer              _timerVideo;
#endif // RECORD_VIDEO
	
	// rendering
	GraphicsView *_gv;
	LEVELTYPE _levelType;
	// string _batch_str;
	
	// display
	// double _content_width;
	// double _content_height;
	
	void _simulateKey( Qt::Key key );
	
	void _init( void );

#ifdef RECORD_VIDEO
	void _timerVideoStart( void );
	void _timerVideoStop( void );
#endif //RECORD_VIDEO
	
	// thread
	void _threadBoundaryForce( void );
	
	void _threadCellCenterForce( void );
	
	void _threadCellComponentForce( void );
	
	void _threadPathwayForce( void );
	
	void _threadOctilinearBoundary( void );
	
	void _generateConf( void );
	
	void _computeOverlaps( ofstream & ofs );

	void steinertree( void );
	
	void spaceCoverage( void );
	
	// level low
	// level componentRegion
	void selectLevelDetailBuildBoundary( void );
	
	void buildLevelDetailBoundaryGraph( void );
	
	void updateLevelDetailPolygonComplex( void );
	
	// timer
#ifdef RECORD_VIDEO
	void timerVideo( void );
#endif //RECORD_VIDEO
	
	// display
	void redrawAllScene( void );
	
	void updateAllScene( void );

protected:
	
	void keyPressEvent( QKeyEvent *event );
	
	void timerEvent( QTimerEvent *event );

public:
	
	explicit Window( QWidget *parent = 0 );
	
	explicit Window( const Window &obj );
	
	~Window();
	
	void init( void ) {
		_init();
	}
	void batch( void );
	void exportTex( void );

};


#endif // WINDOW_H
