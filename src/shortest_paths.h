#ifndef __SHORTEST_PATHS_H__
#define __SHORTEST_PATHS_H__

#include "graph.h"

Path get_shortest_path(City source, City dest, Path not_allowed,
                       bool *mem, uint64_t *dist, int *year, bool remove);

#endif /* __SHORTEST_PATHS_H__ */