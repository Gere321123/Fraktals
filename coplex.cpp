#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <complex>

int main()
{
  using Complex = std::complex<float>;

  std::vector<Complex> c;
  std::vector<bool> display;
  const float escape_threshold = 2.0f;
  const int max_iterations = 30;
  const float gap = 0.01f;

  for (float i = -2; i < 2; i += gap)
  {
    for (float j = -2; j < 2; j += gap)
    {
      c.push_back(Complex(i, j));
    }
  }

  const int window_width = 800;
  const int window_height = 800;
  sf::RenderWindow window(sf::VideoMode(window_width, window_height), "Mandelbrot Set");

  for (const auto &complex : c)
  {
    std::complex<float> z = 0;
    bool escaped = false;

    for (int i = 0; i < max_iterations; i++)
    {
      z = z * z + complex;

      if (std::abs(z) > escape_threshold)
      {
        escaped = true;
        break;
      }
    }

    if (escaped)
    {
      display.push_back(true);
    }
    else
    {
      display.push_back(false);
    }
  }

  while (window.isOpen())
  {
    sf::Event event;
    while (window.pollEvent(event))
    {
      if (event.type == sf::Event::Closed)
        window.close();
    }

    window.clear(sf::Color::Black);

    int index = 0;
    for (float i = -2; i < 2; i += gap)
    {
      for (float j = -2; j < 2; j += gap)
      {
        sf::RectangleShape point(sf::Vector2f(1.0f, 1.0f));
        point.setPosition((i + 2) * (window_width / 4), (j + 2) * (window_height / 4));

        if (display[index])
        {
          point.setFillColor(sf::Color::White);
        }
        else
        {
          point.setFillColor(sf::Color::Black);
        }

        window.draw(point);
        index++;
      }
    }

    window.display();
  }

  return 0;
}
