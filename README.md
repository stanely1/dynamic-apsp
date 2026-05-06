# Fully-Dynamic All-Pairs Shortest Paths

# TODO
- [ ] framework
    - [x] budowanie - cmake, makefile, ...
    - [ ] struktury:
        - [x] graph
        - [x] path
        - [ ] ...
    - [x] klasa bazowa abstrakcyjna (interfejs) dla algorytmów (?)
    - [ ] formatter (clang-format) (optional)
    - [ ] generowanie i uruchamianie testów, porówynwanie wyników
    - [ ] unit testy na poprawność (?) (klas graph, path, ...)
- [ ] brute
    - [x] Floyd-Warshall (O(n^3))
    - [ ] Dijkstra (z std::priority_queue)
    - [ ] Dijkstra z Fibonacci Heap (optional?)
- [ ] praca Italiano,Demetrescu
    - [ ] tie breaking
    - [ ] wariant increase-only (O(n^2 logn))
    - [ ] wariant fully-dynamic
- [ ] praca Thorup
    - [ ] podział na poziomy - wersja O(n^2 log^2(n))
    - [ ] optymalizacja dla grafów rzadkich - ostateczna wersja
