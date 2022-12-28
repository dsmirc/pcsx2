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

#include "PrecompiledHeader.h"
#include "Error.h"
#include <cstdlib>
#include <cstring>
#include <type_traits>

#include "fmt/format.h"

// Platform-specific includes
#if defined(_WIN32)
#include "RedtapeWindows.h"
static_assert(std::is_same<DWORD, unsigned long>::value, "DWORD is unsigned long");
static_assert(std::is_same<HRESULT, long>::value, "HRESULT is long");
#endif

Error::Error() = default;

Error::Error(const Error& c) = default;

Error::Error(Error&& e) = default;

Error::~Error() = default;

void Error::Clear()
{
	m_description = {};
	m_message = {};
}

void Error::SetErrno(int err, std::string message /* = std::string() */)
{
	m_type = Type::Errno;

#ifdef _MSC_VER
	char buf[128];
	if (strerror_s(buf, sizeof(buf), err) != 0)
		m_description = fmt::format("errno {}: {}", err, buf);
	else
		m_description = fmt::format("errno {}: <Could not get error message>", err);
#else
	const char* buf = std::strerror(err);
	if (buf)
		m_description = fmt::format("errno {}: {}", err, buf);
	else
		m_description = fmt::format("errno {}: <Could not get error message>", err);
#endif
	m_message = std::move(message);
}

void Error::SetErrno(Error* errptr, int err, std::string message /*= std::string()*/)
{
	if (errptr)
		errptr->SetErrno(err, std::move(message));
}

void Error::SetUser(int code, std::string message /* = std::string() */)
{
	m_type = Type::User;
	m_description = fmt::format("User Error #{}", code);
	m_message = std::move(message);
}

void Error::SetUser(std::string description, std::string message /* = std::string() */)
{
	m_type = Type::User;
	m_description = std::move(description);
	m_message = std::move(message);
}

void Error::SetUser(Error* errptr, int code, std::string message /*= std::string()*/)
{
	if (errptr)
		errptr->SetUser(code, std::move(message));
}

void Error::SetUser(Error* errptr, std::string description, std::string message /*= std::string()*/)
{
	if (errptr)
		errptr->SetUser(std::move(description), std::move(message));
}

#ifdef _WIN32
void Error::SetWin32(unsigned long err, std::string message /* = std::string() */)
{
	m_type = Type::Win32;

	char buf[128];
	const DWORD r = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, err, 0, buf, sizeof(buf), nullptr);
	if (r > 0)
		m_description = fmt::format("Win32 Error {}: {}", err, std::string_view(buf, r));
	else
		m_description = fmt::format("Win32 Error {}: <Could not resolve system error ID>");

	m_message = std::move(message);
}

void Error::SetWin32(Error* errptr, unsigned long err, std::string message /*= std::string()*/)
{
	if (errptr)
		errptr->SetWin32(err, std::move(message));
}

void Error::SetHResult(long err, std::string message /* = std::string() */)
{
	m_type = Type::HResult;

	char buf[128];
	const DWORD r = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, err, 0, buf, sizeof(buf), nullptr);
	if (r > 0)
		m_description = fmt::format("HRESULT {:08X}: {}", err, std::string_view(buf, r));
	else
		m_description = fmt::format("HRESULT {:08X}: <Could not resolve system error ID>");

	m_message = std::move(message);
}

void Error::SetHResult(Error* errptr, long err, std::string message /*= std::string()*/)
{
	if (errptr)
		errptr->SetHResult(err, std::move(message));
}

#endif

void Error::SetSocket(int err, std::string message /* = std::string() */)
{
	// Socket errors are win32 errors on windows
#ifdef _WIN32
	SetWin32(err, std::move(message));
#else
	SetErrno(err, std::move(message));
#endif
	m_type = Type::Socket;
}

void Error::SetSocket(Error* errptr, int err, std::string message /*= std::string()*/)
{
}

// constructors
Error Error::CreateNone()
{
	return Error();
}

Error Error::CreateErrno(int err, std::string message /* = std::string() */)
{
	Error ret;
	ret.SetErrno(err, std::move(message));
	return ret;
}

Error Error::CreateSocket(int err, std::string message /* = std::string() */)
{
	Error ret;
	ret.SetSocket(err, std::move(message));
	return ret;
}

Error Error::CreateUser(int code, std::string message /* = std::string() */)
{
	Error ret;
	ret.SetUser(code, std::move(message));
	return ret;
}

Error Error::CreateUser(std::string description, std::string message /* = std::string() */)
{
	Error ret;
	ret.SetUser(std::move(description), std::move(message));
	return ret;
}

#ifdef _WIN32
Error Error::CreateWin32(unsigned long err, std::string message /* = std::string() */)
{
	Error ret;
	ret.SetWin32(err, std::move(message));
	return ret;
}

Error Error::CreateHResult(long err, std::string message /* = std::string() */)
{
	Error ret;
	ret.SetHResult(err, std::move(message));
	return ret;
}

#endif

Error& Error::operator=(const Error& e) = default;

Error& Error::operator=(Error&& e) = default;

bool Error::operator==(const Error& e) const
{
	return (m_type == e.m_type && m_message == e.m_message);
}

bool Error::operator!=(const Error& e) const
{
	return (m_type != e.m_type || m_message != e.m_message);
}
