# Food Redistribution Management System (FRMS)

C++ console project for Data Structures in C and OOP with C++.
It matches food donors with NGOs and shelters before the food spoils.

Graphic Era Hill University

## How to run

```
g++ -std=c++11 main.cpp -o frms.exe
./frms.exe
```

Press 23 to load the sample data, then 17 to run the matching.

## Files

```
M1_UserDirectory.h    hash table, donor trust rating
M2_FoodInventory.h    doubly linked list, food decay, shelf life
M3_MatchingEngine.h   circular queue, sorted array, priority score
M4_CityGraph.h        adjacency matrix, delivery route, clock
main.cpp              menu
```

Each file has a C part with structs and functions, and a C++ part
with classes. No STL containers are used anywhere.

## Features done

```
Donor trust rating    S = 0.6F + 0.4Q, approved at 75
Usable shelf life     life left minus travel time
Hygiene risk tiers    4 levels by percent of life left
Priority matching     P = 0.5E + 0.3W + 0.2Z
Delivery route        nearest unvisited zone each time
Simulated clock       +1 and +3 hour jumps
```

Cooked food decays faster than packed food. Open cooked food decays
at twice the clock, a sealed packet at half.

## Still to do

Temperature decay, splitting big donations between shelters,
zero waste pipeline, predicting leftovers, audit stack,
saving and loading from a file.

## Built with

MinGW g++ 6.3.0 on Windows, no libraries.
