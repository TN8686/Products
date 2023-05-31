#pragma once
#include <vector>
#include <memory>

template<typename T>
/// <summary>
/// unique_ptr�ŊǗ�����Ă���I�u�W�F�N�g��vector�ɁA�V���ȗv�f��ǉ������̐��|�C���^���擾����֐��ł�.
/// </summary>
/// <typeparam name="T">unique_ptr�ŊǗ�����Ă���I�u�W�F�N�g</typeparam>
/// <param name="list">unique_ptr��vector�z��</param>
/// <returns>�V���ɒǉ������v�f�̐��|�C���^</returns>
T* pCreateObject(std::vector<std::unique_ptr<T>>& list) {
	list.emplace_back();
	auto e = list.end() - 1;
	e->reset(new T());
	return e->get();
}