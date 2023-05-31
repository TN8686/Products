#include "Scene.h"
#include "ObjectManager.h"

namespace Engine{
	Scene::Scene()
		: objectManager_(nullptr)
		, objMgrHandle_(-1)
	{
		objectManager_ = ObjectManager::getInstance();
	}
}
