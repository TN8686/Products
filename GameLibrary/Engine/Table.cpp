#include "Table.h"

#include <fstream>
#include <vector>

using namespace std;

namespace Engine {
	/////////////////////////
	// Context
	// csvTable��ǂݎ��ȈՃp�[�T�[.
	/////////////////////////
	template<int N>
	class Context {
		// �����o�ϐ�.
	public:
		char str[N];

	private:
		int size;
		int pos;

		// �R���X�g���N�^.
	public:
		Context()
			: size(N)
			, pos(0)
			, str{}
		{}

		// ����.
	public:
		// �p�����[�^���擾���ă|�C���^���ɐi�߂�.
		std::string getAndNext() {
			char c[N] = {};
			int p = 0;
			for (int i = pos; i < size; ++i, ++p, ++pos) {
				if (str[i] == ',') {	// �f�[�^�̋�؂�A�܂��͏I�[
					++pos;
					return c;
				}
				if (str[i] == '\0') {
					return c;			// �I�[�̏ꍇ�Apos��i�߂Ȃ����ŁA������x�Ă΂ꂽ���͋󔒂̕������Ԃ�.
				}

				c[p] = str[i];
			}
			return c;
		}

		// ���Z�b�g.
		void reset() {
			pos = 0;
		}
	};

	/////////////////
	// Record
	/////////////////
	Table::Variant Table::Record::nullVariant;

	// �f�X�g���N�^.
	Table::Record::~Record() {
		// �S�Ĕj��.
		for (auto& e : values) {
			delete  e.second;
		}
	}

	Table::Variant& Table::Record::operator[](const char* key) {
		if (values.find(key) == values.end()) {	// �L�[�̗L�����`�F�b�N.
			return nullVariant;					// ���݂��Ȃ�������nullVariant��Ԃ�.
		}
		return *values[key];					// ���݂����炻�̎Q�Ƃ�Ԃ�.
	}

	void Table::Record::add(const char* key, const Variant& val) {
		if (values.find(key) == values.end()) {	// �L�[�̗L�����`�F�b�N.
			values[key] = new Variant(val);		// ���݂��Ȃ�������V���ȗ̈���m��.
		}
		else {
			(*values[key]) = val;				// ���݂����炻�̗̈������������.
		}
	}


	//////////////////
	// Table
	//////////////////
	Table::Record Table::nullRecord;

	Table::Table(const char* filePath) {
		static constexpr int BUFF = 2048;	// �ЂƂ܂���s�ő�2048�����܂Ŏ擾�\.

		ifstream ifs(filePath);
		if (ifs.is_open()) {		// �t�@�C���I�[�v������.
			// �s�񐔎擾.
			int col = 0, row = 0;
			char s[BUFF] = {};
			ifs.getline(s, BUFF);
			col = atoi(s);
			ifs.getline(s, BUFF);
			row = atoi(s);

			// �f�[�^key�擾.
			vector<string> dataKeys;
			Context<BUFF> ct;
			ifs.getline(ct.str, BUFF);	// ��s���f�[�^���擾.
			for (int c = 0; c < col; ++c) {
				dataKeys.emplace_back(ct.getAndNext());	// �L�[�̃��X�g���쐬.
			}

			// �f�[�^�擾.
			ct.reset();
			for (int r = 0; r < row; ++r) {
				ifs.getline(ct.str, BUFF);	// ��s���f�[�^���擾.
				Record* rec = new Record();
				string key = ct.getAndNext();
				for (int c = 1; c < col; ++c) {	// c�̓L�[�l�̈�̕��Y������1�ŏ�����.
					rec->add(dataKeys[c].c_str(), Variant(ct.getAndNext()));	// �Ώۂ̃L�[�̈ʒu�ɁA��v�f���̃p�����[�^������.
				}
				records[key] = rec;
				ct.reset();
			}
		}
	}

	Table::~Table() {
		// �S�Ẵ��R�[�h��j��.
		for (auto& e : records) {
			delete e.second;
		}
	}

	Table::Record& Table::operator[](const char* key) {
		if (records.find(key) == records.end()) {	// �L�[�̗L�����`�F�b�N.
			return nullRecord;						// ������Ȃ�������nullRecord��Ԃ�.
		}
		return *records[key];						// ����������Ώۂ̃��R�[�h��Ԃ�.
	}

	////////////////
	// intTable
	////////////////
	intTable::intTable(const char* filePath)
	{
		static constexpr int BUFF = 2048;	// �ЂƂ܂���s�ő�2048�����܂Ŏ擾�\.

		ifstream ifs(filePath);
		if (ifs.is_open()) {		// �t�@�C���I�[�v������.
			// �s�񐔎擾.
			int col = 0, row = 0;
			char s[BUFF] = {};
			ifs.getline(s, BUFF);
			col = atoi(s);
			ifs.getline(s, BUFF);
			row = atoi(s);

			// �̈���m��.
			table_.resize(col);
			for (int i = 0; i < col; ++i) {
				table_.push();
			}
			for (auto& e : table_) {
				e.resize(row);
			}

			// �f�[�^�擾.
			Context<BUFF> ct;
			ct.reset();
			for (int r = 0; r < row; ++r) {
				ifs.getline(ct.str, BUFF);	// ��s���f�[�^���擾.
				
				for (int c = 0; c < col; ++c) {
					table_[r].push(atoi(ct.getAndNext().c_str()));	// �Ώۂ̈ʒu�ɁA��v�f���̃p�����[�^������.
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
		static constexpr int BUFF = 2048;	// �ЂƂ܂���s�ő�2048�����܂Ŏ擾�\.

		ifstream ifs(filePath);
		if (ifs.is_open()) {		// �t�@�C���I�[�v������.
			// �s�񐔎擾.
			int col = 0, row = 0;
			char s[BUFF] = {};
			ifs.getline(s, BUFF);
			col = atoi(s);
			ifs.getline(s, BUFF);
			row = atoi(s);


			// �̈���m��.
			table_.resize(col * (col + 1) / 2);	// �g�����̃f�[�^�z��ŁA�g���������̗̈���m��.
			key_.resize(col);

			// �f�[�^�擾.
			Context<BUFF> ct;

			ifs.getline(ct.str, BUFF);
			for (int c = 0; c < col; ++c) {
				key_.push(ct.getAndNext());	// �Ώۂ̈ʒu�ɁA��v�f���̃p�����[�^������.
			}

			ct.reset();
			for (int r = 1; r < row; ++r) {
				ifs.getline(ct.str, BUFF);	// ��s���f�[�^���擾.

				for (int c = 0; c < col; ++c) {
					auto str = ct.getAndNext();	// ��v�f���擾.
					if (str.empty()) {	// �󔒂̏ꍇ���̍s��.
						break;
					}
					table_.push(str);	// �Ώۂ̈ʒu�ɁA��v�f���̃p�����[�^������.
				}
				ct.reset();
			}

		}
	}
}