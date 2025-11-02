#include "pch.h"
#include "ResultsTableManager.h"

ResultsTableManager::ResultsTableManager()
	: m_pList(nullptr)
{
}

ResultsTableManager::~ResultsTableManager()
{
}

bool ResultsTableManager::Initialize(CListCtrl* pListCtrl)
{
	if (!pListCtrl || !::IsWindow(pListCtrl->GetSafeHwnd()))
		return false;

	m_pList = pListCtrl;

	// Расширенные стили
	DWORD exStyle = m_pList->GetExtendedStyle();
	m_pList->SetExtendedStyle(exStyle | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	// Очистить существующие колонки/строки
	int colCount = m_pList->GetHeaderCtrl() ? m_pList->GetHeaderCtrl()->GetItemCount() : 0;
	for (int i = colCount - 1; i >= 0; --i) {
		m_pList->DeleteColumn(i);
	}
	m_pList->DeleteAllItems();

    // Добавить колонки: Место | Очки | Уровень | Линии | Время | Дата
    m_pList->InsertColumn(0, L"Место", LVCFMT_LEFT, 70);
    m_pList->InsertColumn(1, L"Очки", LVCFMT_LEFT, 90);
    m_pList->InsertColumn(2, L"Уровень", LVCFMT_LEFT, 80);
    m_pList->InsertColumn(3, L"Линии", LVCFMT_LEFT, 80);
    m_pList->InsertColumn(4, L"Время", LVCFMT_LEFT, 80);
    m_pList->InsertColumn(5, L"Дата", LVCFMT_LEFT, 150);

	return true;
}

void ResultsTableManager::UpdateTable(DatabaseManager* pDatabase)
{
	if (!m_pList || !::IsWindow(m_pList->GetSafeHwnd()) || !pDatabase)
		return;

	m_pList->DeleteAllItems();

	// Получить последний результат
	GameResult lastResult{};
	bool hasLast = pDatabase->GetLastResult(lastResult);

	int rowIndex = 0;
    if (hasLast)
	{
        CString placeLabel = L"Последний";
		rowIndex = m_pList->InsertItem(rowIndex, placeLabel);
		CString scoreStr; scoreStr.Format(_T("%d"), lastResult.score);
		CString levelStr; levelStr.Format(_T("%d"), lastResult.level);
		CString linesStr; linesStr.Format(_T("%d"), lastResult.lines);
		CString timeStr = FormatTimeMs(lastResult.gameTimeMs);
		m_pList->SetItemText(rowIndex, 1, scoreStr);
		m_pList->SetItemText(rowIndex, 2, levelStr);
		m_pList->SetItemText(rowIndex, 3, linesStr);
		m_pList->SetItemText(rowIndex, 4, timeStr);
		m_pList->SetItemText(rowIndex, 5, lastResult.dateTime);
		++rowIndex;
	}

	// Получить топ-10
	std::vector<GameResult> topResults;
	pDatabase->GetTopResults(topResults, 10);
	for (size_t i = 0; i < topResults.size(); ++i)
	{
		const GameResult& r = topResults[i];
		CString placeStr; placeStr.Format(_T("%d"), static_cast<int>(i + 1));
		int idx = m_pList->InsertItem(rowIndex, placeStr);
		CString scoreStr; scoreStr.Format(_T("%d"), r.score);
		CString levelStr; levelStr.Format(_T("%d"), r.level);
		CString linesStr; linesStr.Format(_T("%d"), r.lines);
		CString timeStr = FormatTimeMs(r.gameTimeMs);
		m_pList->SetItemText(idx, 1, scoreStr);
		m_pList->SetItemText(idx, 2, levelStr);
		m_pList->SetItemText(idx, 3, linesStr);
		m_pList->SetItemText(idx, 4, timeStr);
		m_pList->SetItemText(idx, 5, r.dateTime);
		++rowIndex;
	}
}

void ResultsTableManager::SetVisible(bool visible)
{
	if (m_pList && ::IsWindow(m_pList->GetSafeHwnd()))
	{
		m_pList->ShowWindow(visible ? SW_SHOW : SW_HIDE);
	}
}

CString ResultsTableManager::FormatTimeMs(DWORD ms) const
{
	int totalSeconds = static_cast<int>(ms / 1000);
	int minutes = totalSeconds / 60;
	int seconds = totalSeconds % 60;
	CString s;
	s.Format(_T("%02d:%02d"), minutes, seconds);
	return s;
}


