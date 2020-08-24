#pragma once
#include <vector>
#include <list>
#include <optional>
#include <SFML/Graphics.hpp>
#include "lines.hpp"

class BezierInput;

class BezierPoint {
  sf::Vector2f m_position;
  sf::Vector2f m_tangent;

  bool focused;

  friend class BezierEngine;

public:
  BezierPoint(sf::Vector2f position, sf::Vector2f tangent);

  sf::Vector2f position() const {
    return m_position;
  }
  sf::Vector2f tangent() const {
    return m_tangent;
  }

  void setFocused(bool value) {
    this->focused = value;
  }
};

class BezierEngine;

class BezierInput : public InputHandler {
  enum MouseState {DEFAULT, MOVING_POINT, MOVING_TANGENT, CREATING_POINT};

  MouseState m_currentState;

  BezierEngine* m_engine;
  long m_focusedPoint;

public:
  BezierInput(BezierEngine* engine);
  ~BezierInput() {}

  void MouseMoved(sf::Vector2f position);
  void ButtonPressed(sf::Mouse::Button button, sf::Vector2f position);
  void ButtonReleased(sf::Mouse::Button button, sf::Vector2f position);
};

class BezierEngine : public LineEngine {
  std::vector<BezierPoint> m_points;

public:
  BezierEngine();

  long AddPoint(sf::Vector2f coords, sf::Vector2f tangent);
  void UpdatePointPosition(long idx, sf::Vector2f pos);
  void UpdatePointTangent(long idx, sf::Vector2f tangent);
  void RenderLine(sf::RenderWindow & window) const;

  std::unique_ptr<InputHandler> GetInputHandler();

  long GetHitPointIdx(sf::Vector2f pos);
  BezierPoint& GetPoint(long index);
};

sf::Vector2f bezier_interpolate(float t, BezierPoint const & p0, BezierPoint const & p1);
