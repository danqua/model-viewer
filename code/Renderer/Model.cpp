#include "Model.h"
#include "Image.h"
#include "Material.h"

Harm::Model::Model(const std::string& filename)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        printf("%s\n", importer.GetErrorString());
        return;
    }
    ProcessNode(scene->mRootNode, scene);
    ProcessMaterial(scene);
}

Harm::Model::~Model()
{

}

void Harm::Model::Draw() const
{
    for (auto& mesh : m_Meshes)
    {
        mesh->Draw();
    }
}

void Harm::Model::ProcessNode(aiNode* node, const aiScene* scene)
{
    for (unsigned i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        m_Meshes.push_back(ProcessMesh(mesh, scene));
    }
    for (unsigned i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(node->mChildren[i], scene);
    }
}

void Harm::Model::ProcessMaterial(const aiScene* scene)
{
    m_Materials.reserve(scene->mNumMaterials);

    for (unsigned i = 0; i < scene->mNumMaterials; i++)
    {
        const aiMaterial* material = scene->mMaterials[i];
        Material& mat = m_Materials.emplace_back();

        if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
        {
            aiString texturePath;
            if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS)
            {
                std::string filename(texturePath.data);
                std::string fullPath = "textures/" + filename;
                Image image(fullPath.c_str());
                if (image.GetComponents() == 4)
                {
                    //newMaterial.transparent = true;
                }
                mat.diffuseTexture = std::make_shared<Texture>(image.GetPixels(), image.GetWidth(), image.GetHeight());
            }
        }
        else
        {
            Image image("somebadpath");
            mat.diffuseTexture = std::make_shared<Texture>(image.GetPixels(), image.GetWidth(), image.GetHeight());
        }
    }
}

std::shared_ptr<Harm::Mesh> Harm::Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
    u32 numIndices = 0;
    for (u32 i = 0; i < mesh->mNumFaces; i++)
        for (u32 j = 0; j < mesh->mFaces[i].mNumIndices; j++)
            numIndices++;

    std::vector<Vertex> vertices(mesh->mNumVertices);
    std::vector<u32> indices(numIndices);

    for (u32 i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex& vertex = vertices[i];

        vertex.position.x = mesh->mVertices[i].x;
        vertex.position.y = mesh->mVertices[i].y;
        vertex.position.z = mesh->mVertices[i].z;

        vertex.normal.x = mesh->mNormals[i].x;
        vertex.normal.y = mesh->mNormals[i].y;
        vertex.normal.z = mesh->mNormals[i].z;

        if (mesh->mTextureCoords[0])
        {
            vertex.uv.x = mesh->mTextureCoords[0][i].x;
            vertex.uv.y = mesh->mTextureCoords[0][i].y;
        }
    }

    // Indices
    u32* index = indices.data();
    for (u32 i = 0; i < mesh->mNumFaces; i++)
        for (u32 j = 0; j < mesh->mFaces[i].mNumIndices; j++)
            *index++ = mesh->mFaces[i].mIndices[j];

    return std::make_shared<Harm::Mesh>(vertices, indices, mesh->mMaterialIndex);
}
