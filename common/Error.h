/*  PCSX2 - PS2 Emulator for PCs
 *  Copyright (C) 2002-2022  PCSX2 Dev Team
 *
 *  PCSX2 is free software: you can redistribute it and/or modify it under the terms
 *  of the GNU Lesser General Public License as published by the Free Software Found-
 *  ation, either version 3 of the License, or (at your option) any later version.
 *
 *  PCSX2 is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 *  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *  PURPOSE.  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with PCSX2.
 *  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once
#include "Pcsx2Defs.h"
#include <string>

class Error
{
public:
	Error();
	Error(const Error& e);
	Error(Error&& e);
	~Error();

	enum class Type
	{
		None = 0,
		Errno = 1,
		Socket = 2,
		User = 3,
		Win32 = 4,
		HResult = 5,
	};

	__fi const Type GetType() const { return m_type; }
	__fi const std::string& GetDescription() const { return m_description; }
	__fi const std::string& GetMessageString() const { return m_message; }

	// setter functions
	void Clear();

	/// Error that is set by system functions, such as open().
	void SetErrno(int err, std::string message = std::string());

	/// Error that is set by socket functions, such as socket(). On Unix this is the same as errno.
	void SetSocket(int err, std::string message = std::string());

	/// Will return 'User Error #'.
	void SetUser(int code, std::string message = std::string());

	/// Set both description and message.
	void SetUser(std::string description, std::string message = std::string());

#ifdef _WIN32
	/// Error that is returned by some Win32 functions, such as RegOpenKeyEx. Also used by other APIs through GetLastError().
	void SetWin32(unsigned long err, std::string message = std::string());

	/// Error that is returned by Win32 COM methods, e.g. S_OK.
	void SetHResult(long err, std::string message = std::string());
#endif

	// constructors
	static Error CreateNone();
	static Error CreateErrno(int err, std::string message = std::string());
	static Error CreateSocket(int err, std::string message = std::string());
	static Error CreateUser(int code, std::string message = std::string());
	static Error CreateUser(std::string description, std::string message = std::string());
#ifdef _WIN32
	static Error CreateWin32(unsigned long err, std::string message = std::string());
	static Error CreateHResult(long err, std::string message = std::string());
#endif

	// helpers for setting
	static void SetErrno(Error* errptr, int err, std::string message = std::string());
	static void SetSocket(Error* errptr, int err, std::string message = std::string());
	static void SetUser(Error* errptr, int code, std::string message = std::string());
	static void SetUser(Error* errptr, std::string description, std::string message = std::string());
	static void SetWin32(Error* errptr, unsigned long err, std::string message = std::string());
	static void SetHResult(Error* errptr, long err, std::string message = std::string());

	// operators
	Error& operator=(const Error& e);
	Error& operator=(Error&& e);
	bool operator==(const Error& e) const;
	bool operator!=(const Error& e) const;

private:
	Type m_type = Type::None;
	std::string m_description;
	std::string m_message;
};
