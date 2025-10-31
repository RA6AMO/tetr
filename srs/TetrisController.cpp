#include "pch.h"
#include "TetrisController.h"


// Конструктор
TetrisController::TetrisController()
{
	// Инициализация указателей
	m_pGame = nullptr;
	m_pRenderer = nullptr;
	m_pParentWindow = nullptr;

	// Инициализация состояния
	m_first_game = true;
	m_initialized = false;
	m_timerRunning = false;

	// Настройки по умолчанию
	m_showNextPiece = true;
	m_showGhost = false;
	m_customSpeed = 0; // Автоматическая скорость

	// Инициализация статистики времени
	m_gameStartTime = 0;
	m_pausedTime = 0;
	m_lastPauseStart = 0;

	// Инициализация обратных вызовов
	m_scoreCallback = nullptr;
	m_stateCallback = nullptr;
	m_linesCallback = nullptr;

	// Инициализация буферизации ввода
	memset(m_keyStates, 0, sizeof(m_keyStates));
	memset(m_lastKeyTime, 0, sizeof(m_lastKeyTime));
	m_keyRepeatDelay = 70; // 150 мс между повторами

	// Инициализация кэшированных значений
	m_lastScore = 0;
	m_lastLevel = 0;
	m_lastLines = 0;
	m_wasGameOver = false;
	m_wasPaused = false;
}

// Деструктор
TetrisController::~TetrisController()
{
	Shutdown();
}

// ИНИЦИАЛИЗАЦИЯ И НАСТРОЙКА

// Привязать игровую логику
void TetrisController::SetGameLogic(TetrisGame* pGame)
{
	// TODO: Привязать игровую логику
	m_pGame = pGame;
}

// Привязать рендерер
void TetrisController::SetRenderer(DrawFild* pRenderer)
{
	// TODO: Привязать рендерер и настроить связь с игровой логикой
	m_pRenderer = pRenderer;
	if (m_pRenderer && m_pGame)
	{
		m_pRenderer->SetGame(m_pGame);
	}
}

// Привязать родительское окно
void TetrisController::SetParentWindow(CWnd* pParent)
{
	// TODO: Установить родительское окно для таймеров и сообщений
	m_pParentWindow = pParent;
}

// Инициализировать контроллер
bool TetrisController::Initialize()
{
	// TODO: Проверить корректность всех компонентов и инициализировать систему
	if (!m_pGame || !m_pRenderer || !m_pParentWindow)
	{
		return false; // Не все компоненты привязаны
	}

	// Инициализация завершена
	m_initialized = true;
	return true;
}

// Завершить работу контроллера
void TetrisController::Shutdown()
{
	// TODO: Остановить таймеры, очистить ресурсы
	StopGameTimer();
	m_initialized = false;
}

// УПРАВЛЕНИЕ ИГРОЙ

// Начать новую игру
void TetrisController::StartNewGame()
{
	// TODO: Запустить новую игру через игровую логику
	if (!m_initialized || !m_pGame)
		return;

	m_pGame->StartNewGame();
	m_gameStartTime = GetTickCount();
	m_pausedTime = 0;

	StartGameTimer();
	ForceRedraw();

	// Уведомить UI об изменении состояния
	if (m_stateCallback)
	{
		m_stateCallback(false, false); // Игра не окончена, не на паузе
	}
}

// Приостановить игру
void TetrisController::PauseGame()
{
	// TODO: Приостановить игру и таймеры
	if (!m_initialized || !m_pGame)
		return;

	if (!m_pGame->IsGameOver() && !m_pGame->IsGamePaused())
	{
		m_pGame->PauseGame();
		m_lastPauseStart = GetTickCount();
		StopGameTimer();

		// Уведомить UI
		if (m_stateCallback)
		{
			m_stateCallback(false, true); // Игра на паузе
		}
	}
}

// Возобновить игру
void TetrisController::ResumeGame()
{
	// TODO: Возобновить игру и таймеры
	if (!m_initialized || !m_pGame)
		return;

	if (m_pGame->IsGamePaused())
	{
		m_pGame->ResumeGame();

		// Учесть время паузы
		if (m_lastPauseStart > 0)
		{
			m_pausedTime += GetTickCount() - m_lastPauseStart;
			m_lastPauseStart = 0;
		}

		StartGameTimer();
		ForceRedraw();

		// Уведомить UI
		if (m_stateCallback)
		{
			m_stateCallback(false, false); // Игра возобновлена
		}
	}
}

// Завершить игру
void TetrisController::EndGame()
{
	// TODO: Завершить игру и остановить таймеры
	if (!m_initialized || !m_pGame)
		return;

	m_pGame->EndGame();
	StopGameTimer();

	// Уведомить UI
	if (m_stateCallback)
	{
		m_stateCallback(true, false); // Игра окончена
	}
}

// Перезапустить игру
void TetrisController::RestartGame()
{
	// TODO: Перезапустить игру (завершить текущую и начать новую)
	EndGame();
	StartNewGame();
}

// СОСТОЯНИЕ ИГРЫ

// Проверить, запущена ли игра
bool TetrisController::IsGameRunning() const
{
	bool tmp = m_initialized && m_pGame && !m_pGame->IsGameOver();
	return tmp;
}

// Проверить, приостановлена ли игра
bool TetrisController::IsGamePaused() const
{
	bool tmp = m_initialized && m_pGame && m_pGame->IsGamePaused();
	return tmp;
}

// Проверить, окончена ли игра
bool TetrisController::IsGameOver() const
{
	bool  tmp = m_initialized && m_pGame && m_pGame->IsGameOver();
	return tmp;
}

// ОБРАБОТКА ПОЛЬЗОВАТЕЛЬСКОГО ВВОДА

// Обработка нажатия клавиши
void TetrisController::OnKeyDown(UINT nChar)
{
	// TODO: Обработать нажатие клавиши и выполнить соответствующие действия
	if (!m_initialized || !m_pGame)
		return;

	m_keyStates[nChar] = true;
	m_lastKeyTime[nChar] = GetTickCount();

	// Немедленная обработка некоторых клавиш
	switch (nChar)
	{
	case VK_RETURN:
		if (!IsGameRunning() || IsGameOver() || m_first_game)
		{
			if (m_first_game) m_first_game = false;
			StartNewGame();
		}
		break;
	case KEY_PAUSE:
		if (IsGamePaused())
			ResumeGame();
		else
			PauseGame();
		break;

	case KEY_ESC:
		EndGame();
		break;

	case KEY_SPACE:
		if (IsGameRunning() && !IsGamePaused())
		{
			m_pGame->DropPiece();
			ForceRedraw();
		}
		break;

	case KEY_UP:
		if (IsGameRunning() && !IsGamePaused())
		{
			if (m_pGame->RotatePiece())
			{
				ForceRedraw();
			}
		}
		break;

	case KEY_LEFT:
		if (IsGameRunning() && !IsGamePaused())
		{
			if (m_pGame->MovePiece(-1, 0))
			{
				ForceRedraw();
			}
		}
		break;

	case KEY_RIGHT:
		if (IsGameRunning() && !IsGamePaused())
		{
			if (m_pGame->MovePiece(1, 0))
			{
				ForceRedraw();
			}
		}
		break;

	case KEY_DOWN:
		if (IsGameRunning() && !IsGamePaused())
		{
			if (m_pGame->MovePiece(0, 1))
			{
				ForceRedraw();
			}
		}
		break;

		// Остальные клавиши обрабатываются в ProcessInput()
	}
}

// Обработка отпускания клавиши
void TetrisController::OnKeyUp(UINT nChar)
{
	// TODO: Обновить состояние клавиш
	m_keyStates[nChar] = false;
}

// ИГРОВОЙ ЦИКЛ И ОБНОВЛЕНИЯ

// Обновление игрового состояния
void TetrisController::Update()
{
	// TODO: Основной цикл обновления контроллера
	if (!m_initialized || !m_pGame)
		return;

	ProcessInput();        // Обработка накопленного ввода
	UpdateGameState();     // Обновление состояния игры
	CheckGameEvents();     // Проверка игровых событий
	Render();
}

// Отрисовка игрового поля
void TetrisController::Render()
{
	// TODO: Обновить отображение через рендерер
	if (m_pRenderer)
	{
		m_pRenderer->Invalidate();
		m_pRenderer->UpdateWindow();
	}
}

// Принудительная перерисовка
void TetrisController::ForceRedraw()
{
	if (m_pRenderer)
	{
		m_pRenderer->Invalidate();
		m_pRenderer->UpdateWindow();
	}
}

// ТАЙМЕРЫ

// Запустить игровой таймер
void TetrisController::StartGameTimer()
{
	// TODO: Запустить таймер для игрового цикла
	if (m_pParentWindow && !m_timerRunning)
	{
		int interval = CalculateTimerInterval();
		m_pParentWindow->SetTimer(GAME_TIMER_ID, interval, nullptr);
		m_timerRunning = true;
	}
}

// Остановить игровой таймер
void TetrisController::StopGameTimer()
{
	// TODO: Остановить игровой таймер
	if (m_pParentWindow && m_timerRunning)
	{
		m_pParentWindow->KillTimer(GAME_TIMER_ID);
		m_timerRunning = false;
	}
}

// Обработка таймера
void TetrisController::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Обработать событие таймера
	switch (nIDEvent)
	{
	case GAME_TIMER_ID:
		Update();
		break;

	default:
		break;
	}
}

// НАСТРОЙКИ ИГРЫ (заглушки)

void TetrisController::SetGameSpeed(int level) { m_customSpeed = level; }
void TetrisController::SetShowNextPiece(bool show) { m_showNextPiece = show; }
void TetrisController::SetShowGhost(bool show) { m_showGhost = show; }

int TetrisController::GetGameSpeed() const { return m_customSpeed; }
bool TetrisController::IsNextPieceVisible() const { return m_showNextPiece; }
bool TetrisController::IsGhostVisible() const { return m_showGhost; }

// СТАТИСТИКА И ИНФОРМАЦИЯ

int TetrisController::GetCurrentScore() const
{
	return m_pGame ? m_pGame->GetScore() : 0;
}

int TetrisController::GetCurrentLevel() const
{
	return m_pGame ? m_pGame->GetLevel() : 1;
}

int TetrisController::GetLinesCleared() const
{
	return m_pGame ? m_pGame->GetLinesCleared() : 0;
}

int TetrisController::GetTotalPieces() const
{
	// TODO: Добавить подсчет общего количества фигур в TetrisGame
	return 0; // Заглушка
}

DWORD TetrisController::GetGameTime() const
{
	if (m_gameStartTime == 0)
		return 0;

	DWORD currentTime = GetTickCount();
	DWORD totalTime = currentTime - m_gameStartTime - m_pausedTime;

	// Если игра на паузе, вычесть текущее время паузы
	if (m_lastPauseStart > 0)
	{
		totalTime -= (currentTime - m_lastPauseStart);
	}

	return totalTime;
}

CString TetrisController::GetFormattedGameTime() const
{
	// TODO: Отформатировать время в виде MM:SS
	DWORD totalMs = GetGameTime();
	int totalSeconds = totalMs / 1000;
	int minutes = totalSeconds / 60;
	int seconds = totalSeconds % 60;

	CString result;
	result.Format(_T("%02d:%02d"), minutes, seconds);
	return result;
}

// ОБРАТНЫЕ ВЫЗОВЫ

void TetrisController::SetScoreChangedCallback(ScoreChangedCallback callback)
{
	m_scoreCallback = callback;
}

void TetrisController::SetGameStateChangedCallback(GameStateChangedCallback callback)
{
	m_stateCallback = callback;
}

void TetrisController::SetLinesClearedCallback(LinesClearedCallback callback)
{
	m_linesCallback = callback;
}

// ПРИВАТНЫЕ МЕТОДЫ (заглушки)

// Обработка накопленного ввода
void TetrisController::ProcessInput()
{
	// TODO: Обработать удерживаемые клавиши с учетом времени повтора
	if (!IsGameRunning() || IsGamePaused())
		return;

	DWORD currentTime = GetTickCount();

	// Перемещения по стрелкам перенесены в OnKeyDown для немедленной реакции
}

// Обновление состояния игры
void TetrisController::UpdateGameState()
{
	// TODO: Обновить игровую логику
	if (m_pGame)
	{
		m_pGame->Update();
	}
}

// Проверка игровых событий
void TetrisController::CheckGameEvents()
{
	// TODO: Проверить изменения в игровом состоянии и вызвать соответствующие callback'и
	if (!m_pGame)
		return;

	// Проверка изменения счета
	int currentScore = m_pGame->GetScore();
	int currentLevel = m_pGame->GetLevel();
	int currentLines = m_pGame->GetLinesCleared();

	if (currentScore != m_lastScore || currentLevel != m_lastLevel || currentLines != m_lastLines)
	{
		if (m_scoreCallback)
		{
			m_scoreCallback(currentScore, currentLevel, currentLines);
		}

		m_lastScore = currentScore;
		m_lastLevel = currentLevel;
		m_lastLines = currentLines;
	}

	// Проверка изменения состояния игры
	bool gameOver = m_pGame->IsGameOver();
	bool paused = m_pGame->IsGamePaused();

	if (gameOver != m_wasGameOver || paused != m_wasPaused)
	{
		if (m_stateCallback)
		{
			m_stateCallback(gameOver, paused);
		}

		if (gameOver && !m_wasGameOver)
		{
			HandleGameOver();
		}

		m_wasGameOver = gameOver;
		m_wasPaused = paused;
	}
}

// Обработка окончания игры
void TetrisController::HandleGameOver()
{
	// TODO: Выполнить действия при окончании игры
	StopGameTimer();
}

// Обработка очистки линий
void TetrisController::HandleLinesCleared(int count)
{
	// TODO: Обработать очистку линий (звуки, эффекты)
	if (count > 0)
	{

		if (m_linesCallback)
		{
			m_linesCallback(count);
		}
	}
}

// Установить интервал таймера
void TetrisController::SetTimerInterval(int milliseconds)
{
	if (m_timerRunning)
	{
		StopGameTimer();
		m_pParentWindow->SetTimer(GAME_TIMER_ID, milliseconds, nullptr);
		m_timerRunning = true;
	}
}

// Вычислить интервал таймера по уровню
int TetrisController::CalculateTimerInterval() const
{
	if (m_customSpeed > 0)
		return m_customSpeed;

	// Базируется на уровне игры
	int level = m_pGame ? m_pGame->GetLevel() : 1;
	return max(50, DEFAULT_TIMER_INTERVAL - (level - 1) * 30);
}
