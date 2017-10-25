#include "base.h"
#include <iostream>
#include <sstream>

namespace gui {
    wxIMPLEMENT_APP_NO_MAIN(GuiApp);
    wxBEGIN_EVENT_TABLE(GuiMainFrame, wxFrame)
        EVT_MENU(wxID_NEW,   GuiMainFrame::OnFileNew)
        EVT_MENU(wxID_OPEN,  GuiMainFrame::OnFileOpen)
        EVT_MENU(wxID_CLOSE, GuiMainFrame::OnFileClose)
        EVT_MENU(wxID_EXIT,  GuiMainFrame::OnFileExit)

        EVT_MENU(wxID_ABOUT, GuiMainFrame::OnAbout)
    wxEND_EVENT_TABLE()

    bool GuiApp::OnInit()
    {
        GuiMainFrame* frame = new GuiMainFrame("RepaintBrush", wxSize(640, 480));
        frame->Show(true);
        return true;
    }

    GuiMainFrame::GuiMainFrame(const wxString& name, const wxSize& size)
    : wxFrame(nullptr, wxID_ANY, name, wxDefaultPosition, size)
    {
        // file menu
        wxMenu *menu_file = new wxMenu;
        menu_file->Append(wxID_NEW);
        menu_file->Append(wxID_OPEN);
        m_proj_menus.push_back(menu_file->Append(wxID_CLOSE));
        menu_file->AppendSeparator();
        menu_file->Append(wxID_EXIT);

        // help menu
        wxMenu *menu_help = new wxMenu;
        menu_help->Append(wxID_ABOUT);

        // menu bar
        wxMenuBar *menu = new wxMenuBar;
        menu->Append(menu_file, "&File");
        menu->Append(menu_help, "&Help");

        // initialization
        SetMenuBar(menu);
        CreateStatusBar();
        SetStatusText("Ready.");
        update_gui();
    }

    void GuiMainFrame::update_gui()
    {
        bool do_enable = m_workspace != nullptr;
        for (wxMenuItem* item : m_proj_menus) {
            item->Enable(do_enable);
        }
        // if (m_workspace) {
        Layout();
        Update();
        Refresh();
        // }
    }

    void GuiMainFrame::open_folder(const fs::path& path)
    {
        TRYGUI({
            GuiWorkspace* workspace = open_workspace(this, path);
            if (workspace == nullptr) {
                std::stringstream s;
                s << "Path " << path << " is not a valid project folder.";
                throw s.str();
            }
            this->change_workspace(workspace);
        })
    }

    void GuiMainFrame::new_project(const fs::path& path)
    {
        TRYGUI({
            core::Project p = core::Project::create(path, false);
            GuiWorkspace* workspace = new GuiWorkspace(this, std::move(p));
            change_workspace(workspace);
        })
    }

    void GuiMainFrame::change_workspace(GuiWorkspace* workspace)
    {
        if (m_workspace) {
            m_workspace->Destroy();
        }
        m_workspace = workspace;
        update_gui();
    }

    // file menu
    void GuiMainFrame::OnFileNew(wxCommandEvent& event)
    {
        wxDirDialog* dialog_path = new wxDirDialog(this);

        if (dialog_path->ShowModal() == wxID_OK){
            fs::path path = std::string(dialog_path->GetPath());
            this->new_project(path);
        }
    }
    void GuiMainFrame::OnFileOpen(wxCommandEvent& event)
    {
        wxDirDialog* dialog_path = new wxDirDialog(this);

        if (dialog_path->ShowModal() == wxID_OK){
            fs::path path = std::string(dialog_path->GetPath());
            this->open_folder(path);
        }
    }
    void GuiMainFrame::OnFileClose(wxCommandEvent& event)
    {
        change_workspace(nullptr);
    }
    void GuiMainFrame::OnFileExit(wxCommandEvent& event)
    {
        Close(true);
    }

    void GuiMainFrame::OnAbout(wxCommandEvent& event)
    {
        wxMessageBox("This is Repaintbrush",
                     "About Repaintbrush", wxOK | wxICON_INFORMATION);
    }

    int init(int argc, char** argv)
    {
        return wxEntry(argc, argv);
    }
}
