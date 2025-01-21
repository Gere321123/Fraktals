#include <vector>
#include <iostream>
#include <complex>

int main(int argc, char **argv)
{
  using Complex = std::complex<float>;
  std::vector<Complex> c;
  int index = 0;
  for (float i = -1; i < 2; i += 0.01)
  {
    for (float j = -1; j < 2; j += 0.01)
    {
      c.push_back(Complex(i, j));
    }
    index++;
  }
  std::vector<bool> display;
  std::complex<float> z_previus;
  for (const auto &complex : c)
  {
    std::complex<float> z = 0;
    for (int i = 0; i < 7; i++)
    {
      z_previus = z;
      z = z * z + complex;
    }
    float distance = std::abs(z - z_previus);
    if (distance < 1.0f)
    {
      display.push_back(false);
    }
    else
    {
      display.push_back(true);
    }
  }
  return 0;
}