// =====================================================================================
// Basic axis class that draws the XZ plane grid
// Class made by George Mavroeidis, 2022
// =====================================================================================

#pragma once

#ifndef GRID_H
#define GRID_H

#include <glad/glad.h>

#include "object.h"

class Grid : public Object
{
public:
	Grid(Camera& cam);
	~Grid() = default;

	void draw() const;
private:
	int m_number_of_elements;
	int m_number_of_slices;
	float m_grid_size;
};

#endif