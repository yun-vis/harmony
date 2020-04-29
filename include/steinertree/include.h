#include <fstream>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <vector>
//#include <io>

#define INFINITE 10000000
#define PARTINODE 1
#define NORMALNODE 0

using namespace std;

class Array {
	void **array;

public:
	int rows;
	int cols;
	
	Array( int r, int c );
	
	Array( int c );
	
	void *get( int i, int j );
	
	void *get( int i );
	
	void put( int i, int j, void *k );
	
	void put( int i, void *k );
	
	void sort( int (*)( const void *, const void * ) );
	
	~Array();
};

struct terminal {
	int v;
	int parent;
};

class link {
	int node1, node2, cost;

public:
	link( int c1, int c2, int c ) {
		node1 = c1;
		node2 = c2;
		cost = c;
	}
	
	int getnode1() { return node1; }
	
	int getnode2() { return node2; }
	
	int getCost() { return cost; }
};


struct terminal *dijkstra( int *index, int max, Array *network,
                           int s, int parnum,
                           Array *parti, Array *G1 );

void spanning( Array *conn, Array *solut, int length, int max );

int constructG2( int cities, int parnum, Array *parti,
                 Array *cost, Array *G2, Array *G1solu, terminal **tree );

void printsolu( Array *solu, Array *cost, int index, vector< pair< unsigned int, unsigned int > > &treeedges );

void initialnode( Array *allnode, int max );

int delete_pendant( Array *solu, Array *allnode, Array *cost,
                    int start, int max );

void usage();

int steinertree( int nV, int nE,
                 vector< pair< unsigned int, unsigned int > > &graph,
                 vector< unsigned int > &terminals,
                 vector< pair< unsigned int, unsigned int > > &treeedges );
