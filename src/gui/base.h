#pragma once

#include <gtkmm/button.h>
#include <gtkmm/window.h>

namespace gui {
    class Base : public Gtk::Window
    {

    public:
        Base();
        virtual ~Base();

    protected:
        //Signal handlers:
        void on_button_clicked();

        //Member widgets:
        Gtk::Button m_button;
    };
}
