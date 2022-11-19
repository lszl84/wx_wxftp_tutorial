#pragma once

#include <wx/wx.h>
#include <wx/listctrl.h>

#include "stock.h"

class StocksListControl : public wxListCtrl
{
public:
    StocksListControl(wxWindow *parent, const wxWindowID id, const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize)
        : wxListCtrl(parent, id, pos, size, wxLC_REPORT | wxLC_VIRTUAL)
    {
        this->AppendColumn("Symbol");
        this->AppendColumn("Name");

        this->SetColumnWidth(0, FromDIP(80));
        this->SetColumnWidth(1, FromDIP(700));
    }

    virtual wxString OnGetItemText(long index, long column) const wxOVERRIDE
    {
        const auto &item = items[index];

        switch (column)
        {
        case 0:
            return item.symbol;
        case 1:
            return item.name;
        default:
            return "";
        }
    }

    void RefreshAfterUpdate()
    {
        this->SetItemCount(items.size());
        this->Refresh();
    }

    std::vector<Stock> items;
};