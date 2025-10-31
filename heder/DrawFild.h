#pragma once
#include "TetrisGame.h"

// Форвардные объявления
class TetrisGame;

// Класс-рендер для отображения игрового поля Тетрис
class DrawFild : public CWnd
{
	// Конструкция
public:
	DrawFild();

	// Атрибуты
public:
	// Размеры игрового поля
	static const int FIELD_WIDTH = 10;
	static const int FIELD_HEIGHT = 20;
	static const int BLOCK_SIZE = 25;

protected:
	TetrisGame* m_pGame; // Указатель на игровую логику

	// Операции
public:
	// Методы рендеринга (теперь публичные)
	void DrawField(CDC* pDC, const int field[FIELD_HEIGHT][FIELD_WIDTH]); // Рисование игрового поля
	void DrawBlock(CDC* pDC, int x, int y, COLORREF color); // Рисование блока
	void DrawTetromino(CDC* pDC, Tetromino tetromino); // Рисование фигуры

	// Вспомогательные методы
	void GetFieldCoordinates(int row, int col, int& x, int& y); // Преобразование индексов в координаты
	void SetGame(TetrisGame* pGame); // Привязка к игровой логике

	// Переопределения
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	// Реализация
public:
	virtual ~DrawFild();

protected:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()

private:
	void DrawGrid(CDC* pDC, int startX, int startY); // Рисование сетки
	void DrawBorder(CDC* pDC, int startX, int startY); // Рисование границ поля
};

