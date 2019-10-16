#ifndef Pen_h
#define Pen_h
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Shader.h"
#include <vector>
#include <cmath>
using namespace std;

// 这个类用于创建绘制墨迹的笔
// This class is used to create a pen which is used to draw the ink mark
class Pen {
public:
	// 构造函数，设置墨迹粗细和位置
	// constructor, set size and position of ink mark
	Pen(GLfloat xPos, GLfloat yPos, GLfloat zPos, GLfloat Thickness)
		: originPosition(glm::vec3(xPos, yPos, zPos)), thickness(Thickness){
		configurePen();
	}
	Pen(glm::vec3 penPosition, GLfloat Thickness)
		: originPosition(penPosition), thickness(Thickness){
		configurePen();
	}

	// 配置笔，被构造函数调用
	// configure pen, which is called by constructor
	void configurePen() {
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(float), &originPosition, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glBindVertexArray(0);
	}

	// 绘制画笔
	// draw pen
	void drawPen(Shader shader) {
		shader.use();
		glBindVertexArray(VAO);
		shader.setMat4("model", offsetModel);
		glPointSize(thickness);	// 设置绘制的像素点大小
								// set the size of the rendered pixel
		glEnable(GL_SMOOTH_POINT_SIZE_RANGE);
		glDrawArrays(GL_POINTS, 0, 1);	
	}

	// 设置画笔的位置
	// set the position of pen
	void setOriginPosition(glm::vec3 Position) {
		offsetModel = glm::mat4(1.0);
		offsetModel = glm::translate(offsetModel, glm::vec3(Position));
	}

private:
	GLfloat thickness;	// 墨迹大小
						// size of ink mark
	glm::vec3 originPosition;	// 设置墨迹位置
								// set the position of ink mark
	unsigned int VAO, VBO;	// 设置 OpenGL 获取位置信息所需要的 vertex array object 和 vertex buffer object
							// set the VAO and VBO needed by obtaining position in OpenGL
	glm::mat4 offsetModel;	// 设置墨迹偏移位置
							// set the offset position of ink mark
};

#endif /* Pen_h */
