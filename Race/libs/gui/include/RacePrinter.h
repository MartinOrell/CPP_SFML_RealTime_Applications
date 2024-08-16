#pragma once

#include <vector>
#include <string>
#include <memory>
#include <SFML/Graphics.hpp>

class RacePrinter{
    public:
        RacePrinter(int goal);
        void addRacer(int id, std::string asciiFilename);
        void initPrint();
        void print(std::vector<int>);
    private:
        void initTrack();
        void initRacers();
        void addLines(float xStartPos, float xEndPos, float xOffset, float y, float lineWidth, float lineHeight);

        sf::RenderWindow _window;
        std::vector<sf::RectangleShape> _track;

        struct Racer{
            int id;
            sf::RectangleShape shape;
        };
        std::vector<Racer> _racers;
        std::vector<std::unique_ptr<sf::Texture>> _textures;
        sf::Font _font;
        sf::Text _goalText;

        int _goal;

        float _trackWidth;
        float _trackHeight;
        float _trackYPos;

        float _startWidth;
        float _goalWidth;
        float _raceWidth;

        float _edgeLineWidth;
        float _edgeLineHeight;
        float _edgeLineXOffset;
        float _edgeLineYOffset;

        float _middleLineWidth;
        float _middleLineHeight;
        float _middleLineXOffset;
        float _middleLineYPos;

        float _startLineWidth;
        float _goalLineWidth;
};