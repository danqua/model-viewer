#pragma once
#include "Common/Types.h"
#include "Transform.h"
#include <glm/glm.hpp>

namespace Harm
{
    class Camera : public Transform
    {
    public:
        Camera(f32 fov, f32 aspect, f32 near, f32 far);
        void UpdateProjectionMatrix();
        inline const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
        inline glm::mat4 GetViewMatrix() const { return GetInvWorldMatrix(); }
    private:
        f32 m_Fov;
        f32 m_Aspect;
        f32 m_Near;
        f32 m_Far;
        glm::mat4 m_ProjectionMatrix;
    };
}