#ifndef Arrow_h
#define Arrow_h

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
// 这个类被用作配置和绘制向量箭头
// this class is used to configure and render the vector arrows
class Arrow {
public:
	// 配置箭头的大小和头位置
	// configure the size and the head position of the arrows
	Arrow(float scale = 1.0, glm::vec3 headPosition = glm::vec3(0.0, 1.0, 0.0)) {
		// 通过设置 z 轴，记录 y 轴（头位置的标准化向量）值，构建旋转坐标系，初始化箭头旋转的位置
		// get the rotation coordinate to initialize the rotation position of the arrows by set z-axis and record y-axis(normalized head position)
		glm::vec3 axisZ = glm::vec3(0.0, 0.0, 1.0);
		glm::vec3 axisY = glm::normalize(headPosition);
		glm::vec3 axisX = glm::cross(axisY, axisZ);
		glm::mat3 newCoord = glm::mat3(axisX, axisY, axisZ);

		// 设置正常大小的向量箭头
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

		// 根据 scale 的值设置箭头大小
		// set arrow size according to scale value
		arrowVertices[1].y *= scale;
		arrowVertices[3].y *= scale;
		arrowVertices[4].y *= scale;
		arrowVertices[5].y *= scale;
		arrowVertices[6].y *= scale;

		// 创建数组存储 VAO，EBO 所需要的信息
		// create array to store the information needed by the VAO and EBO
		for (int i = 0; i < 7; i++) {
			arrowVertices[i] = newCoord * arrowVertices[i];
		}
		int arrowIndices[] = {
			0, 1, 2,
			2, 1, 3,
			4, 5, 6
		};

		// 设置成员变量
		// set member variables
		headPos = initHeadPos = glm::vec3(arrowVertices[4]);
		tailPos = initTailPos = glm::vec3(0.0, 0.0, 0.0);
		rotateOffsetModel = glm::mat4(1.0);
		positionOffset = glm::mat4(1.0);
		alertHeadTailPosition(positionOffset);

		// 配置 VAO，VBO 和 EBO
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

	// 绘制箭头
	// draw arrow
	void drawArrow(Shader shader) {
		shader.use();	
		shader.setMat4("model", positionOffset * rotateOffsetModel);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, (void*)0);
		glBindVertexArray(0);
	}

	// 设置旋转矩阵用于旋转向量箭头
	// set rotation matrix to rotate the vector arrows
	void rotateArrow(float degree, glm::vec3 axis = glm::vec3(0.0, 0.0, 1.0)) {
		rotateOffsetModel = glm::rotate(rotateOffsetModel, degree, axis);     
		headPos = glm::vec3(rotateOffsetModel * glm::vec4(initHeadPos, 1.0));
		tailPos = glm::vec3(rotateOffsetModel * glm::vec4(initTailPos, 1.0));
	}

	// 设置箭头的位置
	// set arrow's position
	void setPosition(glm::vec3 Position) {
		positionOffset = glm::translate(positionOffset, Position);
		headPos = glm::vec3(positionOffset * rotateOffsetModel * glm::vec4(initHeadPos, 1.0));
		tailPos = glm::vec3(positionOffset * rotateOffsetModel * glm::vec4(initTailPos, 1.0));
	}

	// 更改头位置和尾位置
	// alter head position and tail position
	void alertHeadTailPosition(glm::mat4 alertModel) {
		headPos = glm::vec3(alertModel * glm::vec4(initHeadPos, 1.0));
		tailPos = glm::vec3(alertModel * glm::vec4(initTailPos, 1.0));
	}

	// 获取头位置
	// get head position
	glm::vec3 getHeadPosition() {
		return headPos;
	}

	// 获取尾位置
	// get tail position
	glm::vec3 getTailPosition() {
		return tailPos;
	}
private:
	unsigned int VAO, VBO, EBO;
	glm::mat4 rotateOffsetModel, positionOffset;	// 旋转矩阵（旋转箭头），位移矩阵（移动箭头）
													// rotation matrix(rotate arrow), offset matrix(move arrow)
	glm::vec3 headPos, tailPos;	// 头位置 尾位置
								// head position, tail position
	glm::vec3 initHeadPos, initTailPos;  // 记录向量最初的位置，用于旋转和位移计算
										 // Record the initial position of the vector for rotation and displacement calculations
	GLfloat Scale;	// 用于设置箭头的大小
					// used to set arrow size 

};

#endif /* Arrow_h */
