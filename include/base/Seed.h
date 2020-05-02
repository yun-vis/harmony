
#ifndef _Seed_H        // beginning of header file
#define _Seed_H        // notifying that this file is included

#include "base/Coord2.h"
#include "base/Polygon2.h"

class Seed {

public:
	
	unsigned int id;                // id
	double weight;                  // weight
	Coord2 *coordPtr;               // position
	Polygon2 *voronoiCellPtr;       // cell polygon
};

#endif // _Seed_H

// end of header file
// Do not add any stuff under this line.