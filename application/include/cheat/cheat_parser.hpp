/**
 * Copyright (C) 2019 WerWolv
 * 
 * This file is part of EdiZon.
 * 
 * EdiZon is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 * 
 * EdiZon is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with EdiZon.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <edizon.hpp>
#include "cheat/dmntcht.hpp"

namespace edz::cheat {

    namespace opcodes {
        
        enum class CheatVmOpcodeType : u32{
            STORE_STATIC = 0x00,
            BEGIN_CONDITIONAL_BLOCK = 0x01,
            END_CONDITIONAL_BLOCK =0x02,
            CONTROL_LOOP = 0x03,
            LOAD_REGISTER_STATIC = 0x04,
            LOAD_REGISTER_MEMORY = 0x05,
            STORE_STATIC_TO_ADDRESS = 0x06,
            PERFORM_ARITHMETIC_STATIC = 0x07,
            BEGIN_KEYPRESS_CONDITIONAL_BLOCK = 0x08,

            /* These are not implemented by Gateway's VM. */
            PERFORM_ARITHMETIC_REGISTER = 0x09,
            STORE_REGISTER_TO_ADDRESS = 0x0A,
            RESERVED_11 = 0x0B,

            /* This is a meta entry, and not a real opcode. */
            /* This is to facilitate multi-nybble instruction decoding. */
            EXTENDED_WIDTH = 0x0C,

            /* Extended width opcodes. */
            BEGIN_REGISTER_CONDITIONAL_BLOCK = 0xC0,
            SAVE_RESTORE_REGISTER = 0xC1,
            SAVE_RESTORE_REGISTER_MASK = 0xC2,

            /* This is a meta entry, and not a real opcode. */
            /* This is to facilitate multi-nybble instruction decoding. */
            DOUBLE_EXTENDED_WIDTH = 0xF0,

            /* Double-extended width opcodes. */
            DEBUG_LOG = 0xFFF,
        };

        enum class MemoryAccessType : u32 {
            MAIN_NSO = 0,
            HEAP = 1,
        };

        enum class ConditionalComparisonType : u32 {
            GT = 1,
            GE = 2,
            LT = 3,
            LE = 4,
            EQ = 5,
            NE = 6,
        };

        enum class RegisterArithmeticType : u32 {
            Addition = 0,
            Subtraction = 1,
            Multiplication = 2,
            LeftShift = 3,
            RightShift = 4,

            /* These are not supported by Gateway's VM. */
            LogicalAnd = 5,
            LogicalOr = 6,
            LogicalNot = 7,
            LogicalXor = 8,

            None = 9,
        };

        enum class StoreRegisterOffsetType : u32 {
            None = 0,
            Reg = 1,
            Imm = 2,
            MemReg = 3,
            MemImm = 4,
            MemImmReg = 5,
        };

        enum class CompareRegisterValueType : u32 {
            MemoryRelAddr = 0,
            MemoryOfsReg = 1,
            RegisterRelAddr = 2,
            RegisterOfsReg = 3,
            StaticValue = 4,
            OtherRegister = 5,
        };

        enum class SaveRestoreRegisterOpType : u32 {
            Restore = 0,
            Save = 1,
            ClearSaved = 2,
            ClearRegs = 3,
        };

        enum class DebugLogValueType : u32 {
            MemoryRelAddr = 0,
            MemoryOfsReg = 1,
            RegisterRelAddr = 2,
            RegisterOfsReg = 3,
            RegisterValue = 4,
        };

        union VmInt {
            u8  bit8;
            u16 bit16;
            u32 bit32;
            u64 bit64;
        };

        struct StoreStaticOpcode {
            u32 bit_width;
            MemoryAccessType mem_type;
            u32 offset_register;
            u64 rel_address;
            VmInt value;
        };

        struct BeginConditionalOpcode {
            u32 bit_width;
            MemoryAccessType mem_type;
            ConditionalComparisonType cond_type;
            u64 rel_address;
            VmInt value;
        };

        struct EndConditionalOpcode {};

        struct ControlLoopOpcode {
            bool start_loop;
            u32 reg_index;
            u32 num_iters;
        };

        struct LoadRegisterStaticOpcode {
            u32 reg_index;
            u64 value;
        };

        struct LoadRegisterMemoryOpcode {
            u32 bit_width;
            MemoryAccessType mem_type;
            u32 reg_index;
            bool load_from_reg;
            u64 rel_address;
        };

        struct StoreStaticToAddressOpcode {
            u32 bit_width;
            u32 reg_index;
            bool increment_reg;
            bool add_offset_reg;
            u32 offset_reg_index;
            u64 value;
        };

        struct PerformArithmeticStaticOpcode {
            u32 bit_width;
            u32 reg_index;
            RegisterArithmeticType math_type;
            u32 value;
        };

        struct BeginKeypressConditionalOpcode {
            u32 key_mask;
        };

        struct PerformArithmeticRegisterOpcode {
            u32 bit_width;
            RegisterArithmeticType math_type;
            u32 dst_reg_index;
            u32 src_reg_1_index;
            u32 src_reg_2_index;
            bool has_immediate;
            VmInt value;
        };

        struct StoreRegisterToAddressOpcode {
            u32 bit_width;
            u32 str_reg_index;
            u32 addr_reg_index;
            bool increment_reg;
            StoreRegisterOffsetType ofs_type;
            MemoryAccessType mem_type;
            u32 ofs_reg_index;
            u64 rel_address;
        };

        struct BeginRegisterConditionalOpcode {
            u32 bit_width;
            ConditionalComparisonType cond_type;
            u32 val_reg_index;
            CompareRegisterValueType comp_type;
            MemoryAccessType mem_type;
            u32 addr_reg_index;
            u32 other_reg_index;
            u32 ofs_reg_index;
            u64 rel_address;
            VmInt value;
        };

        struct SaveRestoreRegisterOpcode {
            u32 dst_index;
            u32 src_index;
            SaveRestoreRegisterOpType op_type;
        };

        struct SaveRestoreRegisterMaskOpcode {
            SaveRestoreRegisterOpType op_type;
            bool should_operate[0x10];
        };

        struct DebugLogOpcode {
            u32 bit_width;
            u32 log_id;
            DebugLogValueType val_type;
            MemoryAccessType mem_type;
            u32 addr_reg_index;
            u32 val_reg_index;
            u32 ofs_reg_index;
            u64 rel_address;
        };

        struct CheatVmOpcode {
            CheatVmOpcodeType opcode;
            bool begin_conditional_block;
            union {
                StoreStaticOpcode store_static;
                BeginConditionalOpcode begin_cond;
                EndConditionalOpcode end_cond;
                ControlLoopOpcode ctrl_loop;
                LoadRegisterStaticOpcode ldr_static;
                LoadRegisterMemoryOpcode ldr_memory;
                StoreStaticToAddressOpcode str_static;
                PerformArithmeticStaticOpcode perform_math_static;
                BeginKeypressConditionalOpcode begin_keypress_cond;
                PerformArithmeticRegisterOpcode perform_math_reg;
                StoreRegisterToAddressOpcode str_register;
                BeginRegisterConditionalOpcode begin_reg_cond;
                SaveRestoreRegisterOpcode save_restore_reg;
                SaveRestoreRegisterMaskOpcode save_restore_regmask;
                DebugLogOpcode debug_log;
            };
        };


    }

    class CheatParser {
    public:
        CheatParser() = delete;
        CheatParser(CheatParser &) = delete;
        CheatParser& operator=(CheatParser &) = delete; 

        static std::pair<EResult, std::optional<dmntcht::CheatDefinition>> parseString(std::string content);
        static std::pair<EResult, std::optional<dmntcht::CheatDefinition>> parseFile(std::string filePath);
    };

}