#include <stdint.h>
#include "list.h"

#ifndef __GRAPH_H__
#define __GRAPH_H__

typedef struct Edge *Road;

/**
 * Informacje potrzebne do algorytmu znajdowania najkrótszych ścieżek.
 */
struct ShortestPathsData {
    uint64_t distance; /**< Najkrótsza odległość. **/
    int min_year; /**< Minimalny rok budowy. **/
    size_t iteration; /**< Numer wywołania algorytmu najkrótszej ścieżki. **/
    bool doubled; /**< Czy wierzchołek został zdublowany. **/
    bool blocked; /**< Czy wierzchołek jest zablokowany. **/
    bool queued; /**< Czy wierzchołek jest na kolejce. **/
    Road backward; /**< Powrót do poprzedniego wierzchołka na ścieżce. **/
};

typedef struct ShortestPathsData SPFAData;

/**
 * Wierzchołek grafu.
 */
struct Vertex {
    char *city_name; /**< Nazwa wierzchołka. **/
    List neighbours; /**< Lista sąsiadów wierzchołka. **/
    SPFAData path_data; /**< Informacje o najkrótszych ścieżkach. **/
};

typedef struct Vertex *City;

/**
 * Krawędź grafu. Dla każdej krawędzi istnieje dokładnie jeden taki obiekt.
 */
struct Edge {
    City begin; /**< Początek krawędzi. **/
    City end; /**< Koniec krawędzi. **/
    unsigned length; /**< Długość krawędzi. **/
    int built_year; /**< Rok budowy. **/
    bool refcount; /**< Czy oba wskaźniki istnieją. **/
    List routes_passing; /**< Lista iteratorów na krawędź drogi krajowej. **/
};

/**
 * Krawędź skierowana - element drogi krajowej.
 */
struct DirectedEdge {
    Road edge; /**< Krawędź właściwa. **/
    City start; /**< Początek krawędzi skierowanej. **/
    unsigned route_id; /**< Numer drogi krajowej. **/
};

typedef struct DirectedEdge DirectedEdge;

typedef List Path;

typedef struct Weight Weight;

int operator_less(uint64_t distance1, int year1,
                  uint64_t distance2, int year2);

City make_vertex(char *city_name);

/**
 * Usuwa wierzchołek wraz z incydentnymi obiektami krawędzi
 * (nie usuwa zdublowanych krawędzi z sąsiadów)
 * @param u
 */
void erase_vertex(City u);

Road make_road(City u, City v, unsigned length, int built_year);

void erase_road(Road r);

Road get_road(City u, City v);

/**
 * Dodaje krawędź na listy incydencji jej końcowych wierzchołków
 * @param r
 * @return
 */
bool populate_road(Road r);

/**
 * Skierowuje krawędź zgodnie z podanym początkiem
 * @param r
 * @param new_road_begin
 */
void orientate_road(Road r, City new_road_begin);

bool update_year(Road r, int repair_year);

char *save_vertex_name(const char *input_name);

#endif /* __GRAPH_H__ */