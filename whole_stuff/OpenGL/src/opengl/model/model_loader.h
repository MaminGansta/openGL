#pragma once

#include <memory>
#include <string>
#include <vector>

#include "model.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"


namespace gl
{
	struct ModelLoader
	{
		static Model LoadModel(
			const std::string& path,
			const glm::vec3& pos = glm::vec3(0.0f),
			const glm::vec3& scale = glm::vec3(1.0f),
			const glm::vec3& rotation = glm::vec3(0.0f));

	private:
		static std::vector<std::pair<std::string, Model*>> stored_paths;
		static std::vector<std::pair<std::string, Texture>> loaded_textures;

		static void ProcessNode(Model& model, aiNode* node, const aiScene* scene);
		static Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
		static void LoadMaterialTextures(std::vector<Texture>& textures, aiMaterial* mat, aiTextureType type, TextureType texture_type);
	};

}