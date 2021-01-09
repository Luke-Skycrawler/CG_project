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

enum ObjType
{
	MeshModel,selfMake
};

typedef struct ObjTree
{
	string name;
	glm::mat4 model;
	ObjType type;
	bool isLeaf;
	unsigned int ObjVAO;
	unsigned int ObjVBO;
	Model* mesh;
	//��������ʾ�����
	struct ObjTree* leftChild, * rightSibling;

	void (*Drawfp)();

}ObjTree;

//selfMake��ԭ������
ObjTree* CreatLeafnode(string name, unsigned int VAO, unsigned int VBO, ObjType type,void (*Draw)())
{
	if (type != selfMake)
	{
		cout << "ERROR:Input doesn't follow the type!" << endl;
		return NULL;
	}
	ObjTree* node = new ObjTree;
	node->type = type;
	node->name = name;
	node->ObjVAO = VAO;
	node->ObjVBO = VBO;
	node->isLeaf = true;
	node->mesh = NULL;
	node->leftChild = NULL;
	node->rightSibling = NULL;
	//��λ����
	node->model = glm::mat4(1.0f);
	//���ƺ���
	node->Drawfp = Draw;
	return node;
}

//mesh model��ԭ������
ObjTree* CreatLeafnode(string name, Model* mesh, ObjType type, void (*Draw)())
{
	if (type != MeshModel)
	{
		cout << "ERROR:Input doesn't follow the type!" << endl;
		return NULL;
	}
	ObjTree* node = new ObjTree;
	node->name = name;
	node->type = type;
	node->isLeaf = true;
	node->leftChild = NULL;
	node->rightSibling = NULL;
	node->mesh = mesh;
	node->ObjVAO = 0;
	node->ObjVBO = 0;
	//��λ����
	node->model = glm::mat4(1.0f);
	//���ƺ���
	node->Drawfp = Draw;
	return node;
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
		//����ԭ������
		T->Drawfp();
	}
	return;
}

//����T֮������Ҷ�ӽڵ��model����
void UpdataModel(ObjTree* T, glm::mat4 newMat)
{
	if (T->leftChild != NULL)
	{
		UpdataModel(T->leftChild, T->model * newMat);
		if (T->rightSibling != NULL)
		{
			UpdataModel(T->rightSibling, T->model * newMat);
		}
	}
	if (T->isLeaf)
	{
		T->model = newMat * T->model;
	}
	return;
}






