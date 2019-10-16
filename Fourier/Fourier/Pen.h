#ifndef Pen_h
#define Pen_h
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Shader.h"
#include <vector>
#include <cmath>
using namespace std;

// ��������ڴ�������ī���ı�
// This class is used to create a pen which is used to draw the ink mark
class Pen {
public:
	// ���캯��������ī����ϸ��λ��
	// constructor, set size and position of ink mark
	Pen(GLfloat xPos, GLfloat yPos, GLfloat zPos, GLfloat Thickness)
		: originPosition(glm::vec3(xPos, yPos, zPos)), thickness(Thickness){
		configurePen();
	}
	Pen(glm::vec3 penPosition, GLfloat Thickness)
		: originPosition(penPosition), thickness(Thickness){
		configurePen();
	}

	// ���ñʣ������캯������
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

	// ���ƻ���
	// draw pen
	void drawPen(Shader shader) {
		shader.use();
		glBindVertexArray(VAO);
		shader.setMat4("model", offsetModel);
		glPointSize(thickness);	// ���û��Ƶ����ص��С
								// set the size of the rendered pixel
		glEnable(GL_SMOOTH_POINT_SIZE_RANGE);
		glDrawArrays(GL_POINTS, 0, 1);	
	}

	// ���û��ʵ�λ��
	// set the position of pen
	void setOriginPosition(glm::vec3 Position) {
		offsetModel = glm::mat4(1.0);
		offsetModel = glm::translate(offsetModel, glm::vec3(Position));
	}

private:
	GLfloat thickness;	// ī����С
						// size of ink mark
	glm::vec3 originPosition;	// ����ī��λ��
								// set the position of ink mark
	unsigned int VAO, VBO;	// ���� OpenGL ��ȡλ����Ϣ����Ҫ�� vertex array object �� vertex buffer object
							// set the VAO and VBO needed by obtaining position in OpenGL
	glm::mat4 offsetModel;	// ����ī��ƫ��λ��
							// set the offset position of ink mark
};

#endif /* Pen_h */
