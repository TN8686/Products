#include "Table.h"

#include <fstream>
#include <vector>

using namespace std;

namespace Engine {
	/////////////////////////
	// Context
	// csvTableを読み取る簡易パーサー.
	/////////////////////////
	template<int N>
	class Context {
		// メンバ変数.
	public:
		char str[N];

	private:
		int size;
		int pos;

		// コンストラクタ.
	public:
		Context()
			: size(N)
			, pos(0)
			, str{}
		{}

		// 操作.
	public:
		// パラメータを取得してポインタを先に進める.
		std::string getAndNext() {
			char c[N] = {};
			int p = 0;
			for (int i = pos; i < size; ++i, ++p, ++pos) {
				if (str[i] == ',') {	// データの区切り、または終端
					++pos;
					return c;
				}
				if (str[i] == '\0') {
					return c;			// 終端の場合、posを進めない事で、もう一度呼ばれた時は空白の文字列を返す.
				}

				c[p] = str[i];
			}
			return c;
		}

		// リセット.
		void reset() {
			pos = 0;
		}
	};

	/////////////////
	// Record
	/////////////////
	Table::Variant Table::Record::nullVariant;

	// デストラクタ.
	Table::Record::~Record() {
		// 全て破棄.
		for (auto& e : values) {
			delete  e.second;
		}
	}

	Table::Variant& Table::Record::operator[](const char* key) {
		if (values.find(key) == values.end()) {	// キーの有無をチェック.
			return nullVariant;					// 存在しなかったらnullVariantを返す.
		}
		return *values[key];					// 存在したらその参照を返す.
	}

	void Table::Record::add(const char* key, const Variant& val) {
		if (values.find(key) == values.end()) {	// キーの有無をチェック.
			values[key] = new Variant(val);		// 存在しなかったら新たな領域を確保.
		}
		else {
			(*values[key]) = val;				// 存在したらその領域を書き換える.
		}
	}


	//////////////////
	// Table
	//////////////////
	Table::Record Table::nullRecord;

	Table::Table(const char* filePath) {
		static constexpr int BUFF = 2048;	// ひとまず一行最大2048文字まで取得可能.

		ifstream ifs(filePath);
		if (ifs.is_open()) {		// ファイルオープン成功.
			// 行列数取得.
			int col = 0, row = 0;
			char s[BUFF] = {};
			ifs.getline(s, BUFF);
			col = atoi(s);
			ifs.getline(s, BUFF);
			row = atoi(s);

			// データkey取得.
			vector<string> dataKeys;
			Context<BUFF> ct;
			ifs.getline(ct.str, BUFF);	// 一行分データを取得.
			for (int c = 0; c < col; ++c) {
				dataKeys.emplace_back(ct.getAndNext());	// キーのリストを作成.
			}

			// データ取得.
			ct.reset();
			for (int r = 0; r < row; ++r) {
				ifs.getline(ct.str, BUFF);	// 一行分データを取得.
				Record* rec = new Record();
				string key = ct.getAndNext();
				for (int c = 1; c < col; ++c) {	// cはキー値領域の分ズラして1で初期化.
					rec->add(dataKeys[c].c_str(), Variant(ct.getAndNext()));	// 対象のキーの位置に、一要素分のパラメータを入れる.
				}
				records[key] = rec;
				ct.reset();
			}
		}
	}

	Table::~Table() {
		// 全てのレコードを破棄.
		for (auto& e : records) {
			delete e.second;
		}
	}

	Table::Record& Table::operator[](const char* key) {
		if (records.find(key) == records.end()) {	// キーの有無をチェック.
			return nullRecord;						// 見つからなかったらnullRecordを返す.
		}
		return *records[key];						// 見つかったら対象のレコードを返す.
	}

	////////////////
	// intTable
	////////////////
	intTable::intTable(const char* filePath)
	{
		static constexpr int BUFF = 2048;	// ひとまず一行最大2048文字まで取得可能.

		ifstream ifs(filePath);
		if (ifs.is_open()) {		// ファイルオープン成功.
			// 行列数取得.
			int col = 0, row = 0;
			char s[BUFF] = {};
			ifs.getline(s, BUFF);
			col = atoi(s);
			ifs.getline(s, BUFF);
			row = atoi(s);

			// 領域を確保.
			table_.resize(col);
			for (int i = 0; i < col; ++i) {
				table_.push();
			}
			for (auto& e : table_) {
				e.resize(row);
			}

			// データ取得.
			Context<BUFF> ct;
			ct.reset();
			for (int r = 0; r < row; ++r) {
				ifs.getline(ct.str, BUFF);	// 一行分データを取得.
				
				for (int c = 0; c < col; ++c) {
					table_[r].push(atoi(ct.getAndNext().c_str()));	// 対象の位置に、一要素分のパラメータを入れる.
				}
				ct.reset();
			}
			
		}
	}

	////////////////
	// CombinationTable
	////////////////
	CombinationTable::CombinationTable(const char* filePath)
	{
		static constexpr int BUFF = 2048;	// ひとまず一行最大2048文字まで取得可能.

		ifstream ifs(filePath);
		if (ifs.is_open()) {		// ファイルオープン成功.
			// 行列数取得.
			int col = 0, row = 0;
			char s[BUFF] = {};
			ifs.getline(s, BUFF);
			col = atoi(s);
			ifs.getline(s, BUFF);
			row = atoi(s);


			// 領域を確保.
			table_.resize(col * (col + 1) / 2);	// 組合せのデータ想定で、組合せ数分の領域を確保.
			key_.resize(col);

			// データ取得.
			Context<BUFF> ct;

			ifs.getline(ct.str, BUFF);
			for (int c = 0; c < col; ++c) {
				key_.push(ct.getAndNext());	// 対象の位置に、一要素分のパラメータを入れる.
			}

			ct.reset();
			for (int r = 1; r < row; ++r) {
				ifs.getline(ct.str, BUFF);	// 一行分データを取得.

				for (int c = 0; c < col; ++c) {
					auto str = ct.getAndNext();	// 一要素分取得.
					if (str.empty()) {	// 空白の場合次の行へ.
						break;
					}
					table_.push(str);	// 対象の位置に、一要素分のパラメータを入れる.
				}
				ct.reset();
			}

		}
	}
}