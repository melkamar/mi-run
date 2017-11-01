
@specialization
    guards - pokud se vybere daná specializace, musí platit tahle podmínka, jinak se nevezme

GitHub - simplelanguage - reference pro truffle

Místo @compilefinal radši používat BranchProfile

ConditionProfile - pro if/then větve - je schopen říct kompilátoru, v jakým pořadí seskládat kód podmínky.
Častěji používaný by mělo být nahoře.


## PŘEDNÁŠKY

začínáme s tím, že máme AST. Z toho se dá generovat strojový kód, byte code...

- AST interpreter
- AST -> bytecode
- Bytecode interpreter
- Memory + GC
- Jitting BC -> MC

Nechceme dělat syntax analysis a tak, takže začneme s easy jazykem - Scheme/Lisp.
Když řešíme jazyk, musíme rozlišovat syntaxi a to, co se s tim vlastně dá dělat. Většinou se dá abstrahovat syntaxe, to
pod ní je vždycky stejný, ty principy programování, řídící if-else bloky atd.

Scheme je starý, ale už před 50 lety měl lambda, namespace, moduly, šecko.
Lisp - programuje se rovnou v AST, scheme je jeden z konkrétních jazyků z rodiny lisp.

"Should learn" - smalltalk, haskell, scheme
sémanticky je smalltalk nadmnožinou ruby, pythonu

            | dynamic typing  | static typing
strong type |  smalltalk ruby |  haskell f#
weak type   |  assembler      |  c, c++

tady se budem bavit o strong type, dynamic typing
další dimenze by byla object-oriented vs. functional

strong - proměnná ví, co je, nedá se s ní dělat nic jinýho než co umí
weak type -


-------

Statický jazyky jako C, Java se dycky pouští kompletně celý
Smalltlk, python atd. jsou REPL - read-eval-print

Doctor racket - download - je to jazyk/studio? a má repl

Scheme nemá overflow, počítá přesně. Např. 1/3 je fakt třetina, neni to 0.3333. To samý s integerama,
jsou nekonečně přesný.

#### Operace

- Přiřazení - `(define a 10)`
- Volání funkcí - `(/ 1 3)`, `(+ 5 8)`
- Dají se používat všechny identifikátory kromě mezery a závorky.
- Builtins: + - * / define < > <= >= =
- Constants: integer, float, string, #t/#f (bools), null
- Tvoření funkcí, přes lambdu: `(Lambda (arglist) body...)`
- Volání funkce: (func blabla) - závorka znamená volání funkce
- `(display "hello world")` - printing

#### Definice funkce
`(define myFunc (lambda () 1234))`
`(define f (lambda (a b) (+ a b 10)))`
`(define f (lambda (a b) (+ a b 10) 1234))` - poslední expression je návratová hodnota

Volání: `(f 1 2)`

Můžu předefinovat cokoliv, např. `(define + -)` - teď se + chová jako - (změnil jsem referenci, kam to ukazuje)

Alternativně definice funkce: `(define (funcname arg1 arg2...) (body))`

#### Podmínky

`(if <condition> <iftrue> <iffalse>)`, e.g. `(if (= 10 11) "yes" "no")`

porovnávání:
- = je aritmeticky, matematicky
- eq? je "je to ten samej objekt, reference?" - jako v javě "=="

Nejsou tu cykly, je tu jenom rekurze.

Blok funkcí - `(begin expr1 expr2 expr3)`

#### Alokace
cons-cell (construct a new cell)
vytvoří nový objekt, pair - má dva sloty.

`(cons a b)` -> vytvoří pair (a,b)

bunky se nazývají (car, cdr)  "contents of address register" | "contents of data register"


#### List
Empty list: `'()`
List je několik (cons a (cons b (cons c nil))) za sebou.
Všechno je rekurzivní.

Tail rekurze je lepší (zavolat rekurzi jako poslední, pak už nic).
Kompilátor je schopnej to vylepšit a rovnou pak skočit na začátek rekurze a neskákat po schodech postupně.
Tail-call optimization. Když je poslední věc ve funkci tail rekurze,
tak na stack nedávám adresy pro return (protože bych je stejně jenom popnul).

To znamená, že když mam funkci na počítání délky pole, bude fungovat na obří pole
jenom když bude využívat tail rekurzi (tam se nebude používat stack), jinak stack overflow.

#### Jméno, name

`(quote (cons 1 2))` -> `'(cons 1 2)`
dostanu název klíče do tabulky "name: reference".
Každá proměnná, funkce má jméno. To je unikátní napříč programem.

`(quote (x y z))` má cukřík: `'(x y z)`


`(eval (quote a))` -> dostanu jméno pro proměnnou a, tj. `'a`, a pak
to evalnu, takže dostanu hodnotu a (tj. totožné jako prostě `a`)

## REPL

Read - in: stream; out: expr
Eval - in: expr; out: value
Print - in: stream; out: expr

# Implementace notes

#### Načítání vstupu
- Mám static buffer pro kratší vstupy, když to nebude stačit, tak mallocuju
  a pak musím free. Je to ale drahý, proto mám 128 bytů by default, abych si to ušetřil.

- readAtom - načítání jednoho "tokenu" (int, string, symbol).
  Ty jsou oddělený závorkama, whitespacema a EOF.
    - pokud je další znak jeden z oddělovačů, tak vytvořím nový atom
    - jinak prostě přidam znak do bufferu

- pro každej typ atomu mám vlastní funkci, co ho přečte. Pomocí peek
  zjistim jakej atom bude a pak zavolam odpovídající funkci.

- objekty jako třeba nil budou singletony, nemá cenu je alokovat pořád dokola

#### eval

Viz scm_eval()

- list znamená function call
- symbol -> value of symbol
- něco jinýho -> eval sám sebe (int, str, #f, #t atd)

- Pro symboly potřebujeme key-value tabulku `global environment`:
    - key: reference na symbol
    - value: reference na hodnotu/referenci přiřazenou danému symbolu

    - bude to hashtable, klíč bude hash symbolu

    - Ale abychom nemuseli pořád hashovat, budem adresovat adresou danýho
    symbolu v paměti. Symbol je tedy singleton, chci jen jeden. Proto
    funkce `getOldSymbolOrNil` a `rememberSymbol`. To je v podstatě
    string-hashed table. Takže hledání symbolu je dražší při jeho čtení
    ze vstupu, ale jak ho jednou mám v AST, je přístup do symbol table
    easy, protože prostě vezmu adresu v paměti.

dummy20 symbol string - jen pro debugging, jinak se při breakpointu
zobrazoval jen jeden char (protože je pole staticky definovaný jako 1
prvek).

Předávání parametrů naším funkcím se nedělá přes C parametry - nevím
kolik parametrů bude která funkce mít. Na to radši uděláme vlastní
stack. To bude flexibilnější a efektivnější než varargs.

#### if

- nemůže to bejt jenom klasická builtin function, protože ta evaluuje všechny svoje
  argumenty. Tady ale chceme evaluovat jenom jednu z nich.
- Proto je to odlišný konstrukt - builtinSyntax.c

#### print

- kvůli printování definujeme tag T_VOID - pokud dostanu tenhle typ z
  funkce, tak neprintuju nic. Jinak by se mi dělo tohle:
```
> (if #t 10 20)
10()
```

#### degenerovaný list

pokud mám `(cons 1 2)`, tak to není list, protože to nemá
null-terminated element. Je to degenerovaný list, a má se to printit jako
`(1 . 2)`.


### Lambda

- Pro definování funkcí potřebujeme, aby eval nepracoval by default
s global environmentem, ale bral jako parametr custom environment.
- Pro vykonání funkce vytvořím nový environment a naplním jména
parametrů fce jejíma hodnotama.


### Closure
Můžu vytvářet lambdy co vrací další lambdy. K tomu ale musím dědit
environmenty. Když v lambdě a vytvořím další lambdu b, tak ta nemůže
mít jako parenta global env, ale env tý funkce, kde byla vytvořená.

-> userdefinedfunction # homeenvironment

### Error handling

pomocí setjmp longjmp - uložím si stack frame/env, a potom kde je error, budu loadovat tenhle env

pokud setjmp vrátí 0, pak je to první průchod - ok.
pokud vrátí něco jinýho, pak se z něj vracím nějakým errorem.

### Continuation passing

- způsob jak fixnout hlubokou rekurzi - pokud je tail rekurze,
tak se dá obejít bez alokování dalších a dalších stacků

- bootstrap "trampoline" - volá první funkci na stacku

# Druhý týden

## Interpretace bytecode místo AST

Převod ast -> bc, projdu strom a zhruba:
foreach node:
    is const? -> emit PUSH_CONST arg
    is variable? -> emit PUSH_LOCAL arg
    ...
    else compile_call arg:

compile_call:
    if builtin? -> compile_builtin

compile_builtin:
    compile_args (push args on the stack recursively)

    if + -> emit ADD
    if * -> emit multiply
    ...

        ex. strom:
            _________________
           |                 |
           |         +       |
           |                 |
           |   *           b |
           |                 |
           | a   10          |
           |_________________|


        z toho bude:
            PUSH_ARG 0
            PUSH_CONST 10
            MUL
            PUSH_ARG 1
            ADD

Viz `compiler.scm`


Přidává se bytecodeInterpreter.c - interpreter umí evaluovat jak
klasický AST, tak i fce, které mají pro sebe definovaný bytecode.

Má to bejt tak, že zavolam fci - provede se klasicky. Případně můžu místo funkce
nadefinovat bytecode a pak se začne provádět ten.

#### 5. přednáška

kompilace if-then-else - backpatching

## Garbage Collection

Tomato meatball-spaghetti soup picture. Stačí si to pamatovat, a každej pochopí garbage collection.

- Když mám nějakej objekt v polívce, kterej není dostupnej z
    - Global table
    - Stack
  tak už je to garbage, nemůžu ho nikdy dál použít.

- Těmhle dvěma se říká "roots"

#### Mark & Sweep

- Pošlu mravence z rootu, on jde rekurzivně přes všechny reference z objektů
  které ještě nejsou označené. Když potká novou, vysere se na ní (cituji).
- Prostě DFS.
- Má to dvě fáze:
    - Mark - DFS, označuju objekty
    - Sweep - projdu všechny objekty co na sobě nemají hovno (cituji) a
      dám je do "free listu", odkud pak můžu brát paměť.

- Nejlepší je prostě vzít si chunk paměti, víc než potřebuju, a pak v
  něm ručně alokovat.
- Když budu mít chunk plnej a budu chtít další objekty, udělám sweep a
  uvolním místo.
- Free list implementuju jako jeden pointer do naší paměti a pak
  zřetězenej seznam (cca jako FAT).
- Existujou nějaký vylepšení tohohle algoritmu, dělení paměti na
  segmenty atd. ale furt to nebude moc dobrý. Budem mít něco lepšího.
- Tady je problém (mimo jiné) že default implementace není realtime -
  musim zastavit program, zatímco se dělá m&s.

- Výhody: jednoduchý, zvládá to cykly
- Nevýhody: pomalý, blokující, velká O() komplexita

- Kdy volat mark and sweep?
    - První nápad: když dojde paměť v našem bloku. Ale to je prej blbý.
      Navíc, když tvořím nový složený objekty, musí bejt vždycky na stacku.
      Může se mi stát např. `new_cons(new_symbol(), new_cons())` - tady
      si v C vytvořím new_cons() a new_symbol(), ale než se provede new_cons, může přijít GC a sežrat mi to.

    - Lepší: mám segmenty, když dojde paměť, naalokuju další blok, ale
      pamatuju si, že mám ten předchozí vyčistit. Tady navíc mám
      možnost zavolat GC kdy potřebuju - když to zavolám v trampolíně,
      mám to bezpečný, protože když jsem v metodě trampolíny, všechno
      musí bejt na stacku.

#### Reference counting

- Každej objekt má interní variable - počet referencí na něj. Klasicky.
- Kdykoliv objekt dojde na 0 referencí, tak se zabije a sníží reference
  svým objektům.

- Výhody: objekty jsou uvolňované, hned jak se stanou nereferencovanýma.
          můžu finalizovat objekty, na který mam reference, např. file handly.
- Nevýhody:
    - Může být blokující - pokud jsem se zbavil obrovskýho stromu,
            tak rekurzivně musim zničit všechny nody. Ale je to řešitelný,
            delayed decrement.
    - Ale performance je hrozná. Overhead M&S
            busy programu je cca 5-15%. Tady to ale může bejt až 50%.
            Při každý operaci s pointerem, i třeba při průchodu polem,
            musím projít celej strom referencí od tohohle pointeru a
            upravit počet.
    - Memory - můžu mít teoreticky miliardy objektů (64bit), a musím
               být schopen takový číslo narvat do čítače referencí. Dá
               se to ohacknout, budu mít jedinej bajt, protože většině
               objektů to stačí. Pokud překročím 254, prostě tam necham
               255 a dám objekt do overflow table, kde si pak pamatuju
               počty pro všechny takovýhle vzácný objekty. Kdykoliv
               najdu 255 v počitači referencí, musím se podívat do tabulky.
    - Cykly nebudou nikdy zcollectovaný. To se pak nechává na
    programátorech - musí se zbavit cyklus linků. Ale to to degraduje
    na manuální alokaci. -> refcounter alg prostě nestačí. Pak se musí
    doplňovat občas m&s nebo cycle-breaker toolem. A to je další overhead.

Pro performance je špatný mít v kritickým kódu branchování.

#### Tracing algoritmus - Baker

- Vezmu všechny rooty a přesunu je do jiný polívky (spolu s nima i to s čím jsou spojený)
- To co zůstane, není referencovaný.

- U každýho objektu budu mít bit "copied". Pokud je ten bit 1, už mám
  tenhle objekt zkopírovanej, budu mít v jeho těle adresu na ten
  skutečně zkopírovanej, kterej můžu použít.

- Kopíruju:
    - Začnu z rootu, zkopíruju objekt kam pointuje. Označím jako copied,
    reference z původního neplatí, ty jsem zahodil, ale platí ty z kopie.
    - Takhle dokola pro všechny rooty.
    - V cílovým bufferu mám alloc pointer - ukazuje na free místo; a
      scan pointer, kterej začíná na 0. Pokud ScanP == AP, pak jsem
      skončil.
    - Pokud scanp<ap, tak: vezmu všechny reference současného scanp co
      pointujou do staré polívky. Pokud je cílový objekt skutečně ve staré
      tabulce, tak ho zkopíruju a upravim reference. Pokud je cílový objekt
      už zkopírovaný (má flag a referenci), pak jenom nastavím referenci
      pro tenhle objekt. Musím taky zprocesovat všechny reference tohohle
      starýho objektu.

- Tady doba běhu záleží jenom na počtu live objektů, garbage to vůbec
nezpomaluje --> O(live)
- Zároveň tohle defragmentuje paměť, po každým průběhu mám objekty
nahoře a kus paměti volnej.
- Ale, všechny live objekty musím pořád kopírovat. Není to ale tak
hrozný, čtení objektu je cca stejný jako kopírování. Musím objekt dostat
do cache, CPU pak na zápis jenom řekne "zapiš asynchronně" a jede dál.
Čtení je blokující, zápis je easy, skoro nula.

##### Vylepšení - Generation Scavenging

- Unger si všimnul, že pravděpodobnost že bude danej objekt zcollectovanej
klesá s jeho stářím. Tj. pokud mám objekt kterej žije už dvě hodiny, nejspíš přežije.
Naproti tomu, objekt kterej zrovna vzniknul bude dost možná sebranej.
- Takže rozdělím objekty do skupin podle stáří:
    - Newspace:
        - Eden - fresh objekty
        - Survivors - ti, co přežili garbage collection v Edenu
        - Mám dva poty, ty se navzájem střídají v rolích.
        - Každý objekt má AGE field - v něm si držím počet GC, který
        přežil.
        - Pokud daný objekt přežil dostatek GC (nějaký threshold, ve
        smalltalk IDE Clause je to 29), tak ho napříště zkopíruju
        do OLD potu. Threshold je adaptivní, závisí na tom, jak moc je
        plnej pot po zkopírování objektů. Když je skoro plnej, tak jich
        pošlu víc do OLD a naopak. (OLD=Tenure)
        - Navíc, např. file descriptory nikdy nepřijdou do OLD, protože
        to by pak mohlo trvat moc dlouho, než by se zcollectovaly.
        Označím si je a řeknu že zůstanou young.

- V grafu vidim vývoj paměti jako pilu - young roste, roste a pak dolů když GC.
- OLD pot je pro mě taky pot rootů (kromě env a stack). Ale nechci procházet
  celej OLD pot (řádově 1GB vs 16 MB young potu), to by bylo moc drahý.
    - budu si držet tabulku objektů, který jsou "asi" rooty (tj. mají reference do young)
      můžu tam ale mít objekty, který rooty nejsou, je to relaxovaná podmínka
      -> Remembered Set
    - Do tohohle setu přidám objekty:
        - který kopíruju (tenuruju) do OLD
        - objekty, u kterých volám settery (nová reference může bejt do young)
    - Pro každej store musím mít write-barrier
    - Při dalším scavengi (GC) projdu všechny obj z remembered setu,
    a když žádná jejich reference nemíří do young potu, tak je ze setu vyndam.

- ALE když potřebuju GC na OLD potu, nebudu to dělat timhle algoritmem, protože bych
musel překopírovávat 1GB+ paměti, to je pomalý.

- DONT POOL! tady to dělá větší overhead. Původně měla java M&S a dostala špatný jméno.
Teď už je to v pohodě.
- Potů může bejt víc, jsou poty pro symboly, fce, lambdy, atd.

- Problém - *early tenure*
    - Přesunul jsem objekt do OLD, ale hned za chvíli umře.

- Worst case scenario pro algoritmus:
    - Všechno v edenu přežije, postupně mi dochází paměť v new -
        pořád něco kopíruju tam a zpátky, a moc paměti nezískávám.
    - Proto dynamicky, např.
        - 95% plný -> threshold bude 1, všechno zkopíruju
        - 90% plný -> např. 2
        - atd.
        - A naopak, pokud je to prakticky prázdný, nechci nic dávat do OLD.

    - Hodnoty jsou jakási heuristika, záleží na aplikaci.


#### Tricolor marking - neblokující Mark&Sweep
- Mám tři barvy, bílá, šedá, černá
- Mark fáze je pořád stejně, označuju bílá -> černá
- Program pořád běží v pozadí, ale když použiju nějakej objekt co je černej, změním
  ho na šedý a přidám do todo listu. Když Mark komplet doběhne, podívá
  se do todo listu - jestli tam něco je, tak tam skočím a půjdu po šedých/bílých a barvím do černa.
- Když program mezitím zase něco program zapíše, zas to dá na todo a tak furt dokola.
- Když bojuju s programem o pár objektů v todo listu, po čase prostě řeknu zdar, program zastavím a dodělám GC.
- Nevýhoda je fragmentace paměti. Můžu to řešit background jobem co mi kopíruje objekty v paměti k sobě.

# 6. přednáška

Trik, abych nemusel pro každej int alokovat novej objekt -
- Všechny pointery jsou zarovnvaný na int, ten má 32bitů. Na konci
  adresy vždycky budou dvě nuly.
- Když mám dostatečně malej integer, kterej se vejde do 31 bitů, tak
  zneužiju pointer -> řeknu si, pokud je poslední bit pointeru 1, tak
  to budu považovat za přímou hodnotu intu, tj. udělam posun doprava a mam číslo.
- Když má poslední bit 0, tak to je klasická adresa a dereferencuju.
- *Tagged integers*

## JIT
#### Code generation - template based
- Každá instrukce bytekódu se přeloží na sekvenci strojových instrukcí
- Tady je problém, že nevím co bude za instrukce. Stack bude bottleneck,
  protože nejsem schopnej inteligentně říct, do jakých registrů dávat co, tak to řeším přes stack.

#### Abstract interpretation
- V JITu se chovám skoro jako bych vykonával instrukce.
- Simuluju stav, ve kterým by počítač byl ve chvíli kdy to vykonávám
    - "Stack state"
    - "Register state"
    - V programu si držím datovou strukturu, kde mám napsáno co je v
      jakém registru.
    - Ve chvíli kdy zavolám "PUSH_ARG", tak hned negeneruju kód, ale jenom
      si to uložím do stack state - simulace stacku. Tohle dělám dokud
      nepotřebuju hodnoty.
    - Přijde instrukce co potřebuje hodnotu - ADD. Ok, vím že potřebuju
      dvě hodnoty. Najdu si free registry, zarezervuju, a vygeneruju kód
      na load hodnoty do těch registrů.
    - Stav registrů si držím v simulovaným stacku, efektivně řeším
      obsazení registrů

#### Inline cache
Nechci skákat po třídách, držím si machine kód metod u sebe

# Literatura
- Baker - uniprocessor garbage collection algorithms
- Abelson, Sussman - Structure & Interpretation of Computer Programs (MIT povinný čtení) + je to na youtube (tuhle by si prej klaus vzal na opuštěnej ostrov!)
- Podívat se na Haskell (tam je "lazy předávání parametrů" - všude se předávají lambdy), ML, OZ/Mozart
- Ph.D. thesis Dave Unger - SOAR

# Kontakt
- cg@exept.de - Klaus
- bg@ - wife
- info@