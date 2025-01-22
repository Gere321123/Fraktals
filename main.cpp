#include <vector>
#include <iostream>
#include <cmath>
#include <CL/opencl.hpp>
#include <GL/glut.h>

// Constants for window dimensions
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;
const float gap = 0.01;
const int num_points_per_dim = static_cast<int>(4.0f / gap);
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

void display()
{
  glClear(GL_COLOR_BUFFER_BIT);
  glBegin(GL_POINTS); // Render individual pixels as points

  // Loop through all points in the grid
  for (int y = 0; y < num_points_per_dim; ++y)
  {
    for (int x = 0; x < num_points_per_dim; ++x)
    {
      int index = y * num_points_per_dim + x;

      // Map the grid coordinates to OpenGL's coordinate space (-1 to 1)
      float gl_x = -1.0f + 2.0f * (x / (float)num_points_per_dim);
      float gl_y = -1.0f + 2.0f * (y / (float)num_points_per_dim);

      // Set color based on whether the point escaped or not
      if (displays[index])
      {
        glColor3f(1.0f, 0.0f, 0.0f); // Red for points inside the set
      }
      else
      {
        glColor3f(0.0f, 0.0f, 0.0f); // Black for points outside the set
      }

      glVertex2f(gl_x, gl_y);
    }
  }

  glEnd();
  glutSwapBuffers(); // Swap buffers to display the rendered image
}

void initOpenGL()
{
  // Set up the OpenGL environment
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(-1.0, 1.0, -1.0, 1.0); // Set the coordinate system to range [-1, 1]
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set the background color to black
}

int main(int argc, char **argv)
{
  // OpenCL initialization and calculation
  initOpenCL();
  calculateValues();

  // OpenGL initialization
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
  glutCreateWindow("Mandelbrot Set Visualization");

  initOpenGL();

  // Set the display function
  glutDisplayFunc(display);

  // Start the main loop
  glutMainLoop();

  return 0;
}
