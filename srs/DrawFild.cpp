#include "pch.h"
#include "DrawFild.h"

// Конструктор
DrawFild::DrawFild()
{
	m_pGame = nullptr; // Инициализация указателя на игру
}

// Деструктор
DrawFild::~DrawFild()
{
}

BEGIN_MESSAGE_MAP(DrawFild, CWnd)
	ON_WM_PAINT()
    ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

BOOL DrawFild::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
    cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
		::LoadCursor(NULL, IDC_ARROW), NULL, NULL);

	return TRUE;
}

void DrawFild::OnPaint()
{
	CPaintDC dc(this);

	CRect rc;
	GetClientRect(&rc);

	// Подготовка бэк-буфера под текущий размер
	if (m_backBmp.GetSafeHandle() == NULL || m_backSize.cx != rc.Width() || m_backSize.cy != rc.Height())
	{
		m_backBmp.DeleteObject();
		CBitmap tmp;
		tmp.CreateCompatibleBitmap(&dc, rc.Width(), rc.Height());
		m_backBmp.Attach((HBITMAP)tmp.Detach());
		m_backSize = CSize(rc.Width(), rc.Height());
	}

	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	CBitmap* pOldBmp = memDC.SelectObject(&m_backBmp);

	// Фон
	memDC.FillSolidRect(&rc, RGB(20, 20, 40));

	// Игровое поле и фигуры
	if (m_pGame != nullptr)
	{
		DrawField(&memDC, m_pGame->GetField());
		DrawTetromino(&memDC, m_pGame->GetCurrentPiece());
	}

	// Выводим буфер на экран
	dc.BitBlt(0, 0, rc.Width(), rc.Height(), &memDC, 0, 0, SRCCOPY);

	memDC.SelectObject(pOldBmp);
}

BOOL DrawFild::OnEraseBkgnd(CDC* /*pDC*/)
{
	// Фон рисуем сами в бэк-буфере
	return TRUE;
}

void DrawFild::DrawField(CDC* pDC, const int field[FIELD_HEIGHT][FIELD_WIDTH])
{
	// Получаем размеры клиентской области
	CRect rect;
	GetClientRect(&rect);

	// Вычисляем начальную позицию для рисования поля по центру
	int startX = (rect.Width() - FIELD_WIDTH * BLOCK_SIZE) / 2;
	int startY = (rect.Height() - FIELD_HEIGHT * BLOCK_SIZE) / 2;

	// Рисуем границу поля
	DrawBorder(pDC, startX, startY);

	// Рисуем сетку
	DrawGrid(pDC, startX, startY);

	// Рисуем заполненные блоки
	for (int row = 0; row < FIELD_HEIGHT; row++)
	{
		for (int col = 0; col < FIELD_WIDTH; col++)
		{
			if (field[row][col] > 0)
			{
				// Выбираем цвет по типу фигуры из значения в поле
				TetrominoType type = (TetrominoType)field[row][col];
				COLORREF color = m_pGame ? m_pGame->GetTetrominoColor(type) : RGB(255, 128, 0);
				DrawBlock(pDC, startX + col * BLOCK_SIZE, startY + row * BLOCK_SIZE, color);
			}
		}
	}
}

void DrawFild::DrawBlock(CDC* pDC, int x, int y, COLORREF color)
{
	// Рисуем блок с градиентом
	CRect blockRect(x + 1, y + 1, x + BLOCK_SIZE - 1, y + BLOCK_SIZE - 1);

	// Основной цвет
	pDC->FillSolidRect(&blockRect, color);

	// Рисуем границы для 3D эффекта
	CPen lightPen(PS_SOLID, 1, RGB(
		GetRValue(color) + 40,
		GetGValue(color) + 40,
		GetBValue(color) + 40));
	CPen darkPen(PS_SOLID, 1, RGB(
		max(0, GetRValue(color) - 40),
		max(0, GetGValue(color) - 40),
		max(0, GetBValue(color) - 40)));

	CPen* pOldPen = pDC->SelectObject(&lightPen);

	// Верхняя и левая границы (свет)
	pDC->MoveTo(x + 1, y + 1);
	pDC->LineTo(x + 1, y + BLOCK_SIZE - 1);
	pDC->LineTo(x + BLOCK_SIZE - 1, y + BLOCK_SIZE - 1);

	pDC->SelectObject(&darkPen);

	// Нижняя и правая границы (темно)
	pDC->MoveTo(x + BLOCK_SIZE - 1, y + BLOCK_SIZE - 1);
	pDC->LineTo(x + BLOCK_SIZE - 1, y + 1);
	pDC->LineTo(x + 1, y + 1);

	pDC->SelectObject(pOldPen);
}

// Рисование сетки
void DrawFild::DrawGrid(CDC* pDC, int startX, int startY)
{
	CPen gridPen(PS_SOLID, 1, RGB(50, 50, 70));
	CPen* pOldPen = pDC->SelectObject(&gridPen);

	// Вертикальные линии
	for (int i = 0; i <= FIELD_WIDTH; i++)
	{
		int x = startX + i * BLOCK_SIZE;
		pDC->MoveTo(x, startY);
		pDC->LineTo(x, startY + FIELD_HEIGHT * BLOCK_SIZE);
	}

	// Горизонтальные линии
	for (int i = 0; i <= FIELD_HEIGHT; i++)
	{
		int y = startY + i * BLOCK_SIZE;
		pDC->MoveTo(startX, y);
		pDC->LineTo(startX + FIELD_WIDTH * BLOCK_SIZE, y);
	}

	pDC->SelectObject(pOldPen);
}

// Рисование границ поля
void DrawFild::DrawBorder(CDC* pDC, int startX, int startY)
{
	CRect fieldRect(startX - 2, startY - 2,
		startX + FIELD_WIDTH * BLOCK_SIZE + 2,
		startY + FIELD_HEIGHT * BLOCK_SIZE + 2);
	pDC->FrameRect(&fieldRect, CBrush::FromHandle((HBRUSH)GetStockObject(WHITE_BRUSH)));
}

// Рисование фигуры (тетромино)
void DrawFild::DrawTetromino(CDC* pDC, Tetromino tetromino)
{
	if (!pDC)
		return;

	// Выбор таблицы смещений по типу фигуры
	const int(*offsets)[4][2] = nullptr;
	switch (tetromino.type)
	{
	case TETROMINO_I: offsets = I_OFFSETS; break;
	case TETROMINO_O: offsets = O_OFFSETS; break;
	case TETROMINO_T: offsets = T_OFFSETS; break;
	case TETROMINO_S: offsets = S_OFFSETS; break;
	case TETROMINO_Z: offsets = Z_OFFSETS; break;
	case TETROMINO_J: offsets = J_OFFSETS; break;
	case TETROMINO_L: offsets = L_OFFSETS; break;
	default: return;
	}

	int rotation = tetromino.rotation & 3;

	// Рисуем 4 блока фигуры
	for (int i = 0; i < 4; ++i)
	{
		int col = tetromino.x + offsets[rotation][i][1];
		int row = tetromino.y + offsets[rotation][i][0];

		// Защита от выхода за поле
		if (row < 0 || row >= FIELD_HEIGHT || col < 0 || col >= FIELD_WIDTH)
			continue;

		int px = 0, py = 0;
		GetFieldCoordinates(row, col, px, py);
		DrawBlock(pDC, px, py, tetromino.color);
	}
}

// Преобразование индексов поля в экранные координаты
void DrawFild::GetFieldCoordinates(int row, int col, int& x, int& y)
{
	CRect rect;
	GetClientRect(&rect);

	int startX = (rect.Width() - FIELD_WIDTH * BLOCK_SIZE) / 2;
	int startY = (rect.Height() - FIELD_HEIGHT * BLOCK_SIZE) / 2;

	x = startX + col * BLOCK_SIZE;
	y = startY + row * BLOCK_SIZE;
}

// Привязка к игровой логике
void DrawFild::SetGame(TetrisGame* pGame)
{
	m_pGame = pGame;
	// Перерисовываем поле при смене игры
	if (GetSafeHwnd())
	{
		Invalidate();
	}
}