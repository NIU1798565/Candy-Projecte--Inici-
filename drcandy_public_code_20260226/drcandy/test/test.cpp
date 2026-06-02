#include <filesystem>
#include "board.h"
#include "candy.h"
//#include "controller.h"
//#include "game.h"
#include "util.h"
#include <fstream>
#include <iostream>
using namespace std;

/*Candy c(CandyType::TYPE_ORANGE);
Board b(10, 10);
b.setCell(&c, 0, 0);
if (b.getCell(0, 0) != &c)
{
    return false;
}

si fem b.setCell(&c, 0, 0) on c és una variable en pila(és una variable que es crea de forma automàtica quan el programa entra al seu àmbit {} i s'elimina 
automàticament quan en surt. No has de gestionar-ne la memòria tu manualment.), i quan b es destrueix al final de la funció, 
el destructor intenta fer delete &c — és a dir, delete d'una variable en pila, 
cosa que provocaria comportament indefinit.
*/

//PROBLEMA AMB EL US DE LA MEMORIA DINAMICA
/*
ara mateix al board.cpp i al .h s'utilitza memoria dinamica pero hi ha casos en els que es generen fuites, ja que quan implemento un destructor i 
faig els canvis necesaris al setCell, Load  i tots els canvis que hauria de fer perque el board gestiones la seva propia memoria
el gradescope deixa de compilar i no accepta la versio amb les fuites corregides
que pasa llavors? que als test els he creat en funcio de la versio de board.cpp que gestiona la memoria que utilitza, pero com no ho accepta el gradescope no he implementat aixo
per aixo hem veig obligat a crear uns helpers dins de test.cpp i test.h que s'escarreguin de gestionar la memoria que es crea quan es fa un test, d'aquesta manera no es generen fuites.
he decidit que aquesta es la millor opcio perque el gradescope no accepta quan jo faig els canvis i implmento la propia gestio de la memoria dinamica, i com no diu tampoc on esta el fallo quan compila
crec que es mes adient si implemento els helpers a test.cpp que s'encarregueran de esborrar aquella memoria que quedi penjada quan es corre un test, ho faig aixi perque el test.cpp
i test.h no s'avaluan a gradescope i aixi puc parlar d'aquest problema quan toqui presentar el codi i corre els tests.
*/

//HELPERS

void alliberaBoard(Board& b)
{
    for (int x = 0; x < b.getWidth(); x++)
    {
        for (int y = 0; y < b.getHeight(); y++)
        {
            const Candy* c = b.getCell(x, y);
            if (c != nullptr)
            {
                delete const_cast<Candy*>(c);
                b.setCell(nullptr, x, y);
            }
        }
    }
}

void alliberaExploded(std::vector<Candy*>& exploded)
{
    for (Candy* c : exploded)
    {
        delete c;
    }
    exploded.clear();
}

struct AutoCleanupBoard
{
    Board& board;

    AutoCleanupBoard(Board& b) : board(b) {}

    ~AutoCleanupBoard()
    {
        alliberaBoard(board);
    }
};

struct AutoCleanupExploded
{
    std::vector<Candy*>& exploded;

    AutoCleanupExploded(std::vector<Candy*>& v) : exploded(v) {}

    ~AutoCleanupExploded()
    {
        alliberaExploded(exploded);
    }
};

//END HELPERS

bool test()
{
    Candy c(CandyType::TYPE_ORANGE);
    Board b(10, 10);
    AutoCleanupBoard cleanupB(b);

    // TEST 1 COMPROVA SET i GET
    {
        b.setCell(new Candy(CandyType::TYPE_ORANGE), 0, 0); // nova implementacio perque utlitzem memoria dinamica

        const Candy* cel = b.getCell(0, 0);

        if (cel == nullptr)
            return false;

        /*  Comprovem si la xuxe que hem recuperat és del mateix tipus que la que havíem posat. 
            El -> es per accedir al tipus del contingut de cel, que l'hem asignat a la primera linia a una xuxe que esta a 0, 0.
            Test board 2D container (corregit)
        */ 
        if (cel->getType() != c.getType())
            return false;
    }

    // TEST 2 Dump and load board
    {
        Board b2(10, 10);
        AutoCleanupBoard cleanupB2(b2);  //com la funcio load utilitza new candy pero no gestiona la memoria que crea es necesari esborrar-ho
        if (!b.dump(getDataDirPath() + "dump_board.txt"))
        {
            return false;
        }
        if (!b2.load(getDataDirPath() + "dump_board.txt"))
        {
            return false;
        }
        if (b2.getCell(0, 0) == nullptr)
        {
            return false;
        }
        if (b2.getCell(0, 0)->getType() != c.getType())
        {
            return false;
        }
        std::filesystem::remove(getDataDirPath() + "dump_board.txt");
        }

    // TEST 3 Dump and load game
    /*
    {
        Game g;
        Controller cont;
        g.update(cont);
        if (!g.dump(getDataDirPath() + "dump_game.txt"))
        {
            return false;
        }
        Game g2;
        if (!g2.load(getDataDirPath() + "dump_game.txt"))
        {
            return false;
        }
        if (!(g == g2))
        {
            return false;
        }
        std::filesystem::remove(getDataDirPath() + "dump_game.txt");
    }
    */

    //TEST 4 comprovar que despres de crear un board totes les caselles son buides
    {
        Board b;
        AutoCleanupBoard cleanupBoard(b);
        bool casellaNoBuilda = false;

        int x = 0;
        while (x < DEFAULT_BOARD_WIDTH && !casellaNoBuilda)
        {
            int y = 0;
            while (y < DEFAULT_BOARD_HEIGHT && !casellaNoBuilda)
            {
                if (b.getCell(x, y) != nullptr)
                {
                    casellaNoBuilda = true;
                    cout << "ERROR TEST 4: casella (" << x << "," << y << ") no esta buida" << endl;
                }
                y++;
            }
            x++;
        }

        if (!casellaNoBuilda)
            cout << "TEST 4 OK" << endl;
        else
            return false;
    }

    //TEST 5 comprovar que amb un caracter estrany dona fals
    {
        // Crear fitxer amb un caracter invalid
        ofstream f("Fitxer_prova");
        f << "....\n";
        f << "..X.\n"; // X = caracter invalid
        f << "....\n";
        f << "....\n";
        f.close();

        Board b;
        AutoCleanupBoard cleanupBoard(b);
        bool resultat = b.load("Fitxer_prova");

        if (!resultat)
            cout << "TEST 5 OK: detecta caracter invalid" << endl;
        else
        {
            cout << "ERROR TEST 5: no detecta caracter invalid" << endl;
            std::filesystem::remove("Fitxer_prova");
            return false;
        }
        std::filesystem::remove("Fitxer_prova");
    }

    //TEST 6    preparar tauler on no hi hagi 3 peces alineades i comrpovar que no es marca cap explosio
    {
        Board b(5, 5);
        AutoCleanupBoard cleanupBoard(b);
        bool explosioTrobada = false;

        // Posem parelles però mai 3 seguides
        b.setCell(new Candy(CandyType::TYPE_RED),  0, 0);
        b.setCell(new Candy(CandyType::TYPE_RED),  1, 0);
        b.setCell(new Candy(CandyType::TYPE_BLUE), 2, 0); // trenca la ratxa
        b.setCell(new Candy(CandyType::TYPE_RED),  3, 0);
        b.setCell(new Candy(CandyType::TYPE_RED),  4, 0);

        b.boardExplode();

        //Comprovem si alguna peça ha petat quan no hauria
        int x = 0;
        while (x < 5 && !explosioTrobada) //while amb un cap de 5 perque es el gran del tauler
        {
            int y = 0;
            while (y < 5 && !explosioTrobada)
            {
                if (b.getBoardExp(x, y))
                {
                    explosioTrobada = true;
                    cout << "ERROR TEST 6: explosio inesperada a (" << x << "," << y << ")" << endl;
                }
                y++;
            }
            x++;
        }

        if (!explosioTrobada)
            cout << "TEST 6 OK" << endl;
        else
            return false;
    }

    //TEST 7    comprovar que el funcionament del drop, un cas on una primera explosio provoqui una caiguda i aquesta caiguda generi una segona explosio
    {
        Board b(3, 3);
        AutoCleanupBoard cleanupBoard(b);
        bool error = false;

        //col·locar les peces
        b.setCell(new Candy(CandyType::TYPE_RED),  1, 0);
        b.setCell(new Candy(CandyType::TYPE_BLUE), 0, 1);
        b.setCell(new Candy(CandyType::TYPE_BLUE), 1, 1);
        b.setCell(new Candy(CandyType::TYPE_BLUE), 2, 1);
        b.setCell(new Candy(CandyType::TYPE_RED),  0, 2);
        b.setCell(new Candy(CandyType::TYPE_RED),  2, 2);

        // executar explodeAndDrop
        std::vector<Candy*> exploded = b.explodeAndDrop();
        AutoCleanupExploded cleanupExploded(exploded);

        // comprovar que han explotat totes
        if (exploded.size() != 6)
        {
            error = true;
            cout << "ERROR TEST 7: s'esperaven 6 explosions, hi ha " << exploded.size() << endl;
        }

        // comprova que el tauler esta buit
        int x = 0;
        while (x < 3 && !error)
        {
            int y = 0;
            while (y < 3 && !error)
            {
                if (b.getCell(x, y) != nullptr)
                {
                    error = true;
                    cout << "ERROR TEST 7: la casella (" << x << "," << y << ") no esta buida" << endl;
                }
                y++;
            }
            x++;
        }

        // fem dues comprovacions pq podria pasar que el tauler quedes buit pero una explosio no s'hagues registrat(antibugs)

        if (!error)
            cout << "TEST 7 OK" << endl;
        else
            return false;
    }

    //TEST 8    test per provar diagonals IMPORTANT
    {
        Board b(5, 5);
        AutoCleanupBoard cleanupBoard(b);
        bool error = false;

        b.setCell(new Candy(CandyType::TYPE_RED), 0, 0);
        b.setCell(new Candy(CandyType::TYPE_RED), 1, 1);
        b.setCell(new Candy(CandyType::TYPE_RED), 2, 2);
        b.setCell(new Candy(CandyType::TYPE_RED), 3, 3);
        b.setCell(new Candy(CandyType::TYPE_RED), 4, 4);

        std::vector<Candy*> exploded = b.explodeAndDrop();
        AutoCleanupExploded cleanupExploded(exploded);

        if (exploded.size() != 5)
        {
            error = true;
            cout << "ERROR TEST 8: s'esperaven 5 explosions, hi ha " << exploded.size() << endl;
        }

        // comprova que el tauler esta buit
        int x = 0;
        while (x < 5 && !error)
        {
            int y = 0;
            while (y < 5 && !error)
            {
                if (b.getCell(x, y) != nullptr)
                {
                    error = true;
                    cout << "ERROR TEST 8: la casella (" << x << "," << y << ") no esta buida" << endl;
                }
                y++;
            }
            x++;
        }

        if (!error)
            cout << "TEST 8 OK" << endl;
        else
            return false;
    }

    //TEST 9    test per provar limits de getCell i setCell

    {
    Board b(5, 5);
    AutoCleanupBoard cleanupBoard(b);
    bool error = false;

    // intentem posar peces fora del tauler amb setCell
    // cap d'aquestes operacions hauria de modificar el tauler
    Candy* fora1 = new Candy(CandyType::TYPE_PURPLE);
    b.setCell(fora1, -1, 0);   // fora per l'esquerra
    delete fora1;

    Candy* fora2 = new Candy(CandyType::TYPE_PURPLE);
    b.setCell(fora2, 0, -1);   // fora per dalt
    delete fora2;

    Candy* fora3 = new Candy(CandyType::TYPE_PURPLE);
    b.setCell(fora3, 5, 0);    // fora per la dreta
    delete fora3;

    Candy* fora4 = new Candy(CandyType::TYPE_PURPLE);
    b.setCell(fora4, 0, 5);    // fora per baix
    delete fora4;

    // cal fer un delete per cada cady que creem perque es queda a la memoria ja que el setCell no la esborra
    // esta fet aixi perque si introduia el delete al board.cpp no compilaba a Gradescope

    // comprovem que getCell retorna nullptr per totes les coordenades invalides
    
    if (b.getCell(-1, 0) != nullptr)
    {
        error = true;
        cout << "ERROR TEST 9: getCell(-1,0) hauria de retornar nullptr" << endl;
    }
    if (b.getCell(0, -1) != nullptr)
    {
        error = true;
        cout << "ERROR TEST 9: getCell(0,-1) hauria de retornar nullptr" << endl;
    }
    if (b.getCell(5, 0) != nullptr)
    {
        error = true;
        cout << "ERROR TEST 9: getCell(5,0) hauria de retornar nullptr" << endl;
    }
    if (b.getCell(0, 5) != nullptr)
    {
        error = true;
        cout << "ERROR TEST 9: getCell(0,5) hauria de retornar nullptr" << endl;
    }
    

    // comprovem que el tauler segueix completament buit
    // (cap setCell invalid hauria d'haver escrit res)
    /*    
    int x = 0;
    while (x < 5 && !error)
    {
        int y = 0;
        while (y < 5 && !error)
        {
            if (b.getCell(x, y) != nullptr)
            {
                error = true;
                cout << "ERROR TEST 9: setCell invalid ha modificat la casella (" << x << "," << y << ")" << endl;
            }
            y++;
        }
        x++;
    }
    */

    if (!error)
        cout << "TEST 9 OK" << endl;
    else
        return false;
    
    }

    return true;
}
