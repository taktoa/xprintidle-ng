// Utilities for the rendering of elapsed time format strings
// 
// Copyright © 2015       Remy Goldschmidt        <taktoa@gmail.com>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of version 2 of the GNU General Public License
// as published by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to
//
//     Free Software Foundation, Inc.,
//     51 Franklin Street, Fifth Floor
//     Boston, MA 02110-1301, USA.

#include <stdbool.h>
#include <signal.h>
#include <time.h>
#include <config.h>
#include "system.h"
#include "xalloc.h"


// Is the current position in a given string escaped with
// an even number of escape characters?
static char is_escaped(const char escape_char, char* pos, char* beg) {
    char* i;
    for(i = pos - 1; i != beg && *i == escape_char; i--);
    return (pos - i) % 2;
}

// String trie
typedef struct trie_node_t trie_node_t;
struct trie_node_t {
    char* replace;
    trie_node_t* children[256];
}; 

// Struct representing a pair of strings
typedef struct str_pair_t str_pair_t;
struct str_pair_t {
    char* pattern;
    char* replacement;
}; 

// Allocate a trie and fill it with zeros
static trie_node_t* calloc_trie() {
    return calloc(1, sizeof(trie_node_t));
}

// Add a pattern-replacement pair to a trie
static void add_word_trie(trie_node_t* root,
                          char* pattern,
                          char* replacement) {
    if(!*pattern) {
        root->replace = replacement;
    } else {
        trie_node_t** children_arr = root->children;
        trie_node_t**  child = children_arr + *pattern;
        if(*child) {
            add_word_trie(*child, pattern + 1, replacement);
        } else {
            *child = calloc_trie();
            add_word_trie(root, pattern, replacement);
        }
    }    
}

// Is the given str_pair_t null?
static char not_null_pair(str_pair_t pair) {
    return (pair.pattern != NULL) && (pair.replacement != NULL);
}

// Generate a trie from an array of pairs of the given length
static trie_node_t* generate_trie(str_pair_t* pairs) {
    trie_node_t* root = calloc_trie();
    for(int i = 0; not_null_pair(pairs[i]); i++) {
        add_word_trie(root, pairs[i].pattern, pairs[i].replacement);
    }
    return root;
}

// Utility function for print_trie
static void indent_depth(int indent) {
    for(int i = 0; i < indent; i++) {
        fprintf(stdout, " ");
    }
}

// Print a trie, for debug purposes
static void print_trie(int indent, trie_node_t* trie) {
    if(trie->replace) {
        indent_depth(indent);
        fprintf(stdout, "Replacement: %s\n", trie->replace);
    }
    for(int i = 0; i < 256; i++) {
        if(trie->children[i]) {
            indent_depth(indent);
            fprintf(stdout, "Child: %i | %c\n", i, i);
            print_trie(indent + 1, trie->children[i]);
        }
    }
}

// Match a single character with a trie
static trie_node_t* trie_match(char character, trie_node_t* trie) {
    if(trie->children[character]) {
        return trie->children[character];
    } else {
        return trie;
    }
}

// Replace values in a string according to a trie
static char* trie_replace(char* input, trie_node_t* trie, char no_escape) {    
    for(int i = 0; i < strlen(input); i++) {
        if(trie_match(input[i], trie) != trie &&
           (no_escape || !is_escaped('\\', input + i, input))) {
            
            char* replace = NULL;
            int pat_len = -1;
            trie_node_t* node = trie;
            
            for(int j = i; j < strlen(input); j++) {
                trie_node_t* newnode = trie_match(input[j], node);
                if(node != newnode) {
                    node = newnode;
                    if(newnode->replace) {
                        replace = newnode->replace;
                        pat_len = j - i;
                        break;
                    }
                } else {
                    break;
                }
            }
            
            if(pat_len >= 0 && replace) {
                int str_len = strlen(input);
                int rep_len = strlen(replace);
                if((rep_len - pat_len) > 0) {
                    size_t new_len = 2 * (str_len + rep_len - pat_len);
                    input = realloc(input, sizeof(char) * new_len);
                }
                memmove(input + i + rep_len,
                        input + i + pat_len + 1,
                        str_len - i);
                memcpy(input + i, replace, rep_len);
                i += rep_len - 1;
            }
        }
    }
    size_t size = strlen(input);
    input = xrealloc(input, strlen(input));
    return input;
}

// Replace the pairs in the given input, optionally ignoring escaped characters
static char* string_replace(char* input, str_pair_t* pairs, char no_escape) {
    trie_node_t* trie = generate_trie(pairs);
    char* retval = trie_replace(input, trie, no_escape);
    free(trie);
    return retval;
}

// Format a time in milliseconds against a format string
char* timefmt(const char* format, ulong millis) {
    char* fmt = xstrdup(format);

    char str_rem_milliseconds_int[40];
    char str_rem_seconds_int[40];
    char str_rem_minutes_int[40];
    char str_rem_hours_int[40];
    char str_total_milliseconds[40];
    char str_total_seconds_int[40];
    char str_total_seconds_float[40];
    char str_total_minutes_int[40];
    char str_total_minutes_float[40];
    char str_total_hours_int[40];
    char str_total_hours_float[40];
    char str_total_days_int[40];
    char str_total_days_float[40];
    char str_start_time_millis[40];
    char str_start_time_seconds[40];

    ulong start_time_millis = (time(NULL) * 1000) - millis;
    
    sprintf(str_rem_milliseconds_int, "%03lu", (millis /       1) % 1000);
    sprintf(str_rem_seconds_int,      "%02lu", (millis /    1000) % 60);
    sprintf(str_rem_minutes_int,      "%02lu", (millis /   60000) % 60);
    sprintf(str_rem_hours_int,        "%02lu", (millis / 3600000) % 24);
    sprintf(str_total_milliseconds,   "%lu",   millis);
    sprintf(str_total_seconds_int,    "%lu",   millis / 1000);
    sprintf(str_total_seconds_float,  "%f",    millis / 1000.0);
    sprintf(str_total_minutes_int,    "%lu",   millis / 60000);
    sprintf(str_total_minutes_float,  "%f",    millis / 60000.0);
    sprintf(str_total_hours_int,      "%lu",   millis / 3600000);
    sprintf(str_total_hours_float,    "%f",    millis / 3600000.0);
    sprintf(str_total_days_int,       "%lu",   millis / 86400000);
    sprintf(str_total_days_float,     "%f",    millis / 86400000.0);
    sprintf(str_start_time_millis,    "%lu",   start_time_millis);
    sprintf(str_start_time_seconds,   "%lu",   start_time_millis / 1000);

    str_pair_t pairs[] = {
        (str_pair_t) { "%rms",  str_rem_milliseconds_int },
        (str_pair_t) { "%rsc",  str_rem_seconds_int },
        (str_pair_t) { "%rmn",  str_rem_minutes_int },
        (str_pair_t) { "%rhr",  str_rem_hours_int },
        (str_pair_t) { "%tms",  str_total_milliseconds },
        (str_pair_t) { "%tsci", str_total_seconds_int },
        (str_pair_t) { "%tscf", str_total_seconds_float },
        (str_pair_t) { "%tmni", str_total_minutes_int },
        (str_pair_t) { "%tmnf", str_total_minutes_float },
        (str_pair_t) { "%thri", str_total_hours_int },
        (str_pair_t) { "%thrf", str_total_hours_float },
        (str_pair_t) { "%tdyi", str_total_days_int },
        (str_pair_t) { "%tdyf", str_total_days_float },
        (str_pair_t) { "%stms", str_start_time_millis },
        (str_pair_t) { "%stsc", str_start_time_seconds },
        (str_pair_t) { NULL, NULL }
    };

    return string_replace(fmt, pairs, 1);
}
