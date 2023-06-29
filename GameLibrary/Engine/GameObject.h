#pragma once
#include<vector>
#include<string>

#include "DxLib.h"

#include "Primitive.h"
#include "HitChecker.h"


namespace Engine {

	class GameObject;
	class ObjectManager;

	struct Transform
	{
		Vector3 position;
		Vector3 rotation;
		Vector3 scale;
		GameObject* parent;

		Transform()
			: scale(1.0f, 1.0f, 1.0f)
			, parent(nullptr)
		{}

		Vector3 getWorldPosition() const;
		Vector3 getWorldRotation() const;
		Vector3 getWorldScale() const;
	};

	class GameObject
	{
		// メンバ変数.
	private:
		Transform transform_;						// 座標系.
		int lifeTime_;								// 累計生存時間.
		std::string name_;							// オブジェクト名.
		uint32_t nameHash_;							// オブジェクト名のハッシュ値.
		std::string tag_;							// タグ名.
		uint32_t tagHash_;							// タグのハッシュ値.
		std::vector<GameObject*> refObjectList_;	// 参照オブジェクトリスト.
		int drawLayer_;								// 描画優先順位.
		bool isActive_;								// アクティブ状態.
		bool isDraw_;								// 描画フラグ.

		// コンストラクタ.
	public:
		GameObject()
			: GameObject("GameObject")
		{}

		explicit GameObject(std::string name);

		// デストラクタ.
		virtual ~GameObject() = default;

		// 属性.
	public:
		// ゲッター.
		bool getActive() const;
		bool getDrawActive() const						{ return isDraw_; }
		bool getActiveSelf() const						{ return isActive_; }
		int getLifeTime() const							{ return lifeTime_; }
		std::string getName() const						{ return name_; }
		uint32_t getNameHash() const					{ return nameHash_; }
		std::string getTag() const						{ return tag_; }
		uint32_t getTagHash() const						{ return tagHash_; }
		Transform getTransform() const					{ return transform_; }
		std::vector<GameObject*>* getRefObjectList()	{ return &refObjectList_; }
		int getDrawLayer() const						{ return drawLayer_; }

		// セッター.
		void setActive(const bool &isActive)			{ isActive_ = isActive; }
		void setDrawActive(const bool &drawActive)		{ isDraw_ = drawActive; }
		void setName(const std::string &name);
		void setTag(const std::string &tag);
		void setDrawLayer(const int& drawLayer)			{ drawLayer_ = drawLayer; }
		void setTransform(const Transform &transform)	{ transform_ = transform; }
		void setPosition(const Vector3 &position)		{ transform_.position = position; }
		void setRotation(const Vector3 &rotation)		{ transform_.rotation = rotation; }
		void setScale(const Vector3 &scale)				{ transform_.scale = scale; }

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

		virtual void onCollisionStay2D(Engine::Collision2D* const ownCollision, Engine::Collision2D* const hitCollision) {}
	};
}