#pragma once
#include <unordered_map>
#include "Mesh.h"
#include "Material.h"

bool ParseOBJMesh(Mesh* mesh, const char* filename);
bool ParseOBJMeshMaterial(std::unordered_map<std::string, Mesh>& meshes, const char* filename);
bool ParseMTLMaterial(std::unordered_map<std::string, Material*>& materials, const char* filename);