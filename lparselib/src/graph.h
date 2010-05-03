// graph.h -- definition of dependency graph for lparse
// Copyright (C) 1999-2007 Tommi Syrjänen <Tommi.Syrjanen@hut.fi>
//  
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//  
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//  
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//  

#ifndef GRAPH_H
#define GRAPH_H

#include "config.h"
#ifndef GLOBAL_H
#include "global.h"
#endif
#ifndef ARRAY_H
#include "array.h"
#endif

struct StronglyConnectedComponent {
  StronglyConnectedComponent() : negative_edge(0), positive_edge(0) { }
  LongList nodes;
  int negative_edge;
  int positive_edge;
  int is_domain;
  static int num_components;
};

// GraphNode implements nodes of the graph. Attributes are:
//     	- long size - how many children the node has
//	- long index - which node this is, no need actually, but can
//		       help debugging.
//	- ResizableArray *children  - array where children are kept
class GraphNode {
public:
  GraphNode(); 
  ~GraphNode() { delete children; delete negative_edge; }
  void AddChild(long chld, bool neg);
  void ClearEdges();
  long size;
  long index;
  ResizableArray *children;
  ResizableArray *negative_edge;
};


/* Graph implements the actual graph.
   Attributes:
   	GraphNodeArray *nodes	-- array of nodes
	long *visited		-- array used in cycle checking for
				   keeping information about visited
				   nodes. 
	DomainType *domain	-- array used in cycle checking for
	                           keeping information about domain
				   types of predicates.
        long size		-- size of the graph

   Methods:
   	void AddNode(long nd)	-- adds node 'nd' to graph. Does
        			   nothing if node is in graph
				   already. 
	void AddEdge(long a, long b) -- adds an edge (a,b) to the
				   graph. Doesn't remove duplicates. 
	
	void CheckCycles()	-- finds cycles in graph. Doesn't
			 	   return anything because Visit
				   updates all needed structures.
	int Visit(long nd)	-- Checks if there are any cycles in
				   paths starting from node 'nd' and
				   calculates domain of those
				   predicates that don't have
				   cycles. Also updates the predicates
				   array to correct domain status. 
	*/
class Graph {
public:
  Graph();
  ~Graph();

  void AddNode(long);
  void AddEdge(long, long, bool);
  void CheckCycles();
  void PrintDomains();

  void FindDomains(StronglyConnectedComponent*);
  // sort the graph topologically, and store the results in "toposort"
  // array. For example, given edges 0->1 and 2->1 the toposort array
  // contains items: { 0, 2, 1 }. Returns '0' if the graph has cycles. 
  int TopologicalSort();
  void ClearEdges(); // empty the graph

  void ExtractCycle(long start, LongList *result);
  
  
  long GetPosition(long);

  void PropagateExternalDefinitions(long nd);

  int IsTight(StronglyConnectedComponent *sc);
  int NotTight(LongList *lst);

  void OutputGraph();
  
private:
  int Visit(long node, StronglyConnectedComponent *);

  int TopologicalVisit(long node);
  int ExtractVisit(long node, LongList *result);
  int TarjanVisit(long node);


  GraphNodeArray *nodes;

  StronglyConnectedComponent *scc;
  void PrintSCCs(StronglyConnectedComponent *sc);
  
  long *visited;
  long *toposort;
  DomainType *domain;
  long size;
  long counter;
};


#endif
