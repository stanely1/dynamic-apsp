# Fully-Dynamic All-Pairs Shortest Paths

# TODO
- [ ] framework
    - [x] budowanie - cmake, makefile, ...
    - [ ] struktury:
        - [ ] graph
        - [ ] path
        - [ ] ...
    - [ ] klasa bazowa abstrakcyjna (interfejs) dla algorytmów (?)
    - [ ] generowanie i uruchamianie testów, porówynwanie wyników
- [ ] brute
    - [ ] Floyd-Warshall (O(n^3))
    - [ ] Dijkstra (z std::priority_queue)
    - [ ] Dijkstra z Fibonacci Heap (optional?)
- [ ] praca Italiano,Demetrescu
    - [ ] tie breaking
    - [ ] wariant increase-only (O(n^2 logn))
    - [ ] wariant fully-dynamic
- [ ] praca Thorup
    - [ ] podział na poziomy - wersja O(n^2 log^2(n))
    - [ ] optymalizacja dla grafów rzadkich - ostateczna wersja
