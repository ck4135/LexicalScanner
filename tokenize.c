/// File: tokenize.c
/// Author: chansung kim
///         ck4135
///
/// Tokenize.c takes in a tm file and builds a transition matrix using the data and information given
#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "classes.h"
#include "scanner.h"

const char delim[] = " \t\n";   // string of delimiters 
int nstates;                    // number of states

/// 
/// Build_matrix given a file, opens the file and builds and returns the transition matrix
///
///@param filename = name of tm file
///@return completed transition matrix
state **build_matrix(const char *filename) {
    // open file
    FILE *fp = fopen(filename, "r");
    // check to make sure the open succeeded
    if (fp == NULL) {
        perror(filename);
        exit(EXIT_FAILURE);
    }

    int data[3];                // first 3 keyword lines
    char* token;                
    char* ptr;
    char* hash = "#";
    size_t BUFLENGTH = 255;
    char discard = 'd';

    // read and process the 'states', 'start', and 'accept' lines
    for (int i=0; i<3; i++) {
        getline(&ptr, &BUFLENGTH, fp);
        token = strtok(ptr, delim);
        if (*token == *hash) {
            i--;
            continue;
        } else {
            token = strtok(NULL, delim);
            data[i] = atoi(token);
        }
    }

    // declaring the transition matrix
    nstates = data[0];
    state **matrix;
    matrix = malloc( nstates * NUM_CLASSES * sizeof(state) );

    for (int i=0; i<nstates * NUM_CLASSES; i++) {
        matrix[i] = malloc( sizeof(state) );
    }

    // filling in matrix to be empty

    for (int i=0; i<nstates; i++) {
        for (int j=0; j<NUM_CLASSES; j++) {
            matrix[i * NUM_CLASSES + j]->transition = -1;
            matrix[i * NUM_CLASSES + j]->action = discard;
            matrix[i * NUM_CLASSES + j]->status = NORMAL;
        }
    }
    
    // setting starting and accepting states
    for (int i=0; i<NUM_CLASSES; i++) {
        matrix[data[1] * NUM_CLASSES + i]->status = STARTING;
        matrix[data[2] * NUM_CLASSES + i]->status = ACCEPTING;
    }

    // read and process all remaining lines from file
    while (getline(&ptr, &BUFLENGTH, fp) != -1) {
        token = strtok(ptr, delim);
        if (*token == *hash) {
            continue;
        }
        int curr = atoi(token); // current state

        // setting number of state in matrix and start/accepting states
        //for (int i=0; i<NUMCLASSES; i++) {
        //    
        //}
        token = strtok(NULL, delim);

        while (token != NULL) {
            char action;               // char representing action
            int class, transition;     // ints representing character class and transition 
            sscanf(token, "%d %*[/] %d %c", &class, &transition, &action);
            matrix[curr * NUM_CLASSES + class]->transition = (int)transition;
            matrix[curr * NUM_CLASSES + class]->action = (char)action;
            token = strtok(NULL, delim);
        }
    }

    // closes the file
    fclose(fp);

    return(matrix);
}

/// Frees the matrix and deallocates the memory used
///
/// @param **matrix = given transition matrix
void free_matrix( state **matrix ) {
    if (matrix == NULL) {
        return;
    }

    for (int i=0; i<nstates; i++) {
        for (int j=0; j<NUM_CLASSES; j++) {
            free(matrix[i * NUM_CLASSES + j]);
        }
    }

    free(matrix);
}

/// Prints the matrix in the correct format
///
///@param **matrix = transition matrix
void print_matrix( state **matrix ) {
    // prints the header lines
    printf("Scanning using the following matrix:\n ");
    for (int header=0; header<NUM_CLASSES; header++) { 
        if (header < 10) {
            printf("    %d", header);
        } 
        else {
            printf("   %d", header);
        }
    }
    printf("\n");

    // prints the transition matrix with row headers
    for (int i=0; i<nstates; i++) {
        if (i < 10) {
            printf(" %d", i);
        } else {
            printf("%d", i);
        }
        for (int j=0; j<NUM_CLASSES; j++) {
            if (matrix[i * NUM_CLASSES + j]->transition == -1 || matrix[i * NUM_CLASSES + j]->transition > 9) {
                printf("  %d%c", matrix[i * NUM_CLASSES + j]->transition, matrix[i* NUM_CLASSES + j]->action);
            } else {
                printf("   %d%c", matrix[i * NUM_CLASSES + j]->transition, matrix[i* NUM_CLASSES + j]->action);
            }
        }
        printf("\n");
    }
}

int main( int argc, char *argv[] ) {
    ///checks if the number of arguments input when calling file is proper
    if (argc != 2) {
        fputs("usage: ./tokenize tmfile\n", stderr);
        exit(1);
    }

    state **matrix;
    matrix = build_matrix(argv[1]);
    print_matrix(matrix);
    process(matrix, nstates);

    free(matrix);

    return(0);
}
