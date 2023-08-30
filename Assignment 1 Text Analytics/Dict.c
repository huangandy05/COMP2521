// COMP2521 21T2 Assignment 1
// Dict.c ... implementation of the Dictionary ADT

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Dict.h"
#include "WFreq.h"

// you may define your own structs here
typedef struct Node *Link;

struct Node {
	WFreq data;
	Link left;
	Link right;
	int height;
};

struct DictRep {
	Link root;
	int nodeCount;
};

// Function Prototypes
void freeTree(Link t);
Link AVLInsert(Link t, char *word);
Link newNode(char *word);
int TreeFind(Link t, char *word);
int height(Link t);
Link rotateRight(Link t);
Link rotateLeft(Link t);
int max(int a, int b);
int min(int a, int b);
int compare(const void *a, const void *b);
void TreeToArray(WFreq *list, Link node, int *size);

// Creates a new Dictionary
Dict DictNew(void) {
	Dict new = malloc(sizeof(*new));
	assert(new != NULL);
	new->root = NULL;
	new->nodeCount = 0;
	return new;
}

// Frees the given Dictionary
void DictFree(Dict d) {
	if (d == NULL) {
		return;
	}
	freeTree(d->root);
	free(d);
}

// Inserts an occurrence of the given word into the Dictionary
void DictInsert(Dict d, char *word) {
	// Dictionary
	if (DictFind(d, word) == 0) {
		d->nodeCount++;
	}
	d->root = AVLInsert(d->root, word);
}

// Returns the occurrence count of the given word. Returns 0 if the word
// is not in the Dictionary.
int DictFind(Dict d, char *word) {
	Link curr = d->root;
	while (curr != NULL) {
		if (strcmp(curr->data.word, word) > 0) {
			curr = curr->left;
		} else if (strcmp(curr->data.word, word) < 0) {
			curr = curr->right;
		} else { // Equal
			return curr->data.freq;
		}
	}
	return 0;
}

// Finds  the top `n` frequently occurring words in the given Dictionary
// and stores them in the given  `wfs`  array  in  decreasing  order  of
// frequency,  and then in increasing lexicographic order for words with
// the same frequency. Returns the number of WFreq's stored in the given
// array (this will be min(`n`, #words in the Dictionary)) in  case  the
// Dictionary  does  not  contain enough words to fill the entire array.
// Assumes that the `wfs` array has size `n`.
int DictFindTopN(Dict d, WFreq *wfs, int n) {
	// Number of words can't be less than nodeCount
	int nWords = min(n, d->nodeCount);
	
	// Insert data from a tree into an array with recursion
	WFreq *list = malloc(d->nodeCount * sizeof(WFreq));
	int *sizeOfDict = &(d->nodeCount);
	TreeToArray(list, d->root, sizeOfDict);

	// Sort List
	qsort(list, d->nodeCount, sizeof(WFreq), compare);

	// Place top n words into wfs array
	for (int k = 0; k < nWords; k++) {
		wfs[k] = list[k];
	}

	free(list);
	return nWords;
}

// Assumes list array is size of tree node
// Places all data in tree (starting at the root node) to an array
void TreeToArray(WFreq *list, Link node, int *size) {
	if (node == NULL) {
		return;
	}
	static int i = 0;
	if (i < *size) {
		list[i] = node->data;
		i++;
	}
	TreeToArray(list, node->left, size);
	TreeToArray(list, node->right, size);

}

// Displays the given Dictionary. This is purely for debugging purposes,
// so  you  may  display the Dictionary in any format you want.  You may
// choose not to implement this.
void DictShow(Dict d) {
	printf("%d\n", d->nodeCount);
}



// Free Tree
void freeTree(Link t) {
	if (t == NULL) {
		return;
	}
	freeTree(t->left);
	freeTree(t->right);
	free(t->data.word);
	free(t);
}

// Insert word into given tree
Link AVLInsert(Link t, char *word) {
	// Node is empty
	if (t == NULL) {
		return newNode(word);
	}

	// Insert word
	int difference = strcmp(t->data.word, word);
	if (difference > 0) {
		t->left = AVLInsert(t->left, word);
	} else if (difference < 0) {
		t->right = AVLInsert(t->right, word);
	} else { // equal
		// Increase frequency of word
		t->data.freq++;
		return t;
	}
	
	// Update height
	t->height = max(height(t->left), height(t->right)) + 1;

	// Rebalance Tree
	int LHeight = height(t->left);
	int RHeight = height(t->right);
	if ((LHeight - RHeight) > 1) {
		if (strcmp(t->left->data.word, word) < 0) {
			t->left = rotateLeft(t->left);
		}
		t = rotateRight(t);
	} else if ((RHeight - LHeight) > 1) {
		if (strcmp(t->right->data.word, word) > 0) {
			t->right = rotateRight(t->right);
		}
		t = rotateLeft(t);
	}
	return t;
}

// Create a new Node
// Recieves word and returns pointer to new node containing word
Link newNode(char *word) {
	Link new = malloc(sizeof(*new));
	// There is enough memory
	assert(new != NULL);
	// Initialise
	new->left = NULL;
	new->right = NULL;
	new->data.word = malloc((strlen(word) + 1) * sizeof(char));
	strcpy(new->data.word, word);
	new->height = 0;
	new->data.freq = 1;
	return new;
}

// Return height of tree
int height(Link t) {
	if (t == NULL) {
		return -1;
	}
	return t->height;
}

// Rotate right
Link rotateRight(Link t) {
	Link newLink = t->left;
	if (t == NULL || newLink == NULL) {
		return t;
	}
	t->left = newLink->right;
	newLink->right = t;

	// // Update Heights
	t->height = max(height(t->left), height(t->right)) + 1;
	newLink->height = max(height(newLink->left), height(newLink->right)) + 1;
	return newLink;
}

// Rotate left
Link rotateLeft(Link t) {
	Link newLink = t->right;
	if (t == NULL || newLink == NULL) {
		return t;
	}
	t->right = newLink->left;
	newLink->left = t;

	// // Update Heights
	t->height = max(height(t->left), height(t->right)) + 1;
	newLink->height = max(height(newLink->left), height(newLink->right)) + 1;
	return newLink;
}

// Utility function to return max of two numbers
int max(int a, int b) {
	if (a > b) {
		return a;
	} else {
		return b;
	}
}

// Utility function to return minimum of two numbers
int min(int a, int b) {
	if (a < b) {
		return a;
	} else {
		return b;
	}
}

// compare 2 WFreqs. return higher number, or lexigraphic order if equal
int compare(const void *a, const void *b) {
	int freq1 = ((WFreq*)a)->freq;
	int freq2 = ((WFreq*)b)->freq;
	// If same frequency, compare lexigraphic order
	if (freq1 == freq2) {
		return strcmp(((WFreq*)a)->word, ((WFreq *)b)->word);
	} else {
		return freq2 - freq1;
	}
}