#ifndef GRID_H
#define GRID_H

#include <glad/glad.h> 

class Grid
{
public:
	Grid();
	~Grid() = default;

	void draw() const;
private:
	GLuint m_vao{};
	int number_of_elements;
	int number_of_slices;
	float grid_size;
};

#endif