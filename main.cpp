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

struct float2
{
  float x;
  float y;
};
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
        __kernel void fill_c(__global float2* c_values, const float gap, const int num_points_per_dim) {
            int i = get_global_id(0);

            int x_idx = i % num_points_per_dim;  
            int y_idx = i / num_points_per_dim;  

          
            float real = -2.0f + x_idx * gap;
            float imagen = -2.0f + y_idx * gap;

          
            c_values[i] = (float2)(real, imagen);
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
  cl::Kernel kernel(program, "fill_c");
  kernel.setArg(0, c_buffer);
  kernel.setArg(1, gap);
  kernel.setArg(2, num_points_per_dim);
  cl::NDRange global_size(num_points);

  queue.enqueueNDRangeKernel(kernel, cl::NullRange, global_size);
  queue.enqueueReadBuffer(c_buffer, CL_TRUE, 0, sizeof(float2) * c_values.size(), c_values.data());
}

// void display()
// {
//   glClear(GL_COLOR_BUFFER_BIT);
//   glBegin(GL_LINE_STRIP);
//   for (size_t i = 0; i < x_values.size(); ++i)
//   {
//     glColor3f(0.5, 0.1, 0.2);
//     glVertex2f(x_values[i], y_values[i]);
//   }
//   glEnd();
//   glutSwapBuffers();
// }

int main(int argc, char **argv)
{
  // OpenCL inicializálása
  initOpenCL();
  calculateValues();

  // OpenGL inicializálása
  // glutInit(&argc, argv);
  // glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  // glutInitWindowSize(800, 600);
  // glutCreateWindow("OpenCL + OpenGL: y = x^2");
  // glMatrixMode(GL_PROJECTION);
  // glLoadIdentity();
  // gluOrtho2D(-10, 10, -10, 100);

  // glutDisplayFunc(display);
  // glutMainLoop();
  return 0;
}
