
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
