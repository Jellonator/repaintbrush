#include "workspace.h"
#include "base.h"
#include <iostream>
#include <sstream>
#include <wx/splitter.h>

namespace gui {
    // Filter Add Dialog
    GuiFilterAddDialog::GuiFilterAddDialog()
    : wxDialog(nullptr, wxID_ANY, "Add a filter"), m_factory(), ok(false)
    {
        int alignleft = wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT;
        int alignright = wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT;
        wxBoxSizer* sz_main = new wxBoxSizer(wxVERTICAL);
        wxBoxSizer* sz_btn = new wxBoxSizer(wxHORIZONTAL);
        wxGridSizer* sz = new wxGridSizer(3, 2, 8, 8);

        sz->Add(new wxStaticText(this, -1, "Filter"), 0, alignright);
        m_choice_filter = new wxChoice(this, GuiFilterAddDialog::EV_FILTER_CHANGE);
        for (const auto& element : m_factory.get_filters()) {
            m_choice_filter->Append(element.first);
        }
        m_choice_filter->SetSelection(0);
        sz->Add(m_choice_filter, 0, alignleft);

        sz->Add(new wxStaticText(this, -1,
            "Type"), 0, alignright);
        m_choice_type = new wxChoice(this, wxID_ANY);
        m_choice_type->Append("Input");
        m_choice_type->Append("Output");
        m_choice_type->SetSelection(0);
        sz->Add(m_choice_type, 0, alignleft);

        sz->Add(new wxStaticText(this, -1, "Arg"), 0, alignright);
        m_text_arg = new wxTextCtrl(this, -1);
        sz->Add(m_text_arg, 0, alignleft);

        Connect(GuiFilterAddDialog::EV_FILTER_CHANGE, wxEVT_CHOICE,
            wxCommandEventHandler(GuiFilterAddDialog::OnFilterChange));

        wxButton* btn_ok = new wxButton(this, wxID_OK);
        wxButton* btn_cancel = new wxButton(this, wxID_CANCEL);
        sz_btn->Add(btn_ok);
        sz_btn->Add(btn_cancel);

        Connect(wxID_OK, wxEVT_COMMAND_BUTTON_CLICKED,
            wxCommandEventHandler(GuiFilterAddDialog::OnOk));
        Connect(wxID_CANCEL, wxEVT_COMMAND_BUTTON_CLICKED,
            wxCommandEventHandler(GuiFilterAddDialog::OnCancel));

        sz_main->Add(sz, 0, wxTOP | wxALIGN_CENTER_HORIZONTAL);
        sz_main->AddStretchSpacer();
        sz_main->Add(sz_btn, 0, wxBOTTOM | wxALIGN_RIGHT);

        SetSizer(sz_main);
        update_text();
    }

    void GuiFilterAddDialog::update_text()
    {
        std::string selection = std::string(m_choice_filter->GetStringSelection());
        const auto& def = m_factory.get_filters().at(selection);
        m_text_arg->Enable(def.has_arg);
    }

    void GuiFilterAddDialog::OnFilterChange(wxCommandEvent& event)
    {
        update_text();
    }

    void GuiFilterAddDialog::OnOk(wxCommandEvent& event)
    {
        ok = true;
        EndModal(wxID_OK);
    }

    void GuiFilterAddDialog::OnCancel(wxCommandEvent& event)
    {
        ok = false;
        EndModal(wxID_CANCEL);
    }

    core::Project::FilterData GuiFilterAddDialog::get_filter_data()
    {
        std::string name, arg;
        name = std::string(m_choice_filter->GetStringSelection());
        if (m_text_arg->IsEnabled()) {
            arg = std::string(m_text_arg->GetValue());
        }
        // input = 0, output = 1
        return core::Project::FilterData {
            m_factory.create(name, arg),
            static_cast<core::Project::filter_t>(m_choice_type->GetSelection()),
            0
        };
    }

    bool GuiFilterAddDialog::is_ok()
    {
        return ok;
    }

    // Filter List
    GuiFilterList::GuiFilterList(wxWindow* parent)
    : wxListCtrl(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
    wxLC_REPORT|wxLC_VIRTUAL|wxLC_SINGLE_SEL)
    {
        this->AppendColumn("Name");
        this->AppendColumn("Type");
        this->AppendColumn("Argument");
    }

    void GuiFilterList::update(core::Project& project)
    {
        auto filters = project.get_filters();
        m_filters = {std::make_move_iterator(std::begin(filters)),
                     std::make_move_iterator(std::end(filters))};
        DeleteAllItems();
        SetItemCount(m_filters.size());
    }

    wxString GuiFilterList::OnGetItemText(long item, long column) const
    {
        if (column == 0) {
            return wxString(this->m_filters[item].filter.get_name());
        } else if (column == 1) {
            return wxString(core::get_ftype_name(this->m_filters[item].type));
        } else {
            return wxString(this->m_filters[item].filter.serialize());
        }
    }

    // Workspace
    void GuiWorkspace::create_buttons(wxBoxSizer* sz_main)
    {
        wxBoxSizer* sz_buttons = new wxBoxSizer(wxHORIZONTAL);
        wxButton* btn_import = new wxButton(this,
            GuiWorkspace::EV_IMPORT, "Import");
        wxButton* btn_export = new wxButton(this,
            GuiWorkspace::EV_EXPORT, "Export");
        sz_buttons->Add(btn_import, 0, 0, 16);
        sz_buttons->Add(btn_export, 0, 0, 16);
        sz_main->Add(sz_buttons, 0, wxBOTTOM | wxALIGN_CENTER, 0);

        Connect(GuiWorkspace::EV_IMPORT,
            wxEVT_COMMAND_BUTTON_CLICKED,
            wxCommandEventHandler(GuiWorkspace::OnImport));
        Connect(GuiWorkspace::EV_EXPORT,
            wxEVT_COMMAND_BUTTON_CLICKED,
            wxCommandEventHandler(GuiWorkspace::OnExport));
    }

    wxWindow* GuiWorkspace::create_filterlist(wxWindow* parent)
    {
        wxPanel* panel = new wxPanel(parent, wxID_ANY);

        wxBoxSizer* sz_buttons = new wxBoxSizer(wxHORIZONTAL);
        wxButton* btn_add = new wxButton(panel,
            GuiWorkspace::EV_FILTER_ADD, "Add Filter");
        wxButton* btn_remove = new wxButton(panel,
            GuiWorkspace::EV_FILTER_REMOVE, "Remove Filter");
        sz_buttons->Add(btn_add, 0, 0, 16);
        sz_buttons->Add(btn_remove, 0, 0, 16);

        GuiFilterList* filters = new GuiFilterList(panel);

        wxBoxSizer* sz_filters = new wxBoxSizer(wxVERTICAL);
        sz_filters->Add(sz_buttons, 0, wxTOP | wxALIGN_LEFT, 16);
        sz_filters->Add(filters, 1, wxEXPAND, 16);

        Connect(GuiWorkspace::EV_FILTER_ADD,
            wxEVT_COMMAND_BUTTON_CLICKED,
            wxCommandEventHandler(GuiWorkspace::OnFilterAdd));
        Connect(GuiWorkspace::EV_FILTER_REMOVE,
            wxEVT_COMMAND_BUTTON_CLICKED,
            wxCommandEventHandler(GuiWorkspace::OnFilterRemove));

        panel->SetSizer(sz_filters);
        m_filterbox = filters;
        return panel;
    }

    wxWindow* GuiWorkspace::create_inputlist(wxWindow* parent)
    {
        wxPanel* panel = new wxPanel(parent, wxID_ANY);

        wxBoxSizer* sz_buttons = new wxBoxSizer(wxHORIZONTAL);
        wxButton* btn_add = new wxButton(panel,
            GuiWorkspace::EV_INPUT_ADD, "Add Input");
        wxButton* btn_remove = new wxButton(panel,
            GuiWorkspace::EV_INPUT_REMOVE, "Remove Input");
        sz_buttons->Add(btn_add, 0, 0, 16);
        sz_buttons->Add(btn_remove, 0, 0, 16);

        wxListBox* filters = new wxListBox(panel, wxID_ANY);

        wxBoxSizer* sz_filters = new wxBoxSizer(wxVERTICAL);
        sz_filters->Add(sz_buttons, 0, wxTOP | wxALIGN_LEFT, 16);
        sz_filters->Add(filters, 1, wxEXPAND, 16);

        Connect(GuiWorkspace::EV_INPUT_ADD,
            wxEVT_COMMAND_BUTTON_CLICKED,
            wxCommandEventHandler(GuiWorkspace::OnInputAdd));
        Connect(GuiWorkspace::EV_INPUT_REMOVE,
            wxEVT_COMMAND_BUTTON_CLICKED,
            wxCommandEventHandler(GuiWorkspace::OnInputRemove));

        panel->SetSizer(sz_filters);
        m_inputbox = filters;
        return panel;
    }

    GuiWorkspace::GuiWorkspace(GuiMainFrame* parent, core::Project project)
    : wxPanel(parent, wxID_ANY), m_project(std::move(project)) {
        m_parent = parent;
        this->OnInit();
    }

    bool GuiWorkspace::OnInit()
    {
        wxBoxSizer* sz_main = new wxBoxSizer(wxVERTICAL);

        wxSplitterWindow* split = new wxSplitterWindow(this, wxID_ANY,
            wxDefaultPosition, wxDefaultSize, wxSP_LIVE_UPDATE);
        sz_main->Add(split, 1, wxEXPAND, 16);
        split->SetSashGravity(0.5);
        split->SetMinimumPaneSize(50);

        wxWindow* filterbox = create_filterlist(split);
        wxWindow* inputbox = create_inputlist(split);
        split->SplitVertically(filterbox, inputbox, 0);
        create_buttons(sz_main);

        SetSizer(sz_main);

        update_filters();
        update_inputs();

        return true;
    }

    core::Project& GuiWorkspace::get_project()
    {
        return m_project;
    }

    void GuiWorkspace::update_filters()
    {
        m_filterbox->update(this->m_project);
    }

    void GuiWorkspace::update_inputs()
    {
        m_inputbox->Clear();
        for (const auto& path : m_project.list_input_folders()) {
            m_inputbox->Append(path.c_str());
        }
    }

    void GuiWorkspace::OnFilterAdd(wxCommandEvent& event)
    {
        TRYGUI({
            GuiFilterAddDialog dialog;
            dialog.ShowModal();
            if (dialog.is_ok()) {
                auto data = dialog.get_filter_data();
                m_project.add_filter(data.type, data.filter);
                m_parent->SetStatusText("Added a filter.");
                this->update_filters();
            }
        })
    }

    void GuiWorkspace::OnFilterRemove(wxCommandEvent& event)
    {
        TRYGUI({
            int selection = m_filterbox->GetNextItem(
                -1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
            if (selection == -1) {
                throw "No filter selected.";
            }
            wxMessageDialog dialog(NULL,
                wxT("Are you sure you want to delete this filter?"),
                wxT("Hold up"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION);
            if (dialog.ShowModal() == wxID_YES) {
                int id = m_filterbox->m_filters[selection].id;
                m_project.remove_filter(id);
                m_parent->SetStatusText("Removed a filter.");
                this->update_filters();
            }
        })
    }

    void GuiWorkspace::OnInputAdd(wxCommandEvent& event)
    {
        TRYGUI({
            wxDirDialog* dialog_path = new wxDirDialog(this,
                "Select an input folder.");

            if (dialog_path->ShowModal() == wxID_OK){
                fs::path path = std::string(dialog_path->GetPath());
                if (this->m_project.add_inputfolder(path)) {
                    throw "Selected input folder already exists.";
                }
                update_inputs();
            }
        })
    }

    void GuiWorkspace::OnInputRemove(wxCommandEvent& event)
    {
        TRYGUI({
            wxArrayInt selections;
            //only one should be selected
            m_inputbox->GetSelections(selections);
            if (selections.GetCount() == 0) {
                throw "No inputs selected.";
            }
            wxMessageDialog dialog(NULL,
                wxT("Are you sure you want to remove this input?"),
                wxT("Hold up"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION);
            if (dialog.ShowModal() == wxID_YES) {
                fs::path p = std::string(m_inputbox->GetStringSelection());
                if (m_project.remove_inputfolder(p)) {
                    throw "Selected input folder could not be removed.\n"
                    "This error should not occur,\n"
                    "please submit a detailed bug report.";
                }
                update_inputs();
                m_parent->SetStatusText("Removed an input.");
            }
        })
    }

    void GuiWorkspace::OnImport(wxCommandEvent& event)
    {
        bool success = false;
        TRYGUI({
            wxDirDialog* dialog_path = new wxDirDialog(this,
                "Select an import folder.", m_project.get_path().string());

            if (dialog_path->ShowModal() == wxID_OK){
                fs::path path = std::string(dialog_path->GetPath());
                m_parent->SetStatusText("Importing...");
                auto result = m_project.import(path, {});
                std::stringstream s;
                if (result.folders == 0) {
                    s << "No import folders to import from.";
                } else if (result.files > 0) {
                    s << "Successfully imported " << result.files <<
                        " new files, with " << result.filtered <<
                        " files excluded.";
                } else {
                    s << "No new files to import.";
                }
                m_parent->SetStatusText(s.str());
                wxMessageDialog dial(nullptr,\
                    s.str(), wxT("Success"), wxOK | wxICON_INFORMATION);\
                dial.ShowModal();\
                success = true;
            }
        })
        if (!success) {
            m_parent->SetStatusText("Import failed.");
        }
    }

    void GuiWorkspace::OnExport(wxCommandEvent& event)
    {
        bool success = false;
        TRYGUI({
            wxDirDialog* dialog_path = new wxDirDialog(this,
                "Select an export folder.", m_project.get_path().string());


            if (dialog_path->ShowModal() == wxID_OK){
                fs::path path = std::string(dialog_path->GetPath());
                if (core::is_path_within_path(path, m_project.get_path())) {
                    throw "You should not export to a folder within the project"
                    " directory";
                }

                m_parent->SetStatusText("Exporting...");
                // auto result = m_project.import(path, {});
                auto result = m_project.export_to_folder(path);
                std::stringstream s;
                 if (result.files > 0) {
                    s << "Successfully exported " << result.files <<
                        " files, with " << result.filtered <<
                        " files excluded.";
                } else {
                    s << "No existing files to export.";
                }
                m_parent->SetStatusText(s.str());
                wxMessageDialog dial(nullptr,\
                    s.str(), wxT("Success"), wxOK | wxICON_INFORMATION);\
                dial.ShowModal();\
                success = true;
            }
        })
        if (!success) {
            m_parent->SetStatusText("Export failed.");
        }
    }
}
