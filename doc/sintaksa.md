naredbe završavaju znakom `;` ili `}`(ako postoji nezatvoren `{`)

### komentari
    # kroz jedan redak
    # razmisliti o sintaksi za kroz više retka

### tipovi podataka
    bool
    int8, int16, int32, int64
    uint8, uint16, uint32, uint64
    float32, float64, float96
    string

#### stringovi
- stringovi su 

#### polja
`lana = bool[]` definira polje boolova
`lana = int32[7]` definira polje intova veličine 7
`lana = {1, 2, 3, 4}` definira polje intova veličine 4 s navedenim vrijednostima
`lana = float[n]` definira polje floatova veličine n
- indeksiranje od 0
- moguce operacije `[2:3]`, `[0:7:2]` itd kao u pythonu

### varijable
zapocinju slovom ili znakom `_` i slovom, ne sadrže znakove `+, -, *, /, %, |, &...`, nisu rezervirane riječi (`and, or, xor, int...`)
moguca definicija bez navođenja tipa podatka, npr `lana = 3`, tada varijabla dobiva tip podatka koji joj se pridruži
moguca definicija uz navođenje tipa podatka, npr `lana = int`
nije moguca promjena tipa podatka neke varijable

### funkcije
#### definicija funkcije
    foo(a = int, b = bool) = double
    {
        return 0.5;
    }

funkcija `foo` prima integer i bool, a vraca double

    foo2(a = int)
    {
        //some code
    }

funkcija `foo2` prima int, a vraca void

#### poziv funkcije
    foo(3, True)


### klase
#### definija klase
    class Lana(Base1, Base2...)
    {
        public_member1 = int;
        private:
            private_member1 = float;

            private_method1(a = int)
            {

            }
        
        public:
            Lana(constructor_argument1=int)
            {
                public_member1 = constructor_argument1;
            }

            Lana()
            {
                //second constructor
            }
    }


#### stvaranje objekata
    lana_instance = Lana();
    lana_instance.public_member1 = 7;

### operatori

#### Pridruživanja 
    =
Moguce je:

    x = y = z = 5;
    x = 7 + (y = 5); // y = 5, x = 12

#### Aritmetički
##### Unarni
    +, - 

##### Binarni
        +, -, *, /, % 
    
#### Rad s bitovima
        |, ~, ^, &, >>, <<

#### Usporedbe
        ==, !=, <, <=, >, >=

#### Logički
        and, or, xor, not

#### pre/post - increment/decrement/multiply/divide
`++`, `+++`(uvecaj za 2), `++++`(uvecaj za 3) ...(nije preporuceno za brojeve vece od 3 zbog nejasnoce citanja jezika) - VIDI SINTAKSU ISPOD
`++1`, `++2`, `++3`, `++n` (n realni broj - NE VARIJABLA) - uvecaj za 1, 2, 3, n
        
isto vrijedi i za -, *, /, %, |, &, ^

rezultati se spremaju u varijablu prije, odnosno nakon izvođenja naredbe
**NAPOMENA:** `x**5` znaci x pomnozeno sa 5, a ne x^5 kao u Pythonu
**NAPOMENA:** `x**`, `x//` očito ne mijenjaju vrijednost varijable x, no nisu sintaksno pogrešni, 
dok `x%%` postavlja vrijednost na 0, nije preporuceno korištenje ovih naredba zbog nejasnoce i nepotrebnosti.

Primjer korištenja:

    int x = 5, y;
    y = 3 + 7++x;
    print(x, y); //TODO format print naredbe

ispisuje: 15, 12
        
Gornji kod je ekvivalentan:

    int x = 5, y;
    y = 3 + ++++++++x;
    print(x, y);
    
##### increment za vrijednost varijable
prirodno se postavlja pitanje kako povećati vrijednost varijable x za vrijednost varijable y,
tj. napraviti ono što bi se u većini programskih jezika napisalo naredbom `x+=y;`  

Jasno je da naredba `x+=y` nije dovoljno dobra za naše potrebe, jer iako omogućava računanje izraza korištenjem 
prefix incrementa, ne omogućava postfix increment.

Naivan pristup rješavanju ovog problema, bio bi uvođenje sintakse: `x++y`, no ovime nije jasno želimo li promijeniti vrijednost varijable x ili y, tj koristimo li prefix ili postfix. 
**TODO** možda sintaksa `x++y` napravi i postfix x i prefix y

**Rješenje problema**
`x++(y)` označava da se mijenja vrijednost varijable x
`(x)++y` označava da se mijenja vrijednost varijable y

u ovakvoj sintaksi je moguce da se u zagradi pojavi bilo koji izraz, pa cak i `x++`, npr `x++(2+x-y)` sto omogucava sve funkcionalnosti operatora `+=` te dodaje nove

#### prioritet operatora
1. zagrade
2. Unarni `+`, Unarni `-`, `~`
3.  `<<`, `>>`
4. `&`
5. `|`, `^`
6. `*`, `/`, `%`
7. binarni `+`, `-`
8. `&&`, `||`,  `^^`, `**`, `//`, `%%`, `++`, `--`, `=`
9. `==`, `!=`, `<`, `<=`, `>`, `>=`
10. `not`
11. `and`
12. `or`, `xor`

### if naredba
    if(uvjet) {

    } else {

    }

### for petlja
    for(pocetna_naredba; uvjet; krajnja_naredba) {

    }

### while petlja
    while(uvjet) {

    }

### ispis podataka
    print(a, b, c)

### unos podataka
    input(a, b, c)