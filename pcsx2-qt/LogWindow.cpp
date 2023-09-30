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

#include "PrecompiledHeader.h"
#include "LogWindow.h"
#include "MainWindow.h"
#include "QtHost.h"
#include "SettingWidgetBinder.h"

#include "pcsx2/Host.h"
#include "pcsx2/LogSink.h"

#include "common/Assertions.h"

#include <QtCore/QLatin1StringView>
#include <QtCore/QUtf8StringView>
#include <QtGui/QIcon>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QScrollBar>

LogWindow* g_log_window;

LogWindow::LogWindow(bool attach_to_main)
	: QMainWindow()
	, m_attached_to_main_window(attach_to_main)
{
	// TODO: probably should save the size..
	resize(700, 400);

	createUi();

	//for (u32 i = 0; i < ConsoleColors_Count; i++)
		//appendMessage(false, i, QString::fromStdString(fmt::format("color {}\n", i)));
}

LogWindow::~LogWindow()
{
	if (g_log_window == this)
		g_log_window = nullptr;
}

void LogWindow::updateSettings()
{
	const bool new_enabled = Host::GetBaseBoolSettingValue("Logging", "EnableWindow", /*false*/ true);
	const bool attach_to_main = Host::GetBaseBoolSettingValue("Logging", "AttachLogWindowToMainWindow", true);
	const bool timestamps = Host::GetBaseBoolSettingValue("Logging", "EnableTimestamps", true);
	const bool curr_enabled = (g_log_window != nullptr);
	if (new_enabled == curr_enabled)
	{
		if (g_log_window && g_log_window->m_attached_to_main_window != attach_to_main)
		{
			g_log_window->m_attached_to_main_window = attach_to_main;
			if (attach_to_main)
				g_log_window->reattachToMainWindow();
		}

		return;
	}

	if (new_enabled)
	{
		g_log_window = new LogWindow(attach_to_main);
		if (attach_to_main && g_main_window && g_main_window->isVisible())
			g_log_window->reattachToMainWindow();

		g_log_window->show();
	}
	else
	{
		delete g_log_window;
	}
}

void LogWindow::reattachToMainWindow()
{
	// Skip when maximized.
	if (g_main_window->windowState() & (Qt::WindowMaximized | Qt::WindowFullScreen))
		return;

	resize(width(), g_main_window->height());

	const QPoint new_pos = g_main_window->pos() + QPoint(g_main_window->width() + 10, 0);
	if (pos() != new_pos)
		move(new_pos);
}

void LogWindow::updateWindowTitle()
{
	QString title;

#if 0
  // TODO FIXME
  const QString& serial = QtHost::GetCurrentGameSerial();

  if (QtHost::IsSystemValid() && !serial.isEmpty())
  {
    const QFileInfo fi(QtHost::GetCurrentGamePath());
    title = tr("Log Window - %1 [%2]").arg(serial).arg(fi.fileName());
  }
  else
#endif
	{
		title = tr("Log Window");
	}

	setWindowTitle(title);
}

void LogWindow::createUi()
{
	QIcon icon;
	icon.addFile(QString::fromUtf8(":/icons/AppIcon64.png"), QSize(), QIcon::Normal, QIcon::On);
	setWindowIcon(icon);
	updateWindowTitle();

	QAction* action;

	QMenuBar* menu = new QMenuBar(this);
	setMenuBar(menu);

	QMenu* log_menu = menu->addMenu("&Log");
	action = log_menu->addAction(tr("&Clear"));
	connect(action, &QAction::triggered, this, &LogWindow::onClearTriggered);
	action = log_menu->addAction(tr("&Save..."));
	connect(action, &QAction::triggered, this, &LogWindow::onSaveTriggered);

	log_menu->addSeparator();

	action = log_menu->addAction(tr("Cl&ose"));
	connect(action, &QAction::triggered, this, &LogWindow::close);

	QMenu* settings_menu = menu->addMenu(tr("&Settings"));

	action = settings_menu->addAction(tr("Enable System Console"));
	action->setCheckable(true);
	SettingWidgetBinder::BindWidgetToBoolSetting(nullptr, action, "Logging", "EnableSystemConsole", false);

	action = settings_menu->addAction(tr("Enable &File Logging"));
	action->setCheckable(true);
	SettingWidgetBinder::BindWidgetToBoolSetting(nullptr, action, "Logging", "EnableFileLogging", false);

	settings_menu->addSeparator();

	action = settings_menu->addAction(tr("Attach To &Main Window"));
	action->setCheckable(true);
	SettingWidgetBinder::BindWidgetToBoolSetting(nullptr, action, "Logging", "AttachLogWindowToMainWindow", true);

	action = settings_menu->addAction(tr("&Verbose Logging"));
	action->setCheckable(true);
#ifndef PCSX2_DEVBUILD
	SettingWidgetBinder::BindWidgetToBoolSetting(nullptr, action, "Logging", "EnableVerbose", false);
#else
	action->setDisabled(true);
#endif

	action = settings_menu->addAction(tr("Show &Timestamps"));
	action->setCheckable(true);
	SettingWidgetBinder::BindWidgetToBoolSetting(nullptr, action, "Logging", "EnableTimestamps", true);

	settings_menu->addSeparator();

#ifdef PCSX2_DEVBUILD
	QMenu* trace_menu = menu->addMenu(tr("&Trace"));
	populateTraceMenu(trace_menu);
#endif

	m_text = new QPlainTextEdit(this);
	m_text->setReadOnly(true);
	m_text->setUndoRedoEnabled(false);
	m_text->setTextInteractionFlags(Qt::TextSelectableByKeyboard);
	m_text->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

#ifndef _WIN32
	QFont font("Monospace");
	font.setStyleHint(QFont::TypeWriter);
#else
	QFont font("Consolas");
	font.setPointSize(8);
#endif
	m_text->setFont(font);

	setCentralWidget(m_text);
}

#ifdef PCSX2_DEVBUILD
void LogWindow::populateTraceMenu(QMenu* trace_menu)
{
}
#endif

void LogWindow::onClearTriggered()
{
	m_text->clear();
}

void LogWindow::onSaveTriggered()
{
	const QString path = QFileDialog::getSaveFileName(this, tr("Select Log File"), QString(), tr("Log Files (*.txt)"));
	if (path.isEmpty())
		return;

	QFile file(path);
	if (!file.open(QFile::WriteOnly | QFile::Text))
	{
		QMessageBox::critical(this, tr("Error"), tr("Failed to open file for writing."));
		return;
	}

	file.write(m_text->toPlainText().toUtf8());
	file.close();

	appendMessage(false, Color_StrongGreen, tr("Log was written to %1.\n").arg(path));
}

void Host::SetLogWindowTitle(std::string_view title)
{
	// TODO FIXME
}

void Host::AppendToLogWindow(bool raw, float timestamp, ConsoleColors color, std::string_view message)
{
	QString qmessage;
	qmessage.reserve(message.length() + static_cast<u32>(!raw));
	qmessage.append(QUtf8StringView(message.data(), message.length()));
	if (!raw)
		qmessage.append(QChar('\n'));

	QtHost::RunOnUIThread([timestamp, color, qmessage = std::move(qmessage)]() {
		if (g_log_window)
			g_log_window->appendMessage(timestamp, color, qmessage);
	});
}

void LogWindow::closeEvent(QCloseEvent* event)
{
	// TODO: Update config.
	QMainWindow::closeEvent(event);
	pxFailRel("This leaves a dangling pointer");
}

void LogWindow::appendMessage(float timestamp, quint32 color, const QString& message)
{
	pxAssert(g_emu_thread->isOnUIThread());

	QTextCursor temp_cursor = m_text->textCursor();
	QScrollBar* scrollbar = m_text->verticalScrollBar();
	const bool cursor_at_end = temp_cursor.atEnd();
	const bool scroll_at_end = scrollbar->sliderPosition() == scrollbar->maximum();

	temp_cursor.movePosition(QTextCursor::End);

	{
		static constexpr const QColor color_mapping[ConsoleColors_Count] = {
			QColor(0xFF, 0xFF, 0xFF), // NONE
			QColor(0x0C, 0x0C, 0x0C), // Black
			QColor(0x13, 0xA1, 0x0E), // Green
			QColor(0xC5, 0x0F, 0x1F), // Red
			QColor(0x00, 0x37, 0xDA), // Blue
			QColor(0x88, 0x17, 0x98), // Magenta
			QColor(0x88, 0x17, 0x98), // Orange FIXME
			QColor(0x76, 0x76, 0x76), // Gray
			QColor(0x3A, 0x96, 0xDD), // Cyan
			QColor(0xC1, 0x9C, 0x00), // Yellow
			QColor(0xCC, 0xCC, 0xCC), // White

			QColor(0x0C, 0x0C, 0x0C), // Black
			QColor(0xE7, 0x48, 0x56), // StrongRed
			QColor(0x16, 0xC6, 0x0C), // StrongGreen
			QColor(0x3B, 0x78, 0xFF), // StrongBlue
			QColor(0xB4, 0x00, 0x9E), // StrongMagenta
			QColor(0xB4, 0x00, 0x9E), // StrongOrange FIXME
			QColor(0xCC, 0xCC, 0xCC), // StrongGray
			QColor(0x61, 0xD6, 0xD6), // StrongCyan
			QColor(0xF9, 0xF1, 0xA5), // StrongYellow
			QColor(0xF2, 0xF2, 0xF2), // StrongWhite
		};
		static constexpr const QColor timestamp_color = QColor(0xcc, 0xcc, 0xcc);

		QTextCharFormat format = temp_cursor.charFormat();

		if (timestamp >= 0.0f)
		{
			const QString qtimestamp = QStringLiteral("[%1] ").arg(timestamp, 10, 'f', 4);
			format.setForeground(QBrush(timestamp_color));
			temp_cursor.setCharFormat(format);
			temp_cursor.insertText(qtimestamp);
		}

		// message has \n already
		format.setForeground(QBrush(color_mapping[std::min(color, static_cast<u32>(std::size(color_mapping)))]));
		temp_cursor.setCharFormat(format);
		temp_cursor.insertText(message);
	}

	if (cursor_at_end)
	{
		if (scroll_at_end)
		{
			m_text->setTextCursor(temp_cursor);
			scrollbar->setSliderPosition(scrollbar->maximum());
		}
		else
		{
			// Can't let changing the cursor affect the scroll bar...
			const int pos = scrollbar->sliderPosition();
			m_text->setTextCursor(temp_cursor);
			scrollbar->setSliderPosition(pos);
		}
	}
}

void Host::OpenLogWindow()
{
	// TODO: This gets called from the emu thread => racey racey. Probably make a static object instead.
	// TODO: Open window, we want to do this here, not elsewhere.
}

void Host::CloseLogWindow()
{
	// TODO
}
