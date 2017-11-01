#pragma once

#include <wx/wxprec.h>
#include <vector>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "workspace.h"

/// This macro will execute a block of statements, and if an exception is
/// thrown, it will create a dialog explaining the error instead of outright
/// crashing. This is usually better I assume.
#define TRYGUI(block) try block catch (std::exception& e) {\
        wxMessageDialog dial(nullptr,\
            wxString(e.what()), wxT("Error"), wxOK | wxICON_ERROR);\
        dial.ShowModal();\
    } catch (std::string& s) {\
        wxMessageDialog dial(nullptr,\
            wxString(s), wxT("Error"), wxOK | wxICON_ERROR);\
        dial.ShowModal();\
    } catch (wxString& s) {\
        wxMessageDialog dial(nullptr,\
            s, wxT("Error"), wxOK | wxICON_ERROR);\
        dial.ShowModal();\
    } catch (const char* s) {\
        wxMessageDialog dial(nullptr,\
            s, wxT("Error"), wxOK | wxICON_ERROR);\
        dial.ShowModal();\
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
        wxSizer* m_sizer;
        std::vector<wxMenuItem*> m_proj_menus;

        // helper functions
        void open_folder(const fs::path& path);
        void new_project(const fs::path& path);
        void change_project(core::Project project);
        void remove_project();

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
