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

#define TESLA_INIT_IMPL
#include <tesla.hpp>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <switch.h>
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

    virtual tsl::elm::Element* createUI() override {
        auto rootFrame = new tsl::elm::OverlayFrame("EdiZon", "Cheats");

        if (edz::cheat::CheatManager::getCheats().size() == 0) {
            auto warning = new tsl::elm::CustomDrawer([](tsl::gfx::Renderer *renderer, u16 x, u16 y, u16 w, u16 h){
                renderer->drawString("\uE150", false, 180, 250, 90, renderer->a(0xFFFF));
                renderer->drawString("No Cheats loaded!", false, 110, 340, 25, renderer->a(0xFFFF));
            });

            rootFrame->setContent(warning);

        } else {
            auto list = new tsl::elm::List();
            
            for (auto &cheat : edz::cheat::CheatManager::getCheats()) {
                auto cheatToggleItem = new tsl::elm::ToggleListItem(edz::hlp::limitStringLength(cheat->getName(), 20), cheat->isEnabled());
                cheatToggleItem->setStateChangedListener([&cheat](bool state) { cheat->setState(state); });

                this->m_cheatToggleItems.push_back(cheatToggleItem);
                list->addItem(cheatToggleItem);
            }

            rootFrame->setContent(list);

        }

        return rootFrame;
    }

    virtual void update() {
        for (u16 i = 0; i < this->m_cheatToggleItems.size(); i++)
            this->m_cheatToggleItems[i]->setState(edz::cheat::CheatManager::getCheats()[i]->isEnabled());
    }

private:
    std::vector<tsl::elm::ToggleListItem*> m_cheatToggleItems;
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

    virtual tsl::elm::Element* createUI() override {
        auto rootFrame = new tsl::elm::OverlayFrame("EdiZon", "Stats");

        auto infos = new tsl::elm::CustomDrawer([this](tsl::gfx::Renderer *renderer, u16 x, u16 y, u16 w, u16 h){
            renderer->drawString("Local IP:", false, 35, 120, 18, renderer->a(0xFFFF));

            renderer->drawString("CPU Temparature:", false, 35, 160, 18, renderer->a(0xFFFF));
            renderer->drawString("PCB Temparature:", false, 35, 190, 18, renderer->a(0xFFFF));

            renderer->drawString("CPU Clock:", false, 35, 230, 18, renderer->a(0xFFFF));
            renderer->drawString("GPU Clock:", false, 35, 260, 18, renderer->a(0xFFFF));
            renderer->drawString("MEM Clock:", false, 35, 290, 18, renderer->a(0xFFFF));

            renderer->drawString("WiFi Signal:", false, 35, 330, 18, renderer->a(0xFFFF));

            if (this->m_runningProcessID != 0) {
                renderer->drawString("Program ID:", false, 35, 370, 18, renderer->a(0xFFFF));
                renderer->drawString("Build ID:", false, 35, 400, 18, renderer->a(0xFFFF));
                renderer->drawString("Process ID:", false, 35, 430, 18, renderer->a(0xFFFF));
            }

            if (this->m_ipAddress == INADDR_LOOPBACK)
                renderer->drawString("Offline", false, 230, 120, 18, renderer->a(0xFFFF));
            else 
                renderer->drawString(edz::hlp::formatString("%d.%d.%d.%d", this->m_ipAddress & 0xFF, (this->m_ipAddress >> 8) & 0xFF, (this->m_ipAddress >> 16) & 0xFF, (this->m_ipAddress >> 24) & 0xFF).c_str(), false, 230, 120, 18, renderer->a(0xFFFF));

            s32 temparature = 0;
            tsGetTemperatureMilliC(TsLocation_Internal, &temparature);
            renderer->drawString(edz::hlp::formatString("%.02f °C", temparature / 1000.0F).c_str(), false, 230, 160, 18, renderer->a(0xFFFF));
            tsGetTemperatureMilliC(TsLocation_External, &temparature);
            renderer->drawString(edz::hlp::formatString("%.02f °C", temparature / 1000.0F).c_str(), false, 230, 190, 18, renderer->a(0xFFFF));

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

            renderer->drawString(edz::hlp::formatString("%.01f MHz", cpuClock / 1'000'000.0F).c_str(), false, 230, 230, 18, renderer->a(0xFFFF));
            renderer->drawString(edz::hlp::formatString("%.01f MHz", gpuClock / 1'000'000.0F).c_str(), false, 230, 260, 18, renderer->a(0xFFFF));
            renderer->drawString(edz::hlp::formatString("%.01f MHz", memClock / 1'000'000.0F).c_str(), false, 230, 290, 18, renderer->a(0xFFFF));

            s32 signalStrength = 0;
            wlaninfGetRSSI(&signalStrength);

            renderer->drawString(edz::hlp::formatString("%d dBm", signalStrength).c_str(), false, 230, 330, 18, renderer->a(0xFFFF));

            if (this->m_runningProcessID != 0) {
                renderer->drawString(edz::hlp::formatString("%016lX", this->m_runningTitleID).c_str(), false, 230, 370, 18, renderer->a(0xFFFF));
                renderer->drawString(edz::hlp::formatString("%016lX", this->m_runningBuildID).c_str(), false, 230, 400, 18, renderer->a(0xFFFF));
                renderer->drawString(edz::hlp::formatString("%lu", this->m_runningProcessID).c_str(), false, 230, 430, 18, renderer->a(0xFFFF));
            }

 
        });

        rootFrame->setContent(infos);

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

    virtual tsl::elm::Element* createUI() {
        auto *rootFrame = new tsl::elm::OverlayFrame("EdiZon", "");

        auto list = new tsl::elm::List();

        auto cheatsItem = new tsl::elm::ListItem("Cheats");
        auto statsItem  = new tsl::elm::ListItem("Stats");
        cheatsItem->setClickListener([](s64 keys) {
            if (keys & KEY_A) {
                tsl::changeTo<GuiCheats>();
                return true;
            }

            return false;
        });

        statsItem->setClickListener([](s64 keys) {
            if (keys & KEY_A) {
                tsl::changeTo<GuiStats>();
                return true;
            }

            return false;
        });

        list->addItem(cheatsItem);
        list->addItem(statsItem);

        rootFrame->setContent(list);

        return rootFrame;
    }

    virtual void update() { }
};

class EdiZonOverlay : public tsl::Overlay {
public:
    EdiZonOverlay() { }
    ~EdiZonOverlay() { }

    void initServices() override {
        edz::dmntcht::initialize();
        edz::cheat::CheatManager::initialize();
        tsInitialize();
        wlaninfInitialize();
        clkrstInitialize();
        pcvInitialize();

    } 

    virtual void exitServices() override {
        edz::dmntcht::exit();
        edz::cheat::CheatManager::exit();
        tsExit();
        wlaninfExit();
        clkrstExit();
        pcvExit();
    }

    std::unique_ptr<tsl::Gui> loadInitialGui() override {
        return initially<GuiMain>();
    }

    
};


int main(int argc, char **argv) {
    return tsl::loop<EdiZonOverlay>(argc, argv);
}