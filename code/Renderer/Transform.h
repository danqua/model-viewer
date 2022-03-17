#pragma once
#include <glm/glm.hpp>
#include <vector>

namespace Harm
{
    class Transform
    {
    public:
        Transform();
        void Update();
        void Add(Transform* transform);
        void Remove(Transform* transform);
        void SetPosition(const glm::vec3& newPosition);
        void Translate(const glm::vec3& direction);
        void TranslateX(float amount);
        void TranslateY(float amount);
        void TranslateZ(float amount);
        void SetRotation(const glm::vec3& newRotation);
        void RotateX(float deg);
        void RotateY(float deg);
        void RotateZ(float deg);
        void RotateAroundAxis(float deg, const glm::vec3& axis);
        void SetPositionAndRotation(const glm::vec3& position, const glm::vec3& rotation);
        void SetScale(const glm::vec3& newScale);
        inline const glm::vec3& GetPosition() const { return m_Position; }
        inline const glm::vec3& GetRotation() const { return m_Rotation; }
        inline const glm::vec3& GetScale() const { return m_Scale; }
        inline const glm::mat4& GetLocalMatrix() const { return m_LocalMatrix; }
        inline const glm::mat4& GetWorldMatrix() const { return m_WorldMatrix; }
        inline glm::mat4 GetInvWorldMatrix() const { return glm::inverse(m_WorldMatrix); }
        glm::vec3 GetForwardVector() const;
        glm::vec3 GetRightVector() const;
        glm::vec3 GetUpVector() const;
    protected:
        glm::vec3 m_Position;
        glm::vec3 m_Rotation;
        glm::vec3 m_Scale;
    private:
        glm::mat4 m_LocalMatrix;
        glm::mat4 m_WorldMatrix;
        Transform* m_Parent;
        std::vector<Transform*> m_Children;
        bool m_NeedsUpdate;
    };
}