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

#include "helpers/background_tasks.hpp"
#include "api/edizon_api.hpp"
#include "helpers/config_manager.hpp"

#include <borealis.hpp>

namespace edz::hlp {

    BackgroundTasks::BackgroundTasks() {
        this->m_running = true;

        this->m_notificationPollerThread = std::thread(&BackgroundTasks::pollNotifications, this);
    }

    BackgroundTasks::~BackgroundTasks() {
        this->m_running = false;

        this->m_notificationPollerThread.join();
    }


    // Task that checks once every minute if there are new notifications on the server
    void BackgroundTasks::pollNotifications() {
        std::this_thread::sleep_for(2s);

        api::EdiZonAPI edizonApi;

        auto lastRun = std::chrono::high_resolution_clock::time_point(0ms);

        while (this->m_running) {
            if (std::chrono::high_resolution_clock::now() - lastRun > 1min) {
                lastRun = std::chrono::high_resolution_clock::now();
                auto [result, notifications] = edizonApi.getNotifications();

                // If notifications can't be loaded, don't try again till next restart
                if (result.failed())
                    break;

                for (auto &notification : notifications) {
                    auto &notificationDates = GET_CONFIG(Online.notificationDates);
                    if (std::find(notificationDates.begin(), notificationDates.end(), notification.date) == notificationDates.end()) {
                        notificationDates.push_back(notification.date);
                        ConfigManager::store();
                        
                        brls::Application::notify(notification.title);
                        std::this_thread::sleep_for(5s);
                    }
                }
            }

            std::this_thread::sleep_for(100ms);
        }
    }

}