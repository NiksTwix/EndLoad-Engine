#pragma once

/*
	EngineMUHF - Engine Most Usable Header Files
	This file includes most popular services and components
	Warning: avoid cycling
*/

//CORE
#include <Core\Core.hpp>
#include <Core\ServiceLocator.hpp>
#include <Core\Logger.hpp>
#include <Services\Settings\SettingsManager.hpp>
#include <Systems\Window\WindowManager.hpp>


//GRAPHICS
#include <Systems\Render/Render.hpp>


//COMPONENTS
#include <Components/AABB.hpp>
#include <Components/Camera.hpp>
#include <Components/Mesh.hpp>
#include <Components/Material.hpp>
#include <Components/Transform.hpp>
#include <Components/Script.hpp>
#include <Components/Sprite.hpp>

//RESOURCES
#include <Services/Resources\ResourceManager.hpp>
#include <Services/Resources\Graphics\TextureResource.hpp>
#include <Services/Resources\Graphics\MeshResource.hpp>
#include <Services/Resources\TextResource.hpp>
#include <Services/Resources\\Graphics\GLShader.hpp>
//SCENE
#include <Services\Scene\SceneContext.hpp>
#include <Services\Scene\SceneManager.hpp>
#include <Services\Scene\SceneLoader.hpp>
#include <Services\Serialization\SerializationService.hpp>