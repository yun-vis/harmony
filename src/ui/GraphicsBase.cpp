#include "ui/GraphicsBase.h"

//------------------------------------------------------------------------------
//	Macro definition
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Private functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------
GraphicsBase::GraphicsBase( void ) {
	_id = 0;
	_weight = 0;
	
	_text = "";
	_textOn = false;
	_font_size = 12;
	_font = QFont( "Arial", _font_size, QFont::Normal, false );
}

GraphicsBase::~GraphicsBase() {
}
