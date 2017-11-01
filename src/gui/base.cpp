#include "base.h"
#include <iostream>
#include <sstream>

namespace gui {
    const char* helptext =
"Welcome to Repaintbrush!\n"
"\n"
"Repaintbrush is a tool for managing images. Specifically, it is used for "
"'repainting' video games; that is, replacing almost every texture in a game "
"with gags for the sake of comedy. Generally with emulators, this means "
"playing through a game to 'dump' textures as they are encountered in game. "
"This tool makes organizing these 'dumped' textures much easier, as they can "
"be sorted as they are found.\n"
"\n"
"Go to File->New to create a new project, or use File->Open to open an existing"
" project. When you are done, you can use File->Close to close the opened "
"project, or use File->Exit to close the application.\n"
"\n"
"An 'input' is a folder where images are pulled from. You can add an input by "
"clicking on the 'Add Input' button. You can remove inputs by selecting an "
"input from the list and clicking the 'Remove Input' button.\n"
"\n"
"A 'filter' is a way to only include images that you want, and ignore the "
"images that you don't care about. For example, a mipmap filter will remove "
"mipmap images. Filters come in two flavors: input filters, and output "
"filters. An input filter will prevent images from being imported, and an "
"output filter will prevent images from being exported.\n"
"\n"
"Click on the 'Import' button when you are ready to import images. Images will "
"be imported from all inputs. Only images that have not already been imported "
"and are not filtered by any input filters will be included. When you import "
"images, you will be prompted to select a folder to import the images into.\n"
"\n"
"Click on the 'Export' button when you want to export images. All images in "
"your project folder that have been previously imported and do not match any "
"output filters will be exported. Note that this means that if you put an "
"image into the project folder yourself, it will not be registered and will "
"therefor not be included in the exporting process.";

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
        GuiMainFrame* frame = new GuiMainFrame("RepaintBrush", wxDefaultSize);
        frame->Show(true);
        return true;
    }

    GuiMainFrame::GuiMainFrame(const wxString& name, const wxSize& size)
    : wxFrame(nullptr, wxID_ANY, name, wxDefaultPosition, size), m_workspace(nullptr)
    {
        // panel
        m_sizer = new wxGridSizer(1, 1, 0, 0);

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
        SetMenuBar(menu);

        // initialization
        SetSizer(m_sizer);
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
        m_sizer->RecalcSizes();
        m_sizer->Layout();
        Refresh();
    }

    void GuiMainFrame::open_folder(const fs::path& path)
    {
        TRYGUI({
            if (m_workspace && path == m_workspace->get_project().get_path()) {
                wxMessageDialog dialog(nullptr,
                    wxString("That project is already open"), wxT("Warning"),
                    wxOK | wxICON_EXCLAMATION);
                dialog.ShowModal();
                return;
            }
            core::Project project = core::Project::connect(path, false);
            auto check = project.check();
            if (check.size() > 0) {
                std::stringstream s;
                s << check.size() << " file";
                if (check.size() > 1) {
                    s << "s";
                }
                s << "have been removed since the last time this project was opened";
                wxMessageDialog dialog(nullptr,
                    wxString(s.str()), wxT("Warning"), wxOK | wxICON_EXCLAMATION);
                dialog.ShowModal();
            }
            change_project(std::move(project));
        })
    }

    void GuiMainFrame::new_project(const fs::path& path)
    {
        TRYGUI({
            core::Project project = core::Project::create(path, false);
            change_project(std::move(project));
        })
    }

    void GuiMainFrame::change_project(core::Project project)
    {
        if (m_workspace) {
            m_workspace->Hide();
            m_workspace->Destroy();
        }
        GuiWorkspace* workspace = new GuiWorkspace(this, std::move(project));
        m_workspace = workspace;
        m_sizer->Add(workspace, 1, wxEXPAND | wxALL, 0);
        update_gui();
    }

    void GuiMainFrame::remove_project()
    {
        if (m_workspace) {
            m_workspace->Hide();
            m_workspace->Destroy();
        }
        m_workspace = nullptr;
        update_gui();
    }

    // file menu
    void GuiMainFrame::OnFileNew(wxCommandEvent& event)
    {
        wxDirDialog* dialog_path = new wxDirDialog(this, "Create a new project");

        if (dialog_path->ShowModal() == wxID_OK){
            fs::path path = std::string(dialog_path->GetPath());
            this->new_project(path);
        }
    }
    void GuiMainFrame::OnFileOpen(wxCommandEvent& event)
    {
        wxDirDialog* dialog_path = new wxDirDialog(this, "Open an existing project");

        if (dialog_path->ShowModal() == wxID_OK){
            fs::path path = std::string(dialog_path->GetPath());
            this->open_folder(path);
        }
    }
    void GuiMainFrame::OnFileClose(wxCommandEvent& event)
    {
        TRYGUI({
            remove_project();
        })
    }
    void GuiMainFrame::OnFileExit(wxCommandEvent& event)
    {
        Close(true);
    }

    void GuiMainFrame::OnAbout(wxCommandEvent& event)
    {
        wxMessageBox(helptext,
                     "About Repaintbrush", wxOK | wxICON_INFORMATION);
    }

    int init(int argc, char** argv)
    {
        return wxEntry(argc, argv);
    }
}
