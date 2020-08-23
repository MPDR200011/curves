#include "bezier.hpp"

const float pointRadius = 5.0f;
const float midPointRadius = 2.0f;

BezierPoint::BezierPoint(sf::Vector2f position, sf::Vector2f tangent) : m_position(position), m_tangent(tangent) {}

void BezierEngine::UpdatePoints() {
  m_shapes.clear();
  std::for_each(std::begin(m_points), std::end(m_points), [this] (auto point) {
      sf::CircleShape& shape = m_shapes.emplace_back(pointRadius);
      shape.setOrigin({pointRadius, pointRadius});
      shape.setPosition(point.position());
      });

  const int num_points = 100;
  const float increment = 1.0/num_points;
  for (size_t i = 0; i < m_points.size() - 1; ++i) {
    for (int j = 1; j < num_points; ++j) {
      sf::CircleShape& shape = m_shapes.emplace_back(midPointRadius);
      shape.setOrigin({midPointRadius, midPointRadius});
      shape.setPosition(bezier_interpolate(increment * j, m_points[i], m_points[i+1]));
    }
  }
}

void BezierEngine::AddPoint(sf::Vector2f coords, sf::Vector2f tangent) {
  m_points.emplace_back(coords, tangent);
  UpdatePoints();
}

void BezierEngine::RenderLine(sf::RenderWindow & window) const {
  for (sf::CircleShape const & shape: m_shapes) {
    window.draw(shape);
  }
}

sf::Vector2f bezier_interpolate(float t, BezierPoint const & p0, BezierPoint const & p1) {
  return 
    (2*t*t*t - 3*t*t + 1)*p0.position()
    + (t*t*t - 2*t*t + t)*p0.tangent()
    + (-2*t*t*t + 3*t*t)*p1.position()
    + (t*t*t - t*t)*p1.tangent();
}
