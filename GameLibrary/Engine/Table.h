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

			// string、int、floatで簡単に返せるようにする.
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

			// ディープコピーの禁止.
			Record(const Record& src) = delete;
			Record& operator =(const Record& src) = delete;

			// デストラクタ.
		public:
			~Record();

			// 操作.
		public:
			Table::Variant& operator [](const char* key);
			void add(const char* key, const Variant& val);
		};

		///////////////////
		// Table
		///////////////////

		// メンバ変数.
	protected:
		static Record nullRecord;
		std::unordered_map<std::string, Record*> records;

		// コンストラクタ.
	public:
		explicit Table(const char* filePath);

		// ディープコピーの禁止.
		Table(const Table& src) = delete;
		Table& operator =(const Table& src) = delete;

		// デストラクタ.
	public:
		virtual ~Table();
	
		// 操作.
	public:
		// レコード取得.
		Record& operator [](const char* key);
	};

	// ステージ情報等に使用するintの二次元配列用テーブル.
	class intTable {
	protected:
		Utility::MyArray<Utility::MyArray<int>> table_;
		
		// コンストラクタ.
	public:
		explicit intTable(const char* filePath);

		// ディープコピーの禁止.
		intTable(const intTable& src) = delete;
		intTable& operator =(const intTable& src) = delete;

		// デストラクタ.
	public:
		virtual ~intTable() = default;

		// 操作.
	public:

		Utility::MyArray<Utility::MyArray<int>>& get() { return table_; }

		// レコードの取得.
		Utility::MyArray<int>& operator [](const int& id) {
			return table_[id];
		}
	};

	// 組合せ情報に使用するstringとVariantの一次元配列用テーブル.
	class CombinationTable {
	protected:
		Utility::MyArray<std::string> key_;
		Utility::MyArray<Table::Variant> table_;

		// コンストラクタ.
	public:
		explicit CombinationTable(const char* filePath);

		// ディープコピーの禁止.
		CombinationTable(const CombinationTable& src) = delete;
		CombinationTable& operator =(const CombinationTable& src) = delete;

		// デストラクタ.
	public:
		virtual ~CombinationTable() = default;

		// 操作.
	public:

		Utility::MyArray<std::string>& getKey() { return key_; }
		Utility::MyArray<Table::Variant>& get() { return table_; }

		// レコードの取得.
		Utility::MyArray<Table::Variant>& operator [](const int& id) {
			return table_;
		}
	};
}