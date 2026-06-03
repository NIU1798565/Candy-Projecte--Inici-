#include "game.h"
#include <random>
#include "graphics.h"
#include "candy.h"
#include <fstream>

using namespace std; 
Game::Game()
{
    m_puntuacio = 0;
    m_falling[0] = new Candy(static_cast<CandyType>(rand() % static_cast<int>(CandyType::COUNT)));
    m_falling[1] = new Candy(static_cast<CandyType>(rand() % static_cast<int>(CandyType::COUNT)));
    m_falling[2] = new Candy(static_cast<CandyType>(rand() % static_cast<int>(CandyType::COUNT)));

    m_fallingX = 4;
    m_fallingY = 0;
    m_fallTimer = 30; /* comptador que anem decrementant a cada 
                      frame que pasa decreix de manera que a meitat 
                      de cicle (60 frames) s'actualitza */
    m_gameOver = false;
    m_vertical = true;
}

Game::~Game() //nomes es crida quan el programa tanca
{
    delete m_falling[0];
    delete m_falling[1];
    delete m_falling[2];
}

void Game::update(const Controller& controller)
{
    // 3 blocs que es repeteixen a cada frame en ordre

    // 1: Llegir les tecles del controller
    if(m_gameOver == false)
    {

        if (controller.isLeftPressed() && m_fallingX > 0)
        {
            m_fallingX--;
        }

        if (controller.isRightPressed() && m_fallingX < m_board.getWidth() - 1)
        {
            m_fallingX++;
        }

        if (controller.isDownPressed() && m_fallingY < m_board.getHeight() - 3)
        {
            m_fallingY++;
        }

        // Tecla Q: rotar caramels del bloc (desplaçament cíclic cap amunt)
        if (controller.isKey1Pressed())
        {
            Candy* tmp = m_falling[0];
            m_falling[0] = m_falling[1];
            m_falling[1] = m_falling[2];
            m_falling[2] = tmp;
        }

        // Tecla W: guardar la partida
        if (controller.isKey2Pressed())
        {
            dump("data/save.txt");
        }

    // 2: Fer baixar el bloc automaticament

        m_fallTimer--;
        if (m_fallTimer <= 0)
        {
            m_fallingY++;
            m_fallTimer = 60;
        }

    // 3: Detectar si el bloc ha aterrat (ocupa y, y+1, y+2)
        if (m_fallingY >= m_board.getHeight() - 3 ||
            m_board.getCell(m_fallingX, m_fallingY + 3) != nullptr)
        {
            m_board.setCell(m_falling[0], m_fallingX, m_fallingY);
            m_board.setCell(m_falling[1], m_fallingX, m_fallingY + 1);
            m_board.setCell(m_falling[2], m_fallingX, m_fallingY + 2);

            std::vector<Candy*> explotats = m_board.explodeAndDrop();
            m_puntuacio += explotats.size();
            for(Candy* c : explotats) delete c;
            /*Assignem el valor d'explotats a una variable que sumarem al total de puntuacio que tenim de manera que a cada cicle s'actualitzi punutacio
            tambe esborrem, un cop acabada l'assignació, la memoria que ocupem creant la variable explotats */

            m_falling[0] = new Candy(static_cast<CandyType>(rand() % static_cast<int>(CandyType::COUNT)));
            m_falling[1] = new Candy(static_cast<CandyType>(rand() % static_cast<int>(CandyType::COUNT)));
            m_falling[2] = new Candy(static_cast<CandyType>(rand() % static_cast<int>(CandyType::COUNT)));
            m_fallingX = 4;
            m_fallingY = 0;
            m_fallTimer = 60;

            if(m_board.getCell(4,0) != nullptr || m_board.getCell(4,1) != nullptr || m_board.getCell(4,2) != nullptr)
            {
                m_gameOver = true;
            }
        }
    }
}

void Game::render(GraphicManager& graphics)
{
    // Note: the following code exhibits the main graphic library features
    // Board: border [draw rectangles] and a single piece of candy
    const int board_size = 10;
    const int board_padding = 3;

    //Part 1: Dibuixar el tauler
        for (int x = 0; x < m_board.getWidth(); x++)
        {
            for(int y = 0; y < m_board.getHeight(); y++)
            {
                const Candy* c = m_board.getCell(x,y);
                if(c != nullptr)
                {
                    graphics.drawImage(c->getResourceName(), 
                    board_padding * CANDY_IMAGE_WIDTH + x * CANDY_IMAGE_WIDTH,
                    board_padding * CANDY_IMAGE_HEIGHT + y * CANDY_IMAGE_HEIGHT);
                    /* columna × mida + marge = posició en píxels. La multi del padding es per separar-ho de la vora esquerra 3 pixels, 
                    la de la x i la y on comença la columna o fila*/
                }
            }
        }
    //Part 2: Dibuixar el bloc que cau (3 caramels en vertical)
        if(!m_gameOver)
        {
            graphics.drawImage(m_falling[0]->getResourceName(),
                board_padding * CANDY_IMAGE_WIDTH + m_fallingX * CANDY_IMAGE_WIDTH,
                board_padding * CANDY_IMAGE_HEIGHT + m_fallingY * CANDY_IMAGE_HEIGHT);

           graphics.drawImage(m_falling[1]->getResourceName(),
                board_padding * CANDY_IMAGE_WIDTH + m_fallingX * CANDY_IMAGE_WIDTH,
                board_padding * CANDY_IMAGE_HEIGHT + (m_fallingY + 1) * CANDY_IMAGE_HEIGHT);

           graphics.drawImage(m_falling[2]->getResourceName(),
                board_padding * CANDY_IMAGE_WIDTH + m_fallingX * CANDY_IMAGE_WIDTH,
                board_padding * CANDY_IMAGE_HEIGHT + (m_fallingY + 2) * CANDY_IMAGE_HEIGHT);

                /* al ser objectes dinamics hem d'utilitzar punters per accedir a la funcio getResourceName, utilitzem memoria dinamica perque aixi 
            podem escollir quan desapareix l'objecte, si ho fessim amb objectes directes el creariem i desapareixeria en acabar la funcio */
        }
    //Part 3: Mostrar gameover quan acabi la partida

        if(m_gameOver == true)
        {
            graphics.drawText("GAME OVER", 200, 350, 70, 255, 0, 0);
        }

    graphics.drawRectangle(
        CANDY_IMAGE_HEIGHT * board_padding, CANDY_IMAGE_HEIGHT * board_padding,
        CANDY_IMAGE_WIDTH * board_size,
        CANDY_IMAGE_HEIGHT * board_size,
        5, 150, 150, 150);

    // Board: place a candy piece
    /*graphics.drawImage(Candy(CandyType::TYPE_PURPLE).getResourceName(),
        CANDY_IMAGE_WIDTH * 3,
        CANDY_IMAGE_HEIGHT * 3);
    */

    // Title [draw images]
    graphics.drawImage("img/logo_small.png", 10, 10);

    // Score and footer [draw text]
    graphics.drawText("Movement: [Up] [Down] [Left] [Right]  --  "
                      "Buttons: [Q] [W] [E]  --  Exit [ESC]",
                      25, 700, 20, 100, 100, 100);
    graphics.drawText("Score: " + std::to_string(m_puntuacio), 450, 10, 70, 125, 200, 125);
}

void Game::run()
{
    const int screen_width = 750;
    const int screen_height = 750;
    const int bg_red = 255;
    const int bg_green = 255;
    const int bg_blue = 255;
    runGraphicGame(*this, screen_width, screen_height, bg_red, bg_green, bg_blue);
}

bool Game::dump(const std::string& output_path) const
{
    // Implement your code here
    bool fet = m_board.dump(output_path);
    if(fet)
    {
        ofstream fitxer;
        fitxer.open(output_path, ios::app); //per obrir en append
        if(fitxer.is_open())
        {
            fitxer << "m_falling" << endl;
            for(int i = 0; i < 3; i++)
            {
                switch (m_falling[i]->getType())
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
            fitxer << endl;
            fitxer << "m_fallingX" << endl << m_fallingX << endl;
            fitxer << "m_fallingY" << endl << m_fallingY << endl;
            fitxer << "m_vertical" << endl << m_vertical << endl;
            fitxer << "m_puntuacio" << endl << m_puntuacio << endl;
            fitxer << "m_fallTimer" << endl << m_fallTimer << endl;
            fitxer << "m_gameOver" << endl << m_gameOver << endl;

        }
        else 
        {
            fet = false;
        }
        fitxer.close();
    }
    return fet;
} 

bool Game::load(const std::string& input_path)
{
    ifstream fitxer(input_path);
    if (!fitxer.is_open()) return false;

    if (!m_board.load(fitxer)) return false;

    std::string header;
    if (!(fitxer >> header) || header != "m_falling") return false;

    for (int i = 0; i < 3; i++)
    {
        char aux;
        if (!(fitxer >> aux)) return false;
        CandyType tipus;
        switch (aux)
        {
            case 'R': tipus = CandyType::TYPE_RED;    break;
            case 'B': tipus = CandyType::TYPE_BLUE;   break;
            case 'G': tipus = CandyType::TYPE_GREEN;  break;
            case 'Y': tipus = CandyType::TYPE_YELLOW; break;
            case 'P': tipus = CandyType::TYPE_PURPLE; break;
            case 'O': tipus = CandyType::TYPE_ORANGE; break;
            default: return false;
        }
        delete m_falling[i];
        m_falling[i] = new Candy(tipus);
    }

    std::string label;
    fitxer >> label >> m_fallingX;
    fitxer >> label >> m_fallingY;
    fitxer >> label >> m_vertical;
    fitxer >> label >> m_puntuacio;
    fitxer >> label >> m_fallTimer;
    fitxer >> label >> m_gameOver;

    return fitxer.good() || fitxer.eof();
}


bool Game::operator==(const Game& other) const
{
    bool cerca = true;

    //1: Dimensions del tauler
    if (m_board.getWidth() != other.m_board.getWidth())   cerca = false;
    if (m_board.getHeight() != other.m_board.getHeight()) cerca = false;

    //2: Contingut de cada cel·la (comparar tipus, no punters)
    for (int x = 0; x < m_board.getWidth() && cerca; x++)
    {
        for (int y = 0; y < m_board.getHeight() && cerca; y++)
        {
            const Candy* a = m_board.getCell(x, y);
            const Candy* b = other.m_board.getCell(x, y);

            if (a == nullptr && b == nullptr)
            {
                //tot bé
            }
            else if (a == nullptr || b == nullptr)      
                cerca = false;
            else if (a->getType() != b->getType())      
                cerca = false;
        }
    }

    //3: Bloc que cau (posicio i tipus)
    if (m_fallingX != other.m_fallingX) 
        cerca = false;
    if (m_fallingY != other.m_fallingY) 
        cerca = false;
    for (int i = 0; i < 3 && cerca; i++)
    {
        if (m_falling[i]->getType() != other.m_falling[i]->getType())
            cerca = false;
    }

    return cerca;
}