#pragma once

#include "framework.h"
#include "Tetris.h"
#include "TetrisDlg.h"
#include "afxdialogex.h"
#include "DrawFild.h"
#include "TetrisController.h"
#include "DatabaseManager.h"
#include "ResultsTableManager.h"

// Форвардные объявления
class DrawFild;
class TetrisGame;
class TetrisController;

// Диалоговое окно CTetrisDlg
class CTetrisDlg : public CDialogEx
{
	// Создание
public:
	CTetrisDlg(CWnd* pParent = nullptr);	// стандартный конструктор
	virtual ~CTetrisDlg(); // Добавлен деструктор

	// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TETRIS_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// поддержка DDX/DDV

	// Игровое состояние (сохранено для совместимости)
	bool m_game_started{ false };
	bool m_main_page{ false };
	bool m_showingResults{ false }; // true - отображаются результаты, false - отображается игра

	// Реализация
protected:
	HICON m_hIcon;

	// Компоненты новой архитектуры
	DrawFild* m_pGameField;          // Рендерер игрового поля
	TetrisGame* m_pGame;             // Игровая логика
	TetrisController* m_pController; // Контроллер игры
	DatabaseManager* m_pDatabase;    // Управление БД результатов
	ResultsTableManager* m_pResultsTable; // Управление таблицей результатов

	// Созданные функции схемы сообщений
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSize(UINT nType, int cx, int cy); // Обработчик изменения размера окна
	afx_msg void OnDestroy(); // Обработчик уничтожения окна
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);

	// Обработчики ввода (новые)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	// Для получения фокуса клавиатуры
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	// Вспомогательные функции
	void CreateGameComponents();     // Создание всех игровых компонентов
	void InitializeGame();          // Инициализация игры
	void CleanupGame();             // Очистка ресурсов игры

	// Callback функции для контроллера
	static void OnScoreChanged(int newScore, int level, int lines);
	static void OnGameStateChanged(bool gameOver, bool paused);
	static void OnLinesCleared(int linesCount);

	// Обновление UI
	void UpdateGameInfo();          // Обновление информации об игре
	void UpdateButtons();           // Обновление состояния кнопок
	void UpdateGameTimeDisplay();   // Обновление отображения времени игры

	// Инфраструктура для обновления счета из callback
	static CTetrisDlg* s_instance;
	static const UINT WM_APP_SCORE_CHANGED;
	static const UINT_PTR TIME_UPDATE_TIMER_ID = 2; // Таймер для обновления времени
	afx_msg LRESULT OnScoreMsg(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

public:
	// Интерфейс для управления игрой
	afx_msg void OnBnClickedResultsandgame();  // Кнопка "Результаты и игра"
	void StartNewGame();            // Начать новую игру
	void PauseResumeGame();         // Пауза/возобновление игры
	void EndCurrentGame();          // Завершить текущую игру
};
