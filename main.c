#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#define MAXWORDLEN 20
#define MINSOLUTIONLEN 3

char grid[4][4];

char** dictWords;
char** solutions;
int numSolutions = 0;

typedef struct gridPoint {
    int row, col;
}gridPoint;

bool isGridPointEqual(gridPoint a, gridPoint b) {
    if(a.row == b.row && a.col == b.col)
        return true;
    else
        return false;
}

void initializeGrid() {
    //get grid from text file and put it into a double array
    memset(grid, 0, 16 * sizeof(char));

    FILE* fp = fopen("./grid.txt", "r");
    assert(fp != NULL && "Could not open grid file!");

    char c = (char) fgetc(fp);

    for(int i = 0; i < 5; i++) {
        for(int j = 0; j < 5; j++) {
            if(c != '\n' && j < 4 && i < 4) {
                grid[i][j] = (char) toupper(c);
            }
            c = (char) fgetc(fp);
        }
    }
    fclose(fp);
}

void printGrid() {
    printf("----------\n");

    for(int i = 0; i < 4; i++) {
        putchar('-');
        for(int j = 0; j < 4; j++) {
            printf("%c", grid[i][j]);
            if(j != 3) putchar(' ');
        }
        printf("-\n");
    }

    printf("----------\n");
}

void printGridNums() {
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            printf("i: %d\tj: %d\t%c\n", i, j, grid[i][j]);
        }
    }
}

int charOccurencesInWord(char* word, char toFind) {
    int occurences = 0;
    for(int i = 0; i < strlen(word); i++) {
        if(word[i] == toFind) occurences++;
    }
    return occurences;
}

int charOccurencesInGrid(char toFind) {
    int occurences = 0;
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            if(grid[i][j] == toFind) occurences++;
        }
    }
    return occurences;
}

bool findCharInGrid(char toFind, gridPoint* point, int occurence) {
    assert(occurence > 0);
    assert(occurence < 17);
    assert(occurence <= charOccurencesInGrid(toFind));
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            if(grid[i][j] == toFind) {
                if(occurence != 1){
                    occurence--;
                    continue;
                }
                point->row = i;
                point->col = j;
                return true;
            }
        }
    }
    point->row = -1;
    point->col = -1;
    return false;
}

void countCharsInFile(char* path, int* numChars, int* numLines) {
    *numChars = 0;
    *numLines = 0;

    //TODO: check if path is valid
    FILE* fp = fopen(path, "r");
    assert(fp != NULL && "dictionary could not be opened!");

    char c = (char) fgetc(fp);

    while(c != EOF) {
        (*numChars)++;
        if(c == '\n') (*numLines)++;
        c = (char) fgetc(fp);
    }
    (*numLines)++;

    fclose(fp);
}

int getDictionary(char* path) {
    //TODO: only allocate words that have enough letters in the grid
    int numStrings = 0;
    int lineLength = 0;

    //TODO: make sure path is valid before passing to fopen
    FILE* fp = fopen(path, "r");
    assert(fp != NULL && "Dictionary could not be opened!");

    char line[MAXWORDLEN];

    while(fgets(line, MAXWORDLEN, fp) != NULL) {
        lineLength = (int) strlen(line);

        if(line[lineLength - 1] == '\n') //remove newline character at the end of a word
            line[--lineLength] = '\0';
        //Needed for the strange Collins Scrabble Words (2019).txt because it has carriage returns
        if(line[lineLength - 1] == '\r') //remove carriage return character at the end of a word
            line[--lineLength] = '\0';

        //dictWords[numStrings] = strndup(line, lineLength);
        dictWords[numStrings] = strdup(line);
        assert(dictWords[numStrings] != NULL && "Error allocating memory for words in dictionary!");

        numStrings++;
    }

    fclose(fp);
    return numStrings;
}

void freeDict(int numStrings) {
    for(int i = 0; i < numStrings; i++) {
        free(dictWords[i]);
    }
    free(dictWords);
}

//unused
bool areCharsNeigbors(char start, int startOccurence, char dest, int destOccurence) {
    int startRow, startCol, destRow, destCol;
    gridPoint startPoint = {};

    if(startOccurence < 1) return false; //TODO:change to assert

    if(destOccurence < 1) return false; //TODO:change to assert

    if(!findCharInGrid(start, &startPoint, startOccurence)) return false;
    if(!findCharInGrid(dest, &startPoint, destOccurence)) return false;

    int rowDist = abs(startRow - destRow);
    int colDist = abs(startCol - destCol);

    if(rowDist <= 1 && colDist <= 1) return true;

    return true;
}

//unused
int getNumChildren(int row, int col) {
    int numChildren = 0;
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            if(row == i && col == j) continue;
            int rowDist = abs(row - i);
            int colDist = abs(col - j);

            if(rowDist <= 1 && colDist <= 1) {
                numChildren++;
            }
        }
    }

    return numChildren;
}

int getChildren(gridPoint point, char** children, gridPoint** points) {
    assert(point.row >= 0);
    assert(point.row < 4);
    assert(point.col >= 0);
    assert(point.col < 4);

    int numChildren = 0;

    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            if(point.row == i && point.col == j) continue;
            int rowDist = abs(point.row - i);
            int colDist = abs(point.col - j);

            if(rowDist <= 1 && colDist <= 1) {
                (*points)[numChildren].row = i;
                (*points)[numChildren].col = j;
                (*children)[numChildren] = grid[i][j];
                numChildren++;
            }
        }
    }

    assert(numChildren < 9);

    return numChildren;
}

//unused
int filterChildren(char toComp, gridPoint* points, int numChildren, gridPoint** retChildPoints) {
    assert(numChildren > 0);
    int retNumChildren = 0;
    for(int i = 0; i < numChildren; i++) {
        if(grid[points[i].row][points[i].col] == toComp) {
            (*retChildPoints)[retNumChildren] = points[i];
            retNumChildren++;
        }
    }

    return retNumChildren;
}

//unused
gridPoint* getUntraversedPoints(gridPoint* path, int pathLen, gridPoint* childPoints, int numChildren, int* returnUntraversedPoints) {
    int uniquePointsIndex = 0;
    gridPoint* untraversedPoints = malloc(10 * sizeof(gridPoint));
    //gridPoint* uniquePoints = malloc(16 * sizeof(gridPoint));
    for(int i = 0; i <= pathLen; i++) {// iterate through path and check that the child has not been traversed through
        for(int j = 0; j < numChildren; j++) {
            if(path[i].row != childPoints[j].row && path[i].col != childPoints[j].col) {
                untraversedPoints[uniquePointsIndex].row = path[i].row;
                untraversedPoints[uniquePointsIndex].col = path[i].col;
                uniquePointsIndex++;
            }
        }
    }
    *returnUntraversedPoints = uniquePointsIndex;
    return untraversedPoints;
}

bool recursiveSolve(char* word, gridPoint point, int counter, gridPoint* path) {
    //FIXED (I think) Make sure it cannot repeat letters
    //FIXED (I think) EORL should not be possible or ENES. When there are many of one character it messes up
    //FIXED (I think) EGER should not be possible
    //TODO: This does not check all possibilities if there are numerous children that are matching letters
    // (what I am trying to fix with the path variable) SEG is not possible when it should be
    //TODO: BISES doesn't work
    //TODO: ISOLEXES
    //TODO: SEG shows up as not possible when it should be
    int numChildren = 0;
    bool firstCalled = false;
    if(counter == strlen(word) - 1){
        return true;
    }

    if(path == NULL) {
        path = malloc(17 * sizeof(gridPoint));
        assert(path != NULL);
        path[0] = point;
        firstCalled = true;
    }

    char* children = (char*) calloc(10, sizeof(char));
    assert(children != NULL);
    gridPoint* childPoints = (gridPoint*) calloc(10, sizeof(gridPoint));
    assert(childPoints != NULL);

    numChildren = getChildren(point, &children, &childPoints);

    //make a for loop to iterate through each occurrence of a letter
    for(int i = 0; i < numChildren; i++) {
        assert(counter >= 0);

        if(grid[childPoints[i].row][childPoints[i].col] == word[counter+1]) {
            for(int j = 0; j <= counter; j++) {
                //if(path[j].row == childPoints[i].row && path[j].col == childPoints[i].col) goto nextIteration;
                if(isGridPointEqual(path[j], childPoints[i])) goto nextIteration;
            }
            counter++;
            path[counter] = childPoints[i];
            if(recursiveSolve(word, childPoints[i], counter, path)) {
                if(firstCalled) free(path);
                free(children);
                free(childPoints);
                return true;
            } else {
                //TODO: make the function backtrack and check children of previous point on the path
                path[counter] = (gridPoint) {-1, -1};
                counter--;
            }
        }
        nextIteration:
        continue;
    }

    //noSolution
    if(firstCalled) free(path);
    free(children);
    free(childPoints);

    return false;
}

bool isWordPossible(char* word) {
    int startRow, startCol, curRow, curCol;
    gridPoint startPoint = {};
    if(strlen(word) < MINSOLUTIONLEN) return false;

    //return false if letter(s) in word are not in grid
    for(int i = 0; i < strlen(word); i++) {
        if(charOccurencesInGrid(word[i]) < 1) return false;
        if(charOccurencesInGrid(word[i]) < charOccurencesInWord(word, word[i])) return false;
    }

    if(!findCharInGrid(word[0], &startPoint, 1)) return false;

    for(int i = 0; i < charOccurencesInGrid(word[0]); i++) {
        int tempCounter = 0;
        gridPoint solvePoint = {};
        findCharInGrid(word[0], &solvePoint, i+1);
        if(recursiveSolve(word, solvePoint, tempCounter, NULL))
            return true;
    }

    return false;
}

int sortCompare(const void* string1, const void* string2) {
    const char* a = *(const char**) string1;
    const char* b = *(const char**) string2;
    return (int) (strlen(a) - strlen(b));
}

void printPuzzle() {
    qsort(solutions, numSolutions, sizeof(*solutions), sortCompare);
    for(int i = 0; i < numSolutions; i++) {
        printf("%s\n", solutions[i]);
    }
}

void freeSolutions() {
    for(int i = 0; i < numSolutions; i++) {
        free(solutions[i]);
    }
    free(solutions);
}

void solvePuzzle(int linesInDict) {
    numSolutions = 0;
    int solutionsIndex = 0;
    for(int i = 0; i < linesInDict; i++) {
        if(isWordPossible(dictWords[i])) {
            numSolutions++;
            solutions = realloc(solutions, numSolutions * sizeof(char*));
            //solutions[solutionsIndex] = (char*) malloc(20 * sizeof(char));
            //strcpy(solutions[solutionsIndex], dictWords[i]);
            solutions[solutionsIndex] = strdup(dictWords[i]);
            assert(solutions[solutionsIndex] != NULL && "Could not allocate enough memory for solutions!");
            solutionsIndex++;
        }
    }
}

int main(int argc, char** argv) {

    //TODO: Get file path for grid and dictionary from command line arguments
    //get lines and characters in dictionary text file
    int charsInDict = 0, linesInDict = 0;
    countCharsInFile("./colins-scrabble-dictionary.txt", &charsInDict, &linesInDict);
    //countCharsInFile("./collins-scrabble-words-2019.txt", &charsInDict, &linesInDict);

    //TODO: Use realloc in getDictionary() and only allocate words that have enough letters in the grid
    //put words from dictionary text file into a string array, char**
    dictWords = (char**) malloc(linesInDict * sizeof(char*));
    assert(dictWords != NULL && "Error allocating memory for initialization of dictionary!");

    int numStrings = getDictionary("./colins-scrabble-dictionary.txt");
    //int numStrings = getDictionary("./collins-scrabble-words-2019.txt");

    printf("numStrings: %d\tlinesInDict: %d\n", numStrings, linesInDict);

    initializeGrid();

    /*if(isWordPossible("SEG")) {
        printf("Possible!\n");
    } else {
        printf("Not possible!\n");
    }*/

    solutions = (char**) malloc(sizeof(char*));
    solvePuzzle(linesInDict);
    printPuzzle();
    printf("Solutions: %d", numSolutions);

    freeSolutions();
    freeDict(numStrings);

    return 0;
}