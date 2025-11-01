#include "pch.h"
#include "TetrisGame.h"

// Определение статического члена
const int TetrisGame::BASE_POINTS[] = { 0, 100, 300, 500, 800 };

// Конструктор
TetrisGame::TetrisGame()
{
	// Инициализация игрового состояния
	m_gameOver = false;
	m_gamePaused = false;
	m_gameStarted = false;

	// Инициализация статистики
	m_score = 0;
	m_level = 1;
	m_linesCleared = 0;
	m_piecesPlaced = 0;

	// Инициализация временных переменных
	m_lastDropTime = 0;
	m_dropDelay = GetDropDelay();

	// Очистка поля
	ClearField();

	// Инициализация генератора случайных чисел
	InitRandomGenerator();
}

// Деструктор
TetrisGame::~TetrisGame()
{
	// Освобождение ресурсов (если необходимо)
}

// Начать новую игру
void TetrisGame::StartNewGame()
{

	ClearField();
	m_score = 0;
	m_level = 1;
	m_linesCleared = 0;
	m_piecesPlaced = 0;
	m_gameOver = false;
	m_gamePaused = false;
	m_gameStarted = true;

	// Генерируем первые фигуры
	GenerateNewPiece(); // Следующая фигура
	GenerateNewPiece(); // Текущая фигура (переместится из следующей)
}

// Приостановить игру
void TetrisGame::PauseGame()
{
	// TODO: Реализовать приостановку игры
	if (m_gameStarted && !m_gameOver)
	{
		m_gamePaused = true;
	}
}

// Возобновить игру
void TetrisGame::ResumeGame()
{
	// TODO: Реализовать возобновление игры
	if (m_gameStarted && !m_gameOver)
	{
		m_gamePaused = false;
		m_lastDropTime = GetTickCount(); // Сброс таймера падения
	}
}

// Завершить игру
void TetrisGame::EndGame()
{
	// TODO: Реализовать завершение игры
	m_gameOver = true;
	m_gameStarted = false;
}

// Обновление игрового состояния (вызывается по таймеру)
void TetrisGame::Update()
{
	// TODO: Реализовать основной игровой цикл
	// - Проверить состояние игры
	// - Обработать автоматическое падение фигур
	// - Проверить заполненные линии
	// - Обновить статистику

	if (!m_gameStarted || m_gameOver || m_gamePaused)
		return;

	DWORD currentTime = GetTickCount();
	if (currentTime - m_lastDropTime >= (DWORD)m_dropDelay)
	{
		// Попытка переместить фигуру вниз
		if (!MovePiece(0, 1))
		{
			// Фигура не может двигаться - размещаем её
			PlacePiece(m_currentPiece);

			// Проверяем заполненные линии
			int clearedLines = ClearFullLines();
			if (clearedLines > 0)
			{
				UpdateScore(clearedLines);
				UpdateLevel();
			}

			// Генерируем новую фигуру
			GenerateNewPiece();

			// Проверяем возможность размещения новой фигуры
			if (!CanPlacePiece(m_currentPiece))
			{
				EndGame(); // Игра окончена
			}
		}

		m_lastDropTime = currentTime;
	}
}

// Переместить текущую фигуру
bool TetrisGame::MovePiece(int dx, int dy)
{
	// TODO: Реализовать перемещение фигуры
	// - Проверить возможность перемещения
	// - Обновить позицию фигуры
	// - Вернуть результат операции

	if (!m_gameStarted || m_gameOver || m_gamePaused)
		return false;

	if (CanPlacePiece(m_currentPiece, dx, dy))
	{
		m_currentPiece.x += dx;
		m_currentPiece.y += dy;
		return true;
	}

	return false;
}

// Повернуть текущую фигуру
bool TetrisGame::RotatePiece()
{
	if (!m_gameStarted || m_gameOver || m_gamePaused)
		return false;

	// Вычисляем новое состояние поворота
	int newRotation = (m_currentPiece.rotation + 1) % 4;

	// Проверяем возможность поворота с помощью универсальной функции
	if (CanPlacePiece(m_currentPiece, 0, 0, newRotation))
	{
		m_currentPiece.rotation = newRotation;
		return true;
	}

	// TODO: Реализовать wall kick system при необходимости
	// Можно попробовать небольшие смещения если поворот на месте невозможен

	return false;
}

// Быстрое падение фигуры
void TetrisGame::DropPiece()
{
	// TODO: Реализовать мгновенное падение
	// - Найти самую нижнюю возможную позицию
	// - Переместить фигуру в эту позицию
	// - Начислить бонусные очки за быстрое падение

	if (!m_gameStarted || m_gameOver || m_gamePaused)
		return;

	while (MovePiece(0, 1))
	{
		// Перемещаем вниз до упора
		m_score += 1; // Бонус за быстрое падение
	}
}

// Получить игровое поле
const int(*TetrisGame::GetField())[FIELD_WIDTH]
{
	return m_field;
}

// Получить текущую фигуру
const Tetromino& TetrisGame::GetCurrentPiece() const
{
	return m_currentPiece;
}

// Получить следующую фигуру
const Tetromino& TetrisGame::GetNextPiece() const
{
	return m_nextPiece;
}

// Получить счет
int TetrisGame::GetScore() const
{
	return m_score;
}

// Получить уровень
int TetrisGame::GetLevel() const
{
	return m_level;
}

// Получить количество очищенных линий
int TetrisGame::GetLinesCleared() const
{
	return m_linesCleared;
}

// Проверить окончание игры
bool TetrisGame::IsGameOver() const
{
	return m_gameOver;
}

// Проверить приостановку игры
bool TetrisGame::IsGamePaused() const
{
	return m_gamePaused;
}

// ПРИВАТНЫЕ МЕТОДЫ - ЗАГЛУШКИ

// Генерация новой фигуры
void TetrisGame::GenerateNewPiece()
{
	// TODO: Реализовать генерацию фигур
	// - Переместить следующую фигуру в текущую
	// - Сгенерировать новую следующую фигуру
	// - Установить начальную позицию
	// - Установить цвет фигуры

	m_currentPiece = m_nextPiece;
	m_currentPiece.x = FIELD_WIDTH / 2 - 2;
	m_currentPiece.y = 0;

	m_nextPiece.type = GetRandomTetrominoType();
	m_nextPiece.rotation = 0;
	m_nextPiece.color = GetTetrominoColor(m_nextPiece.type);
	m_nextPiece.x = FIELD_WIDTH / 2 - 2;
	m_nextPiece.y = 0;
}

// Проверка возможности размещения
bool TetrisGame::CanPlacePiece(const Tetromino& piece, int dx, int dy, int newRotation)
{
	// Вычисляем итоговые координаты и поворот
	int newX = piece.x + dx;
	int newY = piece.y + dy;
	int rotation = (newRotation == -1) ? piece.rotation : newRotation;

	// Массивы смещений блоков для каждого типа тетромино и поворота
	// Формат: [rotation][block][x/y] - смещение от опорной точки

	// I-тетромино (опорная точка: верхний блок)


	// Выбираем массив смещений в зависимости от типа фигуры
	const int(*offsets)[4][2] = nullptr;

	switch (piece.type)
	{
	case TETROMINO_I: offsets = &I_OFFSETS[rotation]; break;
	case TETROMINO_O: offsets = &O_OFFSETS[rotation]; break;
	case TETROMINO_T: offsets = &T_OFFSETS[rotation]; break;
	case TETROMINO_S: offsets = &S_OFFSETS[rotation]; break;
	case TETROMINO_Z: offsets = &Z_OFFSETS[rotation]; break;
	case TETROMINO_J: offsets = &J_OFFSETS[rotation]; break;
	case TETROMINO_L: offsets = &L_OFFSETS[rotation]; break;
	default:
		return false; // Неизвестный тип фигуры
	}

	// Проверяем каждый блок фигуры
	for (int i = 0; i < 4; i++)
	{
		int blockX = newX + (*offsets)[i][1]; // X-смещение (столбец)
		int blockY = newY + (*offsets)[i][0]; // Y-смещение (строка)

		// Проверка границ поля
		if (blockX < 0 || blockX >= FIELD_WIDTH || blockY < 0 || blockY >= FIELD_HEIGHT)
		{
			return false;
		}

		// Проверка коллизий с размещенными блоками
		if (m_field[blockY][blockX] != 0)
		{
			return false;
		}
	}

	return true;
}

// Размещение фигуры на поле
void TetrisGame::PlacePiece(const Tetromino& piece)
{
	// Определяем массив смещений для текущего типа и поворота
	const int(*offsets)[4][2] = nullptr;
	int rotation = piece.rotation % 4;
	switch (piece.type)
	{
	case TETROMINO_I: offsets = &I_OFFSETS[rotation]; break;
	case TETROMINO_O: offsets = &O_OFFSETS[rotation]; break;
	case TETROMINO_T: offsets = &T_OFFSETS[rotation]; break;
	case TETROMINO_S: offsets = &S_OFFSETS[rotation]; break;
	case TETROMINO_Z: offsets = &Z_OFFSETS[rotation]; break;
	case TETROMINO_J: offsets = &J_OFFSETS[rotation]; break;
	case TETROMINO_L: offsets = &L_OFFSETS[rotation]; break;
	default: return; // неизвестный тип — ничего не делаем
	}

	// Размещаем каждый из 4 блоков фигуры
	for (int i = 0; i < 4; ++i)
	{
		int blockX = piece.x + (*offsets)[i][1]; // смещение по X
		int blockY = piece.y + (*offsets)[i][0]; // смещение по Y
		if (blockX >= 0 && blockX < FIELD_WIDTH && blockY >= 0 && blockY < FIELD_HEIGHT)
		{
			m_field[blockY][blockX] = (int)piece.type;
		}
	}

	m_piecesPlaced++;
}

// Получить блоки фигур

// Очистить поле
void TetrisGame::ClearField()
{
	memset(m_field, 0, sizeof(m_field));
}

// Очистить заполненные линии
int TetrisGame::ClearFullLines()
{
	// TODO: Реализовать очистку заполненных линий
	// - Найти все заполненные линии
	// - Удалить их
	// - Опустить линии выше
	// - Вернуть количество очищенных линий

	int clearedCount = 0;

	for (int row = FIELD_HEIGHT - 1; row >= 0; row--)
	{
		if (IsLineFull(row))
		{
			DropLinesAbove(row);
			clearedCount++;
			row++; // Проверяем эту же строку еще раз
		}
	}

	m_linesCleared += clearedCount;
	return clearedCount;
}

// Проверить заполненность линии
bool TetrisGame::IsLineFull(int line)
{
	// TODO: Проверить все блоки в линии
	for (int col = 0; col < FIELD_WIDTH; col++)
	{
		if (m_field[line][col] == 0)
			return false;
	}
	return true;
}

// Опустить линии выше указанной
void TetrisGame::DropLinesAbove(int line)
{
	// TODO: Сдвинуть все линии выше на одну позицию вниз
	for (int row = line; row > 0; row--)
	{
		for (int col = 0; col < FIELD_WIDTH; col++)
		{
			m_field[row][col] = m_field[row - 1][col];
		}
	}

	// Очистить верхнюю линию
	for (int col = 0; col < FIELD_WIDTH; col++)
	{
		m_field[0][col] = 0;
	}
}

// Обновить счет
void TetrisGame::UpdateScore(int linesCleared)
{
	// TODO: Реализовать систему подсчета очков
	// - Базовые очки за линии
	// - Бонус за множественные линии (Tetris)
	// - Множитель уровня

	if (linesCleared > 0 && linesCleared <= 4)
	{
		m_score += BASE_POINTS[linesCleared] * m_level;
	}
}

// Обновить уровень
void TetrisGame::UpdateLevel()
{
	// TODO: Реализовать систему уровней
	// - Увеличение уровня каждые 10 линий
	// - Ускорение падения фигур

	int newLevel = (m_linesCleared / LVL_UP_LINES) + 1;
	if (newLevel != m_level)
	{
		m_level = newLevel;
		m_dropDelay = GetDropDelay();
	}
}

// Получить задержку падения для текущего уровня
int TetrisGame::GetDropDelay() const
{
	// TODO: Реализовать прогрессивное ускорение
	// Стандартная формула: начальная задержка уменьшается с уровнем

	int baseDelay = BASE_DELAY; // 1 секунда для уровня 1
	return max(50, baseDelay - (m_level - 1) * 50); // Минимум 50мс
}

// Получить цвет для типа фигуры
COLORREF TetrisGame::GetTetrominoColor(TetrominoType type)
{
	// TODO: Определить стандартные цвета для каждого типа фигуры
	switch (type)
	{
	case TETROMINO_I: return RGB(0, 255, 255);   // Cyan
	case TETROMINO_O: return RGB(255, 255, 0);   // Yellow
	case TETROMINO_T: return RGB(255, 0, 255);   // Magenta
	case TETROMINO_S: return RGB(0, 255, 0);     // Green
	case TETROMINO_Z: return RGB(255, 0, 0);     // Red
	case TETROMINO_J: return RGB(0, 0, 255);     // Blue
	case TETROMINO_L: return RGB(255, 165, 0);   // Orange
	default: return RGB(128, 128, 128);          // Gray
	}
}

// Инициализация генератора случайных чисел
void TetrisGame::InitRandomGenerator()
{
	srand((unsigned int)time(nullptr));
}

// Получить случайный тип фигуры
TetrominoType TetrisGame::GetRandomTetrominoType()
{
	int randomValue = rand() % 7 + 1; // 1-7
	return (TetrominoType)randomValue;
}
