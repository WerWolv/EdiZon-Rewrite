#include "save/edit/widgets/widget_progressbar.hpp"
#include "helpers/utils.hpp"

namespace edz::save::edit::widget {

    WidgetProgressbar::WidgetProgressbar(std::string name) : Widget(name) {
        this->m_progress = 0;
    }

    WidgetProgressbar::~WidgetProgressbar() {

    }


    WidgetType WidgetProgressbar::getWidgetType() {
        return WidgetType::PROGRESS_BAR;
    }

    View* WidgetProgressbar::getView() {
        if (this->m_widgetView == nullptr) {
            this->m_widgetView = new ProgressDisplay(PROGRESS_DISPLAY_STYLE_PERCENTAGE);
            ProgressDisplay *progressBar = reinterpret_cast<ProgressDisplay*>(this->m_widgetView);
            
            progressBar->setProgress(this->m_progress, 100);
        }

        return this->m_widgetView;
    }

}
