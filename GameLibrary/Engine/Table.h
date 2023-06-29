#pragma once

#include <string>
#include <unordered_map>

#include "../Utility/MyArray.h"

namespace Engine {
	class Table {
	public:
		/////////////////
		// Variant
		/////////////////
		struct Variant
		{
			int iVal;
			float fVal;
			std::string str;
			Variant()
				: iVal()
				, fVal()
			{}
			Variant(const std::string& str)
				: iVal(atoi(str.c_str()))
				, fVal(static_cast<float>(atof(str.c_str())))
				, str(str)
			{}

			// string�Aint�Afloat�ŊȒP�ɕԂ���悤�ɂ���.
			operator std::string() {
				return str;
			}
			operator int() {
				return iVal;
			}
			operator float() {
				return fVal;
			}
		};

		///////////////////
		// Record
		///////////////////
		class Record {
			std::unordered_map<std::string, Variant*> values;
			static Variant nullVariant;

		public:
			Record() = default;

			// �f�B�[�v�R�s�[�̋֎~.
			Record(const Record& src) = delete;
			Record& operator =(const Record& src) = delete;

			// �f�X�g���N�^.
		public:
			~Record();

			// ����.
		public:
			Table::Variant& operator [](const char* key);
			void add(const char* key, const Variant& val);
		};

		///////////////////
		// Table
		///////////////////

		// �����o�ϐ�.
	protected:
		static Record nullRecord;
		std::unordered_map<std::string, Record*> records;

		// �R���X�g���N�^.
	public:
		explicit Table(const char* filePath);

		// �f�B�[�v�R�s�[�̋֎~.
		Table(const Table& src) = delete;
		Table& operator =(const Table& src) = delete;

		// �f�X�g���N�^.
	public:
		virtual ~Table();
	
		// ����.
	public:
		// ���R�[�h�擾.
		Record& operator [](const char* key);
	};

	// �X�e�[�W��񓙂Ɏg�p����int�̓񎟌��z��p�e�[�u��.
	class intTable {
	protected:
		Utility::MyArray<Utility::MyArray<int>> table_;
		
		// �R���X�g���N�^.
	public:
		explicit intTable(const char* filePath);

		// �f�B�[�v�R�s�[�̋֎~.
		intTable(const intTable& src) = delete;
		intTable& operator =(const intTable& src) = delete;

		// �f�X�g���N�^.
	public:
		virtual ~intTable() = default;

		// ����.
	public:

		Utility::MyArray<Utility::MyArray<int>>& get() { return table_; }

		// ���R�[�h�̎擾.
		Utility::MyArray<int>& operator [](const int& id) {
			return table_[id];
		}
	};

	// �g�������Ɏg�p����string��Variant�̈ꎟ���z��p�e�[�u��.
	class CombinationTable {
	protected:
		Utility::MyArray<std::string> key_;
		Utility::MyArray<Table::Variant> table_;

		// �R���X�g���N�^.
	public:
		explicit CombinationTable(const char* filePath);

		// �f�B�[�v�R�s�[�̋֎~.
		CombinationTable(const CombinationTable& src) = delete;
		CombinationTable& operator =(const CombinationTable& src) = delete;

		// �f�X�g���N�^.
	public:
		virtual ~CombinationTable() = default;

		// ����.
	public:

		Utility::MyArray<std::string>& getKey() { return key_; }
		Utility::MyArray<Table::Variant>& get() { return table_; }

		// ���R�[�h�̎擾.
		Utility::MyArray<Table::Variant>& operator [](const int& id) {
			return table_;
		}
	};
}