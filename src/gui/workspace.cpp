#include "workspace.h"
#include "base.h"

namespace gui {
    GuiWorkspace::GuiWorkspace(GuiMainFrame* parent, core::Project project)
    : wxPanel(parent, -1), m_project(std::move(project)) {
        wxBoxSizer* sz_main = new wxBoxSizer(wxVERTICAL);
        wxBoxSizer* sz_filterinputs = new wxBoxSizer(wxHORIZONTAL);
        wxBoxSizer* sz_filters = new wxBoxSizer(wxVERTICAL);
        wxBoxSizer* sz_inputs = new wxBoxSizer(wxVERTICAL);
        wxBoxSizer* sz_buttons = new wxBoxSizer(wxHORIZONTAL);

        wxButton* btn_import = new wxButton(this, -1, "Import");
        wxButton* btn_export = new wxButton(this, -1, "Export");

        sz_filterinputs->Add(new wxPanel(this, -1));
        sz_inputs->Add(new wxPanel(this, -1));
        sz_filterinputs->Add(sz_filters);
        sz_filterinputs->Add(sz_inputs);
        sz_main->Add(sz_filterinputs, 1, wxEXPAND);

        sz_buttons->Add(btn_import);
        sz_buttons->Add(btn_export);
        sz_main->Add(sz_buttons, 0, wxBOTTOM | wxALIGN_CENTER, 10);

        sz_main->SetSizeHints(parent);
        SetSizer(sz_main);
    }

    GuiWorkspace* open_workspace(GuiMainFrame* parent, const fs::path& path)
    {
        auto project = core::open_project(path, false);
        if (!project) {
            return nullptr;
        }
        return new GuiWorkspace(parent, std::move(*project));
    }
}
