#include "Gui.h"

#include <SFML/Window/Event.hpp>

Gui::Gui()
: _window{ { 800u, 500u }, "Dice Roller" }
{
    _font.loadFromFile("res/fonts/arial.ttf");
    float instructionYPos = 150.f;
    float diceSize = 50.f;

    _instructionText.setFont(_font);
    _instructionText.setString("Click or press space bar to roll dice");
    _instructionText.setCharacterSize(40);
    _instructionText.setFillColor(sf::Color(255,255,255));

    _diceText.setFont(_font);
    _diceText.setString("?");
    _diceText.setCharacterSize(40);
    _diceText.setFillColor(sf::Color(255,255,255));

    _diceRect.setFillColor(sf::Color(150,0,0));

    {
        sf::FloatRect textRect = _instructionText.getLocalBounds();
        float x = textRect.left + textRect.width/2.f;
        float y = textRect.top + textRect.height/2.f;
        _instructionText.setOrigin(x,y);
    }
    
    float middleX = _window.getView().getSize().x/2.f;
    _instructionText.setPosition(middleX,instructionYPos);

    {
        sf::FloatRect textRect = _diceText.getLocalBounds();
        float x = textRect.left + textRect.width/2.f;
        float y = textRect.top + textRect.height/2.f;
        _diceText.setOrigin(x,y);
    }
    _diceText.setPosition(_window.getView().getCenter());


    _diceRect.setSize(sf::Vector2f(diceSize,diceSize));
    _diceRect.setOrigin(sf::Vector2f(diceSize/2.f,diceSize/2.f));
    _diceRect.setPosition(_window.getView().getCenter());
}

void Gui::setDiceValue(int value){
    _diceText.setString(std::to_string(value));
    sf::FloatRect textRect = _diceText.getLocalBounds();
    float x = textRect.left + textRect.width/2.f;
    float y = textRect.top + textRect.height/2.f;
    _diceText.setOrigin(x,y);
}


Gui::Event Gui::update(){
    for (auto event = sf::Event{}; _window.pollEvent(event);)
    {
        switch(event.type){
            case sf::Event::Closed:
                _window.close();
                return Gui::Event::End;
            case sf::Event::MouseButtonReleased:
                return Gui::Event::DiceRoll;
            case sf::Event::KeyPressed:
                if(event.key.scancode == sf::Keyboard::Scan::Space){
                    return Gui::Event::DiceRoll;
                }
                break;
            default:
                break;
        }
    }

    _window.clear(sf::Color(30,30,30));
    _window.draw(_diceRect);
    _window.draw(_diceText);
    _window.draw(_instructionText);
    _window.display();
    return Gui::Event::Void;
}