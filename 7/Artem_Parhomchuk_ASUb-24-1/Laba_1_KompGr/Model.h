#ifndef MODEL_H
#define MODEL_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "Shader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

using namespace std;

class Model
{
public:

	vector<Mesh> meshes;
	vector<string> meshNames;
	string directory;

	Model(string const& path)
	{
		loadModel(path);
	}

	void Draw(Shader& shader, float tableX, float spindleHeadY, float saddleZ)
	{
		glm::mat4 globalModel = glm::mat4(1.0f);

		for (size_t i = 0; i < meshes.size(); i++) {
			glm::mat4 currentModel = globalModel;
			string name = meshNames[i];

			if (name == "Base") {

			}

			else if (name == "Stol" || name == "Table") {

			}

			else if (name == "Head" || name == "Spindle") {
				// Перемещение узла по горизонтали 
				currentModel = glm::translate(currentModel, glm::vec3(tableX, 0.0f, saddleZ));
				// вертикальное движение шпинделя
				currentModel = glm::translate(currentModel, glm::vec3(0.0f, spindleHeadY, 0.0f));
			}


			else {
				currentModel = glm::translate(currentModel, glm::vec3(tableX, 0.0f, saddleZ));
			}

			shader.setMat4("model", currentModel);

			glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(currentModel)));
			shader.setMat3("normalMatrix", normalMatrix);

			meshes[i].Draw();
		}
	}

private:

	void loadModel(string const& path)
	{
		Assimp::Importer importer;
		directory = path.substr(0, path.find_last_of('/'));

		const aiScene* scene = importer.ReadFile(path,
			aiProcess_CalcTangentSpace |
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_SortByPType);

		if (nullptr == scene) {
			fprintf(stderr, "Error ASSIMP:: %s\n", importer.GetErrorString());
			return;
		}

		processNode(scene->mRootNode, scene);
	}

	void processNode(aiNode* node, const aiScene* scene)
	{
		for (int i = 0; i < node->mNumMeshes; i++) {
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(processMesh(mesh, scene));

			if (mesh->mName.length > 0) {
				meshNames.push_back(string(mesh->mName.C_Str()));
			}
			else {
				meshNames.push_back(string(node->mName.C_Str()));
			}
		}

		for (int i = 0; i < node->mNumChildren; i++) {
			processNode(node->mChildren[i], scene);
		}
	}

	Mesh processMesh(aiMesh* mesh, const aiScene* scene)
	{
		vector <Vertex> vertices;
		vector <unsigned int> indices;

		for (int i = 0; i < mesh->mNumVertices; i++) {
			Vertex vert_buffer;
			glm::vec3 vector;
			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;

			vert_buffer.Position = vector;

			if (mesh->HasNormals()) {
				vector.x = mesh->mNormals[i].x;
				vector.y = mesh->mNormals[i].y;
				vector.z = mesh->mNormals[i].z;

				vert_buffer.Normal = vector;
			}

			vertices.push_back(vert_buffer);
		}

		for (int i = 0; i < mesh->mNumFaces; i++) {
			aiFace face = mesh->mFaces[i];

			for (int j = 0; j < face.mNumIndices; j++) {
				indices.push_back(face.mIndices[j]);
			}
		}

		return Mesh(vertices, indices);
	}
};

#endif