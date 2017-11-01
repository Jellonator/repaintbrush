#pragma once
#include "../core/project.h"

#include <vector>
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif
#include <wx/listctrl.h>

namespace gui {
    class GuiMainFrame;
    class GuiWorkspace;

    /// Represents a list of filters in the GUI
    class GuiFilterList : public wxListCtrl {
    public:
        GuiFilterList(wxWindow* parent);

        /// Update this list to match the project
        void update(core::Project& project);

        // overrides
        wxString OnGetItemText(long, long) const;

        std::vector<core::Project::FilterData> m_filters;
    };

    /// A dialog used to ask the user to create a filter.
    class GuiFilterAddDialog : public wxDialog {
    public:
        enum {
            EV_FILTER_CHANGE,
        };
        GuiFilterAddDialog();

        /// Get the resulting filter data from this dialog.
        core::Project::FilterData get_filter_data();

        /// Return if the OK button was pressed.
        bool is_ok();

        /// Update the argument box so that it is disabled if the currently
        /// selected filter does not require an argument.
        void update_text();
    private:
        wxChoice* m_choice_filter;
        wxChoice* m_choice_type;
        wxTextCtrl* m_text_arg;
        core::FilterFactory m_factory;
        bool ok;
        void OnFilterChange(wxCommandEvent& event);
        void OnOk(wxCommandEvent& event);
        void OnCancel(wxCommandEvent& event);
    };

    class GuiWorkspace : public wxPanel {
    public:
        enum {
            EV_FILTER_ADD,
            EV_FILTER_REMOVE,
            EV_INPUT_ADD,
            EV_INPUT_REMOVE,
            EV_IMPORT,
            EV_EXPORT,
        };
        GuiWorkspace(GuiMainFrame* parent, core::Project project);
        core::Project& get_project();
        bool OnInit();
    private:
        GuiFilterList* m_filterbox;
        wxListBox* m_inputbox;
        core::Project m_project;
        GuiMainFrame* m_parent;

        // update functions
        void update_filters();
        void update_inputs();

        // gui create
        void create_buttons(wxBoxSizer* sz_main);
        wxWindow* create_filterlist(wxWindow* parent);
        wxWindow* create_inputlist(wxWindow* parent);

        // events
        void OnFilterAdd(wxCommandEvent& event);
        void OnFilterRemove(wxCommandEvent& event);
        void OnInputAdd(wxCommandEvent& event);
        void OnInputRemove(wxCommandEvent& event);
        void OnImport(wxCommandEvent& event);
        void OnExport(wxCommandEvent& event);
    };
}
