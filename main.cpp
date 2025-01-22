#include <vector>
#include <iostream>
#include <cmath>
#include <CL/opencl.hpp>
#include <GL/glut.h>

// OpenCL és OpenGL globális változók
cl::Context context;
cl::CommandQueue queue;
cl::Program program;
cl::Buffer c_buffer;
cl::Buffer displays_buffer;

struct float2
{
  float x, y;

  // Konstruktor
  float2(float x = 0, float y = 0) : x(x), y(y) {}

  // Operátor a komplex számok szorzásához
  float2 operator*(const float2 &other) const
  {
    return float2(x * other.x - y * other.y, x * other.y + y * other.x);
  }

  // Operátor a komplex számokhoz való hozzáadásához
  float2 operator+(const float2 &other) const
  {
    return float2(x + other.x, y + other.y);
  }
};

std::vector<float2> c_values;
std::vector<char> displays; // Change to vector<char>

void initOpenCL()
{
  // Platform és eszköz kiválasztása
  std::vector<cl::Platform> platforms;
  cl::Platform::get(&platforms);
  cl::Platform platform = platforms.front();

  std::vector<cl::Device> devices;
  platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
  cl::Device device = devices.front();

  context = cl::Context(device);
  queue = cl::CommandQueue(context, device);

  // Kernel betöltése
  std::string kernel_code = R"(
        __kernel void mandelbrot(__global float2* c_values, __global char* displays, const float gap, const int num_points_per_dim) {
            int i = get_global_id(0);

            int x_idx = i % num_points_per_dim;  
            int y_idx = i / num_points_per_dim;  

            float real = -2.0f + x_idx * gap;
            float imagen = -2.0f + y_idx * gap;

            c_values[i] = (float2)(real, imagen);

            int iter = 0;
            float2 z = (float2)(0.0f, 0.0f);
            char escaped = 0;

            while (iter < 15) {
                z = (float2)(z.x * z.x - z.y * z.y + c_values[i].x, 2.0f * z.x * z.y + c_values[i].y);
                if (sqrt(z.x * z.x + z.y * z.y) > 2.0f) {
                    escaped = 1;
                    break;
                }
                iter++;
            }

            displays[i] = escaped;
        }
    )";

  std::vector<std::string> sources(1, kernel_code);
  program = cl::Program(context, sources);
  program.build("-cl-std=CL1.2");
}

void calculateValues()
{
  const float gap = 0.01f;
  const int num_points_per_dim = static_cast<int>(4.0f / gap); // 4.0 az intervallum hossza (-2-től 2-ig)
  const int num_points = num_points_per_dim * num_points_per_dim;

  c_values.resize(num_points);
  displays.resize(num_points);

  // OpenCL buffer létrehozása
  c_buffer = cl::Buffer(context, CL_MEM_WRITE_ONLY, sizeof(float2) * c_values.size());
  displays_buffer = cl::Buffer(context, CL_MEM_WRITE_ONLY, sizeof(char) * displays.size());

  // Kernel futtatása
  cl::Kernel kernel(program, "mandelbrot");
  kernel.setArg(0, c_buffer);
  kernel.setArg(1, displays_buffer);
  kernel.setArg(2, gap);
  kernel.setArg(3, num_points_per_dim);
  cl::NDRange global_size(num_points);

  queue.enqueueNDRangeKernel(kernel, cl::NullRange, global_size);
  queue.enqueueReadBuffer(displays_buffer, CL_TRUE, 0, sizeof(char) * displays.size(), displays.data());
}

int main(int argc, char **argv)
{
  // OpenCL inicializálása
  initOpenCL();
  calculateValues();

  // Additional OpenGL code (if required)

  return 0;
}
