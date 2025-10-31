#pragma once
#include <cstdlib>
#include <ctime>
#include "pch.h"
// Типы тетромино
enum TetrominoType
{
	TETROMINO_NONE = 0,
	TETROMINO_I = 1,    // Линия
	TETROMINO_O = 2,    // Квадрат
	TETROMINO_T = 3,    // Т-образная
	TETROMINO_S = 4,    // S-образная
	TETROMINO_Z = 5,    // Z-образная
	TETROMINO_J = 6,    // J-образная
	TETROMINO_L = 7     // L-образная
};

static const int I_OFFSETS[4][4][2] = {
	{{0,0}, {1,0}, {2,0}, {3,0}},  // rotation 0: вертикальная (было rotation 1/3)
	{{0,0}, {0,1}, {0,2}, {0,3}},  // rotation 1: горизонтальная (было rotation 0/2)
	{{0,0}, {1,0}, {2,0}, {3,0}},  // rotation 2: вертикальная (было rotation 1/3)
	{{0,0}, {0,1}, {0,2}, {0,3}}   // rotation 3: горизонтальная (было rotation 0/2)
};

// O-тетромино (опорная точка: верхний левый)
static const int O_OFFSETS[4][4][2] = {
	{{0,0}, {0,1}, {1,0}, {1,1}},  // все повороты одинаковые
	{{0,0}, {0,1}, {1,0}, {1,1}},
	{{0,0}, {0,1}, {1,0}, {1,1}},
	{{0,0}, {0,1}, {1,0}, {1,1}}
};

// T-тетромино (опорная точка: верхний левый)
static const int T_OFFSETS[4][4][2] = {
	{{0,0}, {1,0}, {2,0}, {1,1}},  // rotation 0: (было rotation 1)
	{{1,0}, {1,1}, {1,2}, {2,1}},  // rotation 1: (было rotation 2)
	{{1,0}, {0,1}, {1,1}, {2,1}},  // rotation 2: (было rotation 3)
	{{0,1}, {1,0}, {1,1}, {1,2}}   // rotation 3: (было rotation 0)
};

// S-тетромино (опорная точка: верхний правый)
static const int S_OFFSETS[4][4][2] = {
	{{0,0}, {1,0}, {1,-1}, {2,-1}}, // rotation 0: (было rotation 1/3)
	{{0,0}, {0,-1}, {1,0}, {1,1}},  // rotation 1: (было rotation 0/2)
	{{0,0}, {1,0}, {1,-1}, {2,-1}}, // rotation 2: (было rotation 1/3)
	{{0,0}, {0,-1}, {1,0}, {1,1}}   // rotation 3: (было rotation 0/2)
};

// Z-тетромино (опорная точка: верхний левый)  
static const int Z_OFFSETS[4][4][2] = {
	{{0,1}, {1,0}, {1,1}, {2,0}},  // rotation 0: (было rotation 1/3)
	{{0,0}, {0,1}, {1,1}, {1,2}},  // rotation 1: (было rotation 0/2)
	{{0,1}, {1,0}, {1,1}, {2,0}},  // rotation 2: (было rotation 1/3)
	{{0,0}, {0,1}, {1,1}, {1,2}}   // rotation 3: (было rotation 0/2)
};

// J-тетромино (опорная точка: верхний)
static const int J_OFFSETS[4][4][2] = {
	{{0,0}, {0,1}, {0,2}, {1,2}},  // rotation 0: (было rotation 1)
	{{0,0}, {0,1}, {1,0}, {2,0}},  // rotation 1: (было rotation 2)
	{{0,0}, {1,0}, {1,1}, {1,2}},  // rotation 2: (было rotation 3)
	{{0,0}, {1,0}, {2,0}, {2,-1}}  // rotation 3: (было rotation 0)
};

// L-тетромино (опорная точка: верхний)
static const int L_OFFSETS[4][4][2] = {
	{{0,0}, {0,1}, {0,2}, {1,0}},  // rotation 0: (было rotation 1)
	{{0,0}, {0,1}, {1,1}, {2,1}},  // rotation 1: (было rotation 2)
	{{0,2}, {1,0}, {1,1}, {1,2}},  // rotation 2: (было rotation 3)
	{{0,0}, {1,0}, {2,0}, {2,1}}   // rotation 3: (было rotation 0)
};

// Структура для представления тетромино
struct Tetromino
{
	TetrominoType type;
	int x, y;                // Позиция на поле
	int rotation;            // Поворот (0-3)
	COLORREF color;          // Цвет фигуры

	Tetromino() : type(TETROMINO_NONE), x(0), y(0), rotation(0), color(RGB(255, 255, 255)) {}
};

// Основной класс игровой логики Тетрис
class TetrisGame
{


	// Константы
public:
	static const int FIELD_WIDTH = 10;
	static const int FIELD_HEIGHT = 20;
	static const int TETROMINO_SIZE = 4;    // Максимальный размер фигуры 4x4
	static const int BASE_DELAY = 750;

	// Конструкция
public:
	TetrisGame();
	virtual ~TetrisGame();

	// Основные методы игры
public:
	// Управление игрой
	void StartNewGame();                    // Начать новую игру
	void PauseGame();                       // Приостановить игру
	void ResumeGame();                      // Возобновить игру
	void EndGame();                         // Завершить игру

	// Игровая логика
	void Update();                          // Обновление игрового состояния (вызывается по таймеру)
	bool MovePiece(int dx, int dy);         // Переместить текущую фигуру
	bool RotatePiece();                     // Повернуть текущую фигуру
	void DropPiece();                       // Быстрое падение фигуры

	// Получение данных для отображения
	const int(*GetField())[FIELD_WIDTH];  // Получить игровое поле
	const Tetromino& GetCurrentPiece() const; // Получить текущую фигуру
	const Tetromino& GetNextPiece() const;    // Получить следующую фигуру

	// Игровая статистика
	int GetScore() const;                   // Получить счет
	int GetLevel() const;                   // Получить уровень
	int GetLinesCleared() const;            // Получить количество очищенных линий
	bool IsGameOver() const;                // Проверить окончание игры
	bool IsGamePaused() const;              // Проверить приостановку игры

	COLORREF GetTetrominoColor(TetrominoType type); // Получить цвет для типа фигуры
	// Вспомогательные методы
private:
	// Работа с фигурами
	void GenerateNewPiece();                // Генерация новой фигуры
	bool CanPlacePiece(const Tetromino& piece, int dx = 0, int dy = 0, int newRotation = -1); // Проверка возможности размещения
	void PlacePiece(const Tetromino& piece); // Размещение фигуры на поле

	// Работа с полем
	void ClearField();                      // Очистить поле
	int ClearFullLines();                   // Очистить заполненные линии
	bool IsLineFull(int line);              // Проверить заполненность линии
	void DropLinesAbove(int line);          // Опустить линии выше указанной

	// Система подсчета очков
	void UpdateScore(int linesCleared);     // Обновить счет
	void UpdateLevel();                     // Обновить уровень
	int GetDropDelay() const;               // Получить задержку падения для текущего уровня

	// Цвета фигур
	

	// Данные игры
private:
	// Игровое поле (0 - пусто, >0 - тип фигуры)
	int m_field[FIELD_HEIGHT][FIELD_WIDTH];

	// Текущие фигуры
	Tetromino m_currentPiece;               // Текущая падающая фигура
	Tetromino m_nextPiece;                  // Следующая фигура

	// Игровое состояние
	bool m_gameOver;                        // Флаг окончания игры
	bool m_gamePaused;                      // Флаг приостановки игры
	bool m_gameStarted;                     // Флаг начала игры

	// Игровая статистика
	int m_score;                            // Текущий счет
	int m_level;                            // Текущий уровень
	int m_linesCleared;                     // Количество очищенных линий
	int m_piecesPlaced;                     // Количество размещенных фигур

	// Временные переменные
	DWORD m_lastDropTime;                   // Время последнего падения
	int m_dropDelay;                        // Текущая задержка падения

	// Генератор случайных чисел
	void InitRandomGenerator();             // Инициализация ГСЧ
	TetrominoType GetRandomTetrominoType(); // Получить случайный тип фигуры
};
