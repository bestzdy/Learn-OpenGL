#include "..\common\Common.h"
#include "..\common\vmath.h"

const GLuint NumVaos = 1, NumBuffers = 1;
GLuint Vaos[NumVaos];
GLuint Buffers[NumBuffers];
GLuint EBOs[1];
GLuint const NumVertices = 4;
GLint nVert = 0;
GLuint const NUMSTACKS = 256, NUMSLICES = 256;
GLuint const Num_Functions = 8;
GLuint program;
ShaderInfo** shader;
GLuint windowsWidth = 1280, windowsHeight = 1280;
GLboolean random_diff = FALSE;
enum LIGHTTYPES
{
	PER_VERTEX_PHONG = 4,
	PER_FRAGMENT_PHONG = 5,
};

struct uniforms_block
{
	vmath::mat4     mv_matrix;
	vmath::mat4     view_matrix;
	vmath::mat4     proj_matrix;
};

GLuint uniforms_buffer;
struct
{
	GLint           diffuse_albedo;
	GLint           specular_albedo;
	GLint           specular_power;
} uniforms[1];
//---------------------------------------------------------------------------------------
using namespace vmath;

void init()
{
	//data of vertices
	nVert = 8;
	GLfloat vertices[] = {
		-0.5f, -0.5f, -0.5f, //0
		-0.5f, -0.5f,  0.5f, //1
		-0.5f,  0.5f, -0.5f, //2
		-0.5f,  0.5f,  0.5f, //3
		 0.5f, -0.5f, -0.5f, //4
		 0.5f, -0.5f,  0.5f, //5
		 0.5f,  0.5f, -0.5f, //6
		 0.5f,  0.5f,  0.5f //7
	};
	GLfloat vNormal[] = {
		-0.57735f, -0.57735f, -0.57735f,//0
		-0.57735f, -0.57735f,  0.57735f, //1
		-0.57735f,  0.57735f, -0.57735f, //2
		-0.57735f,  0.57735f,  0.57735f, //3
		 0.57735f, -0.57735f, -0.57735f, //4
		 0.57735f, -0.57735f,  0.57735f, //5
		 0.57735f,  0.57735f, -0.57735f, //6
		 0.57735f,  0.57735f,  0.57735f//7
	};
	GLushort elem[] = {
		6, 4, 2, 0, 3, 1, 7, 5,         // 第一个条带
		0xFFFF,                         // <<-- 重启动索引
		1, 0, 5, 4, 7, 6, 3, 2          // 第二个条带
	};
	//-------------------------------------------------------------------
	// 
	//begin shader

	shader = new ShaderInfo * [2];
	shader[0] = new ShaderInfo[3];
	shader[0][0].type = GL_VERTEX_SHADER;
	shader[0][0].filename = "vertex.vert";
	shader[0][1].type = GL_FRAGMENT_SHADER;
	shader[0][1].filename = "fragment.frag";
	shader[0][2].type = GL_NONE;
	shader[0][2].filename = NULL;
	shader[1] = new ShaderInfo[3];
	shader[1][0].type = GL_VERTEX_SHADER;
	shader[1][0].filename = "vertex2.vert";
	shader[1][1].type = GL_FRAGMENT_SHADER;
	shader[1][1].filename = "fragment2.frag";
	shader[1][2].type = GL_NONE;
	shader[1][2].filename = NULL;

	program = LoadShaders(shader[1]);

	glUseProgram(program);
	//gen buffers
	glGenBuffers(1, &uniforms_buffer);
	glBindBuffer(GL_UNIFORM_BUFFER, uniforms_buffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(uniforms_block), NULL, GL_DYNAMIC_DRAW);

	glGenVertexArrays(NumVaos, Vaos);
	glGenBuffers(1, EBOs);
	glGenBuffers(NumBuffers, Buffers);
	glBindVertexArray(Vaos[0]);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[0]);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * (NUMSLICES + 1) * 2 * (NUMSTACKS), elem, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elem), elem, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, Buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * nVert * 3 * 2, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * nVert * 3, vertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * nVert * 3, sizeof(GLfloat) * nVert * 3, vNormal);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (const void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (const void*)((sizeof(GLfloat) * nVert * 3)));
	glEnableVertexAttribArray(1);
	//-------------------------------------------------------------------
	//select subroutings functions

	//------------------------------------------------------------------
	//glClearColor(0.9, 0.9, 0.9, 1);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
}

void display() {
	glViewport(0, 0, windowsWidth, windowsHeight);
	//-------------------------------------------
	static const GLfloat zeros[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	static const GLfloat gray[] = { 0.05f, 0.05f, 0.05f, 0.0f };
	static const GLfloat ones[] = { 1.0f };
	glClearBufferfv(GL_COLOR, 0, gray);
	glClearBufferfv(GL_DEPTH, 0, ones);
	vmath::vec3 view_position = vmath::vec3(50.0f, -80.0f, 120.0f);
	vmath::mat4 view_matrix = vmath::lookat(view_position, vmath::vec3(0.0f, 0.0f, 0.0f), vmath::vec3(0.0f, 1.0f, 0.0f));

	vmath::vec3 light_position = vmath::vec3(0.0f, 20.0f, 0.0f);

	vmath::mat4 light_proj_matrix = vmath::frustum(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 200.0f);
	vmath::mat4 light_view_matrix = vmath::lookat(light_position, vmath::vec3(0.0f), vmath::vec3(0.0f, 1.0f, 0.0f));
	//vmath::mat4 model_matrix = vmath::scale(7.0f);
	float t = float(GetTickCount() & 0x1FFF) / float(0x1FFF);
	vmath::mat4 model_matrix = vmath::rotate(t * 180 * 2, 0.0f, 1.0f, 0.0f) * vmath::scale(20.0f);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, uniforms_buffer);
	uniforms_block* block = (uniforms_block*)glMapBufferRange(GL_UNIFORM_BUFFER,
		0,
		sizeof(uniforms_block),
		GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
	block->mv_matrix = view_matrix * model_matrix;
	block->view_matrix = view_matrix;
	block->proj_matrix = vmath::perspective(50.0f,
		(float)windowsWidth / (float)windowsHeight,
		0.1f,
		1000.0f);

	glUnmapBuffer(GL_UNIFORM_BUFFER);

	//glUniform1f(uniforms[0].specular_power, 30.0f);
	//glUniform3fv(uniforms[0].specular_albedo, 1, vmath::vec3(1.0f));
	if (random_diff == TRUE) {
		vec3 diff = vec3(cos(t * 2 * M_PI), sin(t * 2 * M_PI), t);
		glUniform3fv(glGetUniformLocation(program, "diffuse_albedo"), 1, diff);
	}

	//--------------------------------------------
	glBindVertexArray(Vaos[0]);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[0]);
	//----------------------------
	//float t = float(GetTickCount() & 0x1FFF) / float(0x1FFF);
	//glUniform1f(glGetUniformLocation(program, "t_time"), t);
	//glDrawArrays(GL_TRIANGLE_STRIP, 0, nVert*3);

	//glDrawElements(GL_TRIANGLE_STRIP, (NUMSLICES + 1) * 2 * (NUMSTACKS), GL_UNSIGNED_SHORT, NULL);
	glDrawElements(GL_TRIANGLE_STRIP, 8, GL_UNSIGNED_SHORT, NULL);
	glDrawElements(GL_TRIANGLE_STRIP, 8, GL_UNSIGNED_SHORT, (const GLvoid*)(9 * sizeof(GLushort)));
	//---------------------------
	glutSwapBuffers();
	glutPostRedisplay();
}

void keyboard(GLboolean key, GLint x, GLint y) {
	static GLfloat amb_r = 0.1;
	static GLfloat amb_g = 0.1;
	static GLfloat amb_b = 0.1;
	static GLfloat specular_power = 15;
	switch (key)
	{
	case 'a':
		if (program)
			glDeleteProgram(program);
		program = LoadShaders(shader[0]);
		cout << shader[0][0].filename << endl;
		glUseProgram(program);
		glUniform1i(glGetUniformLocation(program, "display_mode"), 4);
		break;
	case 's':
		if (program)
			glDeleteProgram(program);
		program = LoadShaders(shader[1]);
		cout << shader[1][0].filename << endl;
		glUseProgram(program);
		glUniform1i(glGetUniformLocation(program, "display_mode"), 4);
		break;
	case 'd':
		glutFullScreen();
		break;
	case 'e':
		glutReshapeWindow(windowsWidth, windowsHeight);
		glutPositionWindow(100, 100);
		break;
	case 'R':
		if (amb_r < 1)
			amb_r += 0.1;
		cout << "ambiant R:" << amb_r << endl;
		glUniform3f(glGetUniformLocation(program, "ambient"), amb_r, amb_g, amb_b);
		break;
	case 'r':
		if (amb_r > 0.1)
			amb_r -= 0.1;
		cout << "ambiant R:" << amb_r << endl;
		glUniform3f(glGetUniformLocation(program, "ambient"), amb_r, amb_g, amb_b);
		break;
	case 'G':
		if (amb_g < 1)
			amb_g += 0.1;
		cout << "ambiant G:" << amb_g << endl;
		glUniform3f(glGetUniformLocation(program, "ambient"), amb_r, amb_g, amb_b);
		break;
	case 'g':
		if (amb_g > 0.1)
			amb_g -= 0.1;
		cout << "ambiant G:" << amb_g << endl;
		glUniform3f(glGetUniformLocation(program, "ambient"), amb_r, amb_g, amb_b);
		break;
	case 'B':
		if (amb_b < 1)
			amb_b += 0.1;
		cout << "ambiant B:" << amb_b << endl;
		glUniform3f(glGetUniformLocation(program, "ambient"), amb_r, amb_g, amb_b);
		break;
	case 'b':
		if (amb_b > 0.1)
			amb_b -= 0.1;
		cout << "ambiant B:" << amb_b << endl;
		glUniform3f(glGetUniformLocation(program, "ambient"), amb_r, amb_g, amb_b);
		break;
	case 'f':
		random_diff ? random_diff = FALSE : random_diff = TRUE;
		break;
	case 'P':
		if (specular_power < 256)
			specular_power += 1;
		cout << "specular power:" << specular_power << endl;
		glUniform1f(glGetUniformLocation(program, "specular_power"), specular_power);
		break;
	case 'p':
		if (specular_power > 1)
			specular_power -= 1;
		cout << "specular power:" << specular_power << endl;
		glUniform1f(glGetUniformLocation(program, "specular_power"), specular_power);
		break;
	case 27:
		glutExit();
		break;
	default:
		break;
	}
}
void setMenu() {
	glutAddMenuEntry("no_light", 0);
	glutAddMenuEntry("only_ambient", 1);
	glutAddMenuEntry("only_diffuse", 2);
	glutAddMenuEntry("only_specular", 3);
	glutAddMenuEntry("per_vertex-Gouraud", 4);
	glutAddMenuEntry("per_fragment-Phong", 5);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}
void processMenu(int key)
{

	switch (key) {
	case 0:
	case 1:
	case 2:
	case 3:
		glUniform1i(glGetUniformLocation(program, "display_mode"), key);
		break;
	default:
		glUniform1i(glGetUniformLocation(program, "display_mode"), 0);
		break;
	case 4:
		if (program)
			glDeleteProgram(program);
		program = LoadShaders(shader[0]);
		cout << shader[0][0].filename << endl;
		glUseProgram(program);
		glUniform1i(glGetUniformLocation(program, "display_mode"), 4);
		break;
	case 5:
		if (program)
			glDeleteProgram(program);
		program = LoadShaders(shader[1]);
		cout << shader[1][0].filename << endl;
		glUseProgram(program);
		glUniform1i(glGetUniformLocation(program, "display_mode"), 4);
		break;
	}
	glutSwapBuffers();
	glutPostRedisplay();
}
void reshape(GLint newWidth, GLint newHeight) {
	//当窗口发生改变时，依然能在某个矩形框内进行显示
	glViewport(0, 0, newWidth, newHeight);
	glutSwapBuffers();
	glutPostRedisplay();
}
int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(windowsWidth, windowsHeight);
	glutInitContextVersion(4, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("BasicLight-Windows");
	glutInitWindowPosition(400, 200);
	if (glewInit())
	{
		cerr << "Unable to Initialize GLEW" << endl;
	}
	init();
	//glutReshapeFunc(reshape);
	glutKeyboardUpFunc(keyboard);
	glutCreateMenu(processMenu);
	setMenu();
	glutDisplayFunc(display);
	glutMainLoop();
	exit(1);
}
