#include "steinertree/include.h"


int directpathcost(int start, int goal, Array *network);

//Dijkstra's algorithm, �åB�إ� shotest path tree
struct terminal * dijkstra(int *index, int max, Array *network, 
						   int s, int parnum, 
						   Array *parti, Array *G1)
{
  int x, y, v1, v2;
  int *totalcost, *visited;
  int mincost, minnode, newvalue, start;
  terminal *treenode;
  treenode = new terminal[max+1];
  totalcost = new int[max+1];
  visited = new int[max+1];
  start = *(int *)parti->get(s);
  
  for (x=1; x<= max; x++ ) {
    visited[x] = 0;
    totalcost[x] = directpathcost(start, x, network);
	treenode[x].v = x;
	treenode[x].parent = start;
  }
  totalcost[start] = INFINITE;
  visited[start] = 1;
  treenode[start].parent = -1;
  for ( x=1; x<max; x++ ) {
    mincost = INFINITE;
    minnode = 0;
    for ( y=1; y<=max; y++ ) {
      if ( visited[y] == 0 ) {
         if (totalcost[y] < mincost) {
            minnode = y;
            mincost = totalcost[y];
         }
      }
    }
	if (minnode == 0) {
		cout << "\n graph not full connected \n";
		x = max;
		continue;
	}
    visited[minnode] = 1;
    for ( y=1; y<=max; y++ ) {
      if (visited[y] == 0) {
        if (totalcost[minnode] == INFINITE) 
           newvalue = INFINITE;
        else {
           newvalue = directpathcost(minnode, y, network);
           if (newvalue != INFINITE)
              newvalue = newvalue + totalcost[minnode];
        }
        if (newvalue < totalcost[y]){
           totalcost[y] = newvalue;
		   treenode[y].parent = minnode;
		}
      }
    }
  }
  v1 = s;
  for (x=1; x<= parnum; x++) {
	  if (v1 < x) {
	     v2 = *(int *)parti->get(x);
		 G1->put(++(*index), new link(v1, x, totalcost[v2]));
	  }
  }
  delete totalcost;
  delete visited;
  return treenode;
}

int directpathcost (int start, int goal, Array *network)
{
  int *cost;
  cost = (int *)network->get( start, goal );
  return *cost;
}
