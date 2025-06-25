# Tema 2 PA

> Mentionez ca dupa corectarea manuala de cod sursa si de coding style am utilizat `clang` pentru o formatare frumoasa.

---

### Problema 1

#### Abordare

Am respectat constrangerile in legatura cu numarul de muchii. In implementare construiesc graful incrementând nivelurile de distanta, conectând fiecare nod nou la un nod existent de la nivelul anterior, deci vor fi `N - 1` muchii, unde N este numarul de noduri. Am tratat 2 cazuri de invalidare a vectorului de distante, prima atunci cand exista un input invalid, ceea ce este garantat exclus conform enuntului, iar a doua cand diferenta dintre 2 distante consecutive, dupa sortare, este mai mare decat 1, ceea ce ar face constructia grafului imposibila. 

In rezolvarea propriu-zisa a problemei, folosesc o abordare de tip **Greedy**, in care sortez vectorul de distante crescator, tinand minte indexul nodului pentru afisarea corecta la output. Dupa aceea, iterez prin vectorul de structuri sortate, si updatez, daca e cazul (cand diferenta e mai mare decat 1), nodul de care leg nodul parcurs in iteratie.
*Observatie* : Graful astfel obtinut este chiar un arbore, unde fiecare nivel corespunde distantei de la radacina la nodurile din nivel.


#### Complexitati

- Complexitatea temporala este `O(n log n)` pentru ca sortam array-ul de structuri cu quicksort, dupa care parcurgem in O(n), unde n este numarul de noduri.
- Complexitatea spatiala este `O(n)`, unde n este numarul de noduri pentru ca exact atat alocam dinamic pentru a stoca acele noduri

---

### Problema 2

#### Mentiuni speciale

Pentru ca am dorit sa rezolv toata tema in `C` (nu am reusit), a fost necesar sa implementez o coada care gestioneaza elemente de tip `Pair`. M-am inspirat din materialele de SDA ale lui ***Mihai Nan*** pentru structura de baza a cozii, pe care am modificat-o ulterior sa opereze pe structuri de tip `Pair`.


#### Abordare

Pentru rezolvarea acestei probleme, am folosit o abordare bazata pe **parcurgere in latime (BFS)** din fiecare celula nevizitata, construind progresiv o zona valida.

1. **Pornesc BFS din fiecare celula** nevizitata, luand acea valoare ca referinta minima.
2. Adaug in zona doar celulele a caror valoare se afla in intervalul `[val_min, val_min + K]`.
3. In timpul parcurgerii, marchez celulele vizitate in alta matrice si numar elementele din zona.
4. Returnez **aria maxima** intalnita dintre toate zonele explorate.


#### Optimizari

- Daca `max(mat) - min(mat) <= K`, intreaga matrice este valida, iar raspunsul este direct `N * M`.
- Pentru eficienta, folosesc doua matrici auxiliare:
- `visited[][]` pentru a evita reexplorarea celulelor deja incluse intr-o zona anterioara.
- `BFS_visited[][]`, care memoreaza un index de BFS (`BFS_idx`) pentru a evita realocari sau reinitializari cu **memset**.
- daca in timpul BFS gasesc o celula cu valoare egala cu minimul curent (`val_min`), o marchez ca `visited` pentru ca pornirea unui BFS din acel punct ar fi echivalenta cu BFS-ul curent


#### Complexitati

- Complexitatea temporala este `O(N^2 × M^2)` deoarece fiecare celula este vizitata de maxim o data de BFS-ul din care face parte, unde `N` este numarul de linii si `M` de coloane, si in teorie avem `N x M` BFS-uri. Dar, optimizarile reduc din complexitate pentru majoritatea test-case-urilor.
- Complexitatea spatiala este `O(N × M)` pentru cele 3 matrici (`mat`, `visited`, `BFS_visited`) si pentru coada folosita in BFS, unde `N` este numarul de linii si `M` de coloane

---

### Problema 3

#### Mentiuni speciale

- Hash-ul unui nod a fost inspirat de urmatorul articol StackOverflow: [How do I combine hash values in C++0x?](https://stackoverflow.com/questions/2590677/how-do-i-combine-hash-values-in-c0x) Acesta mi-a permis sa definesc eficient o functie eficienta custom de hash pentru structurile complexe utilizate in `unordered_map` si `unordered_set`, inspirata de **Boost**.


#### Abordare

- am creat un Graf de Stari si l-am parcurs cu **Dijkstra** pentru a determina costul minim
- Nodul grafului este definit ca un element din produsul cartezian dintre:
  - fiecare bustean
  - fiecare segment al busteanului
  - fiecare moment de timp

- Am ales algoritmul **Dijkstra** deoarece problema impune gasirea drumului cu cost minim de la o pozitie initiala la o pozitie finala, unde fiecare tranzitie are un cost diferit, dar pozitiv (nu exista muchii cu cost negativ).
- Pentru a evita generarea completa a grafului, care ar fi ineficienta din cauza dimensiunii foarte mari, am calculat vecinii unui nod **"on the fly"** (la nevoie, in timpul rularii algoritmului Dijkstra).
- Pozitiile fiecarui bustean la fiecare moment de timp sunt salvate in structura **Log**, intr-un vector, pentru acces rapid si determinarea pozitiilor viitoare.
- Am folosit **structuri de date cu acces O(1)** — `unordered_map` si `unordered_set` — deoarece nodurile nu sunt simple indexuri numerice, ci combinatii complexe de busteni, segmente si timp.
- Pentru accesarea eficienta a segmentelor de bustean disponibile pentru sarituri, am construit un **map de map-uri** care:
  - Mapeaza timpul si coordonatele la o lista de noduri valide spre care se poate face o miscare.
- In timpul executiei algoritmului Dijkstra:
  - Pentru fiecare nod extras din coada de prioritati, calculez vecinii valizi pentru cele 3 tipuri de costuri definite (ex. miscare, salt, stationare).
  - Continuarea cautarii se face numai pe acesti vecini calculati.


- Pentru reconstruirea traseului optim:
  - Am pastrat doua `unordered_map`:
    - unul pentru parintii fiecarui nod
    - unul pentru miscarile asociate fiecarui nod
  - la final, pornind de la nodul tinta (cel unde se afla Miriam), am iterat invers prin parinti pentru a extrage si afisa pasii ceruti.


#### Complexitati

- Complexitatea temporala este `O(N * log N)`, unde:
  - `N ≤ T * B * S` este numarul total de noduri generate (T = timp, B = busteni, S = segmente pe bustean).
  - Pentru fiecare nod, numarul de vecini (D) este constant si mic, deci nu influenteaza complexitatea lui Dijkstra
- Complexitatea spatiala este `O(T * B * S)`, deoarece numarul maxim de noduri stocate in toate structurile de date este numarul total de noduri.


#### Observatii

- Calculul vecinilor "on the fly" reduce drastic timpul de executie si memoria consumata, evitand generarea completa a grafului.
- Utilizarea `unordered_map` si `unordered_set` cu hash custom asigura acces constant si performante bune in gestionarea nodurilor.
- Construirea unui sistem de mapari temporale si spatiale (map de map-uri) pentru a accesa rapid nodurile valide la un moment dat este cruciala deoarece verificand fiecare nod cu fiecare nod ar fi crescut complexitatea drastic.




*Oprea Andrei-Cătălin 324CD 2025*