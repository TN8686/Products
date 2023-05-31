#pragma once
#include<vector>
#include<string>

#include "DxLib.h"

#include "Primitive.h"
#include "CollisionManager.h"


namespace Engine {

	class GameObject;

	struct Transform
	{
		Float3 position;
		Float3 rotation;
		Float3 scale;
		GameObject* parent_;

		Transform()
			: scale(1.0f, 1.0f, 1.0f)
			, parent_(nullptr)
		{}

		Float3 getWorldPosition() const;
		Float3 getWorldRotation() const;
		Float3 getWorldScale() const;
	};

	class GameObject
	{
		// �����o�ϐ�.
	private:
		bool isActive_;								// �A�N�e�B�u���.
		bool isDraw_;								// �`��t���O.
		int lifeTime_;								// �݌v��������.
		std::string name_;							// �I�u�W�F�N�g��.
		size_t nameHash_;							// �I�u�W�F�N�g���̃n�b�V���l.
		Transform transform_;						// ���W�n.
		std::vector<GameObject*> refObjectList_;	// �Q�ƃI�u�W�F�N�g���X�g.

		// �R���X�g���N�^.
	public:
		GameObject()
			: GameObject("GameObject")
		{}

		explicit GameObject(std::string name)
			: isActive_(true)
			, isDraw_(true)
			, lifeTime_(0)
			, name_(name)
			, nameHash_(std::hash<std::string>()(name))
		{}

		// ����.
	public:
		// �Q�b�^�[.
		bool getActive() const;
		bool getDrawActive() const						{ return isDraw_; }
		bool getActiveSelf() const						{ return isActive_; }
		int getLifeTime() const							{ return lifeTime_; }
		std::string getName() const						{ return name_; }
		Transform getTransform() const					{ return transform_; }
		std::vector<GameObject*>* getRefObjectList()	{ return &refObjectList_; }

		// �Z�b�^�[.
		void setActive(const bool &isActive)			{ isActive_ = isActive; }
		void setDrawActive(const bool &drawActive)		{ isDraw_ = drawActive; }
		void setName(const std::string &name)			{ 
			name_ = name;
			nameHash_ = std::hash<std::string>()(name);
		}
		void setTransform(const Transform &transform)	{ transform_ = transform; }
		void setPosition(const Float3 &position)		{ transform_.position = position; }
		void setRotation(const Float3 &rotation)		{ transform_.rotation = rotation; }
		void setScale(const Float3 &scale)				{ transform_.scale = scale; }

		void addRefObject(GameObject* pGameObject)		{ refObjectList_.emplace_back(pGameObject); }

		// ����.
	public:
		virtual bool load()			{ return true; }
		virtual bool init()			{
			lifeTime_ = 0;
			return true;
		}
		virtual bool update()		{ 
			++lifeTime_;
			return true;
		}
		virtual bool lateUpdate()	{ return true; }
		virtual bool render()		{ return true; }
		virtual bool unload()		{ return true; }

		virtual void onAABBStay2D(AABBCollision2D* const collision) {}
	};
}