#pragma once

#include "pch.h"
#include "DatabaseManager.h"
#include <vector>

// Управление таблицей результатов (CListCtrl в режиме Report)
class ResultsTableManager
{
public:
	ResultsTableManager();
	~ResultsTableManager();

	bool Initialize(CListCtrl* pListCtrl);
	void UpdateTable(DatabaseManager* pDatabase);
	void SetVisible(bool visible);

private:
	CListCtrl* m_pList;

	CString FormatTimeMs(DWORD ms) const;
};


