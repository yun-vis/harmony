#ifndef GraphicsBase_H
#define GraphicsBase_H

#include <iostream>
#include <iomanip>
#include <cstdlib>

using namespace std;

#include <QtGui/QFont>
#include <QtGui/QPen>
#include <QtCore/QString>

//------------------------------------------------------------------------------
//	Macro definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Class definition
//------------------------------------------------------------------------------
class GraphicsBase {
private:

protected:
	
	unsigned int _id;
	QString _name;
	double _weight;
	
	QString _text;
	bool _textOn;
	int _font_size;
	QFont _font;

public:
	
	GraphicsBase( void );
	
	~GraphicsBase();

//------------------------------------------------------------------------------
//      Reference to members
//------------------------------------------------------------------------------
	unsigned int &id( void ) { return _id; }
	
	const unsigned int &id( void ) const { return _id; }
	
	QString &name( void ) { return _name; }
	
	const QString &name( void ) const { return _name; }
	
	double &weight( void ) { return _weight; }
	
	const double &weight( void ) const { return _weight; }
	
	QString &text( void ) { return _text; }
	
	const QString &text( void ) const { return _text; }
	
	int &fontSize( void ) { return _font_size; }
	
	const int &fontSize( void ) const { return _font_size; }
	
	bool &textOn( void ) { return _textOn; }
	
	const bool &textOn( void ) const { return _textOn; }

//------------------------------------------------------------------------------
//      Specific methods
//------------------------------------------------------------------------------

};

#endif // GraphicsBase_H
