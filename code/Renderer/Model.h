#pragma once
#include "Mesh.h"
#include "Material.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <vector>
#include <memory>

namespace Harm
{
    class Model
    {
    public:
        Model(const std::string& filename);
        ~Model();
        void Draw() const;
        inline const std::vector<std::shared_ptr<Mesh>>& GetMeshes() const { return m_Meshes; }
        inline const std::vector<Material>& GetMaterials() const { return m_Materials; }
    private:
        void ProcessNode(aiNode* node, const aiScene* scene);
        void ProcessMaterial(const aiScene* scene);
        std::shared_ptr<Mesh> ProcessMesh(aiMesh* mesh, const aiScene* scene);

    private:
        std::vector<std::shared_ptr<Mesh>> m_Meshes;
        std::vector<Material> m_Materials;
    };
}