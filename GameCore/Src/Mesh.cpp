#include "Mesh.h"
#include "JsonData.h"
#include "LoaderModel.h"
#include "Math.h"
#include "Renderer.h"
#include "Texture.h"
#include "VertexArray.h"
#include <rapidjson/document.h>
#include <SDL2/SDL_log.h>
#include <fstream>

Mesh::Mesh()
	: mVertexArray()
	, mRadius(0.0f)
	, mSpecPower(100.0f)
{
}

Mesh::~Mesh()
{
}

bool Mesh::Load(const std::string &fileName, Renderer *renderer)
{
	return LoaderModel::LoadMesh(fileName, *this, renderer);
}