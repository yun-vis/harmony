
#include "ui/Window.h"

int main( int argc, char **argv ) {
	
	QApplication app( argc, argv );
	Common::Common();
	
//----------------------------------------------------------
// data
//----------------------------------------------------------
	Pathway pathway;

//----------------------------------------------------------
// view
//----------------------------------------------------------
	
	Window window;
	window.setWindowTitle( "Harmony" );
	window.setPathwayData( &pathway );
	window.init();
	window.show();
	window.batch();
	
	cerr << "Use: v -> 1 -> 2 -> o -> q -> w -> a -> s -> p -> z -> x -> r" << endl;
	
	return app.exec();
}
