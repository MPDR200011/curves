#include "utils.hpp"

float square_dist(sf::Vector2f const & v1, sf::Vector2f const & v2) {
  float p1 = v1.x - v2.x;
  float p2 = v1.y - v2.y;
  return p1*p1 + p2*p2;
}

