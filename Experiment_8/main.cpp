#include "..\common\Common.h"
#include "..\common\LoadShaders.h"
#include "..\common\vmath.h"
#include "..\common\stb_image.h"

GLint render_model_matrix_loc;
GLint render_projection_matrix_loc;
constexpr auto NumVAOs = 1;//设置顶点数组对象个数;
constexpr auto NumBuffers = 1;//设置缓存个数;
GLuint VAOs[NumVAOs];//设置顶点数组对象的数组；
GLuint Buffers[NumBuffers];//设置缓存数据对象数组；
GLuint ebo[1];//设置索引对象数组
GLint shader_program;
GLint display_mode = -1, dis_loc = -1;
float aspect;
unsigned int texture;
bool flag = 0;
float last_t;

void init()
{
	//1. vertices & texture 数据
	static const GLfloat cube_positions[] =
	{
		//立方体
			  -0.5f, -0.5f, -0.5f, 1.0f,//0
			  -0.5f, -0.5f,  0.5f, 1.0f,//1
			  -0.5f,  0.5f, -0.5f, 1.0f,//2
			  -0.5f,  0.5f,  0.5f, 1.0f,//3
			   0.5f, -0.5f, -0.5f, 1.0f,//4
			   0.5f, -0.5f,  0.5f, 1.0f,//5
			   0.5f,  0.5f, -0.5f, 1.0f,//6
			   0.5f,  0.5f,  0.5f, 1.0f, //7

			   -1.0f, -1.0f,//0
			  -1.0f, 1.0f,//1
			  1.0f,  -1.0f, //2
			  1.0f,  1.0f, //3
			   -1.0f, -1.0f, //4
			   -1.0f, 1.0f, //5
			   1.0f,  -1.0f, //6
			   1.0f,  1.0f //7
	};
	static const GLushort cube_indices[] =
	{
		6, 4, 2, 0, 3, 1, 7, 5,         // 第一个条带
		0xFFFF,                         // <<-- 重启动索引
		1, 0, 5, 4, 7, 6, 3, 2        // 第二个条带
	};
	//2. 创建和使用shaders
	string VertexShaderName = "vertex.vert", FragmentShaderName = "fragment.frag";
	if (!isShaderFileExists(VertexShaderName) || !isShaderFileExists(FragmentShaderName)) {
		cout << "不能找到着色器文件！\n请确认路径！" << endl;
		return;
	}
	ShaderInfo shader[] = {
		{ GL_VERTEX_SHADER, VertexShaderName.c_str()},
		{ GL_FRAGMENT_SHADER, FragmentShaderName.c_str()},
		{ GL_NONE, NULL },
	};
	shader_program = LoadShaders(shader);
	glUseProgram(shader_program);
	render_model_matrix_loc = glGetUniformLocation(shader_program, "model_matrix");
	render_projection_matrix_loc = glGetUniformLocation(shader_program, "projection_matrix");
	//3. 
	glGenVertexArrays(NumVAOs, VAOs);
	glBindVertexArray(VAOs[0]);
	glGenBuffers(1, ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);
	glGenBuffers(NumBuffers, Buffers);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_positions) , cube_positions, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, ((const void*)(32 * sizeof(float))));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	// 加载和创建纹理
	glGenTextures(1, &texture);            // 生成纹理
	glBindTexture(GL_TEXTURE_2D, texture); // 绑定纹理
	// 设置纹理环绕方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // x轴
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // y轴
	// 设置纹理过滤方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // 缩小
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);               // 放大
	// 图像y轴翻转
	stbi_set_flip_vertically_on_load(true);
	// 加载纹理图片,创建纹理和生成多级渐远纹理
	int width, height, nrChannels;
	unsigned char* data = stbi_load("flag2.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		// 生成纹理
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		// 生成多级渐远纹理
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	// 释放图像内存
	stbi_image_free(data);
	// 加载图片
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	//4. clear screen
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void display() {

	float t = float(GetTickCount() & 0x1FFF) / float(0x1FFF);
	static float q = 0.0f;
	static const vmath::vec3 X(1.0f, 0.0f, 0.0f);
	static const vmath::vec3 Y(0.0f, 1.0f, 0.0f);
	static const vmath::vec3 Z(0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (flag) {
		t = last_t;
	}
	else {
		last_t = t;
	}
	vmath::mat4 model_matrix(vmath::translate(0.0f, 0.0f, -2.0f) * rotate(t * 720.0f, X) * rotate(t * 540.0f, Y) * rotate(t * 360.0f, Z));
	vmath::mat4 projection_matrix(vmath::frustum(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 500.0f));
	glUniformMatrix4fv(render_projection_matrix_loc, 1, GL_FALSE, projection_matrix);
	glUniformMatrix4fv(render_model_matrix_loc, 1, GL_FALSE, model_matrix);

	glEnable(GL_CULL_FACE);
	glBindVertexArray(VAOs[0]);
	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(0xFFFF);//一般设置比较比较大的值（不可能的值）
	glDrawElements(GL_TRIANGLE_STRIP, 17, GL_UNSIGNED_SHORT, NULL);
	//glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glutSwapBuffers();
	glutPostRedisplay();
}

void setMenu()
{
	glutAddMenuEntry("Nearest Filter", 0);
	glutAddMenuEntry("Linear  Filter", 1);
	glutAddMenuEntry("MipMap  Filter", 2);
	glutAddMenuEntry("NEAREST_MIPMAP_NEAREST", 3);
	glutAddMenuEntry("LINEAR_MIPMAP_LINEAR", 4);
	glutAddMenuEntry("NEAREST_MIPMAP_LINEAR", 5);
	glutAddMenuEntry("LINEAR_MIPMAP_NEAREST", 6);
	glutAddMenuEntry("Anisotropic On", 7);
	glutAddMenuEntry("Anisotropic Off", 8);
	glutAddMenuEntry("stop", 9);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void processMenu(int key)
{
	GLfloat fLargest;
	switch (key) {
	case 0://
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		break;
	case 1://
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		break;
	case 2://
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_MIPMAP);
		break;
	case 3://
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		break;
	case 4://
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		break;
	case 5://
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		break;
	case 6://
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		break;
	case 7://
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, fLargest);
		break;
	case 8:
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0f);
		break;
	case 9:
		flag = !flag;
		break;
	default:
		break;
	}
}

void keyfunc(GLboolean key, GLint x, GLint y) {
	switch (key)
	{
	case 'd':
		glutFullScreen();
		break;
	case 'e':
		glutReshapeWindow(1000, 1000);
		glutPositionWindow(600, 200);
		break;
	case 27:
		glutExit();
		break;
	default:
		break;
	}
}
void reshape(GLint wd, GLint wh) {
	glViewport(0, 0, wd, wh);
	glutSwapBuffers();
	glutPostRedisplay();
}

int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(1000, 1000);
	glutInitContextVersion(4, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("FILE TEXTURE EXAMPLE");
	glutPositionWindow(600, 200);
	if (glewInit())
	{
		cerr << "Unable to Initialize GLEW" << endl;
	}
	init();
	glutKeyboardFunc(keyfunc);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutCreateMenu(processMenu);
	setMenu();
	glutMainLoop();
	return 0;
}