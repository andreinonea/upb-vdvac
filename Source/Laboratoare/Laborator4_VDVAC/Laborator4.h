#pragma once

#include <Component/SimpleScene.h>
using namespace std;


class Laborator4 : public SimpleScene
{
	

	public:
		Laborator4();
		~Laborator4();

		void Init() override;
		void ModifyImage(int crt_iteration, int iterations);
		Mesh* createQuad(const char *name);
		void createNet(const string &path, const char *fileName);

	private:

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
		Texture2D *sourceImage, *destImage, *interImage;
		Mesh *sourceNet, *destNet, *interNet;
		int iterations;
		int counter;
		
};
