#pragma once

#include "pch.h"
#include "sqlite3.h"
#include <vector>

// Структура результата игры
struct GameResult
{
	int id;
	int score;
	int level;
	int lines;
	DWORD gameTimeMs;
	CString dateTime;
};

// Класс для управления соединением с SQLite и операциями с результатами игр
class DatabaseManager
{
public:
	DatabaseManager();
	~DatabaseManager();

	bool Initialize(const char* databaseFileName = "tetris_results.db");
	void Close();

	bool SaveResult(int score, int level, int lines, DWORD gameTimeMs);
	bool GetTopResults(std::vector<GameResult>& outResults, int count = 10);
	bool GetLastResult(GameResult& outResult);

private:
	sqlite3* m_db;
	bool m_initialized;
	CString m_lastError;

	bool ExecuteSimple(const char* sql);
	bool EnsureSchema();
};


