#pragma once
#include <vector>
#include <memory>

template<typename T>
/// <summary>
/// unique_ptrで管理されているオブジェクトのvectorに、新たな要素を追加しその生ポインタを取得する関数です.
/// </summary>
/// <typeparam name="T">unique_ptrで管理されているオブジェクト</typeparam>
/// <param name="list">unique_ptrのvector配列</param>
/// <returns>新たに追加した要素の生ポインタ</returns>
T* pCreateObject(std::vector<std::unique_ptr<T>>& list) {
	list.emplace_back();
	auto e = list.end() - 1;
	e->reset(new T());
	return e->get();
}