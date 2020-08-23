#pragma once
#include <SFML/Graphics.hpp>

class LineEngine {
  public:
    virtual ~LineEngine();
    virtual void RenderLine(sf::RenderWindow&) const = 0;
};
