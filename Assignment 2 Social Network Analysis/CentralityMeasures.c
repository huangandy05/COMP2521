// Centrality Measures API implementation
// COMP2521 Assignment 2

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "CentralityMeasures.h"
#include "Dijkstra.h"
#include "Graph.h"
#include "PQ.h"

// Helper Functions
static double WassermanFaustFormula(int n, int nV, int total);
static int pathsfromTtoS(PredNode **pred, Vertex t, Vertex s);

NodeValues closenessCentrality(Graph g) {
	int nV = GraphNumVertices(g);
	double *values = malloc (nV * sizeof (double *));

	// Find closeness centrality of each vertex
	for (int i = 0; i < nV; i++) {
		ShortestPaths sps = dijkstra(g, i);
		// Find Number of reachable nodes (n) and sum of all reachable distances 
		int n = 0;
		int total = 0; 
		for (int j = 0; j < nV; j++) {
			if (sps.dist[j] != INFINITY) {
				n++;
				total += sps.dist[j];
			}
		}
		// Use Wasserman and Faust Formula
		if (n == 1) {
			values[i] = 0; // if not connected to any other node
		} else {
			values[i] = WassermanFaustFormula(n, nV, total);
		}
		freeShortestPaths(sps);
	}

	NodeValues nvs = {nV, values};
	return nvs;
}

// Returns value given by Wasserman and Faust formula
static double WassermanFaustFormula(int n, int nV, int total) {
	double x = (n - 1) * (n - 1);
	x = x / ((nV - 1) * total);
	return x;
}

NodeValues betweennessCentrality(Graph g) {
	int nV = GraphNumVertices(g);

	// Create array to add and store betweenness centrality
	double *values = malloc (nV * sizeof (double *));
	// Set all values in array to 0
	for (int i = 0; i < nV; i++) {
		values[i] = 0;
	}

	for (int s = 0; s < nV; s++) {
		ShortestPaths sps = dijkstra(g, s);
		for (int t = 0; t < nV; t++) {
			// s != t and t must be accessible 
			if (t == s || sps.dist[t] == INFINITY) {
				continue;
			}
			for (int v = 0; v < nV; v++) {
				// Condition that v != t and v != s
				if (v == t || v == s) {
					continue;
				}
				// Calculate the total paths from t to s
				int totalPs = pathsfromTtoS(sps.pred, t, s);
				// The total paths passing through v will be the product of number of
				// paths from t to v by number of paths from v to s
				int pathsTtoV = pathsfromTtoS(sps.pred, t, v);
				int pathsVtoS = pathsfromTtoS(sps.pred, v, s);

				double fraction = pathsTtoV * pathsVtoS;
				fraction = fraction / totalPs;

				values[v] += fraction;
			}
		}
		freeShortestPaths(sps);
	}

	NodeValues nvs = {nV, values};
	return nvs;
}

// Recursive formula to calculate number of (shortest) paths from target node to source nodes
static int pathsfromTtoS(PredNode **pred, Vertex t, Vertex s) {
	if (t == s) {
		return 1;
	}
	int total = 0;
	// PredNode to iterate thorugh array
	PredNode *curr = pred[t];
	while (curr != NULL) {
		total += pathsfromTtoS(pred, curr->v, s);
		curr = curr->next;
	}
	return total;
}

NodeValues betweennessCentralityNormalised(Graph g) {
	int nV = GraphNumVertices(g);
	double *values = malloc (nV * sizeof (double *));

	NodeValues bCentrality = betweennessCentrality(g);

	for (int i = 0; i < nV; i++) {
		values[i] = bCentrality.values[i] / ((nV - 1) * (nV - 2));
	}

	freeNodeValues(bCentrality);

	NodeValues nvs = {nV, values};
	return nvs;
}

void showNodeValues(NodeValues nvs) {

}

void freeNodeValues(NodeValues nvs) {
	free(nvs.values);
}

