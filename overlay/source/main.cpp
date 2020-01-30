/**
 * Copyright (C) 2019 - 2020 WerWolv
 * 
 * This file is part of EdiZon
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <switch.h>
#include <tesla.hpp>
#include <filesystem>

#include <switch/nro.h>
#include <switch/nacp.h>

#include <edizon.hpp>
#include "cheat/cheat.hpp"
#include "helpers/utils.hpp"

#include <unistd.h>
#include <netinet/in.h>


class GuiCheats : public tsl::Gui {
public:
    GuiCheats() { }
    ~GuiCheats() { }

    virtual tsl::Element* createUI() {
        auto rootFrame = new tsl::element::Frame();

        auto header = new tsl::element::CustomDrawer(0, 0, 100, FB_WIDTH, [](u16 x, u16 y, tsl::Screen *screen){
            screen->drawString("Cheats", false, 20, 50, 30, tsl::a(0xFFFF));

            if (edz::cheat::CheatManager::getCheats().size() == 0) {
                screen->drawString("\uE150", false, 180, 250, 90, tsl::a(0xFFFF));
                screen->drawString("No Cheats loaded!", false, 110, 340, 25, tsl::a(0xFFFF));
            }
        });

        auto list = new tsl::element::List();
        
        for (auto &cheat : edz::cheat::CheatManager::getCheats()) {
            auto cheatToggleItem = new tsl::element::ToggleListItem(edz::hlp::limitStringLength(cheat->getName(), 20), cheat->isEnabled());
            cheatToggleItem->setStateChangeListener([&cheat](bool state) { cheat->setState(state); });

            this->m_cheatToggleItems.push_back(cheatToggleItem);
            list->addItem(cheatToggleItem);
        }
        
        rootFrame->addElement(header);
        rootFrame->addElement(list);

        return rootFrame;
    }

    virtual void update() {
        for (u16 i = 0; i < this->m_cheatToggleItems.size(); i++)
            this->m_cheatToggleItems[i]->setState(edz::cheat::CheatManager::getCheats()[i]->isEnabled());
    }

private:
    std::vector<tsl::element::ToggleListItem*> m_cheatToggleItems;
};

class GuiStats : public tsl::Gui {
public:
    GuiStats() { 
        if (hosversionAtLeast(8,0,0)) {
            clkrstOpenSession(&this->m_clkrstSessionCpu, PcvModuleId_CpuBus, 3);
            clkrstOpenSession(&this->m_clkrstSessionGpu, PcvModuleId_GPU, 3);
            clkrstOpenSession(&this->m_clkrstSessionMem, PcvModuleId_EMC, 3);
        }

        smInitialize();
        this->m_ipAddress = gethostid();
        smExit();

        this->m_runningTitleID = edz::cheat::CheatManager::getTitleID();
        this->m_runningProcessID = edz::cheat::CheatManager::getProcessID();
        this->m_runningBuildID = edz::cheat::CheatManager::getBuildID();
        
    }
    ~GuiStats() {
        if (hosversionAtLeast(8,0,0)) {
            clkrstCloseSession(&this->m_clkrstSessionCpu);
            clkrstCloseSession(&this->m_clkrstSessionGpu);
            clkrstCloseSession(&this->m_clkrstSessionMem);
        }
     }

    virtual tsl::Element* createUI() {
        auto rootFrame = new tsl::element::Frame();

        auto infos = new tsl::element::CustomDrawer(0, 0, 100, FB_WIDTH, [this](u16 x, u16 y, tsl::Screen *screen){
            screen->drawString("Stats", false, 20, 50, 30, tsl::a(0xFFFF));

            screen->drawString("Local IP:", false, 35, 120, 18, tsl::a(0xFFFF));

            screen->drawString("CPU Temparature:", false, 35, 160, 18, tsl::a(0xFFFF));
            screen->drawString("PCB Temparature:", false, 35, 190, 18, tsl::a(0xFFFF));

            screen->drawString("CPU Clock:", false, 35, 230, 18, tsl::a(0xFFFF));
            screen->drawString("GPU Clock:", false, 35, 260, 18, tsl::a(0xFFFF));
            screen->drawString("MEM Clock:", false, 35, 290, 18, tsl::a(0xFFFF));

            screen->drawString("WiFi Signal:", false, 35, 330, 18, tsl::a(0xFFFF));

            if (this->m_runningProcessID != 0) {
                screen->drawString("Program ID:", false, 35, 370, 18, tsl::a(0xFFFF));
                screen->drawString("Build ID:", false, 35, 400, 18, tsl::a(0xFFFF));
                screen->drawString("Process ID:", false, 35, 430, 18, tsl::a(0xFFFF));
            }

            if (this->m_ipAddress == INADDR_LOOPBACK)
                screen->drawString("Offline", false, 230, 120, 18, tsl::a(0xFFFF));
            else 
                screen->drawString(edz::hlp::formatString("%d.%d.%d.%d", this->m_ipAddress & 0xFF, (this->m_ipAddress >> 8) & 0xFF, (this->m_ipAddress >> 16) & 0xFF, (this->m_ipAddress >> 24) & 0xFF).c_str(), false, 230, 120, 18, tsl::a(0xFFFF));

            s32 temparature = 0;
            tsGetTemperatureMilliC(TsLocation_Internal, &temparature);
            screen->drawString(edz::hlp::formatString("%.02f °C", temparature / 1000.0F).c_str(), false, 230, 160, 18, tsl::a(0xFFFF));
            tsGetTemperatureMilliC(TsLocation_External, &temparature);
            screen->drawString(edz::hlp::formatString("%.02f °C", temparature / 1000.0F).c_str(), false, 230, 190, 18, tsl::a(0xFFFF));

            u32 cpuClock = 0, gpuClock = 0, memClock = 0;

            if (hosversionAtLeast(8,0,0)) {
                clkrstGetClockRate(&this->m_clkrstSessionCpu, &cpuClock);
                clkrstGetClockRate(&this->m_clkrstSessionGpu, &gpuClock);
                clkrstGetClockRate(&this->m_clkrstSessionMem, &memClock);
            } else {
                pcvGetClockRate(PcvModule_CpuBus, &cpuClock);
                pcvGetClockRate(PcvModule_GPU, &gpuClock);
                pcvGetClockRate(PcvModule_EMC, &memClock);
            }

            screen->drawString(edz::hlp::formatString("%.01f MHz", cpuClock / 1'000'000.0F).c_str(), false, 230, 230, 18, tsl::a(0xFFFF));
            screen->drawString(edz::hlp::formatString("%.01f MHz", gpuClock / 1'000'000.0F).c_str(), false, 230, 260, 18, tsl::a(0xFFFF));
            screen->drawString(edz::hlp::formatString("%.01f MHz", memClock / 1'000'000.0F).c_str(), false, 230, 290, 18, tsl::a(0xFFFF));

            s32 signalStrength = 0;
            wlaninfGetRSSI(&signalStrength);

            screen->drawString(edz::hlp::formatString("%d dBm", signalStrength).c_str(), false, 230, 330, 18, tsl::a(0xFFFF));

            if (this->m_runningProcessID != 0) {
                screen->drawString(edz::hlp::formatString("%016lX", this->m_runningTitleID).c_str(), false, 230, 370, 18, tsl::a(0xFFFF));
                screen->drawString(edz::hlp::formatString("%016lX", this->m_runningBuildID).c_str(), false, 230, 400, 18, tsl::a(0xFFFF));
                screen->drawString(edz::hlp::formatString("%lu", this->m_runningProcessID).c_str(), false, 230, 430, 18, tsl::a(0xFFFF));
            }

 
        });

        rootFrame->addElement(infos);

        return rootFrame;
    }

    virtual void update() { }

private:
    ClkrstSession m_clkrstSessionCpu, m_clkrstSessionGpu, m_clkrstSessionMem;
    int m_ipAddress;

    edz::titleid_t m_runningTitleID;
    edz::processid_t m_runningProcessID;
    edz::buildid_t m_runningBuildID;
};

class GuiMain : public tsl::Gui {
public:
    GuiMain() { }
    ~GuiMain() { }

    virtual tsl::Element* createUI() {
        auto *rootFrame = new tsl::element::Frame();

        auto *header = new tsl::element::CustomDrawer(0, 0, 100, FB_WIDTH, [](u16 x, u16 y, tsl::Screen *screen) {
            screen->drawString("EdiZon", false, 20, 50, 30, tsl::a(0xFFFF));
        });

        auto list = new tsl::element::List();

        auto cheatsItem = new tsl::element::ListItem("Cheats");
        auto statsItem  = new tsl::element::ListItem("Stats");
        cheatsItem->setClickListener([](s64 keys) {
            if (keys & KEY_A) {
                Gui::changeTo<GuiCheats>();
                return true;
            }

            return false;
        });

        statsItem->setClickListener([](s64 keys) {
            if (keys & KEY_A) {
                Gui::changeTo<GuiStats>();
                return true;
            }

            return false;
        });

        list->addItem(cheatsItem);
        list->addItem(statsItem);

        rootFrame->addElement(header);
        rootFrame->addElement(list);

        return rootFrame;
    }

    virtual void update() { }
};

class EdiZonOverlay : public tsl::Overlay {
public:
    EdiZonOverlay() { }
    ~EdiZonOverlay() { }

    tsl::Gui* onSetup() { 
        smInitialize();
        edz::dmntcht::initialize();
        edz::cheat::CheatManager::initialize();
        tsInitialize();
        wlaninfInitialize();
        clkrstInitialize();
        pcvInitialize();

        smExit();

        return new GuiMain();
    }

    void onDestroy() { 
        edz::dmntcht::exit();
        edz::cheat::CheatManager::exit();
        tsExit();
        wlaninfExit();
        clkrstExit();
        pcvExit();
    }

    
};


tsl::Overlay *overlayLoad() {
    return new EdiZonOverlay();
}