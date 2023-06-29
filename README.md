# Products
制作物

ポートフォリオURL：https://sites.google.com/da.iwasaki.ac.jp/takafumi-neya/

**MagicBlaster**  
DirectX12の学習で制作したライブラリ上で、二種類の攻撃を使い分けてスコアを稼ぐアクションゲームが実装されています。

**GameLibrary**  
2023年4月から制作中の、DxLibを利用した2Dゲーム制作を補助するためのライブラリ。
制作中のため一部半端な状態になっていますがご容赦ください。
機能の一部を紹介します。

Engineフォルダ内
GameObject.h/cpp
各種オブジェクトの親になるインターフェース。
ゲーム内のオブジェクトは全てこのクラスを継承して制作します。
親をたどってワールド座標に変換する機能などももっています。

ObjectManager.h/cpp
オブジェクトを管理、毎フレーム使用する各種関数を実行します。
管理対象オブジェクトを追加する時は、メモリ管理を外部で行い単にポインタをリストに追加する関数と、追加時にObjectManagerがヒープ上にメモリを確保、破棄時にdeleteする関数があり
厳密にメモリを管理したい時と簡易的に任せたい時の両方に対応しています。

HitChecker.h/cpp
衝突判定を行います。
衝突判定オブジェクトが定義されているCollision.h/cppや、
四分木空間分割で衝突判定の最適化を行うCollisionTrees.hと合わせて機能しています。

Utilityフォルダ
MemoryBuff.h
配置new用にメモリ管理を補助するオブジェクト。

MyArray.h
自作の簡易コンテナクラス。


**CollisionTrees**  
四分木空間分割による衝突判定の最適化の学習。  
Engineフォルダ内のCollisionTrees.hに記述されています。  
上記のGameLibrary上で動作している単純な作品に適用してみています。

**Xevious_clone**  
一年次に制作したXeviousの再現ゲーム。  
SoundManager以外を担当。  
画像、サウンド等の素材は抜いてあるため、実際の動作はポートフォリオの動画を参照してください。
