#ifndef Arrow_h
#define Arrow_h

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
// ����౻�������úͻ���������ͷ
// this class is used to configure and render the vector arrows
class Arrow {
public:
	// ���ü�ͷ�Ĵ�С��ͷλ��
	// configure the size and the head position of the arrows
	Arrow(float scale = 1.0, glm::vec3 headPosition = glm::vec3(0.0, 1.0, 0.0)) {
		// ͨ������ z �ᣬ��¼ y �ᣨͷλ�õı�׼��������ֵ��������ת����ϵ����ʼ����ͷ��ת��λ��
		// get the rotation coordinate to initialize the rotation position of the arrows by set z-axis and record y-axis(normalized head position)
		glm::vec3 axisZ = glm::vec3(0.0, 0.0, 1.0);
		glm::vec3 axisY = glm::normalize(headPosition);
		glm::vec3 axisX = glm::cross(axisY, axisZ);
		glm::mat3 newCoord = glm::mat3(axisX, axisY, axisZ);

		// ����������С��������ͷ
		// set the normal size vector arrow
		Scale = scale;
		glm::vec3 arrowVertices[] = {
			// rectangle
			glm::vec3(-0.003, 0.0,  0.0),      // bottom-left
			glm::vec3(-0.003, 0.86, 0.0),      // up-left
			glm::vec3(0.003, 0.0,  0.0),      // bottom-right
			glm::vec3(0.003, 0.86, 0.0),      // up-right
			// triangle
			glm::vec3(0.0,  0.9,  0.0),      // up
			glm::vec3(-0.01, 0.86,  0.0),      // bottom-left
			glm::vec3(0.01, 0.86,  0.0)       // bottom-right
		};

		// ���� scale ��ֵ���ü�ͷ��С
		// set arrow size according to scale value
		arrowVertices[1].y *= scale;
		arrowVertices[3].y *= scale;
		arrowVertices[4].y *= scale;
		arrowVertices[5].y *= scale;
		arrowVertices[6].y *= scale;

		// ��������洢 VAO��EBO ����Ҫ����Ϣ
		// create array to store the information needed by the VAO and EBO
		for (int i = 0; i < 7; i++) {
			arrowVertices[i] = newCoord * arrowVertices[i];
		}
		int arrowIndices[] = {
			0, 1, 2,
			2, 1, 3,
			4, 5, 6
		};

		// ���ó�Ա����
		// set member variables
		headPos = initHeadPos = glm::vec3(arrowVertices[4]);
		tailPos = initTailPos = glm::vec3(0.0, 0.0, 0.0);
		rotateOffsetModel = glm::mat4(1.0);
		positionOffset = glm::mat4(1.0);
		alertHeadTailPosition(positionOffset);

		// ���� VAO��VBO �� EBO
		// configure VAO, VBO and EBO
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(arrowVertices), &arrowVertices[0], GL_STATIC_DRAW);

		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(arrowIndices), arrowIndices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glBindVertexArray(0);
	}

	// ���Ƽ�ͷ
	// draw arrow
	void drawArrow(Shader shader) {
		shader.use();	
		shader.setMat4("model", positionOffset * rotateOffsetModel);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, (void*)0);
		glBindVertexArray(0);
	}

	// ������ת����������ת������ͷ
	// set rotation matrix to rotate the vector arrows
	void rotateArrow(float degree, glm::vec3 axis = glm::vec3(0.0, 0.0, 1.0)) {
		rotateOffsetModel = glm::rotate(rotateOffsetModel, degree, axis);     
		headPos = glm::vec3(rotateOffsetModel * glm::vec4(initHeadPos, 1.0));
		tailPos = glm::vec3(rotateOffsetModel * glm::vec4(initTailPos, 1.0));
	}

	// ���ü�ͷ��λ��
	// set arrow's position
	void setPosition(glm::vec3 Position) {
		positionOffset = glm::translate(positionOffset, Position);
		headPos = glm::vec3(positionOffset * rotateOffsetModel * glm::vec4(initHeadPos, 1.0));
		tailPos = glm::vec3(positionOffset * rotateOffsetModel * glm::vec4(initTailPos, 1.0));
	}

	// ����ͷλ�ú�βλ��
	// alter head position and tail position
	void alertHeadTailPosition(glm::mat4 alertModel) {
		headPos = glm::vec3(alertModel * glm::vec4(initHeadPos, 1.0));
		tailPos = glm::vec3(alertModel * glm::vec4(initTailPos, 1.0));
	}

	// ��ȡͷλ��
	// get head position
	glm::vec3 getHeadPosition() {
		return headPos;
	}

	// ��ȡβλ��
	// get tail position
	glm::vec3 getTailPosition() {
		return tailPos;
	}
private:
	unsigned int VAO, VBO, EBO;
	glm::mat4 rotateOffsetModel, positionOffset;	// ��ת������ת��ͷ����λ�ƾ����ƶ���ͷ��
													// rotation matrix(rotate arrow), offset matrix(move arrow)
	glm::vec3 headPos, tailPos;	// ͷλ�� βλ��
								// head position, tail position
	glm::vec3 initHeadPos, initTailPos;  // ��¼���������λ�ã�������ת��λ�Ƽ���
										 // Record the initial position of the vector for rotation and displacement calculations
	GLfloat Scale;	// �������ü�ͷ�Ĵ�С
					// used to set arrow size 

};

#endif /* Arrow_h */
