// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "DXF.h"	// include dxframework
#include "TextureShader.h"
#include "ShadowShader.h"
#include "DepthShader.h"

class App1 : public BaseApplication
{
public:

	App1();
	~App1();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN);

	bool frame();

protected:
	bool render();
	void depthPass();
	void finalPass();
	void shadowPass();
	void gui();

private:

	float lightx = 0.0f;
	float lighty = 0.0f;
	float lightz = 0.0f;

	float light2x = 0.0f;
	float light2y = 0.0f;
	float light2z = 0.0f;

	float lightDirx = 0.1f;
	float lightDiry = 0.1f;
	float lightDirz = 0.1f;

	float lightDir2x = 0.1f;
	float lightDir2y = 0.1f;
	float lightDir2z = 0.1f;

	float light1Colour[3];
	float light2Colour[3];

	float matrixRotation = 0;

	int screenW;	// need to uipdatye shadows
	
	int sceneW;
	int sceneH;

	TextureShader* textureShader;
	PlaneMesh* mesh;
	CubeMesh* cubeMesh;
	CubeMesh* cubeMesh2;
	SphereMesh* sphereMesh;
	SphereMesh* sphereMesh2;

	Light* myLights[2];
	AModel* model;
	ShadowShader* shadowShader;
	DepthShader* depthShader;
	OrthoMesh* orthoMesh;
	ShadowMap* shadowMap;

	RenderTexture* renderTexture;
};

#endif