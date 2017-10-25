#pragma once
#include "../core/project.h"

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

namespace gui {
    class GuiMainFrame;

    class GuiWorkspace : public wxPanel
    {
        core::Project m_project;
    public:
        enum {
            EV_FILTERS,
            EV_INPUTS,
            EV_IMPORT,
            EV_EXPORT,
        };
        GuiWorkspace(GuiMainFrame* parent, core::Project project);
    };

    // GuiWorkspace* _workspace(GuiMainFrame* parent, const fs::path& path);
    GuiWorkspace* open_workspace(GuiMainFrame* parent, const fs::path& path);
}
