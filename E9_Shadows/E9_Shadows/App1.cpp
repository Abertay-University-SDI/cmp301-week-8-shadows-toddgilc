// Lab1.cpp
// Lab 1 example, simple coloured triangle mesh
#include "App1.h"

App1::App1()
{

}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in, bool VSYNC, bool FULL_SCREEN)
{
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);

	// Create Mesh object and shader object
	mesh = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext());
	sphereMesh = new SphereMesh(renderer->getDevice(), renderer->getDeviceContext());
	sphereMesh2 = new SphereMesh(renderer->getDevice(), renderer->getDeviceContext());
	cubeMesh2 = new CubeMesh(renderer->getDevice(), renderer->getDeviceContext());
	orthoMesh = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth / 4, screenHeight / 4, -screenWidth / 2.7, screenHeight / 2.7);
	
	model = new AModel(renderer->getDevice(), "res/teapot.obj");
	textureMgr->loadTexture(L"brick", L"res/brick1.dds");

	// initial shaders
	textureShader = new TextureShader(renderer->getDevice(), hwnd);
	depthShader = new DepthShader(renderer->getDevice(), hwnd);
	shadowShader = new ShadowShader(renderer->getDevice(), hwnd);

	// Variables for defining shadow map
	int shadowmapWidth = 1024;
	int shadowmapHeight = 1024;


	// This is your shadow map
	for (int i = 0; i < 2; i++)
	{
		shadowMap[i] = new ShadowMap(renderer->getDevice(), shadowmapWidth, shadowmapHeight);
	}

	for (int i = 0; i < 2; i++)
	{
		myLights[i] = new Light();
	}

	myLights[0]->setDirection(0.0f, -0.7f, 0.7f);
	myLights[0]->setPosition(0.f, 0.f, 10.f);
	myLights[0]->setDiffuseColour(0.3f, 0.2f, 0.6f, 1.0f);
	myLights[0]->setAmbientColour(0.3f, 0.3f, 0.3f, 1.0f);

	myLights[1]->setDirection(0.0f, -0.7f, 0.7f);
	myLights[1]->setPosition(0.f, 0.f, 10.f);
	myLights[1]->setDiffuseColour(0.3f, 1.0f, 1.0f, 1.0f);
	myLights[1]->setAmbientColour(0.3f, 0.3f, 0.3f, 1.0f);

	myLights[0]->generateOrthoMatrix((float)sceneWidth, (float)sceneHeight, 0.1f, 100.f);
	myLights[1]->generateOrthoMatrix((float)sceneWidth, (float)sceneHeight, 0.1f, 100.f);
	
	//myLights[0]->generateProjectionMatrix
	screenW = screenWidth;
	
}

App1::~App1()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();

	// Release the Direct3D object.

}


bool App1::frame()
{
	bool result;

	result = BaseApplication::frame();
	if (!result)
	{
		return false;
	}
	
	// Render the graphics.
	result = render();
	if (!result)
	{
		return false;
	}

	return true;
}

bool App1::render()
{
	myLights[0]->generateOrthoMatrix((float)sceneWidth, (float)sceneHeight, 0.1f, 100.f); // nEED TO FOLOW CAM LIVE yey
	myLights[1]->generateOrthoMatrix((float)sceneWidth, (float)sceneHeight, 0.1f, 100.f);

	myLights[0]->setPosition(lightx, lighty, lightz);
	myLights[1]->setPosition(light2x, light2y, light2z);

	myLights[0]->setDirection(lightDirx, lightDiry, lightDirz);
	myLights[1]->setDirection(lightDir2x, lightDir2y, lightDir2z);

	myLights[0]->setDiffuseColour(light1Colour[0], light1Colour[1], light1Colour[2], 1.0f);
	myLights[1]->setDiffuseColour(light2Colour[0], light2Colour[1], light2Colour[2], 1.0f);


	// Perform depth pass
	depthPass();
	// Render scene
	//shadowPass();
	finalPass();

	return true;
}

void App1::depthPass()
{
	// Set the render target to be the render to texture.
	for (int i = 0; i < 2; i++) {

		shadowMap[i]->BindDsvAndSetNullRenderTarget(renderer->getDeviceContext());

		// get the world, view, and projection matrices from the camera and d3d objects.
		myLights[i]->generateViewMatrix();

		XMMATRIX lightViewMatrix = myLights[i]->getViewMatrix();
		XMMATRIX lightProjectionMatrix = myLights[i]->getOrthoMatrix();


		XMMATRIX worldMatrix = renderer->getWorldMatrix();
		worldMatrix = XMMatrixTranslation(-50.f, 0.f, -50.f); //moves so that teapot is centred 
		// Render floor
		mesh->sendData(renderer->getDeviceContext());
		depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
		depthShader->render(renderer->getDeviceContext(), mesh->getIndexCount());

		worldMatrix = renderer->getWorldMatrix();
		worldMatrix = XMMatrixRotationY(matrixRotation) * XMMatrixTranslation(0.f, 7.f, 5.f);
		XMMATRIX scaleMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
		worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
		// Render model
		model->sendData(renderer->getDeviceContext());
		depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
		depthShader->render(renderer->getDeviceContext(), model->getIndexCount());

		worldMatrix = XMMatrixRotationY(matrixRotation) * XMMatrixTranslation(0.f, 17.f, 19.f);
		//XMMATRIX scaleMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
		worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
		cubeMesh2->sendData(renderer->getDeviceContext());
		depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
		depthShader->render(renderer->getDeviceContext(), cubeMesh2->getIndexCount());

		depthMapText[i] = shadowMap[i]->getDepthMapSRV();

		// Set back buffer as render target and reset view port.
		renderer->setBackBufferRenderTarget();
		renderer->resetViewport();
	}
}


//void App1::shadowPass()
//{
//	// Set the render target to be the render to texture and clear it
//	renderTexture->setRenderTarget(renderer->getDeviceContext());
//	renderTexture->clearRenderTarget(renderer->getDeviceContext(), 1.0f, 0.0f, 0.0f, 1.0f);
//
//	// Get matrices
//	camera->update(); //use cam 1 here for diff cam
//	XMMATRIX worldMatrix = renderer->getWorldMatrix();
//	XMMATRIX viewMatrix = camera->getViewMatrix(); //use cam 1 here for diff cam
//	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();
//
//	// Render shape with simple lighting shader set.
//	cubeMesh->sendData(renderer->getDeviceContext());
//	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"brick"), shadowMap->getDepthMapSRV(), myLights);
//	shadowShader->render(renderer->getDeviceContext(), cubeMesh->getIndexCount());
//
//	// Reset the render target back to the original back buffer and not the render to texture anymore.
//	renderer->setBackBufferRenderTarget();
//}


void App1::finalPass()
{
	// Clear the scene. (default blue colour)
	renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);
	camera->update();

	matrixRotation += 0.01;

	// get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();
	
	worldMatrix = XMMatrixTranslation(-50.f, 0.f, -10.f);
	// Render floor
	mesh->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"brick"), depthMapText, myLights);
	shadowShader->render(renderer->getDeviceContext(), mesh->getIndexCount());


	worldMatrix = XMMatrixTranslation(lightx, lighty, lightz);
	//XMMATRIX scaleMatrix2 = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	sphereMesh->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"brick"), depthMapText, myLights);
	shadowShader->render(renderer->getDeviceContext(), sphereMesh->getIndexCount());

	worldMatrix = XMMatrixTranslation(light2x, light2y, light2z);
	//XMMATRIX scaleMatrix2 = XMMatrixScaling(0.5f, 0.5f, 0.5f);e
	sphereMesh2->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"brick"), depthMapText, myLights);
	shadowShader->render(renderer->getDeviceContext(), sphereMesh2->getIndexCount());


	// Render model
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixRotationY(matrixRotation) * XMMatrixTranslation(0.f, 7.f, 5.f);
	XMMATRIX scaleMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	model->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"brick"), depthMapText, myLights);
	shadowShader->render(renderer->getDeviceContext(), model->getIndexCount());
	worldMatrix = XMMatrixRotationY(0);


	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixRotationY(matrixRotation) * XMMatrixTranslation(0.f, 17.f, 19.f);
	scaleMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	cubeMesh2->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"brick"), depthMapText, myLights);
	shadowShader->render(renderer->getDeviceContext(), cubeMesh2->getIndexCount());
	//worldMatrix = XMMatrixRotationY(0);


	worldMatrix = renderer->getWorldMatrix();
	renderer->setZBuffer(false);
	XMMATRIX orthoMatrix = renderer->getOrthoMatrix();  // ortho matrix for 2D rendering
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix();	// Default camera position for orthographic rendering

	orthoMesh->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, depthMapText[0]);
	textureShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());

	worldMatrix += XMMatrixTranslation(screenW * 1.35, 0, 0);
	orthoMesh->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, depthMapText[1]);
	textureShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());

	renderer->setZBuffer(true);

	gui();
	renderer->endScene();
}


void App1::gui()
{
	// Force turn off unnecessary shader stages.
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	// Build UI
	ImGui::Text("FPS: %.2f", timer->getFPS());
	ImGui::Checkbox("Wireframe mode", &wireframeToggle);

	ImGui::SliderFloat("LightposX", &lightx, -100, 100);
	ImGui::SliderFloat("LightposY", &lighty, -100, 100);
	ImGui::SliderFloat("LightposZ", &lightz, -100, 100);

	ImGui::SliderFloat("LightdirX", &lightDirx, -1, 1);
	ImGui::SliderFloat("LightdirY", &lightDiry, -1, 1);
	ImGui::SliderFloat("LightdirZ", &lightDirz, -1, 1);

	ImGui::ColorPicker3("Light1ColourPicker", light1Colour);

	ImGui::SliderFloat("Light2posX", &light2x, -100, 100);
	ImGui::SliderFloat("Light2posY", &light2y, -100, 100);
	ImGui::SliderFloat("Light2posZ", &light2z, -100, 100);

	ImGui::SliderFloat("Light2dirX", &lightDir2x, -1, 1);
	ImGui::SliderFloat("Light2dirY", &lightDir2y, -1, 1);
	ImGui::SliderFloat("Light2dirZ", &lightDir2z, -1, 1);

	ImGui::ColorPicker3("Light2ColourPicker", light2Colour);

	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

