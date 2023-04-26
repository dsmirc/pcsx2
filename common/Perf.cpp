/*  PCSX2 - PS2 Emulator for PCs
 *  Copyright (C) 2002-2021  PCSX2 Dev Team
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

#include "common/Perf.h"
#include "common/Pcsx2Defs.h"
#include "common/Assertions.h"
#ifdef __unix__
#include <atomic>
#include <array>
#include <cstdarg>
#include <cstring>
#include <ctime>
#include <mutex>
#include <elf.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#endif
#ifdef ENABLE_VTUNE
#include "jitprofiling.h"

#include <string> // std::string
#include <cstring> // strncpy
#include <algorithm> // std::remove_if
#endif

//#define ProfileWithPerf
//#define ProfileWithPerfJitDump
#define MERGE_BLOCK_RESULT

#ifdef ENABLE_VTUNE
#ifdef _WIN32
#pragma comment(lib, "jitprofiling.lib")
#endif
#endif

namespace Perf
{
	// Warning object aren't thread safe
	InfoVector any("");
	InfoVector ee("EE");
	InfoVector iop("IOP");
	InfoVector vu0("VU0");
	InfoVector vu1("VU1");
	InfoVector vif("VIF");

// Perf is only supported on linux
#if defined(__linux__) && (defined(ProfileWithPerf) || defined(ProfileWithPerfJitDump) || defined(ENABLE_VTUNE))

	////////////////////////////////////////////////////////////////////////////////
	// Implementation of the Info object
	////////////////////////////////////////////////////////////////////////////////

	Info::Info(uptr x86, u32 size, const char* symbol)
		: m_x86(x86)
		, m_size(size)
		, m_dynamic(false)
	{
		strncpy(m_symbol, symbol, sizeof(m_symbol));
	}

	Info::Info(uptr x86, u32 size, const char* symbol, u32 pc)
		: m_x86(x86)
		, m_size(size)
		, m_dynamic(true)
	{
		snprintf(m_symbol, sizeof(m_symbol), "%s_0x%08x", symbol, pc);
	}

	void Info::Print(FILE* fp)
	{
		fprintf(fp, "%llx %x %s\n", m_x86, m_size, m_symbol);
	}

#endif

#if defined(__linux__) && (defined(ProfileWithPerf) || defined(ENABLE_VTUNE))

	////////////////////////////////////////////////////////////////////////////////
	// Implementation of the InfoVector object
	////////////////////////////////////////////////////////////////////////////////

	InfoVector::InfoVector(const char* prefix)
	{
		strncpy(m_prefix, prefix, sizeof(m_prefix));
#ifdef ENABLE_VTUNE
		m_vtune_id = iJIT_GetNewMethodID();
#else
		m_vtune_id = 0;
#endif
	}

	void InfoVector::print(FILE* fp)
	{
		for (auto&& it : m_v)
			it.Print(fp);
	}

	void InfoVector::map(uptr x86, u32 size, const char* symbol)
	{
// This function is typically used for dispatcher and recompiler.
// Dispatchers are on a page and must always be kept.
// Recompilers are much bigger (TODO check VIF) and are only
// useful when MERGE_BLOCK_RESULT is defined
#if defined(ENABLE_VTUNE) || !defined(MERGE_BLOCK_RESULT)
		u32 max_code_size = 16 * _1kb;
#else
		u32 max_code_size = _1gb;
#endif

		if (size < max_code_size)
		{
			m_v.emplace_back(x86, size, symbol);

#ifdef ENABLE_VTUNE
			std::string name = std::string(symbol);

			iJIT_Method_Load ml;

			memset(&ml, 0, sizeof(ml));

			ml.method_id = iJIT_GetNewMethodID();
			ml.method_name = (char*)name.c_str();
			ml.method_load_address = (void*)x86;
			ml.method_size = size;

			iJIT_NotifyEvent(iJVM_EVENT_TYPE_METHOD_LOAD_FINISHED, &ml);

//fprintf(stderr, "mapF %s: %p size %dKB\n", ml.method_name, ml.method_load_address, ml.method_size / 1024u);
#endif
		}
	}

	void InfoVector::map(uptr x86, u32 size, u32 pc)
	{
#ifndef MERGE_BLOCK_RESULT
		m_v.emplace_back(x86, size, m_prefix, pc);
#endif

#ifdef ENABLE_VTUNE
		iJIT_Method_Load_V2 ml;

		memset(&ml, 0, sizeof(ml));

#ifdef MERGE_BLOCK_RESULT
		ml.method_id = m_vtune_id;
		ml.method_name = m_prefix;
#else
		std::string name = std::string(m_prefix) + "_" + std::to_string(pc);
		ml.method_id = iJIT_GetNewMethodID();
		ml.method_name = (char*)name.c_str();
#endif
		ml.method_load_address = (void*)x86;
		ml.method_size = size;

		iJIT_NotifyEvent(iJVM_EVENT_TYPE_METHOD_LOAD_FINISHED_V2, &ml);

//fprintf(stderr, "mapB %s: %p size %d\n", ml.method_name, ml.method_load_address, ml.method_size);
#endif
	}

	void InfoVector::reset()
	{
		auto dynamic = std::remove_if(m_v.begin(), m_v.end(), [](Info i) { return i.m_dynamic; });
		m_v.erase(dynamic, m_v.end());
	}

	////////////////////////////////////////////////////////////////////////////////
	// Global function
	////////////////////////////////////////////////////////////////////////////////

	void dump()
	{
		char file[256];
		snprintf(file, 250, "/tmp/perf-%d.map", getpid());
		FILE* fp = fopen(file, "w");

		any.print(fp);
		ee.print(fp);
		iop.print(fp);
		vu.print(fp);

		if (fp)
			fclose(fp);
	}

	void dump_and_reset()
	{
		dump();

		any.reset();
		ee.reset();
		iop.reset();
		vu.reset();
	}

#elif defined(__linux__) && defined(ProfileWithPerfJitDump)

	enum : u32
	{
		JIT_CODE_LOAD = 0,
		JIT_CODE_MOVE = 1,
		JIT_CODE_DEBUG_INFO = 2,
		JIT_CODE_CLOSE = 3,
		JIT_CODE_UNWINDING_INFO = 4
	};

#pragma pack(push, 1)
	struct JITDUMP_HEADER
	{
		u32 magic = 0x4A695444; // JiTD
		u32 version = 1;
		u32 header_size = sizeof(JITDUMP_HEADER);
		u32 elf_mach;
		u32 pad1 = 0;
		u32 pid;
		u64 timestamp;
		u64 flags = 0;
	};
	struct JITDUMP_RECORD_HEADER
	{
		u32 id;
		u32 total_size;
		u64 timestamp;
	};
	struct JITDUMP_CODE_LOAD
	{
		JITDUMP_RECORD_HEADER header;
		u32 pid;
		u32 tid;
		u64 vma;
		u64 code_addr;
		u64 code_size;
		u64 code_index;
		// name
	};
#pragma pack(pop)


	static u64 JitDumpTimestamp()
	{
		struct timespec ts = {};
		clock_gettime(CLOCK_MONOTONIC, &ts);
		return (static_cast<u64>(ts.tv_sec) * 1000000000ULL) + static_cast<u64>(ts.tv_nsec);
	}

	static FILE* s_jitdump_file = nullptr;
	static bool s_jitdump_file_opened = false;
	static std::mutex s_jitdump_mutex;
	static u32 s_jitdump_record_id;

	InfoVector::InfoVector(const char* prefix)
		: m_vtune_id(0)
	{
		strncpy(m_prefix, prefix, sizeof(m_prefix));
	}

	static void write_to_dump(Info* inf)
	{
		std::unique_lock lock(s_jitdump_mutex);
		if (!s_jitdump_file_opened)
		{
			char file[256];
			snprintf(file, std::size(file), "jit-%d.dump", getpid());
			s_jitdump_file = fopen(file, "w+b");
			s_jitdump_file_opened = true;

			if (s_jitdump_file)
			{
				void* perf_marker = mmap(nullptr, 4096, PROT_READ | PROT_EXEC, MAP_PRIVATE, fileno(s_jitdump_file), 0);
				pxAssertRel(perf_marker != MAP_FAILED, "Map perf marker");

				JITDUMP_HEADER jh = {};
				jh.elf_mach = EM_X86_64;
				jh.pid = getpid();
				jh.timestamp = JitDumpTimestamp();
				fwrite(&jh, sizeof(jh), 1, s_jitdump_file);
			}
		}
		if (!s_jitdump_file)
			return;

		const u32 namelen = std::strlen(inf->m_symbol) + 1;

		JITDUMP_CODE_LOAD cl = {};
		cl.header.id = JIT_CODE_LOAD;
		cl.header.total_size = sizeof(cl) + namelen + inf->m_size;
		cl.header.timestamp = JitDumpTimestamp();
		cl.pid = getpid();
		cl.tid = syscall(SYS_gettid);
		cl.vma = 0;
		cl.code_addr = (u64)inf->m_x86;
		cl.code_size = inf->m_size;
		cl.code_index = s_jitdump_record_id++;
		fwrite(&cl, sizeof(cl), 1, s_jitdump_file);
		fwrite(inf->m_symbol, namelen, 1, s_jitdump_file);
		fwrite((const void*)inf->m_x86, inf->m_size, 1, s_jitdump_file);
		fflush(s_jitdump_file);
	}

	void InfoVector::map(uptr x86, u32 size, const char* symbol)
	{
		Info inf(x86, size, symbol);
		write_to_dump(&inf);
	}

	void InfoVector::map(uptr x86, u32 size, u32 pc)
	{
		Info inf(x86, size, m_prefix, pc);
		write_to_dump(&inf);
	}
	void InfoVector::reset() {}

	void dump() {}
	void dump_and_reset() {}


#else

	////////////////////////////////////////////////////////////////////////////////
	// Dummy implementation
	////////////////////////////////////////////////////////////////////////////////

	InfoVector::InfoVector(const char* prefix)
		: m_vtune_id(0)
	{
	}
	void InfoVector::map(uptr x86, u32 size, const char* symbol) {}
	void InfoVector::map(uptr x86, u32 size, u32 pc) {}
	void InfoVector::reset() {}

	void dump() {}
	void dump_and_reset() {}

#endif
} // namespace Perf
