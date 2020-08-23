#pragma once
#include <vector>
#include <SFML/Graphics.hpp>
#include "lines.hpp"

class BezierPoint {
  sf::Vector2f m_position;
  sf::Vector2f m_tangent;

public:
  BezierPoint(sf::Vector2f position, sf::Vector2f tangent);

  sf::Vector2f position() const {
    return m_position;
  }
  sf::Vector2f tangent() const {
    return m_tangent;
  }
};

class BezierInput {

};

class BezierEngine : public LineEngine {
  std::vector<BezierPoint> m_points;
  std::vector<sf::CircleShape> m_shapes;

  void UpdatePoints();

public:
  BezierEngine() {}

  void AddPoint(sf::Vector2f coords, sf::Vector2f tangent);

  void RenderLine(sf::RenderWindow & window) const;
};

sf::Vector2f bezier_interpolate(float t, BezierPoint const & p0, BezierPoint const & p1);
