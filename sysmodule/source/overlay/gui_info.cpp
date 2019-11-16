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

#include "overlay/gui_info.hpp"
#include "overlay/gui_main.hpp"
#include "overlay/styles.hpp"
#include "cheat/cheat.hpp"
#include "helpers/utils.hpp"

namespace edz::ovl {
    
    GuiInfo::GuiInfo() {
        if (hosversionAtLeast(8,0,0)) {
            clkrstOpenSession(&this->m_cpuClkSession, PcvModuleId_CpuBus, 3);
            clkrstOpenSession(&this->m_gpuClkSession, PcvModuleId_GPU, 3);
            clkrstOpenSession(&this->m_memClkSession, PcvModuleId_EMC, 3);
        }
    }

    GuiInfo::~GuiInfo() {
        lv_obj_del(this->m_titleLabel);
        lv_obj_del(this->m_btnBack);

        for (auto& infoLabel : this->m_infoLabels)
            lv_obj_del(infoLabel);
        
        if (hosversionAtLeast(8,0,0)) {
            clkrstCloseSession(&this->m_cpuClkSession);
            clkrstCloseSession(&this->m_gpuClkSession);
            clkrstCloseSession(&this->m_memClkSession);
        }
    }

    void GuiInfo::createUI() {
        setBackgroundStyle();

        this->m_titleLabel = lv_label_create(lv_scr_act(), nullptr);
        lv_obj_set_pos(this->m_titleLabel, 10, 10);
        lv_label_set_text(this->m_titleLabel, "System Information");
        setLabelTitleStyle(this->m_titleLabel);

        u16 y = 50;
        for (auto& infoLabel : this->m_infoLabels) {
            infoLabel = lv_label_create(lv_scr_act(), nullptr);
            setLabelStyle(infoLabel);
            lv_obj_set_pos(infoLabel, 20, y);

            y += 30;
        }

        this->m_btnBack = lv_btn_create(lv_scr_act(), nullptr);
        lv_obj_t *lblBack = lv_label_create(this->m_btnBack, nullptr);
        setLabelStyle(lblBack);
        lv_label_set_text(lblBack, "Back");
        lv_obj_set_pos(this->m_btnBack, 50, 470);
        lv_obj_set_size(this->m_btnBack, 156, 40);
        setButtonStyle(this->m_btnBack);
        lv_obj_set_event_cb(this->m_btnBack, [](lv_obj_t * obj, lv_event_t event) {
            if (event != LV_EVENT_CLICKED)
                return;

            Gui::changeTo<GuiMain>();
        });
    }

    void GuiInfo::update() {
        if (Gui::getFrameCount() % 100) {
            s32 cpuTemp = 0, pcbTemp = 0;
            u32 cpuClk = 0, gpuClk = 0, memClk = 0;
            tsGetTemperatureMilliC(TsLocation_Internal, &cpuTemp);
            tsGetTemperatureMilliC(TsLocation_External, &pcbTemp);
            

            if (hosversionBefore(8,0,0)) {
                pcvGetClockRate(PcvModule_CpuBus, &cpuClk);
                pcvGetClockRate(PcvModule_GPU, &gpuClk);
                pcvGetClockRate(PcvModule_EMC, &memClk);
            }
            else {
                clkrstGetClockRate(&this->m_cpuClkSession, &cpuClk);
                clkrstGetClockRate(&this->m_gpuClkSession, &gpuClk);
                clkrstGetClockRate(&this->m_memClkSession, &memClk);
            }

            lv_label_set_text(this->m_infoLabels[0], hlp::formatString("CPU Temparature: %.1f'C", cpuTemp / 1'000.0F).c_str());
            lv_label_set_text(this->m_infoLabels[1], hlp::formatString("PCB Temparature: %.1f'C", pcbTemp / 1'000.0F).c_str());

            lv_label_set_text(this->m_infoLabels[2], hlp::formatString("CPU Clock: %uMHz", cpuClk / 1'000'000).c_str());
            lv_label_set_text(this->m_infoLabels[3], hlp::formatString("GPU Clock: %uMHz", gpuClk / 1'000'000).c_str());
            lv_label_set_text(this->m_infoLabels[4], hlp::formatString("MEM Clock: %uMHz", memClk / 1'000'000).c_str());

            lv_label_set_text(this->m_infoLabels[5], hlp::formatString("Operation Mode: %s", appletGetOperationMode() == AppletOperationMode_Handheld ? "Handheld" : "Docked").c_str());
        }
    }

}