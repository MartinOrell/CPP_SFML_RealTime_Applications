#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>

class Gui{
    public:
        Gui();
        bool update();
    private:
        
        sf::RenderWindow _window;
        sf::Font _font;
        sf::Text _helloText;
};