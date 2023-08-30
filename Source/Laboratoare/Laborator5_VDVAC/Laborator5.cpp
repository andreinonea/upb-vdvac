#include "Laborator5.h"
#include "FFD.h"
#include <vector>
#include <iostream>
#include <Core/Engine.h>

using namespace std;
#define MAX_DIM 10000

// Order of function calling can be seen in "Source/Core/World.cpp::LoopUpdate()"
// https://github.com/UPB-Graphics/SPG-Framework/blob/master/Source/Core/World.cpp

Laborator5::Laborator5()
{
}

Laborator5::~Laborator5()
{
	//distruge obiecte
	

}


void Laborator5::Init()
{
	ToggleGroundPlane();
	//lumina & material
	light_position = glm::vec3(10, 7, 25);
	material_shininess = 30;
	material_kd = 0.5;
	material_ks = 0.5;

	frame = 0;

	auto camera = GetSceneCamera();
	camera->SetPositionAndRotation(glm::vec3(2, 2, 2), glm::quat(glm::vec3(-30 * TO_RADIANS, 45 * TO_RADIANS, 0)));
	camera->Update();


	

	// Load a mesh from file into GPU memory
	{
		Mesh* mesh = new Mesh("bunny");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Models", "bunny.obj");
		meshes[mesh->GetMeshID()] = mesh;

		positions = mesh->positions;
		normals = mesh->normals;
		indices = mesh->indices;
	}

	

	glm::vec3 pos;
	float minx = MAX_DIM, miny = MAX_DIM, minz = MAX_DIM, maxx = -MAX_DIM, maxy = -MAX_DIM, maxz = -MAX_DIM;
	for (int i = 0; i < positions.size(); i++)
	{
		pos = positions[i];
		if (minx > pos.x) minx = pos.x;
		if (miny > pos.y) miny = pos.y;
		if (minz > pos.z) minz = pos.z;
		if (maxx < pos.x) maxx = pos.x;
		if (maxy < pos.y) maxy = pos.y;
		if (maxz < pos.z) maxz = pos.z;
	}

	float scale_factor = max(maxx - minx, maxy - miny);
	scale_factor = max(scale_factor, maxz - minz);
	scale = glm::vec3(scale_factor);

	for (int i = 0; i < positions.size(); i++)
	{
		positions[i] -= glm::vec3(minx, miny, minz);
		positions[i] /= scale;
	}

	control_points1.resize(64);
	control_points2.resize(64);
	control_points.resize(64);
	coefficients.resize(positions.size() * 64);

	FFD::InitControlPoints(control_points1);
	FFD::InitControlPoints(control_points);
	FFD::CreateBernsteinCoefficients(coefficients, positions);
	FFD::ComputeNewControlPointsPositions(control_points2);


	{
		Mesh* mesh = new Mesh("CP");
		meshes[mesh->GetMeshID()] = mesh;
		for (int i = 0; i < 64; i++)
		{
			indices_cp.push_back(i);
			normals_cp.push_back(glm::vec3(1, 0, 0));
		}
		mesh->InitFromData(control_points1, normals_cp, indices_cp);
		mesh->SetDrawMode(GL_POINTS);
	}


	std::string shaderPath = "Source/Laboratoare/Laborator5_VDVAC/Shaders/";

	// Create a shader program for rendering to texture
	{
		Shader *shader = new Shader("MainShader");
		shader->AddShader(shaderPath + "VertexShader.glsl", GL_VERTEX_SHADER);

		shader->AddShader(shaderPath + "FragmentShader.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

}

void Laborator5::FrameStart()
{
	//TODO4 decomenteaza dupa ce ai calculat ComputeVerticesPosition()
	//FFD::AnimateControlPointPositions(control_points1, control_points2, control_points, frame, 20);
	//FFD::ComputeVerticesPosition(control_points, coefficients, positions);
	frame++;
	if (frame >= 19)
		frame = 0;
}

void Laborator5::Update(float deltaTimeSeconds)
{
	ClearScreen();
	
	{
		auto shader = shaders["MainShader"];

		shader->Use();
		auto camera = GetSceneCamera();
		glm::vec3 eye_position = camera->transform->GetLocalPosition();

		glUniform3f(glGetUniformLocation(shader->program, "light_position"), light_position.x, light_position.y, light_position.z);
		glUniform3f(glGetUniformLocation(shader->program, "eye_position"), eye_position.x, eye_position.y, eye_position.z);
		glUniform1i(glGetUniformLocation(shader->program, "material_shininess"), material_shininess);
		glUniform1f(glGetUniformLocation(shader->program, "material_kd"), material_kd);
		glUniform1f(glGetUniformLocation(shader->program, "material_ks"), material_ks);
		
		glUniform3f(glGetUniformLocation(shader->program, "color"), 0.3, 0.2, 1);
		Mesh *mesh_obj = meshes["bunny"];
		mesh_obj->InitFromData(positions, normals, indices);
		RenderMesh(mesh_obj, shaders["MainShader"], glm::vec3(0,0,0), glm::vec3(1, 1, 1));

		glUniform3f(glGetUniformLocation(shader->program, "color"), 1, 0, 0);
		Mesh *mesh_cp= meshes["CP"];
		mesh_cp->InitFromData(control_points, normals_cp, indices_cp);
		glPointSize(7);
		RenderMesh(mesh_cp, shaders["MainShader"], glm::vec3(0, 0, 0),glm::vec3(1,1,1));

	}
	
}



void Laborator5::FrameEnd()
{
	DrawCoordinatSystem();
	
}

// Read the documentation of the following functions in: "Source/Core/Window/InputController.h" or
// https://github.com/UPB-Graphics/SPG-Framework/blob/master/Source/Core/Window/InputController.h

void Laborator5::OnInputUpdate(float deltaTime, int mods)
{

};

void Laborator5::OnKeyPress(int key, int mods)
{

};

void Laborator5::OnKeyRelease(int key, int mods)
{
	// add key release event
};

void Laborator5::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	// add mouse move event
};

void Laborator5::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button press event
};

void Laborator5::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button release event
}

void Laborator5::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
	// treat mouse scroll event
}

void Laborator5::OnWindowResize(int width, int height)
{
	// treat window resize event
}
