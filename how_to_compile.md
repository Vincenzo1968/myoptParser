To create the static library:
-----------------------------

    gcc -c -Wall -W -O2 -pedantic myoptParser.c myoptLexer.c myoptSymtab.c
    ar  -r -s libmyoptParser.a myoptParser.o myoptLexer.o myoptSymtab.o

To use the library:
-------------------

    gcc -Wall -W -O2 -pedantic -L. sample1.c -lmyoptParser -o sample1
    gcc -Wall -W -O2 -pedantic -L. sample2.c -lmyoptParser -o sample2

    g++ -Wall -W -O2 -pedantic -L. sample1.cpp -lmyoptParser -o sample1cpp
    g++ -Wall -W -O2 -pedantic -L. sample2.cpp -lmyoptParser -o sample2cpp
