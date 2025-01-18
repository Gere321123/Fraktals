#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <cmath>
#include <iostream>

// Ablak méretei
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

// Zoom állapota
float zoom = 1.0f;

void drawFunction()
{
  glBegin(GL_LINE_STRIP);
  for (float x = -10.0f; x <= 10.0f; x += 0.01f)
  {
    float y = x * x;
    glVertex2f(x, y);
  }
  glEnd();
}

int main(int argc, char *argv[])
{
  if (SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    std::cerr << "Nem sikerült inicializálni az SDL-t: " << SDL_GetError() << std::endl;
    return -1;
  }

  SDL_Window *window = SDL_CreateWindow(
      "Parabola rajzolása",
      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
      WINDOW_WIDTH, WINDOW_HEIGHT,
      SDL_WINDOW_OPENGL);

  if (!window)
  {
    std::cerr << "Nem sikerült létrehozni az ablakot: " << SDL_GetError() << std::endl;
    SDL_Quit();
    return -1;
  }

  SDL_GLContext context = SDL_GL_CreateContext(window);
  if (!context)
  {
    std::cerr << "Nem sikerült létrehozni az OpenGL kontextust: " << SDL_GetError() << std::endl;
    SDL_DestroyWindow(window);
    SDL_Quit();
    return -1;
  }

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(-10.0, 10.0, -10.0, 10.0);

  glMatrixMode(GL_MODELVIEW);

  bool running = true;
  SDL_Event event;

  while (running)
  {
    // Eseménykezelés
    while (SDL_PollEvent(&event))
    {
      if (event.type == SDL_QUIT)
      {
        running = false;
      }
      else if (event.type == SDL_MOUSEWHEEL)
      {
        if (event.wheel.y > 0)
        {
          zoom *= 0.9f; // Zoom befelé
        }
        else if (event.wheel.y < 0)
        {
          zoom *= 1.1f; // Zoom kifelé
        }
      }
    }

    glClear(GL_COLOR_BUFFER_BIT);

    // Zoomolás
    glLoadIdentity();
    glScalef(zoom, zoom, 1.0f);

    glColor3f(1.0f, 0.0f, 0.0f);
    drawFunction();

    SDL_GL_SwapWindow(window);
  }

  SDL_GL_DeleteContext(context);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
