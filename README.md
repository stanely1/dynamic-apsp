# Fully-Dynamic All-Pairs Shortest Paths

# TODO
- [ ] framework
    - [x] budowanie - cmake, makefile, ...
    - [x] struktury:
        - [x] graph
        - [x] path
    - [x] klasa bazowa abstrakcyjna (interfejs) dla algorytmów (?)
    - [ ] formatter (clang-format) (optional)
    - [ ] generowanie i uruchamianie testów, porówynwanie wyników
    - [ ] unit testy na poprawność (?) (klas graph, path, ...)
- [ ] brute
    - [x] Floyd-Warshall (O(n^3))
    - [x] Dijkstra (z std::priority_queue)
    - [ ] Dijkstra z Fibonacci Heap (optional?)
- [ ] praca Italiano,Demetrescu
    - [x] tie breaking
    - [ ] wariant increase-only (O(n^2 logn))
    - [x] wariant fully-dynamic
- [ ] praca Thorup
    - [ ] podział na poziomy - wersja O(n^2 log^2(n))
    - [ ] optymalizacja dla grafów rzadkich - ostateczna wersja
- [ ] w pracach jest, żeby co jakiś czas restartować strukturę od zera, żeby się amortyzowało
- [ ] dodać możliwość inicjalizacji algorytmów dla danego grafu (a nie tylko dla pustego)
