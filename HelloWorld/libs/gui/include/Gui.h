#pragma once

#include <vector>
#include <string>
#include <memory>
#include <SFML/Graphics.hpp>

class Gui{
    public:
        Gui();
        bool update();
    private:
        
        sf::RenderWindow _window;
        sf::Font _font;
        sf::Text _helloText;
};