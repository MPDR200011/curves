#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "line_includes.hpp"

int main() {
  sf::RenderWindow window(sf::VideoMode(400,400), "Hello");

  std::unique_ptr<LineEngine> engine = std::make_unique<BezierEngine>();
  std::unique_ptr<InputHandler> inputHandler = engine->GetInputHandler();

  while (window.isOpen()) {
    // check all the window's events that were triggered since the last iteration of the loop
    sf::Event event;
    while (window.pollEvent(event))
    {
      switch(event.type) {
        case sf::Event::Resized:
          {
            //Keep correct aspect ratio when window resizes
            sf::View newView{sf::FloatRect{0, 0, (float)event.size.width, (float)event.size.height}};
            window.setView(newView);
            break;
          }
        case sf::Event::MouseButtonPressed:
          inputHandler->ButtonPressed(event.mouseButton.button, window.mapPixelToCoords({event.mouseButton.x, event.mouseButton.y}));
          break;
        case sf::Event::MouseMoved:
          inputHandler->MouseMoved(window.mapPixelToCoords( {event.mouseMove.x, event.mouseMove.y} ));
          break;
        case sf::Event::MouseButtonReleased:
          inputHandler->ButtonReleased(event.mouseButton.button, window.mapPixelToCoords({event.mouseButton.x, event.mouseButton.y}));
          break;
        case sf::Event::KeyPressed:
          inputHandler->KeyPressed(event.key);
          break;
        case sf::Event::KeyReleased:
          inputHandler->KeyReleased(event.key);
          break;
        case sf::Event::Closed:
          // "close requested" event: we close the window
          window.close();
          break;
        default:
          break;
      }
    }

    window.clear();

    engine->RenderLine(window);

    window.display();
  }

  return 0;
}
