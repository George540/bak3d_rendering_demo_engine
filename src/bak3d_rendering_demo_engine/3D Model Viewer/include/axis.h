// =====================================================================================
// Basic axis class that draws the XYZ 3D axis lines
// Class made by George Mavroeidis, 2022
// =====================================================================================

#pragma once

#ifndef AXIS_H
#define AXIS_H

#include <glad/glad.h> 

#include "object.h"

class Axis : public Object
{
public:
	Axis(Camera& camera, Shader* shader);
	~Axis() = default;

	void draw() const override;
};
#endif