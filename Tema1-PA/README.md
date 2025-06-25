# Tema 1 PA

> Mentionez ca dupa corectarea manuala de cod sursa si de coding style am utilizat `clang` pentru o formatare frumoasa.

---

## Problemele Usoare

### Problema 1 - NoStory

Am avut o abordare de tip **Greedy** in rezolvarea problemei. Am rezolvat prima parte in care trebuia sa luam cele mai mari `n`  
valori pentru a maximiza suma maximelor perechilor. Dupa aceea am realizat ca noi prin abordarea greedy ar trebui sa tindem catre  
acea valoare, deci catre perechi formate dintr-un numar mic si unul mare. Apoi, am calculat ca numarul cu care maximizam este suma viitoare - suma curenta, adica `(min2 + max2) - (max1 + max2) = min2 - max1`, deci al doilea element al perechii a doua ar trebui sa  
fie mai mare decat primul element al primei perechi. Deci practic, pentru a ajunge la o rezolvare maxima, ar trebui sa avem un numar  
din prima jumatate si unul dintr-a doua. Dar, avand un numar limitat de mutari, ar trebui ca `min2 - max1` sa fie maxim. Deci, procedam  
in felul urmator. Spunem ca perechile cu 1 numar din prima si 1 numar dintr-a doua jumatate (ca marime) sunt perechi care nu trebuie  
modificate.

**Demonstratie**:  
Presupunem prin absurd ca se poate, `min2 - max1 > 0 => min2 > max1`, dar `max1` este din prima jumatate, deci si `min2`  
ar trebui sa fie tot din prima, deci vom ajunge de la o pereche cu nr mari si una cu nr medii tot la una cu nr mari si alta cu nr medii.  
Iar pentru un optim local, e mult mai usor sa le luam pe cele mici pentru profit mai mare. 

Deci, ajungem la concluzia ca vom avea un sir  
cu numere mici, sortate in ordine crescatoare a maximului perechii (- `max1`) si unul cu numere mari, sortate in ordine crescatoare a  
minimului perechii (+`min2`). Asadar, iteram pana terminam perechile (care au acelasi size datorita simetriei) sau pana ni se termina mutarile, si adaugam la maximul existent (`min2 + max1`).

**Complexități**:
- Complexitatea temporala la prima parte este `O(n log n)` pentru ca sortam tot sirul, iar `qs` are compl `2n * log(2n)`
- Complexitatea temporala la a doua parte este tot `O(n log n)`, doar ca se face o sortare initiala pentru a afla mijlocul, iar apoi iteram prin  
toate perechile (`O(n)`), dupa care iar sortam de 2 ori si reiteram, ceea ce duce la aceeasi complexitate temporala a sortarii de `O(n log n)`
- Complexitatea spatiala a primei parti este numarul de elemente, `2 * n`, ceea ce duce la o complexitate de `O(n)`
- Complexitatea spatiala a celei de-a doua parte a problemei este tot `O(n)`, deoarece alocam memorie de mai multe ori pentru `n` sau `2 * n` elemente

---

### Problema 2 - Walsh

Am observat ca daca stiu linia si coloana, pot injumatati cautarea cu Divide et Impera. In functie de cadran ajung la cadranul 1 inversat sau nu, dupa  
care ajung din nou si tot asa. Deci ajung la final si vad de cate ori am inversat bitul (eu l-am inversat cu `1 - ...`). Cazul de baza este `0` (matricea cu 1 elem).

**Complexitate**:
- Complexitatea temporala de cautare a unui element este `T(n) = T(n / 2) + O(1)`, deci `O(log(n))`, `n` fiind dimensiunea initiala a matricei, iar pentru `K` elemente este `O(K * log n)`
- Complexitatea spatiala este `O(1)`, deoarece nu alocam decat variabile locale sau doar un apel nerecursiv pe stiva

---

## Problema Medie

### Problema 3 - BadGPT

Am observat ca conteaza numai literele `n` si `u` pentru a numara cate variante exista. Deci:
- pentru 1 `n`, avem o singura varianta → **1**
- pentru 2 de `n` → varianta de dinainte + una noua (`nn`, `m`) → **2**
- pentru 3 de `n` → m + nnn + nm → **3**
- pentru 4 de `n` → 1 varianta cu `m` la final + 2 la care adaugam `n` sau transformam in `m` → **5**
- pentru 5 → 2 cu `m` si 3 cu `n` → `2 + 3 * 2 = 8`, si asa mai departe

Am calculat **Fibonacci** pentru fiecare numar in parte, si pentru a-l face eficient, am folosit tehnica de la laborator  
cu **exponentiere matriceala**, in care calculam matricea la puterea `n / 2` si dupa o inmultim cu ea insasi. De aceea se ajunge din jumatate  
in jumatate la `log(n)`. `T(n) = T(n / 2) + O(1)`, deoarece inmultirea matricilor 2x2 se face in `O(2^3)` <=> `O(1)`.

Am inmultit toate fibonacci-urile de la toate numerele pentru a afla rezultatul final (cu modulo).

**Complexitate**:
- Complexitatea temporala: `O(G * log N)`, unde `G` este numarul de grupuri si `N` puterea la care ridic matricea, adica elementul maxim din grup
- Complexitatea spatiala: `O(L + log N)`, unde `L` e lungimea bufferului si `N` este numarul care ar trebui calculat recursiv cu fibonacci

> De mentionat ca vectorul initial este alcatuit din valorile `0` si `1` ale lui sirului lui fibonacci, iar matricea este aleasa astfel  
> incat `Matrice * (fib(x), fib(x+1)) = (fib(x+1), fib(x+2))`

---

## Problema Grea

### Problema 4 - Crypto

Pentru a rezolva aceasta problema, am apelat la **programare dinamica**. Am luat `N` ca numar de caractere in string si `L` ca numar de  
caractere in subsequence-ul cautat. Am apelat la o matrice cu `(N + 1)` linii si `(L + 1)` coloane, acel `+1` fiind datorita reprezentarii unui  
string gol.

- **Initializare**:
  - un subsequence gol se afla intr-un string gol: `dp[0][0] = 1`
  - intr-un string gol se afla 0 subsequence-uri: `dp[0][x] = 0` for `x in 1 : L`
  - numarul de cazuri cu care inmultim problema curenta este numarul de caractere diferite din subsequence-ul dat daca avem `'?'`, iar daca nu, cel anterior

> Deci un subsequence gol nu se afla doar o data intr-un string, ci de `{nrCaractere}` ori

- Daca caracterele nu coincid: `dp[i][j] = dp[i - 1][j]` numarul de ori in care e subsequence dp[i][j] este numarul in care e subsequence acelasi sequence, doar ca pe un string mai mic cu 1 caracter, deci dp[i - 1][j]
- Daca coincide caracterul: `dp[i][j] = dp[i - 1][j - 1] + dp[i - 1][j]` este acelasi numar de cand stringul si subsequence-ul aveau cu un caracter in - (folosim caracterul nou), la care se adauga de cate ori era de dinainte incadrat in subs (nu folosim caracterul nou)
- Daca avem `'?'`: `dp[i][j] = dp[i - 1][j - 1] + nrCaractereUniceSubstring * dp[i - 1][j]` avem cazul anterior, la care se adauga (toate caracterele unice - 1)

Iteram, deci, prin matrice, pe linii si coloane in 2 loop-uri si aplicam regula anterioara pana ajungem la ultimul element,  
care este rezultatul nostru.

Atat la aceasta problema, cat si la cea medie, am calculat restul cu formula respectiva si am creat 2 functii atat pentru adunare,  
cat si pentru inmultire, care aduna sau inmultesc cu modulo.

**Complexitate**:
- Complexitatea temporala si spatiala a algoritmului este `O(N * L)`, aceasta fiind dimensiunea matricei de Programare Dinamica


Oprea Andrei-Cătălin 324CD 2025
