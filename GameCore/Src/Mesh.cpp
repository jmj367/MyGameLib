#include "Mesh.h"
#include "JsonData.h"
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