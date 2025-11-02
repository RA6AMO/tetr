#include "pch.h"
#include "TetrisDlg.h"
// TetrisDlg.cpp: файл реализации
//

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Диалоговое окно CAboutDlg используется для описания сведений о приложении

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // поддержка DDX/DDV

	// Реализация
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// Диалоговое окно CTetrisDlg



// Статические члены для сообщений об обновлении счета
CTetrisDlg* CTetrisDlg::s_instance = nullptr;
const UINT CTetrisDlg::WM_APP_SCORE_CHANGED = WM_APP + 1;

CTetrisDlg::CTetrisDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TETRIS_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	// Инициализация компонентов новой архитектуры
	m_pGameField = nullptr;
	m_pGame = nullptr;
	m_pController = nullptr;
	m_pDatabase = nullptr;
	m_pResultsTable = nullptr;
    m_showingResults = false;
}

CTetrisDlg::~CTetrisDlg()
{
	CleanupGame();
}

void CTetrisDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTetrisDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_HELPINFO()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_RESULTSANDGAME, &CTetrisDlg::OnBnClickedResultsandgame)
	ON_MESSAGE(WM_APP_SCORE_CHANGED, &CTetrisDlg::OnScoreMsg)
END_MESSAGE_MAP()


// Обработчики сообщений CTetrisDlg

BOOL CTetrisDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Добавление пункта "О программе..." в системное меню.

	// IDM_ABOUTBOX должен быть в пределах системной команды.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Задает значок для этого диалогового окна.  Среда делает это автоматически,
	//  если главное окно приложения не является диалоговым
	SetIcon(m_hIcon, TRUE);			// Крупный значок
	SetIcon(m_hIcon, FALSE);		// Мелкий значок

	// TODO: добавьте дополнительную инициализацию

	// Зарегистрировать текущий экземпляр для статических callback'ов
	s_instance = this;

	ModifyStyle(WS_THICKFRAME, 0, SWP_FRAMECHANGED);

	// Создаем компоненты игры Тетрис
	CreateGameComponents();
	InitializeGame();
	UpdateGameInfo();

	// Инициализация БД и таблицы результатов
	// Скрыть таблицу по умолчанию
	if (CWnd* pListWnd = GetDlgItem(IDC_LIST_TABLE))
	{
		pListWnd->ShowWindow(SW_HIDE);
	}

	// Создать и инициализировать менеджеры
	if (!m_pDatabase)
	{
		m_pDatabase = new DatabaseManager();
		m_pDatabase->Initialize();
	}
	if (!m_pResultsTable)
	{
		m_pResultsTable = new ResultsTableManager();
		CListCtrl* pList = static_cast<CListCtrl*>(GetDlgItem(IDC_LIST_TABLE));
		if (pList)
		{
			m_pResultsTable->Initialize(pList);
		}
	}

    // Начальный текст кнопки: показывать результаты при нажатии
    SetDlgItemText(IDC_RESULTSANDGAME, L"Результаты");

	// Запустить таймер обновления времени (1 секунда)
	SetTimer(TIME_UPDATE_TIMER_ID, 1000, nullptr);

	return TRUE;  // возврат значения TRUE, если фокус не передан элементу управления
}

void CTetrisDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	/*
		// Изменяем размер игрового поля при изменении размера диалога
		if (m_pGameField != nullptr && IsWindow(m_pGameField->GetSafeHwnd()))
		{
			// Оставляем отступы от краев диалога
			int margin = 10;
			m_pGameField->SetWindowPos(nullptr, margin, margin,
				cx - 2 * margin, cy - 2 * margin,
				SWP_NOZORDER | SWP_SHOWWINDOW);
		}*/
}

void CTetrisDlg::OnDestroy()
{
	// Сбросить указатель экземпляра, чтобы callback'и не обращались к уничтоженному окну
	s_instance = nullptr;

	// Остановить таймер обновления времени
	KillTimer(TIME_UPDATE_TIMER_ID);

	CleanupGame();

	// Освободить менеджеры БД и таблицы
	if (m_pResultsTable)
	{
		delete m_pResultsTable;
		m_pResultsTable = nullptr;
	}
	if (m_pDatabase)
	{
		m_pDatabase->Close();
		delete m_pDatabase;
		m_pDatabase = nullptr;
	}
	CDialogEx::OnDestroy();
}

// Создание всех игровых компонентов
void CTetrisDlg::CreateGameComponents()
{
	// Проверяем, что компоненты еще не созданы
	if (m_pGameField != nullptr || m_pGame != nullptr || m_pController != nullptr)
		return;

	try
	{
		// 1. Создаем игровую логику
		m_pGame = new TetrisGame();

		// 2. Создаем рендерер (игровое поле)
		m_pGameField = new DrawFild();

		// Получаем размеры клиентской области диалога
		CRect rect;
		GetClientRect(&rect);

		// Оставляем отступы от краев
		int marginTopBot = 30;
		int marginLeftRight = 100;

		// Создаем окно игрового поля
		if (!m_pGameField->Create(nullptr, nullptr,
			WS_CHILD | WS_VISIBLE | WS_BORDER,
			CRect(10, 0, rect.Width() - marginLeftRight, rect.Height() - marginTopBot),
			this, 1000))
		{
			throw CString(_T("Не удалось создать игровое поле"));
		}

		// 3. Создаем контроллер
		m_pController = new TetrisController();

		// 4. Связываем компоненты
		m_pController->SetGameLogic(m_pGame);
		m_pController->SetRenderer(m_pGameField);
		m_pController->SetParentWindow(this);

		// 5. Устанавливаем callback функции
		m_pController->SetScoreChangedCallback(OnScoreChanged);
		m_pController->SetGameStateChangedCallback(OnGameStateChanged);
		m_pController->SetLinesClearedCallback(OnLinesCleared);
		//m_pController->EndGame();
	}
	catch (const CString& error)
	{
		AfxMessageBox(error);
		CleanupGame();
	}
	catch (...)
	{
		AfxMessageBox(_T("Ошибка при создании игровых компонентов"));
		CleanupGame();
	}
}

// Инициализация игры
void CTetrisDlg::InitializeGame()
{
	if (m_pController != nullptr)
	{
		if (!m_pController->Initialize())
		{
			AfxMessageBox(_T("Ошибка инициализации контроллера игры"));
		}
	}
}

// Очистка ресурсов игры
void CTetrisDlg::CleanupGame()
{
	// Останавливаем контроллер
	if (m_pController != nullptr)
	{
		m_pController->Shutdown();
		delete m_pController;
		m_pController = nullptr;
	}

	// Удаляем игровое поле
	if (m_pGameField != nullptr)
	{
		if (IsWindow(m_pGameField->GetSafeHwnd()))
			m_pGameField->DestroyWindow();
		delete m_pGameField;
		m_pGameField = nullptr;
	}

	// Удаляем игровую логику
	if (m_pGame != nullptr)
	{
		delete m_pGame;
		m_pGame = nullptr;
	}

	// Освободить менеджеры результатов и БД
	if (m_pResultsTable)
	{
		delete m_pResultsTable;
		m_pResultsTable = nullptr;
	}
	if (m_pDatabase)
	{
		m_pDatabase->Close();
		delete m_pDatabase;
		m_pDatabase = nullptr;
	}
}

void CTetrisDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// При добавлении кнопки свертывания в диалоговое окно нужно воспользоваться приведенным ниже кодом,
//  чтобы нарисовать значок.  Для приложений MFC, использующих модель документов или представлений,
//  это автоматически выполняется рабочей областью.

void CTetrisDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // контекст устройства для рисования

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Выравнивание значка по центру клиентского прямоугольника
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Нарисуйте значок
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// Система вызывает эту функцию для получения отображения курсора при перемещении
//  свернутого окна.
HCURSOR CTetrisDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CTetrisDlg::OnHelpInfo(HELPINFO* pHelpInfo)
{
	// Отключаем стандартную справку по F1
	return TRUE;
}



// НОВЫЕ ОБРАБОТЧИКИ ВВОДА

// Обработка нажатия клавиши
void CTetrisDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (m_pController != nullptr)
	{
		m_pController->OnKeyDown(nChar);
	}
	CDialogEx::OnKeyDown(nChar, nRepCnt, nFlags);
}

// Обработка отпускания клавиши
void CTetrisDlg::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (m_pController != nullptr)
	{
		m_pController->OnKeyUp(nChar);
	}
	CDialogEx::OnKeyUp(nChar, nRepCnt, nFlags);
}

// Обработка таймера
void CTetrisDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == TIME_UPDATE_TIMER_ID)
	{
		UpdateGameTimeDisplay();
	}
	else if (m_pController != nullptr)
	{
		m_pController->OnTimer(nIDEvent);
	}
	CDialogEx::OnTimer(nIDEvent);
}

// Перехват сообщений для обработки клавиатуры
BOOL CTetrisDlg::PreTranslateMessage(MSG* pMsg)
{
	// Обрабатываем клавиатурные сообщения для игры
	if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_KEYUP)
	{
		if (m_pController != nullptr)
		{
			if (pMsg->message == WM_KEYDOWN)
				m_pController->OnKeyDown((UINT)pMsg->wParam);
			else
				m_pController->OnKeyUp((UINT)pMsg->wParam);
		}
		return TRUE; // Сообщение обработано
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

// CALLBACK ФУНКЦИИ ДЛЯ КОНТРОЛЛЕРА

// Callback изменения счета (статическая функция)
void CTetrisDlg::OnScoreChanged(int newScore, int level, int lines)
{
	// Отправить сообщение в окно диалога для безопасного обновления UI
	if (s_instance && ::IsWindow(s_instance->m_hWnd))
	{
		s_instance->PostMessage(WM_APP_SCORE_CHANGED, (WPARAM)newScore, MAKELPARAM(level, lines));
	}
}

// Callback изменения состояния игры (статическая функция)
void CTetrisDlg::OnGameStateChanged(bool gameOver, bool paused)
{
	// Сохранить результат при окончании игры
	if (s_instance && ::IsWindow(s_instance->m_hWnd))
	{
		if (gameOver && s_instance->m_pController)
		{
			int score = s_instance->m_pController->GetCurrentScore();
			int level = s_instance->m_pController->GetCurrentLevel();
			int lines = s_instance->m_pController->GetLinesCleared();
			DWORD timeMs = s_instance->m_pController->GetGameTime();
			if (s_instance->m_pDatabase)
			{
				s_instance->m_pDatabase->SaveResult(score, level, lines, timeMs);
			}
		}
	}
}

// Callback очистки линий (статическая функция)
void CTetrisDlg::OnLinesCleared(int linesCount)
{
	// TODO: Показать эффект очистки линий
	// Воспроизвести звук, показать анимацию и т.д.
}

// МЕТОДЫ УПРАВЛЕНИЯ ИГРОЙ

// Начать новую игру
void CTetrisDlg::StartNewGame()
{
	if (m_pController != nullptr)
	{
		m_pController->StartNewGame();
		m_game_started = true;
		UpdateButtons();
	}
}

// Пауза/возобновление игры
void CTetrisDlg::PauseResumeGame()
{
	if (m_pController != nullptr)
	{
		if (m_pController->IsGamePaused())
		{
			m_pController->ResumeGame();
		}
		else if (m_pController->IsGameRunning())
		{
			m_pController->PauseGame();
		}
		UpdateButtons();
	}
}

// Завершить текущую игру
void CTetrisDlg::EndCurrentGame()
{
	if (m_pController != nullptr)
	{
		m_pController->EndGame();
		m_game_started = false;
		UpdateButtons();
	}
}

// ОБНОВЛЕНИЕ UI

// Обновление информации об игре
void CTetrisDlg::UpdateGameInfo()
{
	// TODO: Обновить отображение счета, уровня, времени игры
	if (m_pController != nullptr)
	{
		int score = m_pController->GetCurrentScore();
		int level = m_pController->GetCurrentLevel();
		int lines = m_pController->GetLinesCleared();
		CString gameTime = m_pController->GetFormattedGameTime();

		// Обновить соответствующие элементы UI
		SetDlgItemInt(IDC_SCORE_LABLE, score, FALSE);
		SetDlgItemText(IDC_TIME_SKORE, gameTime);
	}
}

void CTetrisDlg::UpdateGameTimeDisplay()
{
	if (m_pController != nullptr)
	{
		if (m_pController->IsGameRunning() && !m_pController->IsGamePaused())
		{
			CString gameTime = m_pController->GetFormattedGameTime();
			SetDlgItemText(IDC_TIME_SKORE, gameTime);
		}
	}
}

// Обновление состояния кнопок
void CTetrisDlg::UpdateButtons()
{
	// TODO: Включить/выключить кнопки в зависимости от состояния игры
	if (m_pController != nullptr)
	{
		bool gameRunning = m_pController->IsGameRunning();
		bool gamePaused = m_pController->IsGamePaused();
		bool gameOver = m_pController->IsGameOver();

		// Обновить состояние кнопок "Старт", "Пауза", "Стоп" и т.д.
		// GetDlgItem(IDC_BUTTON)->EnableWindow(gameRunning);
	}
}

// ОБРАБОТЧИК КНОПКИ

void CTetrisDlg::OnBnClickedResultsandgame()
{
    // Переключение между игрой и результатами работает только когда игра не запущена
    if (m_pController == nullptr)
        return;

    if (m_pController->IsGameRunning())
    {
        // Игровой процесс идёт — не переключаем режим
        return;
    }

    if (m_showingResults)
    {
        // Переключаемся на игру
        if (m_pResultsTable)
        {
            m_pResultsTable->SetVisible(false);
        }
        if (m_pGameField && ::IsWindow(m_pGameField->GetSafeHwnd()))
        {
            m_pGameField->ShowWindow(SW_SHOW);
        }
        // Показываем элементы игры
        if (CWnd* pWnd = GetDlgItem(IDC_STATIC))
            pWnd->ShowWindow(SW_SHOW);
        if (CWnd* pWnd = GetDlgItem(IDC_SCORE_LABLE))
            pWnd->ShowWindow(SW_SHOW);
        if (CWnd* pWnd = GetDlgItem(IDC_TIME_SKORE))
            pWnd->ShowWindow(SW_SHOW);
        if (CWnd* pWnd = GetDlgItem(IDC_STATIC2))
            pWnd->ShowWindow(SW_SHOW);
        SetDlgItemText(IDC_RESULTSANDGAME, L"Результаты");
        m_showingResults = false;
    }
    else
    {
        // Переключаемся на результаты
        if (m_pGameField && ::IsWindow(m_pGameField->GetSafeHwnd()))
        {
            m_pGameField->ShowWindow(SW_HIDE);
        }
        if (m_pResultsTable)
        {
            m_pResultsTable->SetVisible(true);
            if (m_pDatabase)
            {
                m_pResultsTable->UpdateTable(m_pDatabase);
            }
        }
        // Скрываем элементы игры
        if (CWnd* pWnd = GetDlgItem(IDC_STATIC))
            pWnd->ShowWindow(SW_HIDE);
        if (CWnd* pWnd = GetDlgItem(IDC_SCORE_LABLE))
            pWnd->ShowWindow(SW_HIDE);
        if (CWnd* pWnd = GetDlgItem(IDC_TIME_SKORE))
            pWnd->ShowWindow(SW_HIDE);
        if (CWnd* pWnd = GetDlgItem(IDC_STATIC2))
            pWnd->ShowWindow(SW_HIDE);
        SetDlgItemText(IDC_RESULTSANDGAME, L"Игра");
        m_showingResults = true;
    }
}

// Обработчик пользовательского сообщения об изменении счета
LRESULT CTetrisDlg::OnScoreMsg(WPARAM wParam, LPARAM lParam)
{
	int newScore = static_cast<int>(wParam);
	int level = LOWORD(lParam);
	int lines = HIWORD(lParam);

	UNREFERENCED_PARAMETER(level);
	UNREFERENCED_PARAMETER(lines);

	SetDlgItemInt(IDC_SCORE_LABLE, newScore, FALSE);
	return 0;
}
