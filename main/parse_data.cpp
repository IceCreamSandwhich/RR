#include "include/parse_data.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void parse_enc_line(char* line, int* sec, int* ms, int* right, int* left) {
    char buf[6]; // max characters + '\0'

    // get seconds
    strncpy(buf, line, 4);
    buf[0] = '\0';
    if (strcmp(buf, "OVER") != 0) {
        *sec = NULL;
    }
    else {
        *sec = atoi(buf);
    }

    // get ms
    strncpy(buf, line + 4, 3);
    buf[0] = '\0';
    if (strcmp(buf, "OVR") != 0) {
        *ms = NULL;
    }
    else {
        *ms = atoi(buf);
    }

    // get right encoder revolutions
    
}