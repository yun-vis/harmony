
#include "core/Common.h"

double Common::_content_width = 800;
double Common::_content_height = 600;
string Common::_batch_str = "0";
int Common::_font_size = 8;
ENERGYTYPE Common::_energy_type = ENERGY_FORCE;

Common::Common() {

	cerr << "HERE" << endl;
	string configFilePath = "config/" + Common::getBatchStr() + "/common.conf";
	Base::Config conf( configFilePath );

	if( conf.has( "font_size" ) ) {
		string paramFont = conf.gets( "font_size" );
		setFontSize( stringToDouble( paramFont ) );
	}
	
	if( conf.has( "default_width" ) ) {
		string paramWidth = conf.gets( "default_width" );
		_content_width = stoi( paramWidth );
	}
	
	if( conf.has( "default_height" ) ) {
		string paramHeight = conf.gets( "default_height" );
		_content_height = stoi( paramHeight );
	}
}

//  Common::stringToDouble -- convert string to double
//
//  Inputs
//      string
//
//  Outputs
//  double
//
double Common::stringToDouble( string str ) {
	stringstream ss( str );
	double val = 0;
	ss >> val;
	
	return val;
}

double Common::getContentWidth() {
	return _content_width;
}

void Common::setContentWidth( double contentWidth ) {
	_content_width = contentWidth;
}

double Common::getContentHeight() {
	return _content_height;
}

void Common::setContentHeight( double contentHeight ) {
	_content_height = contentHeight;
}

const string &Common::getBatchStr() {
	return _batch_str;
}

void Common::setBatchStr( const string &batchStr ) {
	_batch_str = batchStr;
}

int Common::getFontSize() {
	return _font_size;
}

void Common::setFontSize( int fontSize ) {
	_font_size = fontSize;
}

ENERGYTYPE Common::getEnergyType() {
	return _energy_type;
}

void Common::setEnergyType( ENERGYTYPE energyType ) {
	_energy_type = energyType;
}

