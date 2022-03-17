#include "Transform.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

Harm::Transform::Transform() :
    m_Position(glm::vec3(0.0f)),
    m_Rotation(glm::vec3(0.0f)),
    m_Scale(glm::vec3(1.0f)),
    m_LocalMatrix(glm::mat4(1.0f)),
    m_WorldMatrix(glm::mat4(1.0f)),
    m_Parent(nullptr),
    m_NeedsUpdate(true)
{

}

void Harm::Transform::Add(Transform* transform)
{
    if (transform->m_Parent)
        transform->m_Parent->Remove(transform);

    transform->m_Parent = this;
    m_Children.push_back(transform);
}

void Harm::Transform::Remove(Transform* transform)
{
    for (auto it = m_Children.begin(); it != m_Children.end(); it++)
    {
        if (*it == transform)
        {
            transform->m_Parent = nullptr;
            m_Children.erase(it);
            return;
        }
    }
}

void Harm::Transform::SetPosition(const glm::vec3& newPosition)
{
    m_Position = newPosition;
    m_NeedsUpdate = true;
}

void Harm::Transform::Translate(const glm::vec3& direction)
{
    m_Position += direction;
    m_NeedsUpdate = true;
}

void Harm::Transform::TranslateX(float amount)
{
    m_Position += GetRightVector() * amount;
    m_NeedsUpdate = true;
}

void Harm::Transform::TranslateY(float amount)
{
    m_Position += GetUpVector() * amount;
    m_NeedsUpdate = true;
}

void Harm::Transform::TranslateZ(float amount)
{
    m_Position += GetForwardVector() * amount;
    m_NeedsUpdate = true;
}

void Harm::Transform::SetRotation(const glm::vec3& newRotation)
{
    m_Rotation = newRotation;
    m_NeedsUpdate = true;
}

void Harm::Transform::RotateX(float deg)
{
    m_Rotation.x += deg;
    m_NeedsUpdate = true;
}

void Harm::Transform::RotateY(float deg)
{
    m_Rotation.y += deg;
    m_NeedsUpdate = true;
}

void Harm::Transform::RotateZ(float deg)
{
    m_Rotation.z += deg;
    m_NeedsUpdate = true;
}

void Harm::Transform::RotateAroundAxis(float deg, const glm::vec3& axis)
{
    m_Rotation += axis * deg;
    m_NeedsUpdate = true;
}

void Harm::Transform::SetPositionAndRotation(const glm::vec3& position, const glm::vec3& rotation)
{
    SetPosition(position);
    SetRotation(rotation);
}

void Harm::Transform::SetScale(const glm::vec3& newScale)
{
    m_Scale = newScale;
    m_NeedsUpdate = true;
}

glm::vec3 Harm::Transform::GetForwardVector() const
{
    return glm::quat(glm::radians(m_Rotation)) * glm::vec3(0.0f, 0.0f, -1.0f);
}

glm::vec3 Harm::Transform::GetRightVector() const
{
    return glm::quat(glm::radians(m_Rotation)) * glm::vec3(1.0f, 0.0f, 0.0f);
}

glm::vec3 Harm::Transform::GetUpVector() const
{
    return glm::quat(glm::radians(m_Rotation)) * glm::vec3(0.0f, 1.0f, 0.0f);
}

void Harm::Transform::Update()
{
    if (m_NeedsUpdate)
    {
        m_LocalMatrix = glm::translate(glm::mat4(1.0f), m_Position);
        m_LocalMatrix = glm::rotate(m_LocalMatrix, glm::radians(m_Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        m_LocalMatrix = glm::rotate(m_LocalMatrix, glm::radians(m_Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        m_LocalMatrix = glm::rotate(m_LocalMatrix, glm::radians(m_Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        m_LocalMatrix = glm::scale(m_LocalMatrix, m_Scale);

        if (m_Parent)
            m_WorldMatrix = m_Parent->m_WorldMatrix * m_LocalMatrix;
        else
            m_WorldMatrix = m_LocalMatrix;

        for (Transform* child : m_Children)
        {
            child->m_NeedsUpdate = true;
            child->Update();
        }
        m_NeedsUpdate = false;
    }
}
