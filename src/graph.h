#include <stdint.h>
#include "list.h"

#ifndef __GRAPH_H__
#define __GRAPH_H__

typedef struct Edge *Road;

struct ShortestPathsData {
    uint64_t distance;
    int min_year;
    size_t iteration;
    bool doubled;
    bool blocked;
    bool queued;
    Road backward;
};

typedef struct ShortestPathsData SPFAData;

struct Vertex {
    char *city_name;
    List neighbours;
    SPFAData path_data;
};

typedef struct Vertex *City;

struct Edge {
    City begin;
    City end;
    unsigned length;
    int built_year;
    bool refcount;
    List routes_passing; //of elements Iterator to DirectedEdge
};


struct DirectedEdge {
    Road edge;
    City start;
    unsigned route_id;
};

typedef struct DirectedEdge DirectedEdge;

typedef List Path;

struct Weight {
    uint64_t distance;
    int min_year;
};

typedef struct Weight Weight;

Weight make_weight(uint64_t distance, int min_year);

int operator_less(uint64_t distance1, int year1,
                  uint64_t distance2, int year2);

int weight_compare(Weight w1, Weight w2);

City make_vertex(char *city_name);

void erase_vertex(City u);

Road make_road(City u, City v, unsigned length, int built_year);

void erase_road(Road r);

Road get_road(City u, City v);

Weight get_vertex_weight(City v);

bool populate_road(Road r);

void orientate_road(Road r, City new_road_begin);

//struct DirectedEdge *to_route_road(Road r, City start);

bool update_year(Road r, int repair_year);

char *save_vertex_name(const char *input_name);

#endif /* __GRAPH_H__ */