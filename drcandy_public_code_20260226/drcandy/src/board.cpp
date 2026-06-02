#include "board.h"
#include <memory>
#include <iostream>
#include <fstream>
#include <string>

/*NOTA IMPORTANT SOBRE ELS VECTORS I PUNTERS:

std::vector<Candy*> resultat = board.explodeAndDrop();

for (Candy* c : resultat)
{
    delete c;
}
    aixo sera per quan ja haguem utilitzat la funcio de vectors per esborrar de la memoria els punters i d'aquesta manera no provocar una fuita de info
*/

Board::Board(int width, int height)
{
    m_boardWidth = width;
    m_boardHeight = height;

    for (int x = 0; x < DEFAULT_BOARD_WIDTH; x++)
    {
        for (int y = 0; y < DEFAULT_BOARD_HEIGHT; y++)
        {
            m_cell[x][y] = nullptr;
            m_boardExplode[x][y] = false;
        }
    }
}

Board::~Board()
{
    for (int i = 0; i < DEFAULT_BOARD_WIDTH; i++)
    {
        for (int j = 0; j < DEFAULT_BOARD_HEIGHT; j++)
        {
            if (m_cell[i][j] != nullptr)
            {
                delete m_cell[i][j];
            }
            m_cell[i][j] = nullptr;
            m_boardExplode[i][j] = false;
        }
    }
}


const Candy* Board::getCell(int x, int y) const
{
    const Candy* resultat = nullptr;

    if (x >= 0 && x < m_boardWidth &&
        y >= 0 && y < m_boardHeight)
    {
        resultat = m_cell[x][y];
    }

    return resultat;
}

void Board::setCell(Candy* candy, int x, int y)
{
    if (x >= 0 && x < m_boardWidth &&
        y >= 0 && y < m_boardHeight)
    {
        m_cell[x][y] = candy;
    }
}


int Board::getWidth() const 
{
    int resultat = m_boardWidth;
    return resultat;
}

int Board::getHeight() const 
{
    int resultat = m_boardHeight;
    return resultat;
}

bool Board::shouldExplode(int x, int y)
{
    reiniciaExplosions(); //tremendament ineficient, pero com el test només executa board explode s'ha de fer així
    boardExplode();
   
    bool resultat = false;

    if (casellaValida(x, y) && m_cell[x][y] != nullptr)
    {
       resultat = m_boardExplode[x][y];
    }

    return resultat;

}

void Board::boardExplode()
{
   // files
    for (int y = 0; y < m_boardHeight; y++)
    {
       shouldExplodeFila(y);
    }


   // columnes
    for (int x = 0; x < m_boardWidth; x++)
    {
       shouldExplodeColumna(x);
    }


   // diagonals cap avall
    for (int y = 0; y < m_boardHeight; y++)
    {
        for (int x = 0; x < m_boardWidth; x++)
        {
            if (y + SHORTEST_EXPLOSION_LINE - 1 < m_boardHeight)
            {
                shouldExplodeDiagonalDown(x, y);
            }
        }
    }

   // diagonals cap amunt
    for (int y = 0; y < m_boardHeight; y++)
    {
        for (int x = 0; x < m_boardWidth; x++)
        {
            if (y - (SHORTEST_EXPLOSION_LINE - 1) >= 0)
            {
                shouldExplodeDiagonalUp(x, y);
            }
        }
    }
}



void Board::shouldExplodeColumna(int x)
{
   int y = 0;


   while (casellaValida(x, y))
   {
       if (m_cell[x][y] == nullptr)
       {
           y++;
       }
       else
       {
           Candy actual = *m_cell[x][y];
           int count = 1;


           while (casellaValida(x, y + count) &&
                  m_cell[x][y + count] != nullptr &&
                  m_cell[x][y + count]->getType() == actual.getType())
           {
               count++;
           }


           if (count >= SHORTEST_EXPLOSION_LINE)
           {
               for (int j = y; j < y + count; j++)
               {
                   m_boardExplode[x][j] = true;
               }  
           }

           y += count;
       }
   }
}



void Board::shouldExplodeFila(int y)
{
    int x = 0;


    while (casellaValida(x, y))
    {
        if (m_cell[x][y] == nullptr)
        {
            x++;
        }
        else
        {
            Candy actual = *m_cell[x][y];
            int count = 1;


            while (casellaValida(x + count, y) &&
                   m_cell[x + count][y] != nullptr &&
                   m_cell[x + count][y]->getType() == actual.getType())
            {
                count++;
            }

            if (count >= SHORTEST_EXPLOSION_LINE)
            {
                for (int j = x; j < x + count; j++)
                {
                    m_boardExplode[j][y] = true;
                }
            }

            x += count;
        }
    }
}



void Board::shouldExplodeDiagonalDown(int x, int y)
{
    if (casellaValida(x, y))
    {
        if (m_cell[x][y] != nullptr)  // <- sin la condicion del boardExplode
        {
            Candy actual = *m_cell[x][y];
            int count = 1;

            while (casellaValida(x + count, y + count) &&
                   m_cell[x + count][y + count] != nullptr &&
                   m_cell[x + count][y + count]->getType() == actual.getType())
            {
                count++;
            }

            if (count >= SHORTEST_EXPLOSION_LINE)
            {
                for (int k = 0; k < count; k++)
                {
                    m_boardExplode[x + k][y + k] = true;
                }
            }
        }
    }
}


/* les funcions aquestes nomes tenen en compte una sola y, 
    això està fet a proposit per simplificar la lectura del codi 
    i es pot solucionar de forma facil implementant un simple for a l'hora de cridar la funció
*/


void Board::shouldExplodeDiagonalUp(int x, int y)
{
    if (casellaValida(x, y))
    {
        if (m_cell[x][y] != nullptr)  // <- sin la condicion del boardExplode
        {
            Candy actual = *m_cell[x][y];
            int count = 1;

            while (casellaValida(x + count, y - count) &&
                   m_cell[x + count][y - count] != nullptr &&
                   m_cell[x + count][y - count]->getType() == actual.getType())
            {
                count++;
            }

            if (count >= SHORTEST_EXPLOSION_LINE)
            {
                for (int k = 0; k < count; k++)
                {
                    m_boardExplode[x + k][y - k] = true;
                }
            }
        }
    }
}



bool Board::casellaValida(int x, int y) const
{
    bool resultat = false;


    if (x >= 0 && x < m_boardWidth &&
        y >= 0 && y < m_boardHeight)
    {
        resultat = true;
    }


    return resultat;
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////


void Board::reiniciaExplosions()
{
    for (int x = 0; x < m_boardWidth; x++)
    {
        for (int y = 0; y < m_boardHeight; y++)
        {
            m_boardExplode[x][y] = false;
        }
    }
}


bool Board::hiHaExplosions() const
{
    bool resultat = false;
    int x = 0;


    while (x < m_boardWidth && !resultat)
    {
        int y = 0;


        while (y < m_boardHeight && !resultat)
        {
            if (m_boardExplode[x][y])
            {
                resultat = true;
            }
            y++;
        }
        x++;
    }


    return resultat;
}


void Board::eliminaExplosions(std::vector<Candy*>& exploded)
{
    for (int x = 0; x < m_boardWidth; x++)
    {
        for (int y = 0; y < m_boardHeight; y++)
        {
            if (m_boardExplode[x][y] && m_cell[x][y] != nullptr)
            {
                exploded.push_back(new Candy(*m_cell[x][y]));
                m_cell[x][y] = nullptr;
            }
        }
    }
}


void Board::fesCaureColumnes()
{
    for (int x = 0; x < m_boardWidth; x++)
    {
        int writeY = m_boardHeight - 1;


        for (int y = m_boardHeight - 1; y >= 0; y--)
        {
            if (m_cell[x][y] != nullptr)
            {
                m_cell[x][writeY] = m_cell[x][y];


                if (writeY != y)
                {
                    m_cell[x][y] = nullptr;
                }


                writeY--;
            }
        }


        while (writeY >= 0)
        {
            m_cell[x][writeY] = nullptr;
            writeY--;
        }
    }
}


std::vector<Candy*> Board::explodeAndDrop()
{
    std::vector<Candy*> exploded;
    bool hiHaExplosionsAlTauler = true;


    while (hiHaExplosionsAlTauler)
    {
        reiniciaExplosions();
        boardExplode();


        hiHaExplosionsAlTauler = hiHaExplosions();


        if (hiHaExplosionsAlTauler)
        {
            eliminaExplosions(exploded);
            fesCaureColumnes();
        }
    }


    return exploded;
}


////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

bool Board::dump(const std::string& output_path) const //DONE
{
    bool result = false;


    std::ofstream fitxer;
    fitxer.open(output_path);


    if (fitxer.is_open())
    {
        fitxer << "m_board" << std::endl; 
        for (int i = 0; i < m_boardWidth; i++)
        {
            for (int j = 0; j < m_boardHeight; j++)
            {


                if (m_cell[i][j] == nullptr)
                {
                    fitxer << ". ";
                }
                else
                {
                    CandyType c = m_cell[i][j]->getType();


                    switch(c)
                    {
                        case CandyType::TYPE_RED:    fitxer << "R "; break;
                        case CandyType::TYPE_BLUE:   fitxer << "B "; break;
                        case CandyType::TYPE_GREEN:  fitxer << "G "; break;
                        case CandyType::TYPE_YELLOW: fitxer << "Y "; break;
                        case CandyType::TYPE_PURPLE: fitxer << "P "; break;
                        case CandyType::TYPE_ORANGE: fitxer << "O "; break;
                        default: fitxer << "? "; break;
                    }
                }


            }
            fitxer << std::endl;
        }


        fitxer.close();
        result = true;
    }


    return result;
}

bool Board::load(std::istream& in)
{
    bool result = false;

    char c;
    bool error = false;
    std::string header;
    if (!(in >> header)) 
    {
        error = true;
    }
    else
    {
        
    }
    if (header == "m_board")
    {
        for (int i = 0; i < m_boardWidth; i++)
        {
            for (int j = 0; j < m_boardHeight; j++)
            {
                if (m_cell[i][j] != nullptr)
                {
                    delete m_cell[i][j];
                }
                m_cell[i][j] = nullptr;
                m_boardExplode[i][j] = false;
            }
        }

        for (int i = 0; i < m_boardWidth && !error; i++)
        {
            for (int j = 0; j < m_boardHeight && !error; j++)
            {
                if (!(in >> c))
                {
                    error = true;
                }
                else if (c == '.')
                {
                    m_cell[i][j] = nullptr;
                }
                else
                {
                    CandyType tipus;
                    bool valid = true;

                    switch (c)
                    {
                        case 'R': tipus = CandyType::TYPE_RED; break;
                        case 'B': tipus = CandyType::TYPE_BLUE; break;
                        case 'G': tipus = CandyType::TYPE_GREEN; break;
                        case 'Y': tipus = CandyType::TYPE_YELLOW; break;
                        case 'P': tipus = CandyType::TYPE_PURPLE; break;
                        case 'O': tipus = CandyType::TYPE_ORANGE; break;
                        default: valid = false; break;
                    }

                    if (!valid)
                    {
                        error = true;
                    }
                    else
                    {
                        m_cell[i][j] = new Candy(tipus);
                    }
                }
            }
        }
    }
    else
    {
        error = true;
    }

    result = !error;
    return result;
}

bool Board::load(const std::string& input_path)
{
    std::ifstream fitxer(input_path);
    if (!fitxer.is_open()) return false;
    bool res = load(fitxer);
    fitxer.close();
    return res;
}