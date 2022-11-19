#include <wx/wx.h>
#include <wx/protocol/ftp.h>

#include <sstream>
#include <iostream>

class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};
class MyFrame : public wxFrame
{
public:
    MyFrame(const wxString &title, const wxPoint &pos, const wxSize &size);

private:
    void DownloadData();
};

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
    MyFrame *frame = new MyFrame("Hello World", wxDefaultPosition, wxDefaultSize);
    frame->Show(true);
    return true;
}

MyFrame::MyFrame(const wxString &title, const wxPoint &pos, const wxSize &size)
    : wxFrame(NULL, wxID_ANY, title, pos, size)
{
    CreateStatusBar();
    SetStatusText("Welcome to wxWidgets!");

    DownloadData();
}

void MyFrame::DownloadData()
{
    wxFTP ftp;

    ftp.Connect("ftp.nasdaqtrader.com");
    if (ftp.IsConnected())
    {
        std::vector<std::string> filenames = {"nasdaqlisted.txt", "otherlisted.txt"};

        ftp.ChDir("/SymbolDirectory");

        for (const auto &fname : filenames)
        {
            wxInputStream *in = ftp.GetInputStream(fname);
            std::stringstream stream;

            if (in)
            {
                static constexpr size_t size = 65536;
                char data[size];
                do
                {
                    in->Read(data, size);
                    stream << data;
                } while (in->LastRead() > 0);

                delete in;
            }
            else
            {
                wxMessageBox("Could not read file.", "Error", wxICON_ERROR);
            }

            std::cout << stream.str() << std::endl;
        }
    }
    else
    {
        wxMessageBox("Could not connect to the server.", "Error", wxICON_ERROR);
    }

    ftp.Close();
}
