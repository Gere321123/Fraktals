#include <vector>
#include <iostream>
// #include <complex>
#include <cmath>
#include <CL/opencl.hpp>
#include <GL/glut.h>
// OpenCL és OpenGL globális változók
cl::Context context;
cl::CommandQueue queue;
cl::Program program;
cl::Buffer x_buffer, y_buffer;

std::vector<float> x_values, y_values;
const int num_points = 2000;

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
        __kernel void calculate_y(__global const float* x_values, __global float* y_values, const int size) {
            int i = get_global_id(0);
            if (i < size) {
                y_values[i] = x_values[i] * x_values[i];
            }
        }
    )";

  std::vector<std::string> sources(1, kernel_code);
  program = cl::Program(context, sources);
  program.build("-cl-std=CL1.2");
}

void calculateValues()
{
  // X értékek inicializálása
  x_values.resize(num_points);
  y_values.resize(num_points);
  for (int i = 0; i < num_points; ++i)
  {
    x_values[i] = -10.0f + i * 20.0f / num_points;
  }

  // OpenCL buffer létrehozása
  x_buffer = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * x_values.size(), x_values.data());
  y_buffer = cl::Buffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * y_values.size());

  // Kernel futtatása
  cl::Kernel kernel(program, "calculate_y");
  kernel.setArg(0, x_buffer);
  kernel.setArg(1, y_buffer);
  kernel.setArg(2, (int)x_values.size());

  queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(num_points));
  queue.enqueueReadBuffer(y_buffer, CL_TRUE, 0, sizeof(float) * y_values.size(), y_values.data());
}

void display()
{
  glClear(GL_COLOR_BUFFER_BIT);
  glBegin(GL_LINE_STRIP);
  for (size_t i = 0; i < x_values.size(); ++i)
  {
    glColor3f(0.2, 0.5, 0.2);
    glVertex2f(x_values[i], y_values[i]);
  }
  glEnd();
  glutSwapBuffers();
}

int main(int argc, char **argv)
{
  // OpenCL inicializálása
  initOpenCL();
  calculateValues();

  // OpenGL inicializálása
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(800, 600);
  glutCreateWindow("OpenCL + OpenGL: y = x^2");
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(-10, 10, -10, 100);

  glutDisplayFunc(display);
  glutMainLoop();
  return 0;
}
