#include <stdio.h>
#include <stdlib.h>
#include "map.h"
#include "parser.h"

int main() {
    Map *m = newMap();

    int result = 0;
    for (size_t line_nr = 1; (result = read_line(m)) != LINE_EOF; ++line_nr) {
        if (result == LINE_ERR) {
            fprintf(stderr, "ERROR %lu\n", line_nr);
        }
        else if (result == LINE_MEM_ERR) {
            deleteMap(m);
            return 0;
        }
    }

    deleteMap(m);
}
