#include <wx/wx.h>
#include <wx/protocol/ftp.h>

#include <sstream>
#include <iostream>
#include <thread>

#include "rapidcsv.h"

#include "stock.h"
#include "stockslistcontrol.h"

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
    StocksListControl *stockList;
    void DownloadData();
};

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
    wxSocketBase::Initialize();

    MyFrame *frame = new MyFrame("Hello World", wxDefaultPosition, wxDefaultSize);
    frame->Show(true);
    return true;
}

MyFrame::MyFrame(const wxString &title, const wxPoint &pos, const wxSize &size)
    : wxFrame(NULL, wxID_ANY, title, pos, size)
{
    CreateStatusBar();
    SetStatusText("Welcome to wxWidgets!");

    auto sizer = new wxBoxSizer(wxHORIZONTAL);
    stockList = new StocksListControl(this, wxID_ANY, wxDefaultPosition, FromDIP(wxSize(800, 600)));
    sizer->Add(stockList, 1, wxEXPAND);

    SetSizerAndFit(sizer);

    DownloadData();
}

void MyFrame::DownloadData()
{
    auto f = [this]()
    {
        std::vector<Stock> stocks;
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

                    rapidcsv::Document doc(stream,
                                           rapidcsv::LabelParams(),
                                           rapidcsv::SeparatorParams('|'));

                    for (int i = 0; i < doc.GetRowCount() - 1; i++)
                    {
                        auto v = doc.GetRow<std::string>(i);
                        std::cout << "Symbol: " << v[0] << ", name: " << v[1] << std::endl;

                        stocks.push_back({v[0], v[1]});
                    }

                    delete in;
                }
                else
                {
                    CallAfter([]()
                              { wxMessageBox("Could not read file.", "Error", wxICON_ERROR); });
                }
            }
        }
        else
        {
            CallAfter([]()
                      { wxMessageBox("Could not connect to the server.", "Error", wxICON_ERROR); });
        }

        std::sort(stocks.begin(), stocks.end(), [](const Stock &lhs, const Stock &rhs)
                  { return lhs.symbol < rhs.symbol; });

        CallAfter([this, stocks]()
                  {
                      stockList->items = stocks;
                      stockList->RefreshAfterUpdate();
                  });

        ftp.Close();
    };

    std::thread t{f};
    t.detach();
}
