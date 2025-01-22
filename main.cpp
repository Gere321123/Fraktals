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

std::vector<float2> c_values;

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
        __kernel void calculate_y(__global float2* c_values, const float gap, const int num_points_per_dim) {
            int i = get_global_id(0);

            int x_idx = i % num_points_per_dim;  
            int y_idx = i / num_points_per_dim;  

          
            float real = -2.0f + x_idx * gap;
            float imagen = -2.0f + y_idx * gap;

          
            c_values[id] = (float2)(real, imagen);
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
  // OpenCL buffer létrehozása
  c_buffer = cl::Buffer(context, CL_MEM_WRITE_ONLY, sizeof(float2) * c_values.size());

  // Kernel futtatása
  cl::Kernel kernel(program, "calculate_y");
  kernel.setArg(0, c_buffer);
  kernel.setArg(1, gap);
  kernel.setArg(2, num_points_per_dim);

  queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(num_points));
}

void display()
{
  glClear(GL_COLOR_BUFFER_BIT);
  glBegin(GL_LINE_STRIP);
  for (size_t i = 0; i < x_values.size(); ++i)
  {
    glColor3f(0.5, 0.1, 0.2);
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
