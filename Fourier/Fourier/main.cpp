/*
	////////////////////////// ATTENTION ע�� /////////////////////////////////////
	�����ô����ʵ��˼·�����  readme.md �ļ�
	if you want to know the implementation thought of the non-referenced codes, please read readme.md file
*/

// windows ��ʹ�����˳��
// windows please use this order
#include <glad/glad.h>
#include <GLFW/glfw3.h>

/* macOS ��ʹ�����˳��
   macOS please use this order
#include <glad/glad.h>
#include <GLFW/glfw3.h>
*/

// Ϊ������������
// include for matrix calculation
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

// Ϊ������ͷ����
// include for create arrows
#include "Arrow.h"

// Ϊ������������
// include for creating painting brush
#include "Pen.h"

// Ϊ�����ʷ�����
// include for delaunay
// ������ https://github.com/Bl4ckb0ne/delaunay-triangulation
// reference from https://github.com/Bl4ckb0ne/delaunay-triangulation
#include "Delaunay/vector2.h"
#include "Delaunay/triangle.h"
#include "Delaunay/delaunay.h"
#include <iterator>
#include <algorithm>
#include <array>
#include <random>
#include <chrono>

// Ϊ��ȡͼƬ����
// include for load image
#define STB_IMAGE_IMPLEMENTATION
#include <stb-master/stb_image.h>

// Ϊ��ɢ����Ҷ�任��DFT����������
// reference for do DFT calculation
#include "fftw3.h"

// ����ͷ�ļ�
// other header files
#include <iostream>
#include <stdio.h>
#include <vector>
#include <map>
#include <unordered_map>
#include <string>
#include <ctime>
#include <cstdio>
#include <cmath>
using namespace std;

// ������Ļ���ڿ�ߣ��⽫Ӱ�촰�ڳߴ��Լ���Ⱦ�ٶȣ�
// set the screen's width and height (This will influence the window size and rendering speed)
#define SCR_WIDTH 300
#define SCR_HEIGHT 300

// ���㲿���㷨�����ѵ�ʱ��
// calculate the time spent on some algorithm
clock_t time_start, time_end;

// Ϊ unsordered_map ������
// define for unsordered_map
// ������ https://blog.csdn.net/zhangpiu/article/details/49837387
// reference for https://blog.csdn.net/zhangpiu/article/details/49837387
struct EqualKey
{
	bool operator () (const glm::vec3 &lhs, const glm::vec3 &rhs) const
	{
		return lhs.x == rhs.x
			&& lhs.y == rhs.y
			&& lhs.z == rhs.z;
	}
};
namespace std
{
	template <>
	struct hash<glm::vec3>
	{
		std::size_t operator()(const glm::vec3 &key) const
		{
			using std::size_t;
			using std::hash;
			// Compute individual hash values for first,
			// second and third and combine them using XOR
			// and bit shifting:
			return ((hash<float>()(key.x)
				^ (hash<float>()(key.y) << 1)) >> 1)
				^ (hash<float>()(key.z) << 1);
		}
	};
}

// �洢����Ϣ
// store edge information
class EdgeInfo {
public:
	glm::vec3 p1, p2;	// �洢���ӱߵĶ���
						// store the vertices used to connect the edge

	double len;			// �洢�ߵĳ���
						// store the length of the edge

	EdgeInfo(glm::vec3 P1, glm::vec3 P2, double len) : p1(P1), p2(P2), len(len) {}
};

// �洢�����Ϣ
// store node information
class Node {
public:
	Node() {
		isCheck = false;		
	}
	Node(glm::vec3 p) {
		isCheck = false;
		position = p;	
	}

	glm::vec3 position;		// �洢���λ��
							// store node position

	vector<glm::vec3> connections;		// �洢��ý�������Ľ��
										// store nodes connecting to the node

	bool isCheck;		// ����Ƿ��߹��õ�
						// check if you have passed the node
};

// OpenGL ���ú���
// OpenGL configuration functions
void framebuffer_size_callback(GLFWwindow* window, int width, int height);	// ���ڻص����������ڵ����ڳߴ�ı�ʱ�������ʵ��Ĳ���
																			// window call back function, which is used to fill proper arguments when you change the window size
void processInput(GLFWwindow* window);	// �������룬������Ҫ���ڶ�ȡ�������� ESC���˳����ƴ���
										// it's used to process input. It mainly used to read ESC key to exit rendering window.
void renderBoard();	// ���ƻ��庯��
					// render sketchpad

// ����������
// load texture function
unsigned int loadTexture(char const *path);

// �鲢���� -- Kruscal �㷨�е�һ��
// Merge Sort -- a step of Kruscal Algorithm
void MergeSort(vector<EdgeInfo>& arr);
void Combine(vector<EdgeInfo>&arr, vector<EdgeInfo>&arr1, vector<EdgeInfo>&arr2);

// ���鼯 -- ȥ����·
// union find set -- remove loops
glm::vec3 find_root(glm::vec3 p, unordered_map<glm::vec3, glm::vec3> & parents);
bool union_find(glm::vec3 p1, glm::vec3 p2, unordered_map<glm::vec3, glm::vec3> & parents, unordered_map<glm::vec3, int> & rank);

// ��ɢ����Ҷ�任
// DFT
void DFT(vector<glm::vec3> pointList, fftw_complex *out);

// �����·��
// find longest path
vector<glm::vec3> longestPath;
void findPath(unordered_map<glm::vec3, Node> tree, vector<glm::vec3> path);
void findLongestPath(unordered_map<glm::vec3, Node> tree);

// ���·����һ���˵㿪ʼ��������н��
// Check all nodes from a end point
vector<glm::vec3> pointList;
void outputAllNodes(unordered_map<glm::vec3, Node> & tree, vector<glm::vec3> longestPath);
void checkBranck(unordered_map<glm::vec3, Node> & tree, unordered_map<glm::vec3, Node>::iterator node);

// ������С������
// Create minimum spanning tree
void createTree(vector<EdgeInfo> edgeInfos, unordered_map<glm::vec3, Node> & tree);

int main() {
	// ��ʼ�� OpenGL ���� 
	// Initialize OpenGL window 
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	
	
	// initialize FBO used to store ink mark
	// ��ʼ�����ڴ洢ī���� FBO
	unsigned int boardFBO;
	glGenFramebuffers(1, &boardFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, boardFBO);
	unsigned int boardColorBuffer;
	glGenTextures(1, &boardColorBuffer);
	glBindTexture(GL_TEXTURE_2D, boardColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, boardColorBuffer, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		cout << "Board Color FBO isn't complete\n" << endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// ��������
	// Ceate pen(painting brush)
	Pen pen = Pen(0.0, 0.0, 0.0, 1.0);
	pen.setOriginPosition(glm::vec3(0.0, 0.0, 0.0));

	// ������ɫ��
	// setting shaders
	Shader arrowShader("shader/arrow.vs", "shader/arrow.fs");	// ���ڻ���������ͷ
																// use to render vector arrow
	Shader boardShader("shader/board.vs", "shader/board.fs");	// ���ڴ洢ī��
																// use to store ink mark																				
	Shader grayProcessShader("shader/edgeProcess.vs", "shader/grayProcess.fs");		// ���ڴ洢�Ҷȴ�����ͼƬ
																					// use to store the texture after doing gray process
	Shader edgeProcessShader("shader/edgeProcess.vs", "shader/edgeProcess.fs");		// ���ڴ洢��Ե�񻯴����ͼƬ
																					// use to store the texture after doing sharpening
	Shader blurProcessShader("shader/edgeProcess.vs", "shader/blurProcess.fs");		// ���ڴ洢��˹ģ��������ͼƬ
																					// use to store the texture after doing Gauss blur
	Shader gradientProcessShader("shader/edgeProcess.vs", "shader/gradientCalculate.fs");	// ���ڴ洢�ݶȴ�����ͼƬ
																							// use to store the texture after doing gradient process
	Shader nmsProcessShader("shader/edgeProcess.vs", "shader/NMS.fs");	// ���ڴ洢�����ֵ���ƣ�Non - maximum suppression��������ͼƬ
																		// use to store the texture after doing NMS(Non - maximum suppression)
	Shader binProcessShader("shader/edgeProcess.vs", "shader/binProcess.fs");	// ���ڶ�ֵ��������ͼƬ
																				// use to store the texture after doing binary

	// ����ͼƬ
	// loading texture
	stbi_set_flip_vertically_on_load(true);	// ���� OpenGL ������������һ��ͼƬ��������� y ���෴�������Ҫ��תͼƬ�� y ��
											// I need to reverse the texture coordinate's y-axis because the texture coordinates of OpenGL is opposite to the Y-axis of the texture coordinates of general pictures.
	unsigned int originImage = loadTexture("img/img1.jpg");	// �˴���ȡͼƬ
															// load texture here		

	// ������ɫ��
	// Configure shader
	boardShader.use();
	boardShader.setInt("boardColorBuffer", 0);

	// ���ûҶȴ��� FBO
	// set gray process FBO
	unsigned int grayFBO;
	glGenFramebuffers(1, &grayFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, grayFBO);
	unsigned int grayColorBuffer;
	glGenTextures(1, &grayColorBuffer);
	glBindTexture(GL_TEXTURE_2D, grayColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, grayColorBuffer, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		cout << "gray FBO isn't complete\n" << endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// ���ø�˹ģ�� FBO
	// set blur processing FBO.
	unsigned int blurProcessFBO[2];
	unsigned int blurTexture[2];
	glGenFramebuffers(2, blurProcessFBO);
	glGenTextures(2, blurTexture);
	for (int i = 0; i < 2; i++) {
		glBindFramebuffer(GL_FRAMEBUFFER, blurProcessFBO[i]);
		glBindTexture(GL_TEXTURE_2D, blurTexture[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blurTexture[i], 0);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			cout << "blurProcess FBO isn't complete" << endl;
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// �����ݶȴ��� FBO
	// set gradient process FBO
	unsigned int gradientProcessFBO;
	glGenFramebuffers(1, &gradientProcessFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, gradientProcessFBO);
	unsigned int gradientTexture;
	glGenTextures(1, &gradientTexture);
	glBindTexture(GL_TEXTURE_2D, gradientTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gradientTexture, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// ���÷����ֵ���� FBO
	// set Non-Maximum Suppression NMS FBO
	unsigned int nmsFBO;
	glGenFramebuffers(1, &nmsFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, nmsFBO);
	unsigned int nmsTexture;
	glGenTextures(1, &nmsTexture);
	glBindTexture(GL_TEXTURE_2D, nmsTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, nmsTexture, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// ���ö�ֵ������ FBO
	// set binary process FBO
	unsigned int binProcessFBO;
	glGenFramebuffers(1, &binProcessFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, binProcessFBO);
	unsigned int binProcessTexture;
	glGenTextures(1, &binProcessTexture);
	glBindTexture(GL_TEXTURE_2D, binProcessTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, binProcessTexture, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	// �Ҷȴ���
	// doing gray process
	glBindFramebuffer(GL_FRAMEBUFFER, grayFBO);
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	grayProcessShader.use();
	glBindTexture(GL_TEXTURE_2D, originImage);
	renderBoard();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// ��˹ģ������
	// doing Gauss blur process
	bool horizontal = true;
	bool isFirstInBlur = true;	
	for (int i = 0; i < 4; i++) {
		glBindFramebuffer(GL_FRAMEBUFFER, blurProcessFBO[i]);
		glClearColor(1.0, 1.0, 1.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		blurProcessShader.use();
		if (isFirstInBlur) {
			glBindTexture(GL_TEXTURE_2D, grayColorBuffer);
			isFirstInBlur = false;
		}
		else {
			glBindTexture(GL_TEXTURE_2D, blurTexture[!horizontal]);
		}
		blurProcessShader.setInt("horizontal", horizontal);
		horizontal = !horizontal;
		renderBoard();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// �ݶȴ���
	// doing gradient proess
	glBindFramebuffer(GL_FRAMEBUFFER, gradientProcessFBO);
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	gradientProcessShader.use();
	glBindTexture(GL_TEXTURE_2D, blurTexture[!horizontal]);
	renderBoard();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// �����ֵ���ƴ���
	// doing Non-maximum suppression process
	glBindFramebuffer(GL_FRAMEBUFFER, nmsFBO);
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	nmsProcessShader.use();
	glBindTexture(GL_TEXTURE_2D, gradientTexture);
	renderBoard();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);	

	// ��ֵ������
	// doing binary process	
	glBindFramebuffer(GL_FRAMEBUFFER, binProcessFBO);
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	binProcessShader.use();
	glBindTexture(GL_TEXTURE_2D, nmsTexture);
	renderBoard();
	float * finalPixel = new float[SCR_WIDTH * SCR_HEIGHT * 3];		
	glReadPixels(0, 0, SCR_WIDTH, SCR_HEIGHT, GL_RGB, GL_FLOAT, finalPixel);  // ��ȡ���մ���������Ϣ
																		      // get final pixels information
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	

	// �洢ͨ��ɸѡ�����ص㣬���������±�תΪ��Ļ����
	// store filtered pixels and convert those array index to screen coordinat
	cout << "�Ӵ�����ɵ������л�ȡ���ص�..." << endl;
	cout << "Get pixel points from processed texture..." << endl;
	vector<glm::vec3> getPoints;
	int index = 0;
	for (int i = 0; i < SCR_WIDTH * SCR_HEIGHT * 3;) {
		if (finalPixel[i] >= 0.5) {
			int tmp = i / 3;
			glm::vec3 point(tmp % SCR_WIDTH, tmp / SCR_WIDTH, 0.0); // transform them to screen coordinate
			getPoints.push_back(point);
			i += 3;
			index++;
		}
		else {
			i++;
		}
	}
	cout << "�������ص��ȡ��ɡ����ص����Ϊ��" << index << endl;
	cout << "Get all pixel points done. The number of pixel points is: " << index << endl << endl;
	

	// ִ�������ʷ��㷨
	// doing delaunay algorithm
	cout << "Begin doing delaunay..." << endl;
	cout << "ִ�������ʷִ���..." << endl;	
	std::vector<Vector2> points;
	for (int i = 0; i < getPoints.size(); ++i) {
		points.push_back(Vector2{ getPoints[i].x, getPoints[i].y });
	}
	Delaunay triangulation;
	const auto start = std::chrono::high_resolution_clock::now();
	const std::vector<Triangle> triangles = triangulation.triangulate(points);
	const auto end = std::chrono::high_resolution_clock::now();
	const std::chrono::duration<double> diff = end - start;
	cout << "delaunay algrithm done.";
	cout << "�����ʷִ������" << endl;
	std::cout << triangles.size() << " triangles generated in " << diff.count() << "s\n";
	std::cout << triangles.size() << " �������Σ��� " << diff.count() << "s �ڴ������\n\n";

	
	// �������ʷֽ���л�ȡ�໥���ӵĵ㹹�ɵı�
	// Obtaining the edges of interconnected points from triangulation results
	cout << "��ʼ��ȡ�ߣ������ʷִ���󣩵�����...\n";
	cout << "Begin obtaining edges(after doing delaunay) data...\n";
	const std::vector<Edge> edges = triangulation.getEdges();
	vector<EdgeInfo> edgeInfos;	
	getPoints.clear();
	for (int i = 0; i < edges.size(); i++) {		
		glm::vec3 point1 = glm::vec3((edges[i].v->x), edges[i].v->y, 0.0);
		glm::vec3 point2 = glm::vec3((edges[i].w->x), edges[i].w->y, 0.0);
		edgeInfos.push_back(EdgeInfo(point1, point2, glm::length(point1 - point2)));
	}
	cout << "��ȡ���������\n";
	cout << "Get edges data done\n\n";


	// ��С���������ң��鲢����Ͳ��鼯��
	// Search minimum spanning tree (Merge sort and union find set)
	cout << "��ʼ�鲢����...\n";
	cout << "Begin merge sort...\n";
	time_start = clock();
	MergeSort(edgeInfos);
	time_end = clock();
	cout << "�鲢�������...\n";
	cout << "Merge sort done\n";
	cout << "�� " << (double)(time_end - time_start) / CLOCKS_PER_SEC << " �롣" << endl;
	cout << "Spend " << (double)(time_end - time_start) / CLOCKS_PER_SEC << " s." << endl << endl;
	cout << "��ʼͨ�����鼯������С������\n";
	cout << "Begin creating minimum spanning tree by union find set...\n";
	time_start = clock();
	unordered_map<glm::vec3, Node> tree;
	createTree(edgeInfos, tree);	
	time_end = clock();
	cout << "������С���������.\n";
	cout << "Create minimum spanning tree done.\n";
	cout << "�� " << (double)(time_end - time_start) / CLOCKS_PER_SEC << " �롣" << endl;
	cout << "Spend " << (double)(time_end - time_start) / CLOCKS_PER_SEC << " s." << endl << endl;

	// �������ջ���˳��
	// build the final drawing order
	cout << "��ʼ�������ջ���˳��\n";
	cout << "start building the final drawing order...\n";
	time_start = clock();
	unordered_map<glm::vec3, Node>::iterator temp = tree.begin();
	findLongestPath(tree);	// �����·��
							// find the longest path		
	outputAllNodes(tree, longestPath);	// �����·������֧·������˳������ pointList ��ȫ�ֱ����У�
										// Traversing the longest path and its branches (drawing order is stored in the public variable "pointList")
	time_end = clock();
	cout << "�������ջ���˳�����\n";
	cout << "build the final drawing order done\n";
	cout << "�� " << (double)(time_end - time_start) / CLOCKS_PER_SEC << " �롣" << endl;
	cout << "Spend " << (double)(time_end - time_start) / CLOCKS_PER_SEC << " s." << endl << endl;


	// ��ɢ����Ҷ�任
	// DFT
	cout << "Begin ִ����ɢ����Ҷ�任...\n";
	cout << "Begin doing DFT...\n";
	time_start = clock();
	unsigned int N = pointList.size();
	fftw_complex *out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N);
	DFT(pointList, out); 
	time_end = clock();
	cout << "��ɢ����Ҷ�任ִ�����\n";
	cout << "Fourier transform done\n";
	cout << "�� " << (double)(time_end - time_start) / CLOCKS_PER_SEC << " �롣" << endl;
	cout << "Spend " << (double)(time_end - time_start) / CLOCKS_PER_SEC << " s." << endl << endl;
	

	// ������ͷ����
	// create arrow vector
	cout << "��ʼ����ͷ...\n";
	cout << "Initialize the arrows...\n";
	time_start = clock();
	float totalLength = 0;
	vector<double> lengths;
	for(int i = 0; i < N; i++){
	    lengths.push_back(sqrt(pow(out[i][0], 2) + pow(out[i][1], 2)));
	    totalLength += lengths[i];
	}
	totalLength /= 5.0;		
	vector<Arrow> arrowList;	
	glm::vec3 position = glm::vec3(0.0, 0.0, 0.0);
	glm::vec3 add = glm::vec3(0.0, 0.0, 0.0);
	for(int i = 0; i < N; i++){
	    Arrow arrow = Arrow(lengths[i] / totalLength, glm::vec3(out[i][0], out[i][1], 0.0));
	    arrowList.push_back(arrow);
	}
	time_end = clock();
	cout << "��ʼ����ͷ��ɡ�\n";
	cout << "initialize arrows done.\n";
	cout << "�� " << (double)(time_end - time_start) / CLOCKS_PER_SEC << " �롣" << endl;
	cout << "Spend " << (double)(time_end - time_start) / CLOCKS_PER_SEC << " s." << endl << endl;


	// ��ʼ��Ⱦ
	// begin rendering
	cout << "��ʼ��Ⱦ...\n";
	cout << "Begin rendering..." << endl;
	time_start = clock();
	double eConst = 2 * M_PI / N;
	int n = 0;
	vector<Arrow> arrowTemp;	
	bool isfirst = true;
	glEnable(GL_MULTISAMPLE);
	while (!glfwWindowShouldClose(window))
	{
		double Const = eConst * n;	// ����������ת���ڳ���
									// constants of calculating rotation period
		
		// OpenGL ���ã������ӿڣ����������Լ�ˢ����Ļ������
		// OpenGL configuration: adjust viewport, process input and refresh screen buffer
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		processInput(window);
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);				
		
		// ��Ⱦī��
		// render ink mark
		glBindFramebuffer(GL_FRAMEBUFFER, boardFBO);
			glClearColor(0.0, 0.0, 0.0, 1.0);
			if(isfirst){
				glClear(GL_COLOR_BUFFER_BIT);
				isfirst = false;
			}
			arrowShader.use();
		
			arrowTemp = arrowList;
			position = glm::vec3(0.0, 0.0, 0.0);		
			for(int i = 0; i < arrowList.size(); i++){			
				arrowTemp[i].rotateArrow(Const * (float)i);
				arrowTemp[i].setPosition(position);
				position = arrowTemp[i].getHeadPosition();			
			}	
			pen.setOriginPosition(position);
			pen.drawPen(arrowShader);		
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		

		// ��Ⱦ��ͷ��ī��
		// render arrow and ink mark
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		boardShader.use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, boardColorBuffer);
		renderBoard();			// �����Ⱦ�õıʼ�
								// output renered ink mark
		for(int i = 0; i < arrowList.size(); i++){
			arrowTemp[i].drawArrow(arrowShader);			// ���Ƽ�ͷ
															// render arrows
		}

		
		if (n < N) {
			n++;
		}				
		else {						
			time_end = clock();
			cout << "�� " << (double)(time_end - time_start) / CLOCKS_PER_SEC << " �롣" << endl;
			cout << "Spend " << (double)(time_end - time_start) / CLOCKS_PER_SEC << " s." << endl << endl;
			cout << "��Ⱦ�������밴������˳���\n";
			cout << "render done, press any key to exit.\n";
			system("pause");
			break;
		}
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}


	glfwTerminate();
	return 0;
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

unsigned int boardVAO = 0;
unsigned int boardVBO;
void renderBoard() {
	if (boardVAO == 0) {
		float boardVertices[] = {
			-1.0,  1.0, 0.0,   0.0, 1.0,    // up-left
			-1.0, -1.0, 0.0,   0.0, 0.0,    // down-left
			 1.0,  1.0, 0.0,   1.0, 1.0,    // up-right
			 1.0, -1.0, 0.0,   1.0, 0.0     // down right
		};
		glGenVertexArrays(1, &boardVAO);
		glBindVertexArray(boardVAO);
		glGenBuffers(1, &boardVBO);
		glBindBuffer(GL_ARRAY_BUFFER, boardVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(boardVertices), boardVertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glBindVertexArray(0);
	}
	glBindVertexArray(boardVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

unsigned int loadTexture(char const *path) {
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data) {
		GLenum format;
		GLenum internalFormat;
		if (nrComponents == 1) {
			format = GL_RED;
			internalFormat = format;
		}
		else if (nrComponents == 3) {
			format = GL_RGB;
			internalFormat = GL_SRGB;
		}
		else if (nrComponents == 4) {
			format = GL_RGBA;
			internalFormat = GL_SRGB_ALPHA;
		}

		glBindTexture(GL_TEXTURE_2D, textureID);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}
	return textureID;
}


void MergeSort(vector<EdgeInfo>& arr) {

	unsigned int n = arr.size();
	if (n <= 1)
		return;
	vector<EdgeInfo> arr1, arr2;
	for (unsigned int i = 0; i < arr.size(); ++i) {
		if (i < n / 2)
			arr1.push_back(arr[i]);
		else
			arr2.push_back(arr[i]);
	}

	MergeSort(arr1);
	MergeSort(arr2);
	arr.clear();
	Combine(arr, arr1, arr2);
}

void Combine(vector<EdgeInfo>&arr, vector<EdgeInfo>&arr1, vector<EdgeInfo>&arr2) {
	int n1 = arr1.size(), n2 = arr2.size();
	int index1 = 0, index2 = 0;
	while (index1 < n1 || index2 < n2) {
		if (index1 >= n1) {
			arr.push_back(arr2[index2++]);
		}
		else if (index2 >= n2) {
			arr.push_back(arr1[index1++]);
		}
		else if (arr1[index1].len > arr2[index2].len) {
			arr.push_back(arr2[index2++]);
		}
		else {
			arr.push_back(arr1[index1++]);
		}
	}
}
glm::vec3 find_root(glm::vec3 p, unordered_map<glm::vec3, glm::vec3> & parents) {
	unordered_map<glm::vec3, glm::vec3>::iterator iter, temp_iter;
	iter = parents.find(p);
	if (iter == parents.end()) {
		return p;
	}
	glm::vec3 p_root = iter->second;
	temp_iter = iter;
	int times = 1;
	while (parents.find(iter->second) != parents.end()) {
		iter = parents.find(iter->second);
		p_root = iter->second;
		times++;
	}
	temp_iter->second = p_root;
	return p_root;
}

bool union_find(glm::vec3 p1, glm::vec3 p2, unordered_map<glm::vec3, glm::vec3> & parents, unordered_map<glm::vec3, int> & rank) {
	glm::vec3 p1_root = find_root(p1, parents);
	glm::vec3 p2_root = find_root(p2, parents);
	glm::vec3 common_root;

	if (p1_root == p2_root) {
		return false;
	}
	else {
		unordered_map<glm::vec3, int>::iterator iter_rank1 = rank.find(p1_root);
		unordered_map<glm::vec3, int>::iterator iter_rank2 = rank.find(p2_root);

		int rank1, rank2;
		if (iter_rank1 == rank.end()) {
			rank.insert(pair<glm::vec3, int>(p1_root, 0));
			rank1 = 0;
		}
		else {
			rank1 = iter_rank1->second;
		}
		if (iter_rank2 == rank.end()) {
			rank.insert(pair<glm::vec3, int>(p2_root, 0));
			rank2 = 0;
		}
		else {
			rank2 = iter_rank2->second;
		}

		iter_rank2 = rank.find(p2_root);

		if (rank1 == rank2 || rank1 < rank2) {
			unordered_map<glm::vec3, glm::vec3>::iterator iter = parents.find(p1_root);
			if (iter == parents.end()) {
				parents.insert(pair<glm::vec3, glm::vec3>(p1_root, p2_root));
			}
			else {
				iter->second = p2_root;
			}
			if (rank1 == rank2) {
				iter_rank2->second++;
				rank2++;
			}
		}
		else {
			unordered_map<glm::vec3, glm::vec3>::iterator iter = parents.find(p2_root);
			if (iter == parents.end()) {
				parents.insert(pair<glm::vec3, glm::vec3>(p2_root, p1_root));
			}
			else {
				iter->second = p1_root;
			}
		}
		return true;
	}
}

void findPath(unordered_map<glm::vec3, Node> tree, vector<glm::vec3> path) {
	unordered_map<glm::vec3, Node>::iterator iter = tree.begin();
	if (longestPath.size() != 0) {
		iter = tree.find(longestPath[0]);
		longestPath.clear();
	}

	while (iter != tree.end()) {
		path.push_back(iter->first);
		iter->second.isCheck = true;
		int checkCount = 0;
		int connectionSize = iter->second.connections.size();
		for (int i = 0; i < connectionSize; i++) {
			unordered_map<glm::vec3, Node>::iterator temp = tree.find(iter->second.connections[i]);
			if (temp != tree.end() && !temp->second.isCheck) {
				iter = temp;
				break;
			}
			if (temp->second.isCheck) {
				checkCount++;
			}
		}
		if (checkCount == connectionSize) {
			int lastIndex = path.size() - 2;
			if (lastIndex < 0)
				return;
			iter = tree.find(path[lastIndex]);
			if (longestPath.size() < path.size())
				longestPath = path;
			path.pop_back();
			path.pop_back();
		}
	}
}
void findLongestPath(unordered_map<glm::vec3, Node> tree) {
	vector<glm::vec3> path;
	findPath(tree, path);
	findPath(tree, path);
}

void checkBranck(unordered_map<glm::vec3, Node> & tree, unordered_map<glm::vec3, Node>::iterator node) {
	pointList.push_back(node->first);
	node->second.isCheck = true;

	for (int i = 0; i < node->second.connections.size(); i++) {
		unordered_map<glm::vec3, Node>::iterator temp = tree.find(node->second.connections[i]);
		if (!temp->second.isCheck) {
			checkBranck(tree, temp);
		}
	}
}
void outputAllNodes(unordered_map<glm::vec3, Node> & tree, vector<glm::vec3> longestPath) {
	int longestPathSize = longestPath.size();
	for (int i = 0; i < longestPathSize; i++) {
		unordered_map<glm::vec3, Node>::iterator iter = tree.find(longestPath[i]);	
		pointList.push_back(iter->first);
		iter->second.isCheck = true;

		int nextPathNodeIndex = i;
		if (i + 1 < longestPathSize)
			nextPathNodeIndex++;

		for (int j = 0; j < iter->second.connections.size(); j++) {
			unordered_map<glm::vec3, Node>::iterator temp = tree.find(iter->second.connections[j]);
			if (!temp->second.isCheck && temp->first != longestPath[nextPathNodeIndex]) {
				checkBranck(tree, temp);
			}
		}
	}
}

void DFT(vector<glm::vec3> pointList, fftw_complex *out) {
	int size = pointList.size();
	fftw_complex *in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * size);

	for (int i = 0; i < pointList.size(); i++) {
		in[i][0] = pointList[i].x;
		in[i][1] = pointList[i].y;
	}

	fftw_plan p = fftw_plan_dft_1d(size, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
	fftw_execute(p); /* repeat as needed */
}

void createTree(vector<EdgeInfo> edgeInfos, unordered_map<glm::vec3, Node> & tree) {		
	unordered_map<glm::vec3, glm::vec3> parents;
	unordered_map<glm::vec3, int> rank;
	unordered_map<glm::vec3, int>::iterator rank_iter;
	unordered_map<glm::vec3, int> hasParent;
	unordered_map<glm::vec3, int>::iterator parent_iter;
	vector<EdgeInfo> paintEdges;
	glm::vec3 halfScreenSize = glm::vec3(SCR_WIDTH / 2, SCR_HEIGHT / 2, 0.0);

	for (int i = 0; i < edgeInfos.size(); i++) {
		if (union_find(edgeInfos[i].p1, edgeInfos[i].p2, parents, rank)) {
			unordered_map<glm::vec3, Node>::iterator iter1 = tree.find(edgeInfos[i].p1 - halfScreenSize);
			unordered_map<glm::vec3, Node>::iterator iter2 = tree.find(edgeInfos[i].p2 - halfScreenSize);

			if (iter1 == tree.end()) {
				Node node1(edgeInfos[i].p1 - halfScreenSize);
				vector<glm::vec3> temp1;
				temp1.push_back(edgeInfos[i].p2 - halfScreenSize);
				node1.connections = temp1;
				tree.insert(make_pair(node1.position, node1));
			}
			else {
				iter1->second.connections.push_back(edgeInfos[i].p2 - halfScreenSize);
			}

			if (iter2 == tree.end()) {
				Node node2(edgeInfos[i].p2 - halfScreenSize);

				vector<glm::vec3> temp2;
				temp2.push_back(edgeInfos[i].p1 - halfScreenSize);
				node2.connections = temp2;
				tree.insert(make_pair(node2.position, node2));
			}
			else {
				iter2->second.connections.push_back(edgeInfos[i].p1 - halfScreenSize);
			}
		}
	}	
}