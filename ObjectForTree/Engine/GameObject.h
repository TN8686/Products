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
		// メンバ変数.
	private:
		bool isActive_;								// アクティブ状態.
		bool isDraw_;								// 描画フラグ.
		int lifeTime_;								// 累計生存時間.
		std::string name_;							// オブジェクト名.
		size_t nameHash_;							// オブジェクト名のハッシュ値.
		Transform transform_;						// 座標系.
		std::vector<GameObject*> refObjectList_;	// 参照オブジェクトリスト.

		// コンストラクタ.
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

		// 属性.
	public:
		// ゲッター.
		bool getActive() const;
		bool getDrawActive() const						{ return isDraw_; }
		bool getActiveSelf() const						{ return isActive_; }
		int getLifeTime() const							{ return lifeTime_; }
		std::string getName() const						{ return name_; }
		Transform getTransform() const					{ return transform_; }
		std::vector<GameObject*>* getRefObjectList()	{ return &refObjectList_; }

		// セッター.
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

		// 操作.
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