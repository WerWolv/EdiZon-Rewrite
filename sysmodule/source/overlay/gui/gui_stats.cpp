/**
 * Copyright (C) 2020 WerWolv
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

#include "overlay/gui/gui_stats.hpp"

#include "overlay/elements/frame.hpp"
#include "overlay/elements/custom_drawer.hpp"
#include "overlay/elements/list.hpp"
#include "overlay/elements/list_item.hpp"
#include "cheat/cheat.hpp"

#include "logo_bin.h"

#include <stratosphere.hpp>

#include <unistd.h>
#include <netinet/in.h>

namespace edz::ovl::gui {

    GuiStats::GuiStats() {
        if (ams::hos::GetVersion() >= ams::hos::Version_800) {
            clkrstOpenSession(&this->m_clkrstSessionCpu, PcvModuleId_CpuBus, 3);
            clkrstOpenSession(&this->m_clkrstSessionGpu, PcvModuleId_GPU, 3);
            clkrstOpenSession(&this->m_clkrstSessionMem, PcvModuleId_EMC, 3);
        }

        ams::sm::DoWithSession([this] {
            this->m_ipAddress = gethostid();
        });

        this->m_runningTitleID = cheat::CheatManager::getTitleID();
        this->m_runningProcessID = cheat::CheatManager::getProcessID();
        this->m_runningBuildID = cheat::CheatManager::getBuildID();
        
    }

    GuiStats::~GuiStats() {
        if (ams::hos::GetVersion() >= ams::hos::Version_800) {
            clkrstCloseSession(&this->m_clkrstSessionCpu);
            clkrstCloseSession(&this->m_clkrstSessionGpu);
            clkrstCloseSession(&this->m_clkrstSessionMem);
        }
    }


    Element* GuiStats::createUI() {
        auto rootFrame = new element::Frame();

        auto infos = new element::CustomDrawer(0, 0, 100, FB_WIDTH, [this](u16 x, u16 y, Screen *screen){
            screen->drawString("Stats", false, 20, 50, 30, a(0xFFFF));

            screen->drawString("Local IP:", false, 35, 120, 18, a(0xFFFF));

            screen->drawString("CPU Temparature:", false, 35, 160, 18, a(0xFFFF));
            screen->drawString("PCB Temparature:", false, 35, 190, 18, a(0xFFFF));

            screen->drawString("CPU Clock:", false, 35, 230, 18, a(0xFFFF));
            screen->drawString("GPU Clock:", false, 35, 260, 18, a(0xFFFF));
            screen->drawString("MEM Clock:", false, 35, 290, 18, a(0xFFFF));

            screen->drawString("WiFi Signal:", false, 35, 330, 18, a(0xFFFF));

            if (this->m_runningProcessID != 0) {
                screen->drawString("Program ID:", false, 35, 370, 18, a(0xFFFF));
                screen->drawString("Build ID:", false, 35, 400, 18, a(0xFFFF));
                screen->drawString("Process ID:", false, 35, 430, 18, a(0xFFFF));
            }

            if (this->m_ipAddress == INADDR_LOOPBACK)
                screen->drawString("Offline", false, 230, 120, 18, a(0xFFFF));
            else 
                screen->drawString(hlp::formatString("%d.%d.%d.%d", this->m_ipAddress & 0xFF, (this->m_ipAddress >> 8) & 0xFF, (this->m_ipAddress >> 16) & 0xFF, (this->m_ipAddress >> 24) & 0xFF).c_str(), false, 230, 120, 18, a(0xFFFF));

            s32 temparature = 0;
            tsGetTemperatureMilliC(TsLocation_Internal, &temparature);
            screen->drawString(hlp::formatString("%.02f °C", temparature / 1000.0F).c_str(), false, 230, 160, 18, a(0xFFFF));
            tsGetTemperatureMilliC(TsLocation_External, &temparature);
            screen->drawString(hlp::formatString("%.02f °C", temparature / 1000.0F).c_str(), false, 230, 190, 18, a(0xFFFF));

            u32 cpuClock = 0, gpuClock = 0, memClock = 0;

            if (ams::hos::GetVersion() >= ams::hos::Version_800) {
                clkrstGetClockRate(&this->m_clkrstSessionCpu, &cpuClock);
                clkrstGetClockRate(&this->m_clkrstSessionGpu, &gpuClock);
                clkrstGetClockRate(&this->m_clkrstSessionMem, &memClock);
            } else {
                pcvGetClockRate(PcvModule_CpuBus, &cpuClock);
                pcvGetClockRate(PcvModule_GPU, &gpuClock);
                pcvGetClockRate(PcvModule_EMC, &memClock);
            }

            screen->drawString(hlp::formatString("%.01f MHz", cpuClock / 1'000'000.0F).c_str(), false, 230, 230, 18, a(0xFFFF));
            screen->drawString(hlp::formatString("%.01f MHz", gpuClock / 1'000'000.0F).c_str(), false, 230, 260, 18, a(0xFFFF));
            screen->drawString(hlp::formatString("%.01f MHz", memClock / 1'000'000.0F).c_str(), false, 230, 290, 18, a(0xFFFF));

            s32 signalStrength = 0;
            wlaninfGetRSSI(&signalStrength);

            screen->drawString(hlp::formatString("%d dBm", signalStrength).c_str(), false, 230, 330, 18, a(0xFFFF));

            if (this->m_runningProcessID != 0) {
                screen->drawString(hlp::formatString("%016lX", this->m_runningTitleID).c_str(), false, 230, 370, 18, a(0xFFFF));
                screen->drawString(hlp::formatString("%016lX", this->m_runningBuildID).c_str(), false, 230, 400, 18, a(0xFFFF));
                screen->drawString(hlp::formatString("%lu", this->m_runningProcessID).c_str(), false, 230, 430, 18, a(0xFFFF));
            }

 
        });

        rootFrame->addElement(infos);

        return rootFrame;
    }

    void GuiStats::update() {
        
    }

}