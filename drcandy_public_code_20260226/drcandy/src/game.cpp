#include "game.h"
#include <random>
#include "graphics.h"
#include "candy.h"

Game::Game()
{
    m_puntuacio = 0;
    m_falling[0] = new Candy(static_cast<CandyType>(rand() % static_cast<int>(CandyType::COUNT)));
    m_falling[1] = new Candy(static_cast<CandyType>(rand() % static_cast<int>(CandyType::COUNT)));

    m_fallingX = 4; // posicio horitzontal a la que apareix el blockd de candies
    m_fallingY = 0; // 0 perque sigui la fila d'adalt del tot
    m_fallTimer = 30; /* comptador que anem decrementant a cada 
                      frame que pasa decreix de manera que a meitat 
                      de cicle (60 frames) s'actualitza */
}

Game::~Game() //nomes es crida quan el programa tanca
{
    delete m_falling[0];
    delete m_falling[1];
}

void Game::update(const Controller& controller)
{
    // 3 blocs que es repeteixen a cada frame en ordre

    // 1: Llegir les tecles del controller

        if (controller.isLeftPressed() && m_fallingX > 0)
        {
            m_fallingX--;
        }

        if (controller.isRightPressed() && m_fallingX < 9)
        {
            m_fallingX++;
        }

        if (controller.isDownPressed() && m_fallingY < 8) // 8 perque ocupa dues posicions la peça
        {
            m_fallingY++;
        }

    // 2: Fer baixar el bloc automaticament
        
        m_fallTimer--;
        if (m_fallTimer <= 0)
        {
            m_fallingY++; // fem que baixi una posicio ja que han pasat els 30 frames
            m_fallTimer = 30;
        } 


    // 3: Detectar si el bloc ha aterrat
        if (m_fallingY >= 8 || m_board.getCell(m_fallingX, m_fallingY + 2) != nullptr)
        {
            m_board.setCell(m_falling[0], m_fallingX, m_fallingY);
            m_board.setCell(m_falling[1], m_fallingX, m_fallingY + 1);

            m_board.explodeAndDrop();

            m_falling[0] = new Candy(static_cast<CandyType>(rand() % static_cast<int>(CandyType::COUNT)));
            m_falling[1] = new Candy(static_cast<CandyType>(rand() % static_cast<int>(CandyType::COUNT)));
            m_fallingX = 4;
            m_fallingY = 0;
            m_fallTimer = 30; // aixi el nou bloc tambe inicia amb 30 frames
        }
}

void Game::render(GraphicManager& graphics)
{
    // Implement your code here
    
    // Note: the following code exhibits the main graphic library features
    // Board: border [draw rectangles] and a single piece of candy
    const int board_size = 10;
    const int board_padding = 3;
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
    return false;
}

bool Game::load(const std::string& input_path)
{
    // Implement your code here
    return false;
}

bool Game::operator==(const Game& other) const
{
    // Implement your code here
    return false;
}