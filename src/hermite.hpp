#pragma once
#include <vector>
#include <list>
#include <optional>
#include <SFML/Graphics.hpp>
#include "lines.hpp"

class CubicHermiteInput;

class CubicHermitePoint {
  sf::Vector2f m_position;
  sf::Vector2f m_tangent;

  bool focused;

  friend class CubicHermiteEngine;

public:
  CubicHermitePoint(sf::Vector2f position, sf::Vector2f tangent);

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

class CubicHermiteEngine;

class CubicHermiteInput : public InputHandler {
  enum MouseState {DEFAULT, MOVING_POINT, MOVING_TANGENT, CREATING_POINT};

  MouseState m_currentState;

  CubicHermiteEngine* m_engine;
  long m_focusedPoint;

public:
  CubicHermiteInput(CubicHermiteEngine* engine);
  ~CubicHermiteInput() {}

  void MouseMoved(sf::Vector2f position);
  void ButtonPressed(sf::Mouse::Button button, sf::Vector2f position);
  void ButtonReleased(sf::Mouse::Button button, sf::Vector2f position);
  void KeyPressed(sf::Event::KeyEvent event);
  void KeyReleased(sf::Event::KeyEvent event);
};

class CubicHermiteEngine : public LineEngine {
  std::vector<CubicHermitePoint> m_points;

public:
  CubicHermiteEngine();

  long AddPoint(sf::Vector2f coords, sf::Vector2f tangent);
  void UpdatePointPosition(long idx, sf::Vector2f pos);
  void UpdatePointTangent(long idx, sf::Vector2f tangent);
  void RenderLine(sf::RenderWindow & window) const;
  void DeleteLastPoint();

  std::unique_ptr<InputHandler> GetInputHandler();

  long GetHitPointIdx(sf::Vector2f pos);
  CubicHermitePoint& GetPoint(long index);
};

sf::Vector2f cubic_hermite_interpolation(float t, CubicHermitePoint const & p0, CubicHermitePoint const & p1);
