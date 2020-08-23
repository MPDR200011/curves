#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include "bezier.hpp"

float square_dist(sf::Vector2f const & v1, sf::Vector2f const & v2) {
  float p1 = v1.x - v2.x;
  float p2 = v1.y - v2.y;
  return p1*p1 + p2*p2;
}

int main() {
  sf::RenderWindow window(sf::VideoMode(400,400), "Hello");

  BezierEngine engine;

  engine.AddPoint({10,10}, {1000,0});
  engine.AddPoint({390, 390}, {-500, 0});

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

    engine.RenderLine(window);

    window.display();
  }

  return 0;
}
