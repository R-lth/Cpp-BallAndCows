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

	// 앞자리 숫자 검증
	if (value[0] == '0') {
		cout << "파일 데이터 오류: [" << value << "]\n";
		return false;
	}

	return true;
}

int GetRandomRow(const int rowCount, bool hasHeader = true)
{
	// 헤더 제외
	int start = hasHeader ? 1 : 0;
	if (rowCount <= start) {
		std::cout << "파일 행이 없습니다.\n";
		return -1;
	}

	// 랜덤 생성
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<int> dist(start, rowCount - 1);

	int randomRow = dist(gen);
	return randomRow;
}

string GetData() {
	// 파일 열기
	string filePath = "Data/Test1.csv";
	ifstream file(filePath);

	if (!file.is_open())
	{
		cout << "파일 열기 실패: " << filePath << '\n';
		// cout << "현재 실행 경로: " << std::filesystem::current_path() << '\n';
		return "";
	}

	// 파일 행 개수
	int rowCount = 0;
	string line;

	// 전체 라인 수 카운트
	while (getline(file, line)) {
		if (!line.empty()) {
			++rowCount;
		}
	}

	if (rowCount == 0) {
		std::cout << "파일이 비어있습니다.\n";
		return "";
	}

	// 파일 포인터 원위치
	file.clear();
	file.seekg(0);

	// 랜덤으로 행 지정
	int curRow = 0;
	int targetRow = GetRandomRow(rowCount, 0);
	if (targetRow == -1) {
		std::cout << "파일이 비어있습니다.\n";
		return "";
	}

	string data;
	while (getline(file, data)) {
		if (curRow == targetRow) {
			break;
		}
		++curRow;
	}

	// 파일 끝까지 갔는데 못 찾은 경우
	if (data.empty()) {
		cout << "파일 행이 존재하지 않습니다\n";
		return "";
	}

	// 게임 정답 숫자 추출
	stringstream ss(data);
	string value;
	getline(ss, value, ','); // csv이므로 ,로 읽음
	
	// 파일 닫기
	file.close();

	if (NormalizeAndCheck(value)) {
		return value;
	}
	else {
		return "";
	}
}

void Input(int& input_n, string& input_str) {
	while (true) {
		cin >> input_n;

		if (cin.fail()) {
			cin.clear();
			cin.ignore(1000, '\n'); // 잘못된 입력 버퍼 비우기
			cout << "세자리 숫자로 입력하세요" << '\n';
			continue;
		}

		input_str = to_string(input_n);
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
	if (answer == input_str) {
		return true;
	}

	// 스트라이크 개수
	int strike = 0;
	if (answer[0] == input_str[0]) { ++strike; }
	if (answer[1] == input_str[1]) { ++strike; }
	if (answer[2] == input_str[2]) { ++strike; }

	// 볼 개수
	int ball = 0;
	int arr1[10] = { 0 };
	int arr2[10] = { 0 };
	for (const char ch : answer) {
		++arr1[ch - '0'];
	}

	for (const char ch : input_str) {
		++arr2[ch - '0'];
	}

	for (int i = 0; i < 10; ++i) {
		if (arr1[i] > 0 && arr2[i] > 0) {
			++ball;
		}
	}
	ball -= strike;

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
		cout << "[ ROUND " << i << " ]" << '\n';
		cout << "  └─ 숫자 입력 : ";

		int input_n;
		string input_str;
		Input(input_n, input_str);
		
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

	return 0;
}

