#pragma once
#include <SFML/Graphics.hpp>

class InputHandler {
public:
  virtual ~InputHandler() {};
  virtual void MouseMoved(sf::Vector2f vector) = 0;
  virtual void ButtonPressed(sf::Mouse::Button button, sf::Vector2f position) = 0;
  virtual void ButtonReleased(sf::Mouse::Button button, sf::Vector2f position) = 0;
  virtual void KeyPressed(sf::Event::KeyEvent event) = 0;
  virtual void KeyReleased(sf::Event::KeyEvent event) = 0;
};

class LineEngine {
  public:
    virtual ~LineEngine() {};
    virtual void RenderLine(sf::RenderWindow&) const = 0;

    virtual std::unique_ptr<InputHandler> GetInputHandler() = 0;
};
