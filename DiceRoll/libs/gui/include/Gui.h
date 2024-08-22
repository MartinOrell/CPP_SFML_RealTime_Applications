#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

class Gui{
    public:
        enum Event{Void, End, DiceRoll};

        Gui();
        void setDiceValue(int);
        Event update();
    private:
        
        sf::RenderWindow _window;
        sf::Font _font;
        sf::Text _instructionText;
        sf::Text _diceText;
        sf::RectangleShape _diceRect;
};