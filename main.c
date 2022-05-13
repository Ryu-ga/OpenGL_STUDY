#include "common.h"
#include "shader.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <math.h>
#define RADIAN(x) x*(M_PI/180.0f)
#define MAT4(X) { \
	(X), 0, 0, 0, \
    0, (X), 0, 0, \
    0, 0, (X), 0, \
    0, 0, 0, (X) \
	}

struct JRWINDOW {
	GLFWwindow* window;
	unsigned int width; unsigned int height;
};

struct JRCAMERA {
	float horizontalAngle, verticalAngle;
	vec3 pos, dir, right, up;
};

struct JRINPUT {
	GLfloat x, y;
	int keys[1024];
}; struct JRWINDOW* mainwindow = NULL; struct JRCAMERA* maincamera = NULL;
struct JRINPUT* maininput = NULL;
double lastTime = 0, currentTime = 0;

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (maininput == NULL) {
		fprintf(stderr, "Please Check Main Devices\n");
		return;
	}
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	if (key >= 0 && key < 1024) {
		if (action == GLFW_PRESS) 
			maininput->keys[key] = 1;
		else if (action == GLFW_RELEASE)
			maininput->keys[key] = 0;
	}
}

void KeyControl() {
	double deltaTime = currentTime - lastTime;
	
	if (maininput == NULL || maincamera == NULL) {
		fprintf(stderr, "Please Check Main Devices\n");
		return;
	}

	vec3 dx, dy, dz;

	glm_vec3_scale(maincamera->right, deltaTime*1.0f, dx);
	glm_vec3_scale(maincamera->up, deltaTime*1.0f, dy);
	glm_vec3_scale(maincamera->dir, deltaTime*1.0f, dz);

	int IsShiftOn = 0;

	if (maininput->keys[GLFW_KEY_LEFT_SHIFT]) {
		IsShiftOn = 1;
	}

	if (maininput->keys[GLFW_KEY_W]) {
		glm_vec3_add(maincamera->pos, dz, maincamera->pos);
	}
	if (maininput->keys[GLFW_KEY_A]) {
		glm_vec3_sub(maincamera->pos, dx, maincamera->pos);
	}
	if (maininput->keys[GLFW_KEY_S]) {
		glm_vec3_sub(maincamera->pos, dz, maincamera->pos);
	}
	if (maininput->keys[GLFW_KEY_D]) {
		glm_vec3_add(maincamera->pos, dx, maincamera->pos);
	}
	if (maininput->keys[GLFW_KEY_SPACE]) {
		if (IsShiftOn) {
			glm_vec3_sub(maincamera->pos, dy, maincamera->pos);
		} else {
			glm_vec3_add(maincamera->pos, dy, maincamera->pos);
		}
	}
}

void MouseCallback(GLFWwindow* window, double xPos, double yPos) {
	double deltaTime = currentTime - lastTime;

	if (mainwindow == NULL || maincamera == NULL || maininput == NULL) {
		fprintf(stderr, "Please Check Main Devices\n");
		return;
	}

	float xOffset = mainwindow->width/2.0f - xPos, yOffset = mainwindow->height/2.0f - yPos;
	
	maininput->x = xPos;
	maininput->y = yPos;
	glfwSetCursorPos(window, mainwindow->width/2.0f, mainwindow->height/2.0f);
	maincamera->horizontalAngle += 0.1f*deltaTime*xOffset;
	maincamera->verticalAngle += 0.1f*deltaTime*yOffset;

	maincamera->dir[0] = cos(maincamera->verticalAngle) * sin(maincamera->horizontalAngle);
	maincamera->dir[1] = sin(maincamera->verticalAngle);
	maincamera->dir[2] = cos(maincamera->verticalAngle) * cos(maincamera->horizontalAngle);

	maincamera->right[0] = sin(maincamera->horizontalAngle - RADIAN(90.0f));
	maincamera->right[1] = 0.0f;
	maincamera->right[2] = cos(maincamera->horizontalAngle - RADIAN(90.0f));

	glm_vec3_cross(maincamera->right, maincamera->dir, maincamera->up);
}

void ScrollCallback(GLFWwindow* window, double xOffset, double yOffset) {

}

void JRGetCameraMVP(mat4 mvp, float fovy, float aspect, float zNear, float zFar) {
		mat4 proj, view, model = MAT4(1.0f);
		glm_perspective(fovy, aspect, zNear, zFar, proj);
		glm_look(maincamera->pos, maincamera->dir, maincamera->up, view);
		glm_mat4_mulN((mat4 *[]){&proj, &view, &model}, 3, mvp);
}

void _update_fps_counter(struct JRWINDOW* window);

int JRInit() {
	if (!glfwInit()) {
		fprintf(stderr, "Failed to Initialize GLFW\n");
		return 1;
	}	

	return 0;
}

int JRHintWindow() {
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // for mac
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	return 0;
}

int JRInitWindow(struct JRWINDOW* window, const char* title) {
	if (window->window) {
		fprintf(stderr, "Already Initialize\n");
		return 1;
	}

	GLFWwindow* glfwWindow = glfwCreateWindow(window->width, window->height, title, NULL, NULL);
	if (glfwWindow == NULL) {
		fprintf(stderr, "Failed to Create Window!\n");
		return 1;
	}	

	window->window = glfwWindow;
	return 0;
}

int JRSetCurrentWindow(const struct JRWINDOW* window) {
	glfwMakeContextCurrent(window->window);

	return 0;
}

int JRWindowShouldClose(const struct JRWINDOW* window) {
	return !glfwWindowShouldClose(window->window);
}

int JRTerminateWindow(const struct JRWINDOW* window) {
	glfwDestroyWindow(window->window);

	return 0;
}

int JRTerminate() {
	glfwTerminate();

	return 0;
}

struct JROBJECT {
	GLuint vao, vbo, ebo;
	GLuint shader;
	GLuint model_matrix;
};

int JRInitObject(struct JROBJECT* obj, const void* vertices, size_t buffer_size, const void* indices, size_t element_size) {
	glGenVertexArrays(1, &obj->vao);
	glGenBuffers(1, &obj->vbo);
	glGenBuffers(1, &obj->ebo);

	glBindVertexArray(obj->vao);
	glBindBuffer(GL_ARRAY_BUFFER, obj->vbo);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj->ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, element_size, indices, GL_STATIC_DRAW);
	glBindVertexArray(0);

	return 0;
}

int JRFreeObject(struct JROBJECT* obj) {
	if (obj->vao)
		glDeleteVertexArrays(1, &obj->vao);
	if (obj->vbo)
		glDeleteBuffers(1, &obj->vbo);
	if (obj->ebo)
		glDeleteBuffers(1, &obj->ebo);
	if (obj->shader)
		glDeleteProgram(obj->shader);

	return 0;
}

int JRDrawObject(const struct JROBJECT* obj, GLenum type, GLint size) {
	glBindVertexArray(obj->vao);
	glDrawElements(type, size, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	return 0;
}

int main(void) {
	JRInit();
	JRHintWindow();

	struct JRWINDOW window = {
		.width = 800,
		.height = 600,
	};

	struct JRCAMERA camera = {
		.pos = {0.0f, 1.0f, 0.5f},
	};

	struct JRINPUT input = {
		0
	};

	mainwindow = &window;
	maincamera = &camera;
	maininput = &input;

	JRInitWindow(mainwindow, "Main Window");
	glfwSetKeyCallback(mainwindow->window, KeyCallback);
	glfwSetCursorPosCallback(mainwindow->window, MouseCallback);

	JRSetCurrentWindow(mainwindow);
	
	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to Initialize GLEW\n");
		return 1;
	}

	glfwSwapInterval(1);
	glEnable(GL_DEPTH_TEST | GL_BLEND);
	glDepthFunc(GL_LESS);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	vec3 vertices[] = {
		0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
		0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f, 0.0f, 1.0f};

	GLuint indices[] = {
		0, 1, 3,
		1, 2, 3
	};

	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(1);
	unsigned char* data = stbi_load("yjm.jpg", &width, &height, &nrChannels, 0);
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);

	struct JROBJECT rect1 = {0};
	JRInitObject(&rect1, vertices, sizeof(vertices), indices, sizeof(indices));
	glBindVertexArray(rect1.vao);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void* )(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void* )(3*sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	rect1.shader = NewProgramFromFile("img_vertex.glsl", "img_fragment.glsl");

	stbi_set_flip_vertically_on_load(0);
	data = stbi_load("usy.jpg", &width, &height, &nrChannels, 0);
	GLuint texture2;
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);

	struct JROBJECT rect2 = {0};
	JRInitObject(&rect2, vertices, sizeof(vertices), indices, sizeof(indices));
	glBindVertexArray(rect2.vao);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void* )(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void* )(3*sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	rect2.shader = NewProgramFromFile("img_vertex.glsl", "img_fragment.glsl");

	GLint transform_location1 = glGetUniformLocation(rect1.shader, "mvp");
	GLint texture_location1 = glGetUniformLocation(rect1.shader, "ourTexture");

	GLint transform_location2 = glGetUniformLocation(rect2.shader, "mvp");
	GLint texture_location2 = glGetUniformLocation(rect2.shader, "ourTexture");

	do {
		lastTime	= currentTime;
		currentTime = glfwGetTime();
		glfwPollEvents();
		KeyControl();
		
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f); glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		_update_fps_counter(mainwindow);
		
		GLfloat d = glfwGetTime()*-0.5f;

		mat4 mvp;
		JRGetCameraMVP(mvp, RADIAN(45.0f), 4.0f/3.0f, 0.1f, 100.0f);

		glUseProgram(rect1.shader);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

		glUniformMatrix4fv(transform_location1, 1, GL_FALSE, (GLfloat* )mvp);
		glUniform1i(texture_location1, 0);

		JRDrawObject(&rect1, GL_TRIANGLES, 6);

		glUseProgram(rect2.shader);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);

		mat4 model;
		glm_translate_to(mvp, (vec3){sin(glfwGetTime()), cos(glfwGetTime()), 0}, model);
		glm_rotate(model, sin(glfwGetTime()), (vec3){0.0f, 0.0f, 1.0f});
		glUniformMatrix4fv(transform_location2, 1, GL_FALSE, (GLfloat* )model);
		glUniform1i(texture_location2, 1);

		JRDrawObject(&rect2, GL_TRIANGLES, 6);

		glfwSwapBuffers(mainwindow->window);
	} while (JRWindowShouldClose(mainwindow));
	
	JRFreeObject(&rect1);
	JRTerminate();
	return 0;
}

void _update_fps_counter(struct JRWINDOW* window) {
  static double previous_seconds = 0;
  if (previous_seconds == 0)
	  previous_seconds = glfwGetTime();
  static int frame_count;
  double current_seconds = glfwGetTime();
  double elapsed_seconds = current_seconds - previous_seconds;
  if (elapsed_seconds > 0.25) {
    previous_seconds = current_seconds;
    double fps = (double)frame_count / elapsed_seconds;
    char tmp[128];
    sprintf(tmp, "opengl @ fps: %.2f", fps);
    glfwSetWindowTitle(window->window, tmp);
    frame_count = 0;
  }
  frame_count++;
}
