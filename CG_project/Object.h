#pragma once

#include <iostream>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "model.h"

using namespace std;



typedef struct ObjTree
{
	string name;
	glm::mat4 model;
	//����isLeaf=trueʱ��VAO�Լ�VBO�е�ֵ��������
	bool isLeaf;
	unsigned int ObjVAO;
	unsigned int ObjVBO;
	//��������ʾ�����
	struct ObjTree* leftChild, * rightSibling;

}ObjTree;


ObjTree* CreatLeafnode(string name, unsigned int VAO, unsigned int VBO)
{
	ObjTree* node = new ObjTree;
	node->name = name;
	node->ObjVAO = VAO;
	node->ObjVBO = VBO;
	node->isLeaf = true;
	node->leftChild = NULL;
	node->rightSibling = NULL;
	//��λ����
	node->model = glm::mat4(1.0f);
	
}


ObjTree* UnionTree(string PName, ObjTree* T1, ObjTree* T2)
{
	if (!T1 || !T2)
	{
		cout << "Empty input node!" << endl;
		return NULL;
	}
	T1->rightSibling = T2;
	ObjTree* ParentNode = new ObjTree;
	ParentNode->name = PName;
	ParentNode->isLeaf = false;
	ParentNode->leftChild = T1;
	ParentNode->rightSibling = NULL;
	//������ȡֵ
	ParentNode->ObjVAO = 0;
	ParentNode->ObjVBO = 0;
	ParentNode->model = glm::mat4(1.0f);
	return ParentNode;
}


//����TΪ�����������弯��
void DrawObjCollection(ObjTree* T)
{

	if (T->leftChild != NULL)
	{
		DrawObjCollection(T->leftChild);
	}
	else if (T->rightSibling != NULL)
	{
		DrawObjCollection(T->rightSibling);
	}
	if (T->isLeaf)
	{
		DrawBaseElement();
	}
	return;
}

//����T֮������Ҷ�ӽڵ��model����
void UpdataModel(ObjTree* T, glm::mat4 newMat)
{
	if (T->leftChild != NULL)
	{
		UpdataModel(T->leftChild, T->model * newMat);
	}
	else if (T->rightSibling != NULL)
	{
		UpdataModel(T->rightSibling, T->model * newMat);
	}
	if (T->isLeaf)
	{
		T->model = newMat * T->model;
	}
	return;
}


//���ƻ���ͼԪ
void DrawBaseElement()
{

}



