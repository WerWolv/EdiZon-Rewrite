#pragma once

#include <edizon.hpp>

namespace edz::ui {

    class Gui {
    public:
        Gui() { }
        virtual ~Gui() { }

        virtual View* setupUI() = 0;

        virtual void update() = 0;


        static void tick() {
            if (Gui::s_currentGui != nullptr)
                Gui::s_currentGui->update();
        }

        template<typename T, typename... Args>
        static void changeTo(Args... args) {
            if (Gui::s_currentGui != nullptr)
                delete Gui::s_currentGui;
            
            Gui::s_currentGui = new T(args...);

            Application::pushView(Gui::s_currentGui->setupUI());
            Gui::s_guiStackSize++;
        }

        static void goBack() {
            if (s_guiStackSize <= 1)
                return;

            Application::popView();
            Gui::s_guiStackSize--;
        }

        static void fatal(std::string error) {
            Application::crash(error);
            while (Application::mainLoop());
        }

    private:
        static inline Gui *s_currentGui = nullptr;
        static inline u8 s_guiStackSize = 0;
    };

}