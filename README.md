# Fully-Dynamic All-Pairs Shortest Paths

## TODO
- [x] brute
    - [x] Floyd-Warshall (O(n^3))
    - [x] Dijkstra (z `std::priority_queue`)
    - [ ] Dijkstra z Fibonacci Heap (optional)
- [x] praca Italiano,Demetrescu
    - [x] tie breaking
    - [x] wariant fully-dynamic
    - [ ] optymalizacja
- [ ] praca Thorup
    - [ ] podział na poziomy - wersja O(n^2 log^2(n))
        - [ ] debug
    - [ ] optymalizacja dla grafów rzadkich
- [ ] w pracach jest, żeby co jakiś czas restartować strukturę od zera, żeby się amortyzowało
- [ ] test z losowymi wagami typu `double` (żeby były unikalne długości ścieżek)
- [ ] odpalić testy z większym n (>1000 ?) - zobaczyć, czy i kiedy pokonamy brute
- [ ] formatter (clang-format) (optional)

## Correctness Tests
![](assets/correctness_pass.png)

## Performance Benchmarks

Comparison of **Floyd-Warshall**, **Dijkstra**, and **Demetrescu-Italiano** on the dynamic All-Pairs Shortest Paths problem.
Each plot shows **minimum**, **average**, and **maximum** time per update (in ms).

---

### Random Undirected Trees

![Random undirected trees](assets/Random_undirected_trees.png)

---

### Random Graphs with m = O(n)

![Random graphs with m = O(n)](assets/Random_graphs_with_m___O_n.png)

---

### Random Graphs with n=100 and Growing Density (m)

![Random graphs with n=100 and growing density](assets/Random_graphs_with_n_100_and_growing_density__m.png)

---

### Clique with All Equal Weights (weight=1)

![Clique with all equal weights](assets/Clique_with_all_equal_weights__weight_1.png)

---

### Random Clique with Almost Equal Weights (weights in {1, 2})

![Random clique with almost equal weights](assets/Random_clique_with_all_almost_equal_weights__weights_in__1__2.png)

---

### Pathological Case - n³ Changes in Locally Shortest Paths (Figure 4)

![Pathological case figure 4](assets/Pathological_case_triggering_n_3_changes_in_locally_shortest_paths_from_paper__figure_4.png)

---

### Worst-Case Instance - n³ Historical Paths (Figure 6)

![Worst-case instance figure 6](assets/Worst-case_instance_with_n_3_historical_paths_from_paper__figure_6.png)
