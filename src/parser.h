/** @file
 * Interfejs klasy zajmującej się parsowaniem danych z wejścia, a następnie
 * przekierowywaniem odpowiednich poleceń wejścia na odpowiadające im funkcje
 * mapy.
 * @author Szymon Tworkowski
 * @date 18.05.2019
 */

#ifndef __PARSER_H__
#define __PARSER_H__

/**
 * Definicja umożliwiająca użycie funkcji getline()
 */
#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include "map.h"

/** @brief Wczytuje i obsługuje pojedyncze polecenie ze standardowego wejścia.
 * @param[in,out] map    – wskaźnik na strukturę przechowującą mapę dróg;
 * @return Wartość @p 0, jeśli polecenie wykonało się poprawnie;
 * wartość @p 1, jeśli wykonanie polecenia zakończyło się błędem;
 * wartość @p -1, jeśli nie ma już więcej poleceń do wczytania;
 * wartość @p -2, jeśli zabrakło pamięci na wczytanie linii.
 * Wypisuje coś na standardowe wyjście tylko w przypadku wypisywania
 * wyniku funkcji getRouteDescription.
 * Nie wypisuje nic na wyjście diagnostyczne.
 */
int read_line(Map *m);


/**
 * Kod wyjścia oznaczający brak pamięci na wczytanie linii wejścia.
 */
#define LINE_MEM_ERR (-2)

/**
 * Kod wyjścia oznaczający koniec strumienia wejścia.
 */
#define LINE_EOF (-1)

/**
 * Kod wyjścia oznaczający wczytanie poprawnej linii wejścia i pomyślne
 * wykonanie operacji
 */
#define LINE_OK (0)

/**
 * Kod wyjścia oznaczający niepoprawne wejście lub
 * niepomyślne wykonanie operacji
 */
#define LINE_ERR (1)

#endif /* __PARSER_H__ */
