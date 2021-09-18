#include <bangtal>
#include <random>

using namespace bangtal;

#define INIT_X 169
#define INIT_Y 0
#define PIECE_SIZE 128
#define PUZZLE_SIZE 4

#define UP 1
#define DOWN 2
#define LEFT 4
#define RIGHT 8

#define NOT_EMPTY 0

// 퍼즐 조각 클래스
class Piece {
	
	// bangtal 라이브러리로 만들어진 퍼즐 조각 객체
	bangtal::ObjectPtr pieceObject;
	
	// 해당 퍼즐 조각의 행 값
	int row;
	
	// 해당 퍼즐 조각의 열 값
	int column;
	
	// 퍼즐 조각의 좌표들
	int xCoord;
	int yCoord;

	// 해당 퍼즐 조각의 인접한 위치 중 빈칸이 존재하는 방향. 
	// 빈칸이 없다면 NOT_EMPTY 값을 갖는다. 
	int emptyDir;

public:

	Piece() {
		pieceObject = nullptr;
		row = 0;
		column = 0;
		xCoord = 0;
		yCoord = 0;
	}
	Piece(const std::string& imageFile, bangtal::ScenePtr background, int imageColumn, int imageRow, int dir = NOT_EMPTY) {
		pieceObject = Object::create(imageFile, background, INIT_X + PIECE_SIZE * (imageRow - 1), INIT_Y + PIECE_SIZE * (4 - imageColumn));
		this->row = imageRow;
		this->column = imageColumn;
		this->xCoord = INIT_X + PIECE_SIZE * (imageRow - 1);
		this->yCoord = INIT_Y + PIECE_SIZE * (4 - imageColumn);
		this->emptyDir = dir;
	}

	// 이 클래스 객체에 대한 -> 연산은 bangtal 라이브러리로 만들어진 peiceObject에 연결된다. 
	bangtal::ObjectPtr operator->() {
		return pieceObject;
	}

	void ChangeEmptyDir(int dir) {
		emptyDir = dir;
	}

	// 퍼즐 조각을 이동시키는 함수
	bool Move(bangtal::ScenePtr PuzzleBackground) {
		if (emptyDir == UP) {
			pieceObject->locate(PuzzleBackground, xCoord, yCoord + PIECE_SIZE);
			yCoord += PIECE_SIZE;
			column--;
		}
		else if (emptyDir == DOWN) {
			pieceObject->locate(PuzzleBackground, xCoord, yCoord - PIECE_SIZE);
			yCoord -= PIECE_SIZE;
			column++;
		}
		else if (emptyDir == LEFT) {
			pieceObject->locate(PuzzleBackground, xCoord - PIECE_SIZE, yCoord);
			xCoord -= PIECE_SIZE;
			row--;
		}
		else if (emptyDir == RIGHT) {
			pieceObject->locate(PuzzleBackground, xCoord + PIECE_SIZE, yCoord);
			xCoord += PIECE_SIZE;
			row++;
		}
		emptyDir = NOT_EMPTY;
		return true;
	}

	int getRow() {
		return row;
	}

	int getColumn() {
		return column;
	}
};

// 주어진 좌표 값이 올바른지 체크하는 함수
bool checkXY(int x, int y, int row, int column) {
	if ((x > INIT_X + PIECE_SIZE * column && x < INIT_X + PIECE_SIZE * (column + 1)) &&
		(y > INIT_Y + PIECE_SIZE * (PUZZLE_SIZE - row - 1) && (y < INIT_Y + PIECE_SIZE * (PUZZLE_SIZE - row)))) {
		return true;
	}
	return false;
}

struct RowColSet {
	int row;
	int column;
};

// 좌표 값을 받아 이에 해당하는 행과 열 값을 반환하는 함수
RowColSet xyToRowColumn(int x, int y) {
	RowColSet result = { 0, };
	for (int i = 0; i < PUZZLE_SIZE + 1; i++) {
		if (x > INIT_X + PIECE_SIZE * i && x < INIT_X + PIECE_SIZE * (i + 1)) {
			result.row = i + 1;
		}
		if (y > INIT_Y + PIECE_SIZE * (PUZZLE_SIZE - i - 1) && y < INIT_Y + PIECE_SIZE * (PUZZLE_SIZE - i)) {
			result.column = i + 1;
		}
	}
	return result;
}

// 전반적인 퍼즐의 상태를 조정하는 핸들러 클래스.
class PuzzleHandler {
private:
	bangtal::ScenePtr PuzzleBackground;
	Piece piece[PUZZLE_SIZE][PUZZLE_SIZE];
	bool isPieceExist[PUZZLE_SIZE][PUZZLE_SIZE];
	int emptySpace;

public:
	PuzzleHandler() {

		for (auto& i : isPieceExist) {
			for (auto& j : i) {
				j = true;
			}
		}

		// 퍼즐 배경과 조각들 생성.
		PuzzleBackground = Scene::create("퍼즐배경", "images/background.png");
		piece[0][0] = Piece("images/Puzzle1-1.png", PuzzleBackground, 1, 1);
		piece[0][1] = Piece("images/Puzzle1-2.png", PuzzleBackground, 1, 2);
		piece[0][2] = Piece("images/Puzzle1-3.png", PuzzleBackground, 1, 3);
		piece[0][3] = Piece("images/Puzzle1-4.png", PuzzleBackground, 1, 4);
		piece[1][0] = Piece("images/Puzzle2-1.png", PuzzleBackground, 2, 1);
		piece[1][1] = Piece("images/Puzzle2-2.png", PuzzleBackground, 2, 2);
		piece[1][2] = Piece("images/Puzzle2-3.png", PuzzleBackground, 2, 3);
		piece[1][3] = Piece("images/Puzzle2-4.png", PuzzleBackground, 2, 4);
		piece[2][0] = Piece("images/Puzzle3-1.png", PuzzleBackground, 3, 1);
		piece[2][1] = Piece("images/Puzzle3-2.png", PuzzleBackground, 3, 2);
		piece[2][2] = Piece("images/Puzzle3-3.png", PuzzleBackground, 3, 3);
		piece[2][3] = Piece("images/Puzzle3-4.png", PuzzleBackground, 3, 4, DOWN);
		piece[3][0] = Piece("images/Puzzle4-1.png", PuzzleBackground, 4, 1);
		piece[3][1] = Piece("images/Puzzle4-2.png", PuzzleBackground, 4, 2);
		piece[3][2] = Piece("images/Puzzle4-3.png", PuzzleBackground, 4, 3, RIGHT);

		isPieceExist[3][3] = false;

		// 퍼즐 조각을 직접 클릭하는 것이 아닌, 
		// 투명한 이미지를 클릭하여 그 위치에 따라 적절한 퍼즐 조각을 이동시킨다. 
		auto PuzzleMouseChecker = Object::create("images/background_test.png", PuzzleBackground);

		// 클릭 시 퍼즐 조각 이동
		PuzzleMouseChecker->setOnMouseCallback([&](auto object, int x, int y, auto action)->bool {
			UpdatePieces();
			RowColSet curRowCol = xyToRowColumn(x, y);

			for (int i = 0; i < PUZZLE_SIZE; i++) {
				for (int j = 0; j < PUZZLE_SIZE; j++) {
					if (curRowCol.row == piece[i][j].getRow() && curRowCol.column == piece[i][j].getColumn()) {
						piece[i][j].Move(PuzzleBackground);
						isPieceExist[curRowCol.column - 1][curRowCol.row - 1] = false;
						isPieceExist[piece[i][j].getColumn() - 1][piece[i][j].getRow() - 1] = true;
					}
				}
			}
			CheckWin();

			return true;
			});
		auto restartButton = Object::create("images/RestartButton.png", PuzzleBackground, INIT_X + PIECE_SIZE * 5 + 50, INIT_Y + PIECE_SIZE * 3 + 50);
		restartButton->setScale(0.03f);
		restartButton->setOnMouseCallback([&](auto object, int x, int y, auto action)->bool {
			Shuffle();
			return true;
			});
	};

	// 현재 퍼즐 판에 존재하는 모든 퍼즐들의 상태를 업데이트하는 함수. 
	// 인접한 빈칸의 방향 값을 업데이트한다. 
	bool UpdatePieces() {
		int emptyRow, emptyColumn;

		for (int i = 0; i < PUZZLE_SIZE; i++) {
			for (int j = 0; j < PUZZLE_SIZE; j++) {
				if (isPieceExist[i][j] == false) {
					emptyColumn = i + 1;
					emptyRow = j + 1;
				}
			}
		}

		for (auto& column : piece) {
			for (auto& row : column) {
				row.ChangeEmptyDir(NOT_EMPTY);
			}
		}

		int direstionSet = 15;
		if (emptyColumn == PUZZLE_SIZE) {
			direstionSet -= DOWN;
		}
		if (emptyColumn == 1) {
			direstionSet -= UP;
		}
		if (emptyRow == PUZZLE_SIZE) {
			direstionSet -= RIGHT;
		}
		if (emptyRow == 1) {
			direstionSet -= LEFT;
		}

		if (direstionSet & UP) {
			getPieseWithRowCol(emptyRow, emptyColumn - 1)->ChangeEmptyDir(DOWN);
		}
		if (direstionSet & DOWN) {
			getPieseWithRowCol(emptyRow, emptyColumn + 1)->ChangeEmptyDir(UP);
		}
		if (direstionSet & LEFT) {
			getPieseWithRowCol(emptyRow - 1, emptyColumn)->ChangeEmptyDir(RIGHT);
		}
		if (direstionSet & RIGHT) {
			getPieseWithRowCol(emptyRow + 1, emptyColumn)->ChangeEmptyDir(LEFT);
		}
		return true;
	}

	// 특정 위치에 해당하는 행, 열 값을 갖는 퍼즐 조각을 반환하는 함수
	Piece* getPieseWithRowCol(int toFindRow, int toFindColumn) {
		for (int i = 0; i < PUZZLE_SIZE; i++) {
			for (int j = 0; j < PUZZLE_SIZE; j++) {
				if (piece[i][j].getRow() == toFindRow && piece[i][j].getColumn() == toFindColumn) {
					return &piece[i][j];
				}
			}
		}
		return nullptr;
	}

	// 현재 상태가 승리인지 확인하는 함수
	bool CheckWin() {
		for (int i = 0; i < PUZZLE_SIZE; i++) {
			for (int j = 0; j < PUZZLE_SIZE; j++) {
				if (i == 3 && j == 3) {
					continue;
				}
				if (piece[i][j].getColumn() - 1 != i || piece[i][j].getRow() - 1 != j) {
					return false;
				}
			}
		}
		showMessage("우승!");
		return true;
	}

	// 퍼즐 조각들을 섞는 함수.
	bool Shuffle() {
		PuzzleBackground->enter();

		int shuffleCount = 600;

		RowColSet empty;
		for (int i = 0; i < PUZZLE_SIZE; i++) {
			for (int j = 0; j < PUZZLE_SIZE; j++) {
				if (isPieceExist[i][j] == false) {
					empty.column = i + 1;
					empty.row = j + 1;
				}
			}
		}

		int direction = 0;
		for (int i = 0; i < shuffleCount; i++) {
			while (true) {\
				std::random_device rd;
				std::mt19937 gen(rd());
				std::uniform_int_distribution<int> dis(1, 4);
				direction = dis(gen);
				if (empty.row == PUZZLE_SIZE && direction == 4) {
					continue;
				}
				if (empty.row == 1 && direction == 3) {
					continue;
				}
				if (empty.column == PUZZLE_SIZE && direction == 2) {
					continue;
				}
				if (empty.column == 1 && direction == 1) {
					continue;
				}
				break;
			}
			UpdatePieces();
			switch (direction) {
			case 1:
				getPieseWithRowCol(empty.row, empty.column - 1)->Move(PuzzleBackground);
				isPieceExist[empty.column - 2][empty.row - 1] = false;
				isPieceExist[empty.column - 1][empty.row - 1] = true;
				empty.row = empty.row;
				empty.column = empty.column - 1;
				break;
			case 2:
				getPieseWithRowCol(empty.row, empty.column + 1)->Move(PuzzleBackground);
				isPieceExist[empty.column][empty.row - 1] = false;
				isPieceExist[empty.column - 1][empty.row - 1] = true;
				empty.row = empty.row;
				empty.column = empty.column + 1;
				break;
			case 3:
				getPieseWithRowCol(empty.row - 1, empty.column)->Move(PuzzleBackground);
				isPieceExist[empty.column - 1][empty.row - 2] = false;
				isPieceExist[empty.column - 1][empty.row - 1] = true;
				empty.row = empty.row - 1;
				empty.column = empty.column;
				break;
			case 4:
				getPieseWithRowCol(empty.row + 1, empty.column)->Move(PuzzleBackground);
				isPieceExist[empty.column - 1][empty.row] = false;
				isPieceExist[empty.column - 1][empty.row - 1] = true;
				empty.row = empty.row + 1;
				empty.column = empty.column;
				break;
			default:
				printf("wrong direction : %d\n", direction);
				exit(1);
			}
		}

		return true;
	}
};

int main() {
	setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_ROOM_TITLE, false);

	auto TitleBackground = Scene::create("타이틀배경", "images/OriginalImage.png");
	auto Title = Object::create("images/Title.png", TitleBackground, 280, 240);
	auto StartButton = Object::create("images/StartButton.png", TitleBackground, 310, 70);

	StartButton->setScale(0.3f);
	PuzzleHandler handler;

	StartButton->setOnMouseCallback([&](auto object, int x, int y, auto action) -> bool {
		Title->hide();
		StartButton->hide();
		handler.Shuffle();
		return true;
		});

	startGame(TitleBackground);
}