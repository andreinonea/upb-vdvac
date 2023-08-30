#pragma once

#include <Component/SimpleScene.h>
using namespace std;


class Laborator6 : public SimpleScene
{
	

	public:
		Laborator6();
		~Laborator6();

		void Init() override;
		
	private:
		void bind(int i);
		void FrameStart() override;
		void Update(float deltaTimeSeconds) override;
		void FrameEnd() override;

		void OnInputUpdate(float deltaTime, int mods) override;
		void OnKeyPress(int key, int mods) override;
		void OnKeyRelease(int key, int mods) override;
		void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
		void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
		void OnWindowResize(int width, int height) override;

	private:
		std::unordered_map<std::string, Texture2D*> mapTextures;
		int indice;
		int timp;
		string girl;

		glm::vec3 light_position;
		unsigned int material_shininess;
		float material_kd;
		float material_ks;
};
