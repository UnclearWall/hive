/*
* Copyright (C) 2009-2012 Rajko Stojadinovic <http://github.com/rajkosto/hive>
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#pragma once

#include "Shared/Common/Types.h"

#include <Poco/Mutex.h>

class Database;
class QueryResult;
class QueryNamedResult;
class SqlPreparedStatement;
class SqlStmtParameters;

//
class SqlConnection
{
public:
	virtual ~SqlConnection() {}

	//method for initializing DB connection
	virtual bool Initialize(const std::string& infoString) = 0;
	//public methods for making queries
	virtual QueryResult* Query(const char* sql) = 0;
	virtual QueryNamedResult* QueryNamed(const char* sql) = 0;

	//public methods for making requests
	virtual bool Execute(const char* sql) = 0;

	//escape string generation
	virtual unsigned long escape_string(char* to, const char* from, unsigned long length);

	// nothing do if DB not support transactions
	virtual bool BeginTransaction();
	virtual bool CommitTransaction();
	// can't rollback without transaction support
	virtual bool RollbackTransaction();

	//methods to work with prepared statements
	bool ExecuteStmt(int nIndex, const SqlStmtParameters& id);

	//SqlConnection object lock
	class Lock
	{
	public:
		Lock(SqlConnection* conn) : m_pConn(conn) { m_pConn->m_mutex.lock(); }
		~Lock() { m_pConn->m_mutex.unlock(); }

		SqlConnection* operator->() const { return m_pConn; }

	private:
		SqlConnection* const m_pConn;
	};

	//get DB object
	Database& DB() { return m_db; }

protected:
	SqlConnection(Database& db) : m_db(db) {}

	virtual SqlPreparedStatement* CreateStatement(const std::string& fmt);
	//allocate prepared statement and return statement ID
	SqlPreparedStatement* GetStmt(int nIndex);

	Database& m_db;

	//free prepared statements objects
	void FreePreparedStatements();

private:
	typedef Poco::Mutex LOCK_TYPE;
	LOCK_TYPE m_mutex;

	typedef std::vector<SqlPreparedStatement* > StmtHolder;
	StmtHolder m_holder;
};