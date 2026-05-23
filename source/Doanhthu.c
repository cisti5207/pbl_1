#include <stdio.h>
#include <time.h>
#include "libmanage.h"
#include <stdlib.h>
#include <string.h>
int laygiathue(const char *masachcantim) {
    FILE *f = fopen("data/dataTruyen.txt", "r"); 
    if (f == NULL) return 5000; 

    char line[1024]; 
    int lineCount = 0;

    while (fgets(line, sizeof(line), f)) {
        lineCount++;
        if (lineCount <= 3) continue;
        char ma[20];
        if (sscanf(line, " | %[^ | ]", ma) == 1) {
            if (strcmp(ma, masachcantim) == 0) {
                int giathue;
                sscanf(line, " | %*[^|] | %*[^|] | %*[^|] | %*[^|] | %*[^|] | %*[^|] | %*[^|] | %*[^|] | %*[^|] | %*[^|] | %d |", &giathue);
                fclose(f);
                return giathue;
            }
        }
    }
    fclose(f);
    return 5000; 
}
