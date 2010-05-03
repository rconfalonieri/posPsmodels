// graph.cc -- implementation of dependency graphs for lparse
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

#include "config.h"
#include <string.h>
#include <stdlib.h>

#ifndef PREDICATE_H
#include "predicate.h"
#endif
#ifndef GLOBAL_H
#include "global.h"
#endif
#ifndef GRAPH_H
#include "graph.h"
#endif
#ifndef DEBUG_H
#include "debug.h"
#endif
#ifndef ARRAY_H
#include "array.h"
#endif
#ifndef ERROR_H
#include "error.h"
#endif
#include "symbol.h"
static char *domain_string[] = { "DM_UNKNOWN", "DM_FALSE", "DM_TRUE",
				 "DM_DOMAIN" }; 

int StronglyConnectedComponent::num_components;


GraphNode::GraphNode()
  : children(NULL)
{
  size = 0;
  children = new ResizableArray();
  negative_edge = new ResizableArray();
  if (!children)
    error(SYS_ERR, "malloc error");
}

void GraphNode::AddChild(long child, bool neg)
{
  (*(*children)[size]) = child;
  (*(*negative_edge)[size++]) = neg;
}


void GraphNode::ClearEdges()
{
  delete children;
  delete negative_edge;
  children = new ResizableArray(size ? size : 10);
  negative_edge = new ResizableArray(size ? size : 10);
  size = 0;
}

Graph::Graph()
{
  size = 0; visited = NULL; 
  domain = NULL;
  scc = NULL;
  nodes = new GraphNodeArray();
  if (!nodes)
    error(SYS_ERR, "malloc error");
}

Graph::~Graph()
{
  delete nodes;
  if (domain) 
    delete [] domain; 
  if (visited)
    delete [] visited;
  if (scc)
    delete [] scc;
}

void Graph::AddNode(long nd)
{
  (*nodes)[nd]->index = size++;
  
  /* delete old information */
  if (domain) {
    delete [] domain;
    domain = NULL;
  }
}

void Graph::AddEdge(long x, long y, bool neg)
{
  /* sanity check */
  if ( (x > size) || (x < 0) || (y > size) || (y < 0))
    int_error("invalid edge in AddEdge", x);

  (*nodes)[x]->AddChild(y, neg);
}

void Graph::CheckCycles()
{
  long i, j;
  long p;
  debug(DBG_GRAPH, 2, "Finding cycles in graph");
  debug(DBG_GRAPH, 4, "Creating tables");

  visited = new long[size+1];
  scc = new StronglyConnectedComponent[size+1];
  for (i = 0; i <= size; i++) {
    scc[i].negative_edge = 0;
    scc[i].positive_edge = 0;
    scc[i].is_domain = 0;
  }
  
  if (!domain)
    domain = new DomainType[size+1];

  if (!domain || !visited)
    error(SYS_ERR, "malloc error");

  // initialize domain and visited information 
  for (i = 0; i <= size; i++) {
    domain[i] =  predicates[i]->Status();
    visited[i] = 0;
  }

  // visit every node 
  for (i = 0; i < size; i++) {
    if (!visited[i])
      TarjanVisit(i);
  }

  // construct the scc-graph
  if (!scc_graph) {
    scc_graph = new Graph;
  }
  // update visited to contain the correct scc number
  for (i = 0; i < StronglyConnectedComponent::num_components; i++) {
    scc_graph->AddNode(i);
    scc[i].nodes.ClearIterator();
    while ((p = scc[i].nodes.Iterate())) {
      p--;
      visited[p] = i;
    }
  }
  // go through all edges, put into scc_graph those that connect
  // components and mark all sccs with negative edges into scc-array
  long i_component, j_component, chld;
  int neg;
  for (i = 0; i < size; i++) {
    i_component = visited[i];
    predicates[i]->num_components = scc[i_component].nodes.Size();
    for (j = 0; j < (*nodes)[i]->size; j++) {
      chld = *((*(*nodes)[i]->children)[j]); // ugly
      neg = *((*(*nodes)[i]->negative_edge)[j]); // ugly
      
      j_component = visited[chld];

      if (!neg &&(i_component == j_component)) {
	scc[i_component].positive_edge = 1;
      }
      
      if (i_component != j_component) {
	scc_graph->AddEdge(i_component, j_component, neg);
      } else if (neg) {
	scc_graph->AddEdge(i_component, j_component, neg);
      }
    }
  }
  scc_graph->FindDomains(scc);
  if (sys_data.check_tightness) {
    sys_data.is_tight = IsTight(scc);
  }
  return;
}

void Graph::FindDomains(StronglyConnectedComponent *sc)
{
  long i;
  if (!visited)
    visited = new long[size+1];
  
  if (!domain)
    domain = new DomainType[size+1];

  // initialize domain and visited information 
  for (i = 0; i <= size; i++) {
    domain[i] =  DM_UNKNOWN;
    visited[i] = 0;
  }
  // visit every node 
  for (i = 0; i < size; i++) 
    if (domain[i] == DM_UNKNOWN)
      Visit(i, sc);
  return;
}



int Graph::TarjanVisit(long node)
{
  static long *stack = NULL;
  static long top = 0;
  static long id = 0;
  long chld = 0;
  int m = 0, min, i;
  
  if (!stack) {
    stack = new long[size+1];
    for (i = 0; i < size+1; i++) {
      stack[i] = -1;
    }
  }
  
  visited[node] = ++id;
  min = id;

  stack[top++] = node;
  
  for (i = 0; i < (*nodes)[node]->size; i++) {
    chld = *((*(*nodes)[node]->children)[i]); // ugly
    
    if (!visited[chld]) {
      m = TarjanVisit(chld);
    } else {
      m = visited[chld];
    }
    if (m < min)
      min = m;
  }

  if (min == visited[node]) {
    int current_component = StronglyConnectedComponent::num_components++;
    //    output(cout,"Component: ");
    while (stack[top] != node) {
      stack[top] = -1;
      top--;

      scc[current_component].nodes.Insert(stack[top]+1);
      //      output(cout,"%s/%d ", predicates[stack[top]]->Name(),
      //      	     predicates[stack[top]]->Arity());
      visited[stack[top]] = size+2;
    }
    //    output(cout,"\n");
  }
  return min;
}


int Graph::Visit(long node, StronglyConnectedComponent *sc)
{
  long i, val = 0, chld = 0, neg = 0;
  debug(DBG_GRAPH, 4, "Visiting node %d: %s", node, predicates[node]->Name());

  // mark this node as visited
  visited[node] = 1;
  
  // suppose by default that node is a domain predicate
  domain[node] = DM_TRUE;

  // now visit each children of the node
  for (i = 0; i < (*nodes)[node]->size; i++) {
    chld = *((*(*nodes)[node]->children)[i]); // ugly
    neg = *((*(*nodes)[node]->negative_edge)[i]); // ugly
    debug(DBG_GRAPH, 5, "\tChild %d", chld);
    // if we have visited the child previously a cycle is found 
    if ( visited[chld] == 1 ) {
      domain[node] = DM_FALSE;
      val = DM_FALSE;
    }
    // if we haven't processed the child before process it, otherwise
    // use the calculated value.
    else if ( domain[chld] == DM_UNKNOWN)
      val = Visit(chld, sc);
    else
      val = domain[chld];
    
    debug(DBG_GRAPH, 5, "\t\tDomain %d", domain[chld]);
    if (val == DM_FALSE)
      domain[node] = DM_FALSE;
  }
  // predicates with special rules can't be domain predicates. 
  if (Predicate::HasSpecialRules(&sc[node])) {
    domain[node] = DM_FALSE;
  }
  // clear the visited mark of the node
  visited[node] = 0;
  
  // if the node is a domain predicate, calculate its domain.
  if (domain[node] == DM_TRUE) {
    Predicate::CalculateSCCDomain(&sc[node]);
  } else {
    Predicate::SetDomainFalse(&sc[node]);
  }
  //    if (predicates[node]->CheckRestrict(0) == RT_STRONG)
  //      predicates[node]->CalculateDomain();
  //  } else {
  //    predicates[node]->SetStatus(DM_FALSE);
  //  }
  // return the domain value of the node.
  return (int) domain[node];
}

// prints the domain values. For debugging only.
void Graph::PrintDomains()
{
  long i;
  if (!domain)
    return;

  for (i = 0; i < size; i++) {
    output(cout,"Node %ld: %s\n", i, domain_string[domain[i]]);
  }

  return;
}

void Graph::ClearEdges()
{
  int i =0;

  for (i = 0; i < size; i++) {
    if (visited) 
      visited[i] = 0;
    if (domain)
      domain[i] = DM_UNKNOWN;
    if (toposort)
      toposort[i] = -1;
  }
  nodes->ClearEdges();
}

void Graph::ExtractCycle(long start, LongList *result)
{
  if (!visited) {
    visited = new long[size];
  }
  
  for (int i = 0; i < size; i++) {
    visited[i] = 0;
  }

  if (ExtractVisit(start, result) == 0) {
    result->Insert(start+1);
  }

  return;
  
}

int Graph::TopologicalSort()
{
  int i = 0;

  if (!visited) {
    visited = new long[size];
    toposort = new long[size];
  }
  for (i = 0; i < size; i++) {
    visited[i] = 0;
    toposort[i] = -1;
  }

  counter = 0;
  for (i = 0; i < size; i++) {
    if (toposort[i] < 0) {
      if (!TopologicalVisit(i)) {
	return 0;
      }
    }
  }
  return 1;
}

int Graph::TopologicalVisit(long node)
{
  long i = 0, chld = 0;
  // mark this node as visited
  visited[node] = 1;
  
  // now visit each children of the node
  for (i = 0; i < (*nodes)[node]->size; i++) {
    chld = *((*(*nodes)[node]->children)[i]); // ugly
    if (visited[chld]) // a cycle
      return 0;

    TopologicalVisit(chld);
  }

  // mark the order of this node
  toposort[node] = counter++;
  visited[node] = 0;
  return 1;
}


int Graph::ExtractVisit(long node, LongList *result)
{
  long i = 0, chld = 0, neg = 0;
  // mark this node as visited
  visited[node] = 1;
  
  // now visit each children of the node
  for (i = 0; i < (*nodes)[node]->size; i++) {
    chld = *((*(*nodes)[node]->children)[i]); // ugly
    neg = *((*(*nodes)[node]->negative_edge)[i]); // ugly
    if (visited[chld]) { // a cycle
      if ((chld != node) || neg) { // true loop, add to result
	result->Insert( chld +1);
	return 0;
      }
    } else {
      if (ExtractVisit(chld, result) == 0) {
	result->Insert( chld +1);
	return 0;
      }
    }
  }
  visited[node] = 0;
  return 1;
}

long Graph::GetPosition(long x)
{
  return toposort[x];
}


void Graph::PropagateExternalDefinitions(long nd)
{
  long chld, i, j;
  int changed = 1;

  while (changed) {
    changed = 0;

    for (i = 0; i < size; i++) {
      for (j = 0; j < (*nodes)[i]->size; j++) {
	chld = *((*(*nodes)[i]->children)[j]); // ugly

	if ((chld == nd) || predicates[chld]->depends_on_external_atoms) {
	  if (!predicates[i]->depends_on_external_atoms)
	    changed = 1;
	  predicates[i]->depends_on_external_atoms = 1;
	}
      }
    }
  }
  
}

void Graph::PrintSCCs(StronglyConnectedComponent *sc)
{
  long i, j, l, chld;

  for (i = 0; i < StronglyConnectedComponent::num_components; i++) {
    output(cout,"%ld: ", i);
    sc[i].nodes.ClearIterator();
    while ((l = sc[i].nodes.Iterate())) {
      
      l--;
      output(cout,"%s ", predicates[l]->Name());
    }
    output(cout,"\n");
  }
  for (i = 0; i < size; i++) {
    for (j = 0; j < (*nodes)[i]->size; j++) {
      chld = *((*(*nodes)[i]->children)[j]); 

      output(cout,"%ld %ld\n", i, chld);
    }
  }
}

int Graph::NotTight(LongList *lst)
{
  return 1;
}

int Graph::IsTight(StronglyConnectedComponent *sc)
{
  long i; //, j, l, chld;
  
  for (i = 0; i < StronglyConnectedComponent::num_components; i++) {
    if (sc[i].positive_edge)
      if (sys_data.print_domains == PR_ALL)
	return 0;
      else if (!sc[i].is_domain)
	return 0;
  }
  return 1;
}


void ppreds()
{
  predicate_table->PrintItems();
}

void pvars()
{
  variable_table->PrintItems();
}


void Graph::OutputGraph()
{
  long i;
  long chld, neg;
  long node;

  for (node = 0; node < size; node++) {
    for (i = 0; i < (*nodes)[node]->size; i++) {
      chld = *((*(*nodes)[node]->children)[i]);
      neg = *((*(*nodes)[node]->negative_edge)[i]);
      if (!predicates[chld]->Internal() &&
	  !predicates[i]->Internal()) {
	output(sys_data.output_file, "%s(%s,%s).\n",
		neg? "neg" : "pos",
		predicates[node]->Name(),
		predicates[chld]->Name());
      }
    }	
  }
}
