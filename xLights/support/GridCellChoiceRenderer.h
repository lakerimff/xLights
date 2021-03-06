#ifndef _GRIDCELLCHOICERENDERER_H
#define _GRIDCELLCHOICERENDERER_H

#include "wx/grid.h"
#include "wx/laywin.h"
#include <wx/dc.h>

class wxGridCellChoiceRenderer : public wxGridCellStringRenderer
{
public:
	wxGridCellChoiceRenderer(wxLayoutAlignment border = wxLAYOUT_NONE) :
		m_border(border) {}
	virtual void Draw(wxGrid& grid,
		wxGridCellAttr& attr,
		wxDC& dc,
		const wxRect& rect,
		int row, int col,
		bool isSelected);
	virtual wxGridCellRenderer *Clone() const
		{ return new wxGridCellChoiceRenderer; }
private:
	wxLayoutAlignment m_border;
};


#endif // _GRIDCELLCHOICERENDERER_H
