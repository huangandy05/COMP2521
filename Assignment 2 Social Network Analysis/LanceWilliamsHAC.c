// Lance-Williams Algorithm for Hierarchical Agglomerative Clustering
// COMP2521 Assignment 2

#include <assert.h>
#include <float.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include "Graph.h"
#include "LanceWilliamsHAC.h"

#define INFINITY DBL_MAX

// Struct to store a pair of numbers
// This will be used to check which two clusters have the samllest distance
// The value will represent the positions in the linked list or row and column in table
typedef struct Pair {
	int x;
	int y;
} Pair;

typedef struct ListRep *List;

// Structs for linked list
struct Node {
	Dendrogram data;
	struct Node *next;
};

struct ListRep {
	struct Node *first;
	struct Node *last;
	int size;
};

// Helper Functions //
static double LWFormula(double ik, double jk, int method);
static Pair findSmallestDist(double **array, int size);
static void free2dArray(double **array, int size);
// Linked List Functions
static List newList(void);
static struct Node *newListNode(Dendrogram d);
static void listAppend(List l, Dendrogram d);
static Dendrogram connectTwoNodes(List l, int first, int second);


/**
 * Generates  a Dendrogram using the Lance-Williams algorithm (discussed
 * in the spec) for the given graph  g  and  the  specified  method  for
 * agglomerative  clustering. The method can be either SINGLE_LINKAGE or
 * COMPLETE_LINKAGE (you only need to implement these two methods).
 * 
 * The function returns a 'Dendrogram' structure.
 */
Dendrogram LanceWilliamsHAC(Graph g, int method) {

	int nV = GraphNumVertices(g);

	// Create distances array and place all dist values inside
	// As clusters decrease, size of table won't change but less rows.columns will be used
	double **distArray = malloc(nV * sizeof(*distArray));
	for (int i = 0; i < nV; i++) {
		distArray[i] = calloc(nV, sizeof(*distArray[i]));
	}
	// Temporary Table to store values from distArray as it is getting updated
	double **tempTable = malloc(nV * sizeof(*tempTable));
	for (int i = 0; i < nV; i++) {
		tempTable[i] = calloc(nV, sizeof(*tempTable[i]));
	}

	// Set all values in array to infinity
	// Does not initilaise when i == j
	for (int i = 0; i < nV; i++) {
		for (int j = 0; j < nV; j++) {
			if (i != j) {
				distArray[i][j] = DBL_MAX;
			}
		}
	}

	// Calculate distance between each node and place in array
	// distArray values will be replaced smaller distance is found
	for (int i = 0; i < nV; i++) {
		// Edges going out
		for (AdjList curr = GraphOutIncident(g, i); curr != NULL; curr = curr->next) {
			// edge is smaller
			double edgeL = 1.0 / curr->weight;
			if (edgeL < distArray[i][curr->v]) {
				distArray[i][curr->v] = edgeL;
				distArray[curr->v][i] = edgeL;
			}
		}

		// Edges going in
		for (AdjList curr = GraphInIncident(g, i); curr != NULL; curr = curr->next) {
			// edge is smaller
			double edgeL = 1.0 / curr->weight;
			if (edgeL < distArray[i][curr->v]) {
				distArray[i][curr->v] = edgeL;
				distArray[curr->v][i] = edgeL;
			}
		}
	}


	// Create linked list containing starting clusters
	List l = newList();
	for (int i = 0; i < nV; i++) {
		Dendrogram d = malloc(sizeof (DNode));
		d->vertex = i;
		d->left = NULL;
		d->right = NULL;
		listAppend(l, d);
	}

	// Join Dendrograms until one node remains
	while (l->size > 1) {
		// Find pair of clusters with smallest distance
		Pair p = findSmallestDist(distArray, l->size);

		// Transfer values from one table to another
		for (int i = 0; i < l->size; i++) {
			for (int j = 0; j < l->size; j++) {
				if (i != j) {
					tempTable[i][j] = distArray[i][j];
				}
			}
		}


		// Remove the pairs of clusters from linked list and return decdogram containing those clusters
		Dendrogram d = connectTwoNodes(l, p.x, p.y);

		// Add new cluster to list, size of list would have decreased by one
		listAppend(l, d);

		// Update table, with new distances
		// The rows and columns containing joined clusters are essentially removed
		// This is done by shifting all other row/columns up or left by required amount
		for (int i = 0; i <= l->size; i++) {
			for (int j = 0; j <= l->size; j++) {
				if (i != p.x && i != p.y && j != p.x && j != p.y && i != j) {
					int shiftLeft = ((j > p.x) + (j > p.y));
					int shiftUp = ((i > p.x) + (i > p.y));
					distArray[i - shiftUp][j - shiftLeft] = distArray[i][j];
				}
			}
		}


		// Add last row and column containing distances from new cluster
		for (int i = 0; i <= l->size; i++) {
			if (i != p.x && i != p.y) {
				int shift = ((i > p.x) + (i > p.y));
				distArray[l->size - 1][i - shift] = LWFormula(tempTable[p.x][i], tempTable[p.y][i], method);
				distArray[i - shift][l->size - 1] = LWFormula(tempTable[p.x][i], tempTable[p.y][i], method);
			}
		}	
	}

	// There is one remaining cluster in the linked list- the final Dendrogram!!!
	Dendrogram finalD = l->first->data;

	// Free linked list and distance tables
	free(l->first);
	free(l);
	free2dArray(distArray, nV);
	free2dArray(tempTable, nV);

	// Return final dendrogram
	return finalD;
}

/**
 * Frees all memory associated with the given Dendrogram structure.
 */
void freeDendrogram(Dendrogram d) {
	if (d == NULL) {
		return;
	}
	freeDendrogram(d->right);
	freeDendrogram(d->left);
	free(d);
}

//////////////////////
// Helper Functions //
//////////////////////

// Returns the Lance-Williams formula distance between cij and ck
// Assumes if method is not single linkage, its complete linkage
static double LWFormula(double ik, double jk, int method) {

	if (method == SINGLE_LINKAGE) {
		if (ik < jk) return ik;
		return jk;
	} else {
		if (ik > jk) return ik;
		return jk;
	}
}

// Return pair of numbers with smallest distance in 2d array of a given size
static Pair findSmallestDist(double **array, int size) {
	Pair p;
	double smallest = INFINITY;
	for (int i = 0; i < size; i++) {
		for (int j = i + 1; j < size; j++) {
			if (array[i][j] < smallest) {
				smallest = array[i][j];
				p.x = i;
				p.y = j;
			}
		}
	}
	// If all values in table are infinity, join first two nodes in linked list
	if (smallest == INFINITY) {
		p.x = 0;
		p.y = 1;
	}
	return p;
}

// Free a 2D array
static void free2dArray(double **array, int size) {
	for (int i = 0; i < size; i++) {
		free(array[i]);
	}
	free(array);
}


///////////////////////////////
// Functions for Linked List //
///////////////////////////////

// Creates a new list containing Dendrograms as data values
// Has a pointer to first and last value
static List newList(void) {
	struct ListRep *l = malloc(sizeof(*l));
	l->size = 0;
	l->first = NULL;
	l->last = NULL;
	return l;
}

// Creates a node given a Dendrogram
static struct Node *newListNode(Dendrogram d) {
	struct Node *n = malloc(sizeof(*n));
	n->data = d;
	n->next = NULL;
	return n;
}

// Places a Dendrogram at the end of the linked list
static void listAppend(List l, Dendrogram d) {
	struct Node *n = newListNode(d);
	if (l->first == NULL) {
		l->first = l->last = n;
	} else {
		l->last->next = n;
		l->last = n;
	}
	l->size++;
}

// Returns a Dendrogram which has two children as the two values specified in the position of list
// Assumes that first cluster number is smaller than second cluster number
static Dendrogram connectTwoNodes(List l, int first, int second) {
	// Create dendrogram to store removed clusters
	Dendrogram d = malloc(sizeof (DNode));

	// Connect the first child
	struct Node *curr = l->first;
	for (int i = 0; i < first; i++) {
		curr = curr->next;
	}
	d->left = curr->data;
	// Connect second child
	curr = l->first;
	for (int i = 0; i < second; i++) {
		curr = curr->next;
	}
	d->right = curr->data;

	// Remove and free the second cluster
	curr = l->first;
	struct Node *prev;
	for (int i = 0; i < second; i++) {
		prev = curr;
		curr = curr->next;
	}
	prev->next = curr->next;
	free(curr);
	// Check if prev is last node in linked list
	if (prev->next == NULL) {
		l->last = prev;
	}

	// Remove and free the first cluster
	curr = l->first;
	if (first == 0) {
		l->first = curr->next;
		free(curr);
	} else {
		for (int i = 0; i < first; i++) {
			prev = curr;
			curr = curr->next;
		}
		prev->next = curr->next;
		free(curr);
		if (prev->next == NULL) {
			l->last = prev;
		}
	}
	// Update size of linked list
	l->size = l->size - 2;

	return d;
}