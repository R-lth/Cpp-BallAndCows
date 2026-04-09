#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib> 
#include <random>

using namespace std;

bool NormalizeAndCheck(string& value) {
	// UTF-8 BOM 제거 (엑셀 CSV 대응)
	if (value.size() >= 3 &&
		static_cast<unsigned char>(value[0]) == 0xEF &&
		static_cast<unsigned char>(value[1]) == 0xBB &&
		static_cast<unsigned char>(value[2]) == 0xBF) {
		value.erase(0, 3);
	}

	// 따옴표 제거
	value.erase(remove(value.begin(), value.end(), '"'), value.end());

	// 앞쪽 공백 제거
	value.erase(
		value.begin(),
		find_if(value.begin(), value.end(),
			[](unsigned char ch) { return !isspace(ch); })
	);
	// 뒤쪽 공백 제거
	value.erase(
		find_if(value.rbegin(), value.rend(),
			[](unsigned char ch) { return !isspace(ch); }).base(),
		value.end()
	);

	// 3자리 숫자인지 검증
	if (value.size() != 3 ||
		!all_of(value.begin(), value.end(),
			[](unsigned char ch) { return isdigit(ch); })) {
		cout << "파일 데이터 오류: [" << value << "]\n";
		return false;
	}

	// 중복 숫자 검증
	if (value[0] == value[1] || value[0] == value[2] || value[1] == value[2]) {
		cout << "파일 데이터 오류: [" << value << "]\n";
		return false;
	}

	return true;
}

string GetData() {
	string filePath = "Data/Test1.csv";
	ifstream file(filePath);

	vector<string> candidate;
	string line;

	if (!file.is_open()) return "";

	while (getline(file, line)) {
		stringstream ss(line);
		string value;
		if (getline(ss, value, ',')) {
			if (NormalizeAndCheck(value)) {
				candidate.push_back(value);
			}
		}
	}

	if (candidate.empty()) return "";

	// 유효한 정답 후보들 중 하나를 랜덤 선택
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<int> dist(0, candidate.size() - 1);

	return candidate[dist(gen)];
}

void Input(string& input_str) {
	while (true) {
		cin >> input_str;

		if (cin.fail()) {
			cin.clear();
			cin.ignore(1000, '\n'); // 잘못된 입력 버퍼 비우기
			cout << "세자리 숫자로 입력하세요" << '\n';
			continue;
		}

		if (input_str.length() != 3) {
			cout << "세자리 숫자로 입력하세요" << '\n';
			continue;
		}

		if (input_str[0] == input_str[1] || input_str[0] == input_str[2] || input_str[1] == input_str[2]) {
			cout << "중복 없는 숫자로 입력하세요" << '\n';
			continue;
		}

		break;
	}
}

bool Check(const string& answer, const string& input_str) {
	int strike = 0;
	int ball = 0;

	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			if (answer[i] == input_str[j]) {
				if (i == j) strike++; 
				else ball++;        
			}
		}
	}

	if (strike == 3) return true;

	cout << "--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*-" << '\n';
	if (strike == 0 && ball == 0) {
		cout << "OUT! 정답에 포함된 숫자가 하나도 없습니다." << '\n';
	}
	else {
		cout << "아쉽네요!" << " " << strike << "스트라이크" << " " << ball << "볼" << "~~" << '\n';
	}
	cout << "----------------------------------------------------------------" << '\n';
	return false;
}

void GameIntro() {
	cout << "정해진 파일에 숫자를 입력하고 저장하면, 그 중 일부 숫자가 게임 내 정답으로 랜덤 적용됩니다." << '\n';
	cout << "┌──────────────────────────────────────────────────────────────┐" << '\n';
	cout << "│                 @ [ 숫자 야구 게임 규칙 ] @                  │" << '\n';
	cout << "├──────────────────────────────────────────────────────────────┤" << '\n';
	cout << "│  1. 숫자 선택 : 0~9 중 중복 없는 3자리 숫자를 입력하세요.    │" << '\n';
	cout << "│  2. 힌트 확인 : 입력한 숫자에 따라 판정이 내려집니다.        │" << '\n';
	cout << "│     ------------------------------------------------------   │" << '\n';
	cout << "│     [S] 스트라이크 : 숫자와 자릿수가 모두 일치할 때          │" << '\n';
	cout << "│     [B]      볼    : 숫자는 맞지만 자릿수가 틀릴 때          │" << '\n';
	cout << "│     [O]     아웃   : 제시한 숫자가 하나도 없을 때            │" << '\n';
	cout << "│     ------------------------------------------------------   │" << '\n';
	cout << "│  3. 승리 조건 : 3스트라이크 달성 시 'HOMERUN' 승리!          │" << '\n';
	cout << "└──────────────────────────────────────────────────────────────┘" << '\n';
	cout << '\n';
}

int setChance() {
	int n = 9;
	cout << "┌──────────────────────────────────────────────────────────────┐" << '\n';
	cout << "│            오늘 경기는 " << n << "회말 투아웃까지 진행됩니다.          │" << '\n';
	cout << "└──────────────────────────────────────────────────────────────┘" << '\n';

//	int n;
//	int i = 0;
//	for (; i < 3; ++i) {
//		cout << "  └─ ♥ 입력 : "; cin >> n;
//		if (n > 0 && n < 10) { break; }
//
//		if (cin.fail()) {
//			cin.clear();
//			cin.ignore(1000, '\n');
//		}
//
//		cout << "심판의 인내심 ";
//		for (int j = 0; j < 2 - i; ++j) {
//			cout << "■";
//		}
//		for (int j = 0; j < i + 1; ++j) {
//			cout << "□";
//		}
//		cout << " 남은 기회 " << 2 - i << "번!" << '\n';
//	}
//
//	if (n < 1 || n > 9) {
//		cout << "--------------------------------------------------" << '\n';
//		cout << "RED CARD! 아무키나 누르고 엔터를 치면 경기를 시작합니다. " << '\n';
//		cin.ignore(1000, '\n');
//		cin.get();
//		n = 9;
//	}
//
//	// 타섯 횟수 안내
//	system("cls");
//	GameIntro();

	return n;
}

void GameOver(const bool isSuccuess, const string answer) {
	if (isSuccuess) {
		cout << "================================================================" << '\n';
		cout << "             HOMERUN! 3스트라이크입니다.             " << '\n';
		cout << "             GAME OVER! 정답은 " << answer << "였습니다.               " << '\n';
		cout << "================================================================" << '\n';
	}
	else {
		cout << "================================================================" << '\n';
		cout << "             GAME OVER! 정답은 " << answer << "였습니다.               " << '\n';
		cout << "================================================================" << '\n';
	}
}

void Game(const string& answer, int chance) {
	cout << "================================================================" << '\n';
	cout << "             GAME START!                   " << '\n';
	cout << "================================================================" << '\n';

	bool isSuccess = false;

	for (int i = 0; i < chance; ++i) {
		cout << "[ ROUND " << i + 1 << " ]" << '\n';
		cout << "  └─ 숫자 입력 : ";

		int input_n;
		string input_str;
		Input(input_str);
		
		isSuccess = Check(answer, input_str);
		if (isSuccess) {
			GameOver(true, answer);
			break;
		}
	}

	if (!isSuccess) {
		GameOver(false, answer);
	}
}

int main(void) {
	// 파일 읽기
	string answer = GetData();
	bool run = !answer.empty();
	
	// 게임 실행
	if (run) {
		GameIntro();
		int chance = setChance();

		Game(answer, chance);
	}
	// 게임 실행 불가
	else {
		cout << "파일을 수정해주세요." << '\n';
	}

	cin.clear();
	cin.ignore(1000, '\n');
	cin.get();

	return 0;
}

