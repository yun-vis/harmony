//==============================================================================
// PathwayData.h
//  : header file for the map
//
//==============================================================================

#ifndef _PATHWAYDATA_H        // begining of header file
#define _PATHWAYDATA_H        // notifying that this file is included

//----------------------------------------------------------------------
//  Including header files
//----------------------------------------------------------------------
#include <QtGui/QPainter>
#include <QtCore/QString>

#include "Pathway.h"


class PathwayData
{
protected:

	Pathway             *_pathway;

    void _pickColor( COLORTYPE colorScheme, unsigned int id, vector< double > &rgb );
	void _pickPredefinedColor( unsigned int id, vector< double > &rgb );
	void _pickMonotoneColor( unsigned int id, vector< double > &rgb );
	void _pickPastelColor( unsigned int id, vector< double > &rgb );
	void _pickBrewerColor( unsigned int id, vector< double > &rgb );

public:

    PathwayData();                      // default constructor
    PathwayData( const PathwayData & obj );     // Copy constructor
    virtual ~PathwayData();             // Destructor

//------------------------------------------------------------------------------
//  Specific functions
//------------------------------------------------------------------------------
	void setPathwayData( Pathway *pathway, double &width, double &height );

//------------------------------------------------------------------------------
//      I/O
//------------------------------------------------------------------------------
    friend ostream & operator << ( ostream & stream, const PathwayData & obj );
    // Output
    friend istream & operator >> ( istream & stream, PathwayData & obj );
    // Input

    virtual const char * className( void ) const { return "PathwayData"; }
    // Class name
};



#endif // _PATHWAYDATA_H

// end of header file
// Do not add any stuff under this line.
