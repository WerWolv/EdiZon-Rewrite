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

#include <edizon.hpp>

#include <fcntl.h>
#include <stdlib.h>
#include <malloc.h>
#include <new>

#include "ui/gui.hpp"
#include "helpers/utils.hpp"
#include "helpers/debug_helpers.hpp"

#if LOG_ALLOCATION

    typedef struct {
        void* ptr;
        size_t size;
    } AllocatedData;

    static AllocatedData allocatedData[0xA000];
    static u32 allocatedDataIndex = 0;
    static u64 allocatedSize = 0;

    void* operator new(std::size_t sz) {
        void* ptr = std::malloc(sz);
        if (ptr) {
            allocatedSize += sz;
            for (u32 i = 0; i < allocatedDataIndex; i++)
                if (allocatedData[i].ptr == nullptr) {
                    allocatedData[i] = { ptr, sz };
                    allocatedDataIndex--;
                    break;
                }
            
            allocatedDataIndex++;

            Logger::debug("Allocated %d bytes, now at %d bytes", sz, allocatedSize);
            return ptr;
        }
        else
            throw std::bad_alloc();
    }

    void operator delete(void* ptr) noexcept {
        size_t size = 0;
        for (u32 i = 0; i < allocatedDataIndex; i++)
            if (allocatedData[i].ptr == ptr) {
                size = allocatedData[i].size;
                allocatedSize -= size;
                allocatedData[i] = { 0, 0 };
                break;
            }


        Logger::debug("Deallocated %d bytes, now at %d bytes", size, allocatedSize);
        std::free(ptr);
    }

#endif


#if REDIRECT_LOG_TO_FILE

    static int logFile = -1;

    void stdioRedirectToFile() {
        logFile = open(EDIZON_BASE_DIR "/edizon.log", O_CREAT | O_RDWR);

        if (logFile > 0) {
            fflush(stdout);
            dup2(logFile, STDOUT_FILENO);
            fflush(stderr);
            dup2(logFile, STDERR_FILENO);
        }
    }

    void stdioRedirectCleanup() {
        if (logFile > 0)
            close(logFile);
    }

#else
    void stdioRedirectToFile() { }
    void stdioRedirectCleanup() { }
#endif

#if !DISABLE_EXCEPTION_HANDLER

static void printDebugInfo(ThreadExceptionDump *ctx) {
    u64 stackTrace[0x20] = { 0 };
    s32 stackTraceSize = 0;
    edz::hlp::unwindStack(stackTrace, &stackTraceSize, 0x20, ctx->fp.x);

    brls::Logger::error("[PC] BASE + 0x%016lx", ctx->pc.x - edz::hlp::getHomebrewBaseAddress());
    brls::Logger::error("[LR] BASE + 0x%016lx", ctx->lr.x - edz::hlp::getHomebrewBaseAddress());
    brls::Logger::error("[SP] BASE + 0x%016lx", ctx->sp.x - edz::hlp::getHomebrewBaseAddress());
    brls::Logger::error("[FP] BASE + 0x%016lx", ctx->fp.x - edz::hlp::getHomebrewBaseAddress());
    brls::Logger::error("== Begin stack trace ==");
    for (s8 i = 0; i < stackTraceSize - 2; i++)
        brls::Logger::error("[%d] BASE + 0x%016lx", stackTraceSize - i - 3, stackTrace[i] - edz::hlp::getHomebrewBaseAddress());
    brls::Logger::error("== End stack trace ==");
}

extern "C" {
   
    alignas(16) u8 __nx_exception_stack[0x8000];
    u64 __nx_exception_stack_size = sizeof(__nx_exception_stack);
    void NORETURN __nx_exit(Result rc, LoaderReturnFn retaddr);

    void __libnx_exception_handler(ThreadExceptionDump *ctx) {
        std::string errorDesc;
        static bool alreadyCrashed = false;
        static ThreadExceptionDump ctxBackup;

        appletUnlockExit();

        if (alreadyCrashed) {
            brls::Logger::error("Fatal exception thrown during exception handling. Closing immediately.");
            printDebugInfo(ctx);

            // Setup FatalCpuContext to pass on crash information to fatal
            FatalCpuContext fatalCtx = { 0 };
            for (u8 i = 0; i < 29; i++)
                fatalCtx.aarch64_ctx.x[i] = ctxBackup.cpu_gprs[i].x;

            fatalCtx.aarch64_ctx.pc = ctxBackup.pc.x;
            fatalCtx.aarch64_ctx.lr = ctxBackup.lr.x;
            fatalCtx.aarch64_ctx.sp = ctxBackup.sp.x;
            fatalCtx.aarch64_ctx.fp = ctxBackup.fp.x;
            
            fatalCtx.aarch64_ctx.start_address = edz::hlp::getHomebrewBaseAddress();
            edz::hlp::unwindStack(fatalCtx.aarch64_ctx.stack_trace, reinterpret_cast<s32*>(&fatalCtx.aarch64_ctx.stack_trace_size), 32, ctxBackup.fp.x);
            fatalCtx.aarch64_ctx.afsr0 = ctxBackup.afsr0;
            fatalCtx.aarch64_ctx.afsr1 = ctxBackup.afsr1;
            fatalCtx.aarch64_ctx.esr = ctxBackup.esr;
            fatalCtx.aarch64_ctx.far = ctxBackup.far.x;
            fatalCtx.aarch64_ctx.register_set_flags = 0xFFFFFFFF;
            fatalCtx.aarch64_ctx.pstate = ctxBackup.pstate;
            
            fatalThrowWithContext(edz::ResultEdzErrorDuringErrorHandling, FatalPolicy_ErrorScreen, &fatalCtx);
            while (true)
                svcSleepThread(U64_MAX);
        }

        alreadyCrashed = true;
        std::memcpy(&ctxBackup, ctx, sizeof(ThreadExceptionDump));

        switch (ctx->error_desc) {
            case ThreadExceptionDesc_BadSVC:
                errorDesc = "Bad SVC";
                break;
            case ThreadExceptionDesc_InstructionAbort:
                errorDesc = "Instruction Abort";
                break;
            case ThreadExceptionDesc_MisalignedPC:
                errorDesc = "Misaligned Program Counter";
                break;
            case ThreadExceptionDesc_MisalignedSP:
                errorDesc = "Misaligned Stack Pointer";
                break;
            case ThreadExceptionDesc_SError:
                errorDesc = "SError";
                break;
            case ThreadExceptionDesc_Trap:
                errorDesc = "SIGTRAP";
                break;
            case ThreadExceptionDesc_Other:
                errorDesc = "Segmentation Fault";
                break;
            default:
                errorDesc = "Unknown Exception [ 0x" + edz::hlp::toHexString<u16>(ctx->error_desc) + " ]";
                break;
        }

        brls::Application::removeFocus();
        edz::ui::Gui::fatal("%s\n\n%s: %s\nPC: BASE + 0x%016lx",
            "A fatal exception occured!",
            "Reason",
            errorDesc.c_str(),
            ctx->pc.x - edz::hlp::getHomebrewBaseAddress());

        printDebugInfo(ctx);

        appletRequestExitToSelf();
    }

}

#endif