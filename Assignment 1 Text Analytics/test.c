// COMP2521 21T2 Assignment 1
// tw.c ... compute top N most frequent words in file F
// Usage: ./tw [Nwords] File

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Dict.h"
#include "stemmer.h"
#include "WFreq.h"

int main (void) {
    char word;
    Dict d = DictNew();
    DictInsert(d, "hi");
    int x = DictFind(d, "hi");
    printf("%d", x);
}