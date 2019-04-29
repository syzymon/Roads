#include "map.h"
#include <stdlib.h>
#include <string.h>

#undef NDEBUG

#include <assert.h>

void test1() {
    Map *m = newMap();
    assert(m);

    assert(addRoad(m, "A", "B", 1, 2020));
    assert(addRoad(m, "B", "C", 2, 2020));
    assert(addRoad(m, "B", "D", 3, 2020));
    assert(addRoad(m, "C", "E", 4, 2020));
    assert(addRoad(m, "D", "E", 5, 2020));
    assert(addRoad(m, "E", "B", 8, 2020));
    assert(addRoad(m, "E", "F", 3, 2020));
    assert(!repairRoad(m, "E", "C", 2019));
    assert(repairRoad(m, "E", "C", 2021));
    assert(!repairRoad(m, "E", "A", 2020));
    assert(addRoad(m, "F", "G", 4, 2020));
    assert(addRoad(m, "A", "G", 10, 2020));
    assert(addRoad(m, "H", "G", 5, 2020));
    assert(addRoad(m, "F", "C", 2, 2020));
    assert(!addRoad(m, "F", "C", 2, 2020));
    assert(!addRoad(m, "C", "F", 2, 2020));
    assert(!repairRoad(m, "E", "B", 2018));
    assert(repairRoad(m, "E", "C", 2021));
    assert(repairRoad(m, "E", "F", 2023));
    assert(addRoad(m, "H", "C", 3, 2020));

    assert(newRoute(m, 10, "A", "E"));

    assert(extendRoute(m, 10, "H"));
}

void sample() {
    char const *str;

    Map *m = newMap();
    assert(m);

    assert(addRoad(m, "Alinów", "Bór", 1, 2020));
    assert(addRoad(m, "Bór", "Cielińsk-Niekłańsk", 2, 2020));
    assert(addRoad(m, "Bór", "Dąb Stary", 3, 2020));
    assert(addRoad(m, "Cielińsk-Niekłańsk", "Emiliew", 4, 2020));
    assert(addRoad(m, "Dąb Stary", "Emiliew", 5, 2020));
    assert(addRoad(m, "Emiliew", "Bór", 8, 2020));
    assert(addRoad(m, "Emiliew", "Fraźnik Nowy", 3, 2020));
    assert(!repairRoad(m, "Emiliew", "Cielińsk-Niekłańsk", 2019));
    assert(repairRoad(m, "Emiliew", "Cielińsk-Niekłańsk", 2021));
    assert(!repairRoad(m, "Emiliew", "Alinów", 2020));
    assert(addRoad(m, "Fraźnik Nowy", "Grzegrzewo", 4, 2020));
    assert(addRoad(m, "Alinów", "Grzegrzewo", 10, 2020));
    assert(addRoad(m, "Homilcin", "Grzegrzewo", 5, 2020));
    assert(addRoad(m, "Fraźnik Nowy", "Cielińsk-Niekłańsk", 2, 2020));
    assert(!addRoad(m, "Fraźnik Nowy", "Cielińsk-Niekłańsk", 2, 2020));
    assert(!addRoad(m, "Cielińsk-Niekłańsk", "Fraźnik Nowy", 2, 2020));
    assert(!repairRoad(m, "Emiliew", "Bór", 2018));
    assert(repairRoad(m, "Emiliew", "Cielińsk-Niekłańsk", 2021));
    assert(repairRoad(m, "Emiliew", "Fraźnik Nowy", 2023));
    assert(addRoad(m, "Homilcin", "Cielińsk-Niekłańsk", 3, 2020));


    assert(newRoute(m, 10, "Alinów", "Emiliew"));

    str = getRouteDescription(m, 10);
    assert(strcmp(str,
                  "10;Alinów;1;2020;Bór;2;2020;Cielińsk-Niekłańsk;4;2021;Emiliew") ==
           0);
    free((void *) str);

    assert(extendRoute(m, 10, "Homilcin"));

    str = getRouteDescription(m, 10);
    assert(strcmp(str,
                  "10;Alinów;1;2020;Bór;2;2020;Cielińsk-Niekłańsk;4;2021;Emiliew"
                  ";3;2023;Fraźnik Nowy;4;2020;Grzegrzewo;5;2020;Homilcin") ==
           0);
    free((void *) str);

    deleteMap(m);
}

void forum1() {
    Map *m = newMap();

    assert(addRoad(m, "A", "D", 10, 2019));

    assert(addRoad(m, "A", "B", 1, 2019));

    assert(addRoad(m, "B", "C", 1, 2019));

    assert(addRoad(m, "C", "D", 1, 2019));

    assert(newRoute(m, 1, "A", "D"));

    assert(!removeRoad(m, "B", "C"));

    deleteMap(m);
}

void forum2() {
    Map *m = newMap();

    assert(addRoad(m, "A", "B", 1, 2000));
    assert(addRoad(m, "B", "C", 1, 2000));
    assert(addRoad(m, "C", "D", 1, 2000));
    assert(addRoad(m, "C", "E", 1, 2000));
    assert(addRoad(m, "E", "F", 1, 2000));
    assert(addRoad(m, "E", "D", 7, 2000));

    assert(newRoute(m, 10, "A", "D"));
    assert(extendRoute(m, 10, "F"));
    char const *str = getRouteDescription(m, 10);
    assert(!strcmp(str,
                   "10;A;1;2000;B;1;2000;C;1;2000;D;7;2000;E;1;2000;F"));
    free((void *) str);
    deleteMap(m);
}

void forum3() {
    Map *m = newMap();

    assert(addRoad(m, "A", "B", 1, 2020));

    assert(addRoad(m, "B", "C", 1, 2020));

    assert(newRoute(m, 1, "A", "C"));

//Utworzy się teraz droga krajowa nr 1: A-B-C

    assert(newRoute(m, 2, "B", "C"));

    deleteMap(m);
}


#include "list.h"

void pointers() {
    List l = empty_list();
    int *a, *b, *c;
    a = calloc(1, sizeof(int));
    b = calloc(1, sizeof(int));
    c = calloc(1, sizeof(int));
    *a = 1, *b = 2, *c = 3;
    emplace_back(l, a);
    emplace_back(l, b);
    emplace_back(l, c);
    list_shallow_clear(l);
}

void easy() {
    Map *m = newMap();

    assert(addRoad(m, "a", "b", 1, 2020));

    deleteMap(m);
}

int main() {
//    pointers();
//    sample();
//    forum1();
    forum2();
//    forum3();
//    easy();
//    easy();
//    easy();
//    easy();
    return 0;
}
