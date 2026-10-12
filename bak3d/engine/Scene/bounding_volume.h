#pragma once

#include "Objects/camera.h"

/*
 * Base for shapes that can be tested against a frustum.
 *  Volumes are stored in local space and transformed by a model matrix at test time.
 */
class BoundingVolume
{
public:
    virtual ~BoundingVolume() = default;

    // Returns true if the volume is at least partially inside the frustum
    [[nodiscard]] virtual bool is_on_frustum(const Frustum& frustum, const glm::mat4& model) const = 0;
};

/*
 * Spherical version of the bounding volume (used for lights and circular shaped meshes).
 */
class BoundingSphere : public BoundingVolume
{
public:
    explicit BoundingSphere(const glm::vec3& center = glm::vec3(0.0f), const float radius = 0.0f) : m_center(center), m_radius(radius) {}

    [[nodiscard]] bool is_on_frustum(const Frustum& frustum, const glm::mat4& model) const override
    {
        const glm::vec3 world_center = glm::vec3(model * glm::vec4(m_center, 1.0f));
        const float max_scale = glm::max(glm::length(glm::vec3(model[0])), glm::max(glm::length(glm::vec3(model[1])), glm::length(glm::vec3(model[2]))));
        const float world_radius = m_radius * max_scale;

        for (const glm::vec4& plane : frustum.planes)
        {
            if (glm::dot(glm::vec3(plane), world_center) + plane.w < -world_radius)
            {
                return false;
            }
        }
        return true;
    }

    [[nodiscard]] const glm::vec3& get_center() const { return m_center; }
    [[nodiscard]] float get_radius() const { return m_radius; }

private:
    glm::vec3 m_center;
    float m_radius;
};

/*
 * Using AABB algorithm to decide intersection algorithms.
 */
class BoundingAABB : public BoundingVolume
{
public:
    explicit BoundingAABB(const glm::vec3& center = glm::vec3(0.0f), const glm::vec3& extents = glm::vec3(0.0f)) : m_center(center), m_extents(extents) {}

    static BoundingAABB from_min_max(const glm::vec3& min, const glm::vec3& max)
    {
        return BoundingAABB((min + max) * 0.5f, (max - min) * 0.5f);
    }

    [[nodiscard]] bool is_on_frustum(const Frustum& frustum, const glm::mat4& model) const override
    {
        const glm::vec3 world_center = glm::vec3(model * glm::vec4(m_center, 1.0f));

        const glm::mat3 abs_model(glm::abs(glm::vec3(model[0])), glm::abs(glm::vec3(model[1])), glm::abs(glm::vec3(model[2])));
        const glm::vec3 world_extents = abs_model * m_extents;

        for (const glm::vec4& plane : frustum.planes)
        {
            const glm::vec3 normal(plane);
            const float radius = glm::dot(world_extents, glm::abs(normal)); // box projected onto plane normal
            if (glm::dot(normal, world_center) + plane.w < -radius)
            {
                return false;
            }
        }
        return true;
    }

    [[nodiscard]] const glm::vec3& get_center() const { return m_center; }
    [[nodiscard]] const glm::vec3& get_extents() const { return m_extents; }

private:
    glm::vec3 m_center;
    glm::vec3 m_extents; // half-size per axis
};
