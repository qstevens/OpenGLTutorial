#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "shader.hpp"
#include "texture.hpp"
#include "controls.hpp"
#include "object.hpp"
#include "vboindexer.hpp"

int main() {
	glewExperimental = true;
	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window;
	window = glfwCreateWindow(1024, 768, "Tutorial 01", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwPollEvents();
	glfwSetCursorPos(window, 1024 / 2, 768 / 2);

	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE); 

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Load shaders
	GLuint programID = LoadShaders("Vertex.glsl", "Fragment.glsl");

	// Create vertex array
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Setup camera
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	GLuint ModelID = glGetUniformLocation(programID, "M");
	GLuint ViewID = glGetUniformLocation(programID, "V");

	// Create texture
	GLuint Texture = loadDDS("uvmap.DDS");
	GLuint TextureID = glGetUniformLocation(programID, "myTextureSampler");

	// Add light
	GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");

	// Load .obj file vertex data
	std::vector< glm::vec3 > vertices;
	std::vector< glm::vec2 > uvs;
	std::vector< glm::vec3 > normals;
	bool res = loadOBJ("cube.obj", vertices, uvs, normals);

	// Add vertex attributes to index
	std::vector<unsigned short> indices;
	std::vector<glm::vec3> indexed_vertices;
	std::vector<glm::vec2> indexed_uvs;
	std::vector<glm::vec3> indexed_normals;
	indexVBO(vertices, uvs, normals, indices, indexed_vertices, indexed_uvs, indexed_normals);

	// Create vertex buffer
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);

	// Create UV buffer
	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size()*sizeof(glm::vec2), &indexed_uvs[0], GL_STATIC_DRAW);

	// Create normals buffer
	GLuint normalbuffer;
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);

	// Create element vertex buffer
	GLuint elementbuffer;
	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);

	//// Create extra frame buffer
	//GLuint framebuffer = 0;
	//glGenFramebuffers(1, &framebuffer);
	//glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	//// Create rendered texture
	//GLuint renderedTexture;
	//glGenTextures(1, &renderedTexture);
	//glBindTexture(GL_TEXTURE_2D, renderedTexture);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1024, 768, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//// Create depth render buffer
	//GLuint depthrenderbuffer;
	//glGenRenderbuffers(1, &depthrenderbuffer);
	//glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1024, 768);
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

	//glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);

	//GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	//glDrawBuffers(1, DrawBuffers); 
	//
	//if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	//	return false;

	//static const GLfloat g_quad_vertex_buffer_data[] = {
	//	-1.0f, -1.0f, 0.0f,
	//	 1.0f, -1.0f, 0.0f,
	//	-1.0f,  1.0f, 0.0f,
	//	-1.0f,  1.0f, 0.0f,
	//	 1.0f, -1.0f, 0.0f,
	//	 1.0f,  1.0f, 0.0f,
	//};

	//GLuint quad_vertexbuffer;
	//glGenBuffers(1, &quad_vertexbuffer);
	//glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);

	//GLuint quad_programID = LoadShaders("PassthroughVertex.glsl", "WobblyTextureFragment.glsl");
	//GLuint texID = glGetUniformLocation(quad_programID, "renderedTexture");
	//GLuint timeID = glGetUniformLocation(quad_programID, "time");

	double lastTime = glfwGetTime();
	int nbFrames = 0;

	do {
		double currentTime = glfwGetTime();
		nbFrames++;
		if (currentTime - lastTime >= 1.0) {
			printf("%f ms/frame\n", 1000.0 / double(nbFrames));
			nbFrames = 0;
			lastTime += 1.0;
		}
		/*
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glViewport(0, 0, 1024, 768);*/

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		computeMatricesFromInputs(window);
		glm::mat4 Projection = getProjectionMatrix();
		glm::mat4 View = getViewMatrix();
		glm::mat4 Model = glm::mat4(1.0f);
		glm::mat4 mvp = Projection * View * Model;

		// Use shaders
		glUseProgram(programID);

		// Pass transformation
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);
		glUniformMatrix4fv(ModelID, 1, GL_FALSE, &Model[0][0]);
		glUniformMatrix4fv(ViewID, 1, GL_FALSE, &View[0][0]);

		// Pass light position
		glm::vec3 lightPos = glm::vec3(4, 4, 4);
		glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

		// Pass textures
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		glUniform1i(TextureID, 0);

		// Configure vertex buffer
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute 0. Must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// Configure UV buffer
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		glVertexAttribPointer(
			1,                                // attribute 1. Must match layout in shader.
			2,                                // size : U+V => 2
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// Configure normals buffer
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
		glVertexAttribPointer(
			2,                                // attribute
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// Configure element index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

		glViewport(0, 0, 1024, 768);

		// Draw
		glDrawElements(
			GL_TRIANGLES, 
			indices.size(), 
			GL_UNSIGNED_SHORT, 
			(void*)0
		);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);

		//// Draw rendered texture
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//glViewport(0, 0, 1024, 768);

		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//glUseProgram(quad_programID);

		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, renderedTexture);
		//glUniform1i(texID, 0);

		//glUniform1f(timeID, (float)(glfwGetTime() * 10.0f));

		//// Configure vertex buffer
		//glEnableVertexAttribArray(0);
		//glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
		//glVertexAttribPointer(
		//	0,                  // attribute 0.
		//	3,                  // size
		//	GL_FLOAT,           // type
		//	GL_FALSE,           // normalized?
		//	0,                  // stride
		//	(void*)0            // array buffer offset
		//);

		//glDrawArrays(GL_TRIANGLES, 0, 6);

		//glDisableVertexAttribArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	} 
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteBuffers(1, &normalbuffer);
	glDeleteBuffers(1, &elementbuffer);
	glDeleteProgram(programID);
	glDeleteTextures(1, &Texture);

	//glDeleteFramebuffers(1, &framebuffer);
	//glDeleteTextures(1, &renderedTexture);
	//glDeleteRenderbuffers(1, &depthrenderbuffer);
	//glDeleteBuffers(1, &quad_vertexbuffer);
	glDeleteVertexArrays(1, &VertexArrayID);

	glfwTerminate();

	return 0;
}
