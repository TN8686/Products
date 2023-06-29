#include "Scene.h"
#include "ObjectManager.h"

namespace Engine{
	Scene::Scene()
		: objectManager_(nullptr)
	{
		objectManager_ = ObjectManager::getInstance();
	}
}
