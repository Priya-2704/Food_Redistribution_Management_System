# Food Redistribution Management System (FRMS)

A C++ console application that matches surplus food donors (restaurants, hostel
messes, caterers) with recipients (NGOs, shelters, orphanages), so edible food
reaches people instead of being thrown away.

Graphic Era Hill University · Data Structures in C + Object Oriented Programming with C++

---

## Build and run

```bash
g++ -std=c++11 main.cpp -o frms.exe
```

```bash
./frms.exe
```

Press **20** to load demo data, then **14** to run the priority matching.

### Running one member's module on its own

Every module also compiles and runs independently:

```bash
g++ -x c++ -std=c++11 -DM1_STANDALONE M1_UserDirectory.h -o m1.exe
```

Replace the number in both places for M2, M3 and M4.

---

## Project structure

| File | Member | Data structure | Feature |
|---|---|---|---|
| `M1_UserDirectory.h` | 1 | Hash Table with Separate Chaining | S4 — Donor Trust Rating |
| `M2_FoodInventory.h` | 2 | Doubly Linked List | F1 — Usable Shelf Life, F4 — Risk Tiers |
| `M3_MatchingEngine.h` | 3 | Circular Queue + Sorted Array | F2 — Priority Matching |
| `M4_CityGraph.h` | 4 | Graph (Adjacency Matrix) + Singly Linked List | F5 — Route, F8 — Simulated Clock |
| `main.cpp` | 4 | — | Console menu, system integration |

Each file is split into two clearly marked sections:

- **Part A** — C data structure layer: plain `struct`, standalone functions, `malloc`/`free`
- **Part B** — C++ OOP layer: classes, inheritance, virtual functions, operator overloading

No STL containers are used anywhere. Storage is built from scratch with arrays,
pointers and `char` strings.

---

## Features implemented

| ID | Feature | Formula / method |
|---|---|---|
| **S4** | Donor Trust & Reliability Rating | `S = 0.6F + 0.4Q`, auto-approve at S ≥ 75 |
| **F1** | Usable Shelf Life | remaining life − transit time |
| **F4** | Risk-Tiered Hygiene Categorisation | 4 tiers by % of shelf life left |
| **F2** | Multi-Factor Priority Matching | `P = 0.5E + 0.3W + 0.2Z` |
| **F5** | Multi-Stop Route Aggregator | greedy nearest-neighbour traversal |
| **F8** | Simulated Clock | +1h / +3h time jumps |

### Spoilage model

One clock hour is not one hour of shelf life:

| Food state | Decay rate |
|---|---|
| Cooked, covered | 1.5× |
| Cooked, open | 2.0× |
| Packaged, sealed | 0.5× |
| Packaged, opened | 1.0× |

---

## How the modules work together

Menu option **14** is where all four members' work runs on the same data:

```
Member 4  gives the distance and travel time between zones
Member 2  checks whether the food survives that journey   (Feature F1)
Member 3  scores and ranks whatever survives              (Feature F2)
Member 1  owns the donors and NGOs being matched
```

Sample output:

```
skipped F201 -> N502 (only -2.0 hrs usable after 4 hr trip)
9 pairs accepted, 6 rejected by shelf life.

1.  F201 -> N503 | 50 meals | 0 km | P = 73.0
2.  F204 -> N503 | 30 meals | 0 km | P = 73.0
```

The same biryani is **rejected** for one shelter (3-hour trip, arrives spoiled)
and **ranked first** for another (same zone, 0 km).

---

## Future work

| Feature | Description |
|---|---|
| S2 | Ambient temperature decay multiplier |
| F3 | Greedy multi-split allocator for bulk donations |
| S1 | 3-tier zero-waste fallback pipeline (animal rescue → bio-compost) |
| S5 | Predictive pre-reservation using exponential smoothing |
| — | Max-heap priority queue, replacing the sorted array (O(n) → O(log n)) |
| — | LIFO stack for audit transaction logs |
| — | File I/O persistence |

---

## Build environment

- MinGW g++ 6.3.0, compiled with `-std=c++11`
- Windows 11, VS Code
- No external libraries, no STL containers
