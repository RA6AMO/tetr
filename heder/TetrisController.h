#pragma once
#include "TetrisGame.h"
#include "DrawFild.h"
#include "pch.h"

// Форвардные объявления
class TetrisGame;
class DrawFild;

// Контроллер для управления игрой Тетрис
// Связывает игровую логику (TetrisGame) с отображением (DrawFild) и пользовательским вводом
class TetrisController
{
	// Конструкция
public:
	TetrisController();
	virtual ~TetrisController();

	// Инициализация и настройка
public:
	// Привязка компонентов
	void SetGameLogic(TetrisGame* pGame);       // Привязать игровую логику
	void SetRenderer(DrawFild* pRenderer);      // Привязать рендерер
	void SetParentWindow(CWnd* pParent);        // Привязать родительское окно

	// Инициализация игры
	bool Initialize();                          // Инициализировать контроллер
	void Shutdown();                            // Завершить работу контроллера

	// Управление игрой
public:
	// Основные команды игры
	void StartNewGame();                        // Начать новую игру
	void PauseGame();                          // Приостановить игру
	void ResumeGame();                         // Возобновить игру
	void EndGame();                            // Завершить игру
	void RestartGame();                        // Перезапустить игру

	// Состояние игры
	bool IsGameRunning() const;                // Проверить, запущена ли игра
	bool IsGamePaused() const;                 // Проверить, приостановлена ли игра
	bool IsGameOver() const;                   // Проверить, окончена ли игра

	// Обработка пользовательского ввода
public:
	// Обработка клавиатуры
	void OnKeyDown(UINT nChar);                // Обработка нажатия клавиши
	void OnKeyUp(UINT nChar);                  // Обработка отпускания клавиши

	// Игровой цикл и обновления  
public:
	// Основной игровой цикл
	void Update();                             // Обновление игрового состояния (вызывается по таймеру)
	void Render();                             // Отрисовка игрового поля
	void ForceRedraw();                        // Принудительная перерисовка

	// Таймеры
	void StartGameTimer();                     // Запустить игровой таймер
	void StopGameTimer();                      // Остановить игровой таймер
	void OnTimer(UINT_PTR nIDEvent);          // Обработка таймера

	// Настройки игры
public:
	// Управление настройками
	void SetGameSpeed(int level);              // Установить скорость игры
	void SetShowNextPiece(bool show);          // Показывать/скрыть следующую фигуру
	void SetShowGhost(bool show);              // Показывать/скрыть фантомную фигуру

	// Получение настроек
	int GetGameSpeed() const;                  // Получить скорость игры
	bool IsNextPieceVisible() const;           // Проверить видимость следующей фигуры
	bool IsGhostVisible() const;               // Проверить видимость фантомной фигуры

	// Статистика и информация
public:
	// Получение игровой статистики
	int GetCurrentScore() const;               // Получить текущий счет
	int GetCurrentLevel() const;               // Получить текущий уровень
	int GetLinesCleared() const;               // Получить количество очищенных линий
	int GetTotalPieces() const;                // Получить количество размещенных фигур

	// Информация о времени игры
	DWORD GetGameTime() const;                 // Получить время игры в миллисекундах
	CString GetFormattedGameTime() const;      // Получить отформатированное время игры

	// Обратные вызовы и события
public:
	// Функции обратного вызова (для уведомления UI)
	typedef void (*ScoreChangedCallback)(int newScore, int level, int lines);
	typedef void (*GameStateChangedCallback)(bool gameOver, bool paused);
	typedef void (*LinesClearedCallback)(int linesCount);

	// Установка обратных вызовов
	void SetScoreChangedCallback(ScoreChangedCallback callback);     // Установить callback изменения счета
	void SetGameStateChangedCallback(GameStateChangedCallback callback); // Установить callback изменения состояния
	void SetLinesClearedCallback(LinesClearedCallback callback);     // Установить callback очистки линий

	// Вспомогательные методы
private:
	// Внутренняя логика
	void ProcessInput();                       // Обработка накопленного ввода
	void UpdateGameState();                    // Обновление состояния игры
	void CheckGameEvents();                    // Проверка игровых событий
	void HandleGameOver();                     // Обработка окончания игры
	void HandleLinesCleared(int count);        // Обработка очистки линий

	// Управление таймерами
	void SetTimerInterval(int milliseconds);   // Установить интервал таймера
	int CalculateTimerInterval() const;        // Вычислить интервал таймера по уровню

	// Константы
private:
	// Идентификаторы таймеров
	static const UINT_PTR GAME_TIMER_ID = 1;
	static const UINT_PTR INPUT_TIMER_ID = 2;

	// Настройки по умолчанию
	static const int DEFAULT_TIMER_INTERVAL = 500;  // 500 мс
	static const int FAST_TIMER_INTERVAL = 50;      // 50 мс для быстрого ввода

	// Коды клавиш
	static const UINT KEY_LEFT = VK_LEFT;
	static const UINT KEY_RIGHT = VK_RIGHT;
	static const UINT KEY_DOWN = VK_DOWN;
	static const UINT KEY_UP = VK_UP;           // Поворот
	static const UINT KEY_SPACE = VK_SPACE;     // Быстрое падение
	static const UINT KEY_PAUSE = VK_PAUSE;     // Пауза
	static const UINT KEY_ESC = VK_ESCAPE;      // Выход

	// Данные контроллера
private:
	// Компоненты системы
	TetrisGame* m_pGame;                       // Указатель на игровую логику
	DrawFild* m_pRenderer;                     // Указатель на рендерер
	CWnd* m_pParentWindow;                     // Указатель на родительское окно


	// Состояние контроллера
	bool m_first_game;
	bool m_initialized;                        // Флаг инициализации
	bool m_timerRunning;                       // Флаг работы таймера

	// Настройки игры                      
	bool m_showNextPiece;                      // Показывать следующую фигуру
	bool m_showGhost;                          // Показывать фантомную фигуру
	int m_customSpeed;                         // Пользовательская скорость (0 = авто)

	// Статистика времени
	DWORD m_gameStartTime;                     // Время начала игры
	DWORD m_pausedTime;                        // Время в паузе
	DWORD m_lastPauseStart;                    // Время начала последней паузы

	// Обратные вызовы
	ScoreChangedCallback m_scoreCallback;      // Callback изменения счета
	GameStateChangedCallback m_stateCallback;  // Callback изменения состояния
	LinesClearedCallback m_linesCallback;      // Callback очистки линий

	// Буферизация ввода (для плавного управления)
	bool m_keyStates[256];                     // Состояния клавиш
	DWORD m_lastKeyTime[256];                  // Время последнего нажатия каждой клавиши
	DWORD m_keyRepeatDelay;                    // Задержка повтора клавиш

	// Кэшированные значения для оптимизации
	int m_lastScore;                           // Последний известный счет
	int m_lastLevel;                           // Последний известный уровень
	int m_lastLines;                           // Последнее количество линий
	bool m_wasGameOver;                        // Предыдущее состояние окончания игры
	bool m_wasPaused;                          // Предыдущее состояние паузы
};


