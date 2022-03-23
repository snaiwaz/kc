/* kc -- Read out and search plain text files on standard output.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Differences from the GNU Coreutils cat:
 * This program is not supposed to be a replacement for cat, it aims to expand some features
 * one may find useful when operating with single files and their contents
 *
 * By snaiwaz, snaiwaz@koishi.de
 * koishi.de
 * GitHub.com/snaiwaz */

#include <stdio.h> // Standard input-output
#include <stdlib.h> // For memory management and useful macros
#include <getopt.h> // Needed for parameters and filename
#include <string.h> // For easy string operations

// Defining miscellaneous
#define PROGRAM_NAME "kc"
#define AUTHOR "snaiwaz"

// Defining buffer lengths for code readability
#define LINE_COUNTER_BUF_LEN 20
#define LINE_LENGTH_BUFFER 150
#define FILENAME_LENGTH_BUFFER 50
#define WORD_LENGTH_BUFFER 20
#define CHARACTER_COUNT_BUFFER 20000

// Globally defined character pointers
char *infile;
char *linebuffer;
char *searchterm;

// Help function
void usage () {
    printf("Usage: [OPTION]... [FILE]...\n\n"
           "\tExample use: kc -OPTION FILE\n\n"
           "\t-h for help\n"
           "\t-s to print out only lines containing a word\n"
           "\t\t Example: kc -s searchterm examplefile\n"
           "\t-l to number all lines in output\n"
           "\t-p to print out paragraphs containing a certain word\n"

           "\n\n\n%s Copyright (C) 2022 %s\n"
           "\nThis program comes with ABSOLUTELY NO WARRANTY;\n"
           "This is free software, and you are welcome to redistribute it\n"
           "under certain conditions; you may consult the license included\n"
           "with this program's source code to find out more.\n"
           , PROGRAM_NAME, AUTHOR
    );
}

void search (char * string, char * search, int linenumber) {
    int i, j, stringlen, searchlen;
    int comp = 0;
    stringlen = strlen(string);
    searchlen = strlen(search);

    // Simplifies the code required for post-word character comparison
    char postwordcharacters [] = {' ', '\t', '\n', '.', ',', '\0'};

    i = 0;
    while (i < stringlen){
        j = 0;
        while (i < stringlen && j < searchlen && string[i] == search[j]) {   // While characters are equal keep iterating
            ++i;
            ++j;
        }

        for (comp = 0; comp <= sizeof(postwordcharacters); comp++) {         // After they stop matching, compare the following character to an array to determine if it is a word-to-word match.
            if (string[i] == postwordcharacters[comp]) {                     // This is implemented to prevent false matches like 'help' and 'helpful'.
                if (j == searchlen)
                    printf("Line: %d %s", linenumber, string);
            }
        }

        while (i < stringlen && string[i] != ' ') {
            ++i;
        }
        ++i;
    }

}

void cat (FILE *fptr, char opt, char * searchterm) {
    int line = 1;

    switch (opt) {
        case ' ' :
            while (fgets(linebuffer, LINE_LENGTH_BUFFER, fptr) != NULL){
                printf("%s", linebuffer);
            }
                break;
        case 'l' :
            while (fgets(linebuffer, LINE_LENGTH_BUFFER, fptr) != NULL){
                printf("%d %s", line, linebuffer);
                ++line;
            }
                break;
        case 's' :
                while (fgets(linebuffer, LINE_LENGTH_BUFFER, fptr) != NULL){
                    search(linebuffer, searchterm, line);
                    ++line;
                }
                break;
    }

}

int main (int argc, char **argv) {
    FILE *fptr;
    int option;
    char opt = ' '; // Space representing empty by default to make code more understandable

    infile = (char *) malloc(FILENAME_LENGTH_BUFFER);
    linebuffer = (char *) malloc(LINE_LENGTH_BUFFER);

    while ((option = getopt(argc, argv, ":s:hl")) != -1) {
        switch (option) {
            case 'l' :
                opt = 'l';
                break;
            case 's' :
                opt = 's';
                searchterm = (char *) malloc(FILENAME_LENGTH_BUFFER);
                strcpy(searchterm, optarg);
                break;
            case 'h' :
                usage();
                return 0;
            default :
                printf("Invalid option, please see -h for help\n");
                return 0;
        }
    }

    if (argv[optind] != NULL)
        strcpy(infile, argv[optind]);
    else {
        printf("Please supply a file name or option \n");
        return 0;
    }

    fptr = fopen(infile, "r");

    if (fptr == NULL){
        printf("Error! File does not exist or can not be read. Missing read permissions or misspelled name?\n");
        return 0;
    }

    cat(fptr, opt, searchterm);

    // Freeing up memory and closing file as is good practice
    fclose(fptr);
    if (searchterm != NULL) // This conditional is used in case the program was invoked without the -s flag
        free(searchterm);   // therefore resulting in 'searchterm' never having been assigned any memory via malloc
    free(infile);
    free(linebuffer);

    return 0;
}
