/*  PCSX2 - PS2 Emulator for PCs
 *  Copyright (C) 2002-2023 PCSX2 Dev Team
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

#include "common/Console.h"
#include "common/Pcsx2Defs.h"

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPlainTextEdit>
#include <span>

class LogWindow final : public QMainWindow
{
	Q_OBJECT

public:
	LogWindow(bool attach_to_main);
	~LogWindow();

	static void updateSettings();

	__fi bool isAttachedToMainWindow() const { return m_attached_to_main_window; }
	void reattachToMainWindow();

	void updateWindowTitle();

	void appendMessage(float timestamp, quint32 color, const QString& message);

private:
	void createUi();

#ifdef PCSX2_DEVBUILD
	void populateTraceMenu(QMenu* trace_menu);
#endif

protected:
	void closeEvent(QCloseEvent* event);

private Q_SLOTS:
	void onClearTriggered();
	void onSaveTriggered();

private:
	QPlainTextEdit* m_text;

	bool m_attached_to_main_window = true;
};

extern LogWindow* g_log_window;
