#include "Gui.h"

Gui::Gui()
: _window{ { 800u, 500u }, "Hello World!" }
{
    _font.loadFromFile("res/fonts/arial.ttf");

    _helloText.setFont(_font);
    _helloText.setString("Hello World!");
    _helloText.setCharacterSize(40);
    _helloText.setFillColor(sf::Color(255,255,255));

    sf::FloatRect textRect = _helloText.getLocalBounds();
    float x = textRect.left + textRect.width/2.f;
    float y = textRect.top + textRect.height/2.f;
    _helloText.setOrigin(x,y);
    _helloText.setPosition(_window.getView().getCenter());
}


bool Gui::update(){
    for (auto event = sf::Event{}; _window.pollEvent(event);)
    {
        if (event.type == sf::Event::Closed)
        {
            _window.close();
            return false;
        }
    }

    _window.clear(sf::Color(30,30,30));
    _window.draw(_helloText);
    _window.display();
    return true;
}