#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

Harm::Camera::Camera(f32 fov, f32 aspect, f32 near, f32 far) :
    m_Fov(fov),
    m_Aspect(aspect),
    m_Far(far),
    m_Near(near)
{
    UpdateProjectionMatrix();
}

void Harm::Camera::UpdateProjectionMatrix()
{
    m_ProjectionMatrix = glm::perspective(glm::radians(m_Fov), m_Aspect, m_Near, m_Far);
}
