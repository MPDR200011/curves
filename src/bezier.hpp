#pragma once

#include <vector>
#include <optional>
#include <SFML/Graphics.hpp>
#include "lines.hpp"

class BezierEngine;

class BezierPoint {
  sf::Vector2f m_position;

  friend class BezierEngine;

public:
  BezierPoint(sf::Vector2f position);

  sf::Vector2f position() const {
    return m_position;
  }
};

class BezierEngine;

class BezierInput : public InputHandler {
  enum BezierState {DEFAULT, MOVING_POINT};
  BezierEngine* m_engine;

  std::optional<size_t> m_focusedPoint;

  BezierState m_currentState;
public:
  BezierInput(BezierEngine* engine);

  void MouseMoved(sf::Vector2f vector);
  void ButtonPressed(sf::Mouse::Button button, sf::Vector2f position);
  void ButtonReleased(sf::Mouse::Button button, sf::Vector2f position);
  void KeyPressed(sf::Event::KeyEvent event);
  void KeyReleased(sf::Event::KeyEvent event);
};

class BezierEngine : public LineEngine {
  std::vector<BezierPoint> m_points;

  unsigned m_degree;
  bool m_smoothMode;

  sf::Vector2f InterpolateFromPoint(float t, size_t pointIdx) const;
  void CorrectSmooth();

public:
  BezierEngine(unsigned degree = 2);

  void RenderLine(sf::RenderWindow&) const;

  size_t AddPoint(sf::Vector2f coords);
  void UpdatePointPosition(size_t idx, sf::Vector2f pos);
  size_t DeleteLastPoint();

  void ToggleSmoothMode();

  void IncrementDegree();
  void DecrementDegree();

  std::unique_ptr<InputHandler> GetInputHandler();

  std::optional<size_t> GetHitPointIdx(sf::Vector2f pos);
  BezierPoint& GetPoint(long index);
};
