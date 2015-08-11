#pragma once

#include "Vector.h"
class Tree;
class Director
{
public:
	Director();

	~Director();
	
	static Director* GetInstance();

	int Run();

	void MainLoop();

	void End();

	Tree* GetCurrentTree();
public:
	Vector<Tree*> _trees;
};