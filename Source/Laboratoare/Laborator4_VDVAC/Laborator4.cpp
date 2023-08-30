#include "Laborator4.h"

#include <vector>
#include <iostream>

#include <Core/Engine.h>

using namespace std;

// Order of function calling can be seen in "Source/Core/World.cpp::LoopUpdate()"
// https://github.com/UPB-Graphics/SPG-Framework/blob/master/Source/Core/World.cpp

Laborator4::Laborator4()
{
}

Laborator4::~Laborator4()
{
	//distruge obiecte
}

Mesh* Laborator4::createQuad(const char *name)
{

	vector<VertexFormat> vertices
	{
		VertexFormat(glm::vec3(-1, -1, -1), glm::vec3(0, 0, 0),glm::vec3(0, 0, 0),glm::vec2(0,1)),
		VertexFormat(glm::vec3(1, -1, -1), glm::vec3(0, 0, 0),glm::vec3(0, 0, 0),glm::vec2(1,1)),
		VertexFormat(glm::vec3(1, 1, -1), glm::vec3(0, 0, 0),glm::vec3(0, 0, 0),glm::vec2(1,0)),
		VertexFormat(glm::vec3(-1, 1, -1), glm::vec3(0, 0, 0),glm::vec3(0, 0, 0),glm::vec2(0,0)),
		
	};
	vector<unsigned int> indices =
	{
		0, 1, 2,
		0, 2, 3,
	};

	meshes[name] = new Mesh(name);
	meshes[name]->InitFromData(vertices, indices);
	return meshes[name];
}


void Laborator4::createNet(const string &path, const char *fileName)
{
	int control_points_no;
	int triangles_no;
	string word;
	std::string::size_type word_size;
	int vertex_id;
	glm::vec3 v1, v2;
	vector<VertexFormat> vertices1, vertices2;
	vector<unsigned int> indices;
	ifstream f;
	
	
	f.open((path + (fileName ? (string("/") + fileName) : "")).c_str());
	f >> control_points_no;
	
	//citire perechile de puncte de control din cele doua retele
	for (int i = 0; i < control_points_no; i++)
	{
		f >> word;
		v1.x = stof(word, &word_size);
		f >> word;
		v1.y = stof(word, &word_size);
		v1.z = 0;
		f >> word;
		f >> word;
		v2.x = stof(word, &word_size);
		f >> word;
		v2.y = stof(word, &word_size);
		v2.z = 0;

		vertices1.push_back(VertexFormat(v1));
		vertices2.push_back(VertexFormat(v2));
	}
	f >> triangles_no;
	
	//citire triunghiurile formate
	for (int i = 0; i < triangles_no * 3; i++)
	{
		f >> vertex_id;
		indices.push_back(vertex_id);
	}

	//creare retele de triunghiuri
	sourceNet->InitFromData(vertices1, indices);
	destNet->InitFromData(vertices2, indices);
	f.close();
}



void Laborator4::Init()
{
	iterations = 10;
	counter = 0;

	std::string shaderPath = "Source/Laboratoare/Laborator4_VDVAC/Shaders/";

	// Create a shader program for rendering to texture
	{
		Shader *shader = new Shader("MainShader");
		shader->AddShader(shaderPath + "VertexShader.glsl", GL_VERTEX_SHADER);

		shader->AddShader(shaderPath + "FragmentShader.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	{
		Mesh* mesh = createQuad("quad");
		mesh->UseMaterials(false);
	}


	sourceImage = TextureManager::LoadTexture(RESOURCE_PATH::TEXTURES + "ImageAnimation/ex1/img1.png", nullptr, "sourceImage", true, true);
	interImage = TextureManager::LoadTexture(RESOURCE_PATH::TEXTURES + "ImageAnimation/ex1/img1.png", nullptr, "interImage", true, true);
	destImage = TextureManager::LoadTexture(RESOURCE_PATH::TEXTURES + "ImageAnimation/ex1/img2.png", nullptr, "destImage", true, true);
	
	
	sourceNet = new Mesh("sourceNet");
	destNet = new Mesh("destNet");
	createNet(RESOURCE_PATH::TEXTURES + "ImageAnimation/ex1/net.txt", nullptr);

	interNet = new Mesh("interNet");
	interNet->InitFromData(sourceNet->vertices, sourceNet->indices);
}

float Area(glm::vec2 v0, glm::vec2 v1, glm::vec2 v2)
{
	return abs((v0.x *(v1.y - v2.y) -v1.x * (v0.y - v2.y) + v2.x * (v0.y - v1.y))/2);
}

//returns true if pos is inside triangle v0v1v2 and computes baricentric coordinates
bool Baricentric(glm::vec2 pos, glm::vec2 v0, glm::vec2 v1, glm::vec2 v2, float &u, float &v, float &w)
{
	float A_u = 0 , A_v = 0, A_w = 0, A = 0;
	//TODO compute areas
	A = Area(v0, v1, v2);
	A_u = Area(pos, v1, v2);
	A_v = Area(v0, pos, v2);
	A_w = Area(v0, v1, pos);

	if (A_u + A_v + A_w > A+0.0001 && A>0.00001)
		return false;

	//TODO compute u,v,w;
	u = A_u / A;
	v = A_v / A;
	w = A_w / A;
	return true;
}

void Laborator4::ModifyImage(int crt_iteration, int iterations)
{
	vector<VertexFormat> vertices = interNet->vertices;
	vector<unsigned int> indices = interNet->indices;

	float ti = (float)crt_iteration / iterations;
	for (int i = 0; i < interNet->vertices.size(); i++)
	{
		//TODO compute vertices of the intermediary net through interpolation
		vertices[i].position = sourceNet->vertices[i].position;
	}

	unsigned int channels = sourceImage->GetNrChannels();
	unsigned char* sourceData = sourceImage->GetImageData();
	unsigned char* destData = destImage->GetImageData();
	unsigned char* interData = interImage->GetImageData();


	if (channels < 3)
		return;

	glm::ivec2 imageSize = glm::ivec2(sourceImage->GetWidth(), sourceImage->GetHeight());
	int offset, offsetSource, offsetDest;
	glm::vec2 v0, v1, v2, vsource0, vsource1, vsource2, vdest0, vdest1, vdest2;
	glm::vec2 pos, posSource, posDest;
	float u, v, w;
	int isource, jsource;
	int idest, jdest;
	

	for (int j = 0; j < imageSize.y; j++)
	{
		for (int i = 0; i < imageSize.x; i++)
		{
			offset = channels * (j * imageSize.x + i);
			pos.y = (float)(imageSize.y - j - 1) / imageSize.y;
			pos.x = (float)i / imageSize.x;
			
			for (int k = 0; k < indices.size(); k += 3)
			{
				//TODO compute v0, v1 and v2, vertices from the current triangle in the intermediary net
				v0 = glm::vec2(0, 0);
				v1 = glm::vec2(0, 0);
				v2 = glm::vec2(0, 0);
				if (Baricentric(pos, v0, v1, v2,u,v,w))
				{
					
					//TODO compute vsource0, vsource1 and vsource2, vertices from the current triangle in the source net
					vsource0 = glm::vec2(0, 0);
					vsource1 = glm::vec2(0, 0);
					vsource2 = glm::vec2(0, 0);
					
					//TODO compute vdest0, vdest1 and vdest2, vertices from the current triangle in the destination net
					vdest0 = glm::vec2(0, 0);
					vdest1 = glm::vec2(0, 0);
					vdest2 = glm::vec2(0, 0);

					//TODO compute posSource, the correspondent of pos in the source net
					posSource = glm::vec2(0, 0);
					
					//TODO compute posDest, the correspondent of pos in the destination net
					posDest = glm::vec2(0, 0);


					isource = posSource.x * imageSize.x + 0.5;
					jsource = imageSize.y - (posSource.y * imageSize.y + 0.5) - 1;

					idest = posDest.x * imageSize.x + 0.5;
					jdest = imageSize.y - (posDest.y * imageSize.y + 0.5) - 1;

					if (isource < 0) isource = 0;
					if (jsource < 0) jsource = 0;
					if (idest < 0) idest = 0;
					if (jdest < 0) jdest = 0;
					if (isource >= imageSize.x) isource = imageSize.x -1;
					if (jsource >= imageSize.y) jsource = imageSize.y - 1;
					if (idest >= imageSize.x) idest = imageSize.x - 1;
					if (jdest >= imageSize.y) jdest = imageSize.y - 1;


					offsetSource = channels * (jsource * imageSize.x + isource);
					offsetDest = channels * (jdest * imageSize.x + idest);
					
					//TODO compute RGB for the current pixel in the intermediary image (through interpolation)
					interData[offset] = sourceData[offset];
					interData[offset + 1] = sourceData[offset + 1];
					interData[offset + 2] = sourceData[offset + 2];
					break;
				}
				
			}
		}
	}

	interImage->UploadNewData(interData);
	interNet->InitFromData(vertices, indices);
}

void Laborator4::FrameStart()
{

}


void Laborator4::Update(float deltaTimeSeconds)
{
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);

	auto resolution = window->GetResolution();
	glViewport(0, 0, resolution.x/3, resolution.y);

	
	auto shader = shaders["MainShader"];
	int locTexture = shader->GetUniformLocation("textura1");
	int locImage = shader->GetUniformLocation("is_image");

	//source image
	glUniform1i(locTexture, 0);
	sourceImage->BindToTextureUnit(GL_TEXTURE0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glUniform1i(locImage, 1);
	RenderMesh(meshes["quad"], shader, glm::scale(glm::mat4(1), glm::vec3(1, 1, 1)));
		
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glUniform1i(locImage, 0);
	RenderMesh(sourceNet, shader, glm::scale(glm::mat4(1), glm::vec3(1, 1, 1)));


	//intermediary image
	glViewport(resolution.x / 3, 0, resolution.x / 3, resolution.y);
	glUniform1i(locTexture, 0);
	interImage->BindToTextureUnit(GL_TEXTURE0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glUniform1i(locImage, 1);
	RenderMesh(meshes["quad"], shader, glm::scale(glm::mat4(1), glm::vec3(1, 1, 1)));

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glUniform1i(locImage, 0);
	RenderMesh(interNet, shader, glm::scale(glm::mat4(1), glm::vec3(1, 1, 1)));

	//destination image
	glViewport(resolution.x * 2/3, 0, resolution.x / 3, resolution.y);
	glUniform1i(locTexture, 0);
	destImage->BindToTextureUnit(GL_TEXTURE0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glUniform1i(locImage, 1);
	RenderMesh(meshes["quad"], shader, glm::scale(glm::mat4(1), glm::vec3(1, 1, 1)));

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glUniform1i(locImage, 0);
	RenderMesh(destNet, shader, glm::scale(glm::mat4(1), glm::vec3(1, 1, 1)));
		

}




void Laborator4::FrameEnd()
{
	//DrawCoordinatSystem();
	
}

// Read the documentation of the following functions in: "Source/Core/Window/InputController.h" or
// https://github.com/UPB-Graphics/SPG-Framework/blob/master/Source/Core/Window/InputController.h

void Laborator4::OnInputUpdate(float deltaTime, int mods)
{
	
};

void Laborator4::OnKeyPress(int key, int mods)
{
	if (key == GLFW_KEY_A)
	{
		
		if (counter < iterations)
		{
			counter++;
			ModifyImage(counter, iterations);
		}
	}
	else
	if (key == GLFW_KEY_Z)
	{
		
		if (counter > 0)
		{
			counter--;
			ModifyImage(counter, iterations);
		}
	}
	

};

void Laborator4::OnKeyRelease(int key, int mods)
{
	// add key release event
};

void Laborator4::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	// add mouse move event
};

void Laborator4::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button press event
};

void Laborator4::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button release event
}

void Laborator4::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
	// treat mouse scroll event
}

void Laborator4::OnWindowResize(int width, int height)
{
	// treat window resize event
}
