#include <vector>
#include <iostream>
#include <complex>

int main(int argc, char **argv)
{
  using Complex = std::complex<float>;
  std::vector<Complex> c;
  int index = 0;
  for (float i = -1; i < 2; i += 0.001)
  {
    for (float j = -1; j < 2; j += 0.001)
    {
      c.push_back(Complex(i, j));
    }
    index++;
  }

  for (const auto &complex : c)
  {
    std::cout << "(" << complex.real() << ", " << complex.imag() << ")" << std::endl;
  }

  return 0;
}