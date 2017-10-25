#pragma once

#include <wx/wxprec.h>
#include <vector>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "workspace.h"

#define TRYGUI(block) try block catch (std::exception& e) {\
        wxMessageDialog *dial = new wxMessageDialog(nullptr,\
            wxString(e.what()), wxT("Error"), wxOK | wxICON_ERROR);\
        dial->ShowModal();\
    } catch (std::string& s) {\
        wxMessageDialog *dial = new wxMessageDialog(nullptr,\
            wxString(s), wxT("Error"), wxOK | wxICON_ERROR);\
        dial->ShowModal();\
    }

namespace gui {
    int init(int argc, char** argv);

    class GuiApp : public wxApp
    {
    public:
        bool OnInit();
    };

    class GuiMainFrame : public wxFrame
    {
    public:
        GuiMainFrame(const wxString& name, const wxSize& size);
        void update_gui();
    private:
        GuiWorkspace* m_workspace;
        std::vector<wxMenuItem*> m_proj_menus;

        // helper functions
        void open_folder(const fs::path& path);
        void new_project(const fs::path& path);
        void change_workspace(GuiWorkspace* workspace);

        // file menu
        void OnFileNew(wxCommandEvent& event);
        void OnFileOpen(wxCommandEvent& event);
        void OnFileClose(wxCommandEvent& event);
        void OnFileExit(wxCommandEvent& event);

        // project menu
        // nothing yet

        // help menu
        void OnAbout(wxCommandEvent& event);

        wxDECLARE_EVENT_TABLE();
    };
}
