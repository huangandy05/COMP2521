// Dijkstra API implementation
// COMP2521 Assignment 2

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "Dijkstra.h"
#include "Graph.h"
#include "PQ.h"

ShortestPaths dijkstra(Graph g, Vertex src) {
	// Initialise ShortestPaths variables
	int nV = GraphNumVertices(g);
	int *dist = malloc(sizeof (int *) * nV);
	PredNode **pred = malloc(nV * sizeof (*pred));

	// Initialise all dist to infinity
	for (int i = 0; i < nV; i++) {
		dist[i] = INFINITY;
	}

	// Set dist[source] = 0
	dist[src] = 0;

	// Initialise all pred to NULL
	for (int i = 0; i < nV; i++) {
		pred[i] = NULL;
	}

	// Place all vertices of graph in set
	PQ vSet =  PQNew();
	for (int i = 0; i < nV; i++) {
		PQInsert(vSet, i, dist[i]);
	}

	while (!PQIsEmpty(vSet)) {
		// Find v with minimum dist & remove from vSet
		Vertex v = PQDequeue(vSet);
		// Condition that min distance is INF
		if (dist[v] == INFINITY) {
			continue;
		}

		for (AdjList curr = GraphOutIncident(g, v); curr != NULL; curr = curr->next) {
			// Calculate distance from src to curr
			int srcDist = dist[v] + curr->weight;

			if (srcDist < dist[curr->v]) {
				PQUpdate(vSet, curr->v, srcDist);
				dist[curr->v] = srcDist;
				// Shorter path is found, free all predecessors and insert into pred array
				PredNode *predCurr = pred[curr->v];
				while (predCurr != NULL) {
					PredNode *next = predCurr->next;
					free(predCurr);
					predCurr = next;
				}
				PredNode *new = malloc(sizeof (*new));
				new->v = v;
				new->next = NULL;
				pred[curr->v] = new;

			} else if (srcDist == dist[curr->v]) {
				// Another shortest path is found, add to predecessors array
				PredNode *new = malloc(sizeof (*new));
				new->v = v;
				new->next = NULL;
				// Insert at end of list;
				PredNode *predCurr = pred[curr->v];
				while (predCurr->next != NULL) {
					predCurr = predCurr->next;
				}
				predCurr->next = new;
			}
		}
	}
	PQFree(vSet);
	ShortestPaths sps = {nV, src, dist, pred};
	return sps;
}

void showShortestPaths(ShortestPaths sps) {

}

void freeShortestPaths(ShortestPaths sps) {
	// Free Distance Array
	free(sps.dist);

	// Free Pred Array
	for (int i = 0; i < sps.numNodes; i++) {
		PredNode *curr = sps.pred[i];
		while (curr != NULL) {
			PredNode *next = curr->next;
			free(curr);
			curr = next;
		}
	}
	free(sps.pred);
}

