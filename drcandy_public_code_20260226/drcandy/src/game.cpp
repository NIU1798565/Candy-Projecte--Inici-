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

            m_board.explodeAndDrop();

            m_falling[0] = new Candy(static_cast<CandyType>(rand() % static_cast<int>(CandyType::COUNT)));
            m_falling[1] = new Candy(static_cast<CandyType>(rand() % static_cast<int>(CandyType::COUNT)));
            m_falling[2] = new Candy(static_cast<CandyType>(rand() % static_cast<int>(CandyType::COUNT)));
            m_fallingX = 4;
            m_fallingY = 0;
            m_fallTimer = 60;
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
        
    //Part 3: Dibuixar la puntuacio
        graphics.drawText("Puntaucio: " + std::to_string(m_puntuacio), 450, 10, 70, 125, 200, 125);
                                                            // posicio pixels (450, 10), mida lletra (70), color RGB verd suau (125, 200, 125)

    graphics.drawRectangle(
        CANDY_IMAGE_HEIGHT * board_padding, CANDY_IMAGE_HEIGHT * board_padding,
        CANDY_IMAGE_WIDTH * board_size,
        CANDY_IMAGE_HEIGHT * board_size,
        5, 150, 150, 150);
    // Board: place a candy piece
    graphics.drawImage(Candy(CandyType::TYPE_PURPLE).getResourceName(),
        CANDY_IMAGE_WIDTH * 3,
        CANDY_IMAGE_HEIGHT * 3);
    // Title [draw images]
    graphics.drawImage("img/logo_small.png", 10, 10);
    // Score and footer [draw text]
    graphics.drawText("Movement: [Up] [Down] [Left] [Right]  --  "
                      "Buttons: [Q] [W] [E]  --  Exit [ESC]",
                      25, 700, 20, 100, 100, 100);
    graphics.drawText("Score: ", 450, 10, 70, 125, 200, 125);
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
    // Implement your code her
    return false;
}

bool Game::operator==(const Game& other) const
{
    // Implement your code here
    return false;
}