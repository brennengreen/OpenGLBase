#pragma once

#include "Lights.h"
#include "Model.h"
#include "Camera.h"
#include "Cubemap.h"

#include <memory>

class Scene {
public:
	Scene()=default;
	~Scene()=default;

	std::shared_ptr<Camera> mCamera;
	std::vector<std::shared_ptr<Cubemap>> mSkyboxes;
	std::vector<std::shared_ptr<Model>> mModels;
	std::vector<std::shared_ptr<DirectionalLight>> mDirLights;
	std::vector<std::shared_ptr<PointLight>> mPointLights;
	std::vector<std::shared_ptr<SpotLight>> mSpotLights;
};