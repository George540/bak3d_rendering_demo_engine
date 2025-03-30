#pragma once

#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include "object.h"

// TODO: Make a generic Cube class that inherits from Object (to be used for making a BoundingBox and inheriting to Light)
class BoundingBox : Object
{
public:
    BoundingBox(Material* material);
    ~BoundingBox() override = default;

    void update(float dt) override;
    void draw() const override;
};

#endif