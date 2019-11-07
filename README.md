# Roads
Student's work (Individual Programming Project 2018/2019 course) at University of Warsaw 

The purpose of the project, written in pure C11, was to implement a CLI for a
map of cities, routes and roads, which form a weighted graph, and numerous CRUD operations on them.
There is a Doxygen documentation in Polish provided.

The project required concise and easy to use implementations of i.a.
* SPFA algorithm (```src/shortest_paths.c``` file)
* Own hashmap (```src/hashtable.c```)
* Singly linked list with almost any possible operations (```src/list.c```)

which used all together give a good overview of pointer-based dynamic memory management,
dealing with no OOP and exceptions available and trying to implement polymorphism in C
(```src/list.c```).
