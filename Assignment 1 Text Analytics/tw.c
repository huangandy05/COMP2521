// COMP2521 21T2 Assignment 1
// tw.c ... compute top N most frequent words in file F
// Usage: ./tw [Nwords] File

// Written by Andy Huang (z5311916) starting 7/07/21
// References
// https://www.tutorialspoint.com/
// The above website was used to help understand functions including:
// qsort (in Dict.c), strtok, strncmp
// All other code was written by myself 


#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Dict.h"
#include "stemmer.h"
#include "WFreq.h"

#define MAXLINE 1000
#define MAXWORD 100

#define STOPWORDS	"stopwords"
#define START	"*** START OF"
#define END		"*** END OF"

#define isWordChar(c) (isalnum(c) || (c) == '\'' || (c) == '-')

// Function prototypes
void tokenise_and_normalise(char string[]);


int main(int argc, char *argv[]) {
	int   nWords;    		// number of top frequency words to show
	char *fileName;  		// name of file containing book text
	
	char line[MAXLINE + 1];	// line to scanned
	Dict sw_dict;			// dictionary to store stopwords
	Dict freq_Dict;			// dictionary to store words from text
	FILE *fp;				// Currently opened file

	// process command-line args
	switch (argc) {
		case 2:
			nWords = 10;
			fileName = argv[1];
			break;
		case 3:
			nWords = atoi(argv[1]);
			if (nWords < 10) nWords = 10;
			fileName = argv[2];
			break;
		default:
			fprintf(stderr,"Usage: %s [Nwords] File\n", argv[0]);
			exit(EXIT_FAILURE);
	}

	// Place stopwords into a dictionary
	sw_dict = DictNew();
	fp = fopen(STOPWORDS, "r");
	if (fp == NULL) {
		fprintf(stderr, "Can't open stopwords\n");
		exit(EXIT_FAILURE);
	}
	while (fgets(line, MAXLINE + 1, fp) != NULL) {
		char *w = strtok(line, "\n");
		DictInsert(sw_dict, w);
	}
	fclose(fp);
	
	// Open book file
	fp = fopen(fileName, "r");
	if (fp == NULL) {
		fprintf(stderr, "Can't open %s\n", fileName);
		exit(EXIT_FAILURE);
	}

	// Skip lines until 'START OF'
	bool contains_start = false;
	while (fgets(line, MAXLINE + 1, fp) != NULL) {
		if (strncmp(line, START, strlen(START)) == 0){
			contains_start = true;
			break;
		}
	}

	// Check if file contains "*** START OF"
	if (contains_start == false) {
		fprintf(stderr, "Not a Project Gutenberg book\n");
		exit(EXIT_FAILURE);
	}

	// Scan words in text, process them and place into freq_Dict
	freq_Dict = DictNew();
	bool contains_end = false;
	while (fgets(line, MAXLINE + 1, fp) != NULL) {
		// Check if file contains "*** END OF"
		if (strncmp(line, END, strlen(END)) == 0) {
			contains_end = true;
			break;
		}

		// Tokenise and normalise current line
		tokenise_and_normalise(line);
		
		// Finish process words and place in dict
		char delim[] = " \n";
		for (char *token = strtok(line, delim); token != NULL; token = strtok(NULL, delim)) {
			// Discard single-character tokens and stopwords
			if (strlen(token) > 1 && DictFind(sw_dict, token) == 0) {
				// Stem word
				stem(token, 0, strlen(token) - 1);
				// Place in dictionary
				DictInsert(freq_Dict, token);
			}
		}
	}
	fclose(fp);

	// Check for "END OF"
	if (contains_end == false) {
		fprintf(stderr, "Not a Project Gutenberg book\n");
		exit(EXIT_FAILURE);	
	}

	// Print out most frequent words
	WFreq *top_words = malloc(nWords * sizeof (WFreq));
	int num = DictFindTopN(freq_Dict, top_words, nWords);
	for (int i = 0; i < num; i++) {
		printf("%d %s\n", top_words[i].freq, top_words[i].word);
	}

	// Free all memory
	free(top_words);
	DictFree(freq_Dict);
	DictFree(sw_dict);

	return EXIT_SUCCESS;
}


// Tokenises and normalises an array
// Inputs an array
// Outputs an array with non-word characters as space and word characters as lower case
void tokenise_and_normalise(char string[]) {
	for (int i = 0; i < strlen(string); i++) {
		if (isWordChar(string[i]) == 1) {
			// Normalise
			string[i] = tolower(string[i]);
		} else {
			string[i] = ' ';
		}
	}
}
