/*  PCSX2 - PS2 Emulator for PCs
 *  Copyright (C) 2002-2010  PCSX2 Dev Team
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

#ifdef __linux__
#include <signal.h>
#endif

#include "vtlb.h"

#include "common/emitter/x86_intrin.h"

// [TODO] This *could* be replaced with an assignment operator on u128 that implicitly
// uses _mm_store and _mm_load internally.  However, there are alignment concerns --
// u128 is not alignment strict.  (we would need a u128 and u128a for types known to
// be strictly 128-bit aligned).
static __fi void CopyQWC( void* dest, const void* src )
{
	_mm_store_ps( (float*)dest, _mm_load_ps((const float*)src) );
}

static __fi void ZeroQWC( void* dest )
{
	_mm_store_ps( (float*)dest, _mm_setzero_ps() );
}

static __fi void ZeroQWC( u128& dest )
{
	_mm_store_ps( (float*)&dest, _mm_setzero_ps() );
}

#define PSM(mem)	(vtlb_GetPhyPtr((mem)&0x1fffffff)) //pcsx2 is a competition.The one with most hacks wins :D

#define psHs8(mem)	(*(s8 *)&eeHw[(mem) & 0xffff])
#define psHs16(mem)	(*(s16*)&eeHw[(mem) & 0xffff])
#define psHs32(mem)	(*(s32*)&eeHw[(mem) & 0xffff])
#define psHs64(mem)	(*(s64*)&eeHw[(mem) & 0xffff])
#define psHu8(mem)	(*(u8 *)&eeHw[(mem) & 0xffff])
#define psHu16(mem)	(*(u16*)&eeHw[(mem) & 0xffff])
#define psHu32(mem)	(*(u32*)&eeHw[(mem) & 0xffff])
#define psHu64(mem)	(*(u64*)&eeHw[(mem) & 0xffff])
#define psHu128(mem)(*(u128*)&eeHw[(mem) & 0xffff])

#define psMs8(mem)	(*(s8 *)&eeMem->Main[(mem) & 0x1ffffff])
#define psMs16(mem)	(*(s16*)&eeMem->Main[(mem) & 0x1ffffff])
#define psMs32(mem)	(*(s32*)&eeMem->Main[(mem) & 0x1ffffff])
#define psMs64(mem)	(*(s64*)&eeMem->Main[(mem) & 0x1ffffff])
#define psMu8(mem)	(*(u8 *)&eeMem->Main[(mem) & 0x1ffffff])
#define psMu16(mem)	(*(u16*)&eeMem->Main[(mem) & 0x1ffffff])
#define psMu32(mem)	(*(u32*)&eeMem->Main[(mem) & 0x1ffffff])
#define psMu64(mem)	(*(u64*)&eeMem->Main[(mem) & 0x1ffffff])

#define psRs8(mem)	(*(s8 *)&eeMem->ROM[(mem) & 0x3fffff])
#define psRs16(mem)	(*(s16*)&eeMem->ROM[(mem) & 0x3fffff])
#define psRs32(mem)	(*(s32*)&eeMem->ROM[(mem) & 0x3fffff])
#define psRs64(mem)	(*(s64*)&eeMem->ROM[(mem) & 0x3fffff])
#define psRu8(mem)	(*(u8 *)&eeMem->ROM[(mem) & 0x3fffff])
#define psRu16(mem)	(*(u16*)&eeMem->ROM[(mem) & 0x3fffff])
#define psRu32(mem)	(*(u32*)&eeMem->ROM[(mem) & 0x3fffff])
#define psRu64(mem)	(*(u64*)&eeMem->ROM[(mem) & 0x3fffff])

#define psR1s8(mem)		(*(s8 *)&eeMem->ROM1[(mem) & 0x3fffff])
#define psR1s16(mem)	(*(s16*)&eeMem->ROM1[(mem) & 0x3fffff])
#define psR1s32(mem)	(*(s32*)&eeMem->ROM1[(mem) & 0x3fffff])
#define psR1s64(mem)	(*(s64*)&eeMem->ROM1[(mem) & 0x3fffff])
#define psR1u8(mem)		(*(u8 *)&eeMem->ROM1[(mem) & 0x3fffff])
#define psR1u16(mem)	(*(u16*)&eeMem->ROM1[(mem) & 0x3fffff])
#define psR1u32(mem)	(*(u32*)&eeMem->ROM1[(mem) & 0x3fffff])
#define psR1u64(mem)	(*(u64*)&eeMem->ROM1[(mem) & 0x3fffff])

#define psR2s8(mem)		(*(s8 *)&eeMem->ROM2[(mem) & 0x7ffff])
#define psR2s16(mem)	(*(s16*)&eeMem->ROM2[(mem) & 0x7ffff])
#define psR2s32(mem)	(*(s32*)&eeMem->ROM2[(mem) & 0x7ffff])
#define psR2s64(mem)	(*(s64*)&eeMem->ROM2[(mem) & 0x7ffff])
#define psR2u8(mem)		(*(u8 *)&eeMem->ROM2[(mem) & 0x7ffff])
#define psR2u16(mem)	(*(u16*)&eeMem->ROM2[(mem) & 0x7ffff])
#define psR2u32(mem)	(*(u32*)&eeMem->ROM2[(mem) & 0x7ffff])
#define psR2u64(mem)	(*(u64*)&eeMem->ROM2[(mem) & 0x7ffff])

#define psSs32(mem)		(*(s32 *)&eeMem->Scratch[(mem) & 0x3fff])
#define psSs64(mem)		(*(s64 *)&eeMem->Scratch[(mem) & 0x3fff])
#define psSs128(mem)	(*(s128*)&eeMem->Scratch[(mem) & 0x3fff])
#define psSu32(mem)		(*(u32 *)&eeMem->Scratch[(mem) & 0x3fff])
#define psSu64(mem)		(*(u64 *)&eeMem->Scratch[(mem) & 0x3fff])
#define psSu128(mem)	(*(u128*)&eeMem->Scratch[(mem) & 0x3fff])


extern void memSetKernelMode();
//extern void memSetSupervisorMode();
extern void memSetUserMode();
extern void memBindConditionalHandlers();

extern void memMapVUmicro();

#define memRead8 vtlb_memRead<mem8_t>
#define memRead16 vtlb_memRead<mem16_t>
#define memRead32 vtlb_memRead<mem32_t>
#define memRead64 vtlb_memRead<mem64_t>

#define memWrite8 vtlb_memWrite<mem8_t>
#define memWrite16 vtlb_memWrite<mem16_t>
#define memWrite32 vtlb_memWrite<mem32_t>
#define memWrite64 vtlb_memWrite<mem64_t>

static __fi void memRead128(u32 mem, mem128_t* out) { _mm_store_si128((__m128i*)out, vtlb_memRead128(mem)); }
static __fi void memRead128(u32 mem, mem128_t& out) { memRead128(mem, &out); }

static __fi void memWrite128(u32 mem, const mem128_t* val)	{ vtlb_memWrite128(mem, r128_load(val)); }
static __fi void memWrite128(u32 mem, const mem128_t& val)	{ vtlb_memWrite128(mem, r128_load(&val)); }


extern u16 ba0R16(u32 mem);
