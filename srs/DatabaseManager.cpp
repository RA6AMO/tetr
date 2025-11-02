#include "pch.h"
#include "DatabaseManager.h"
#include <string>

namespace {
	static CString Utf8ToCString(const char* utf8)
	{
		if (!utf8)
			return CString();
		int wlen = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, nullptr, 0);
		if (wlen <= 0)
			return CString();
		CString result;
		LPWSTR buf = result.GetBuffer(wlen);
		MultiByteToWideChar(CP_UTF8, 0, utf8, -1, buf, wlen);
		result.ReleaseBuffer();
		return result;
	}
}

DatabaseManager::DatabaseManager()
	: m_db(nullptr)
	, m_initialized(false)
{
}

DatabaseManager::~DatabaseManager()
{
	Close();
}

bool DatabaseManager::Initialize(const char* databaseFileName)
{
	if (m_initialized)
		return true;

	int rc = sqlite3_open(databaseFileName, &m_db);
	if (rc != SQLITE_OK)
	{
		m_lastError = Utf8ToCString(sqlite3_errmsg(m_db));
		Close();
		return false;
	}

	if (!EnsureSchema())
	{
		Close();
		return false;
	}

	m_initialized = true;
	return true;
}

void DatabaseManager::Close()
{
	if (m_db)
	{
		sqlite3_close(m_db);
		m_db = nullptr;
	}
	m_initialized = false;
}

bool DatabaseManager::ExecuteSimple(const char* sql)
{
	char* errMsg = nullptr;
	int rc = sqlite3_exec(m_db, sql, nullptr, nullptr, &errMsg);
	if (rc != SQLITE_OK)
	{
		m_lastError = Utf8ToCString(errMsg ? errMsg : "");
		sqlite3_free(errMsg);
		return false;
	}
	return true;
}

bool DatabaseManager::EnsureSchema()
{
	static const char* createSql =
		"CREATE TABLE IF NOT EXISTS game_results ("
		"  id INTEGER PRIMARY KEY AUTOINCREMENT,"
		"  score INTEGER NOT NULL,"
		"  level INTEGER NOT NULL,"
		"  lines INTEGER NOT NULL,"
		"  game_time_ms INTEGER NOT NULL,"
		"  date_time TEXT NOT NULL DEFAULT (datetime('now','localtime'))"
		");";
	return ExecuteSimple(createSql);
}

bool DatabaseManager::SaveResult(int score, int level, int lines, DWORD gameTimeMs)
{
	if (!m_initialized || !m_db)
		return false;

	static const char* insertSql =
		"INSERT INTO game_results(score, level, lines, game_time_ms, date_time)"
		" VALUES(?, ?, ?, ?, datetime('now','localtime'));";

	sqlite3_stmt* stmt = nullptr;
	int rc = sqlite3_prepare_v2(m_db, insertSql, -1, &stmt, nullptr);
	if (rc != SQLITE_OK)
	{
		m_lastError = Utf8ToCString(sqlite3_errmsg(m_db));
		return false;
	}

	sqlite3_bind_int(stmt, 1, score);
	sqlite3_bind_int(stmt, 2, level);
	sqlite3_bind_int(stmt, 3, lines);
	sqlite3_bind_int(stmt, 4, static_cast<int>(gameTimeMs));

	rc = sqlite3_step(stmt);
	sqlite3_finalize(stmt);
	if (rc != SQLITE_DONE)
	{
		m_lastError = Utf8ToCString(sqlite3_errmsg(m_db));
		return false;
	}

	return true;
}

bool DatabaseManager::GetTopResults(std::vector<GameResult>& outResults, int count)
{
	outResults.clear();
	if (!m_initialized || !m_db)
		return false;

	static const char* selectSql =
		"SELECT id, score, level, lines, game_time_ms, date_time "
		"FROM game_results "
		"ORDER BY score DESC, id ASC "
		"LIMIT ?;";

	sqlite3_stmt* stmt = nullptr;
	int rc = sqlite3_prepare_v2(m_db, selectSql, -1, &stmt, nullptr);
	if (rc != SQLITE_OK)
	{
		m_lastError = Utf8ToCString(sqlite3_errmsg(m_db));
		return false;
	}

	sqlite3_bind_int(stmt, 1, count);

	while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
	{
		GameResult r{};
		r.id = sqlite3_column_int(stmt, 0);
		r.score = sqlite3_column_int(stmt, 1);
		r.level = sqlite3_column_int(stmt, 2);
		r.lines = sqlite3_column_int(stmt, 3);
		r.gameTimeMs = static_cast<DWORD>(sqlite3_column_int(stmt, 4));
		r.dateTime = Utf8ToCString(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5)));
		outResults.push_back(r);
	}

	sqlite3_finalize(stmt);
	return rc == SQLITE_DONE;
}

bool DatabaseManager::GetLastResult(GameResult& outResult)
{
	if (!m_initialized || !m_db)
		return false;

	static const char* selectSql =
		"SELECT id, score, level, lines, game_time_ms, date_time "
		"FROM game_results "
		"ORDER BY id DESC LIMIT 1;";

	sqlite3_stmt* stmt = nullptr;
	int rc = sqlite3_prepare_v2(m_db, selectSql, -1, &stmt, nullptr);
	if (rc != SQLITE_OK)
	{
		m_lastError = Utf8ToCString(sqlite3_errmsg(m_db));
		return false;
	}

	rc = sqlite3_step(stmt);
	if (rc == SQLITE_ROW)
	{
		outResult = GameResult{};
		outResult.id = sqlite3_column_int(stmt, 0);
		outResult.score = sqlite3_column_int(stmt, 1);
		outResult.level = sqlite3_column_int(stmt, 2);
		outResult.lines = sqlite3_column_int(stmt, 3);
		outResult.gameTimeMs = static_cast<DWORD>(sqlite3_column_int(stmt, 4));
		outResult.dateTime = Utf8ToCString(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5)));
		sqlite3_finalize(stmt);
		return true;
	}

	sqlite3_finalize(stmt);
	return false;
}


