#pragma once

#include <stdio.h>
#include <tchar.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <stdlib.h>

/* Use glew.h instead of gl.h to get all the GL prototypes declared */

#include "GL/glew.h"
#include "GL/freeglut.h"
#include "LoadShaders.h"

#ifdef _DEBUG
#  pragma comment (lib, "glew32.lib")
#  pragma comment (lib, "freeglut.lib")
#else
#  pragma comment (lib, "glew32.lib")
#  pragma comment (lib, "freeglut.lib")
#endif
#define BUFFER_OFFSET(x)  ((const void*) sizeof(x))


using namespace std;

bool isShaderFileExists(string& );

