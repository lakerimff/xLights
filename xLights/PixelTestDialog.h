#ifndef PIXELPixelTestDialog_H
#define PIXELPixelTestDialog_H

// Need to do these manually due to issues with wxSmith and wxTreeListCtrl
#include <wx/treelist.h>
#include <wx/treectrl.h>
#include <wx/filename.h>

//(*Headers(PixelTestDialog)
#include <wx/notebook.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/checkbox.h>
#include <wx/splitter.h>
#include <wx/aui/aui.h>
#include <wx/radiobut.h>
#include <wx/slider.h>
#include <wx/panel.h>
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/timer.h>
//*)

#include "xLightsTimer.h"
#include <string>
#include <list>
#include "models/ModelManager.h"
#include "outputs/OutputManager.h"
#include "SequenceData.h"

#ifdef __WXOSX__
#include "osxMacUtils.h"
#endif

class ModelGroup;
typedef SequenceData SeqDataType;

class ChannelTracker
{
    long _changeCount;
    std::list<wxLongLong> _ranges;
    long _lastReturnedChannel;
    #define NORANGE (wxLongLong)0xFFFFFFFFFFFFFFF
    wxLongLong _lastReturnedRange;

    wxLongLong SetStart(wxLongLong value, long start) const { return (value & 0xFFFFFFFF) + (((wxLongLong)start) << 32); }
    wxLongLong SetEnd(wxLongLong value, long end) const { return (value & 0xFFFFFFFF00000000) + end; }
    wxLongLong SetBoth(long start, long end) const { return (((wxLongLong)start) << 32) + end; }

    void ClearLast()
    {
        _lastReturnedChannel = -1;
        _lastReturnedRange = NORANGE;
    }


public:

    ChannelTracker()
    {
        _changeCount = 0;
        ClearLast();
    }

    virtual ~ChannelTracker() {}

    static long GetStart(wxLongLong value) { return (value >> 32).ToLong(); }
    static long GetEnd(wxLongLong value) { return (value & 0xFFFFFFFF).ToLong(); }
    void FixOverlaps();
    void Dump();
    void AddRange(long start, long end);
    void RemoveRange(long start, long end);
    long GetChangeCount() const { return _changeCount; }
    void Clear()
    {
        _changeCount++;
        _ranges.clear();
    }
    bool IsChannelOn(long ch) const
    {
        for (auto it = _ranges.begin(); it != _ranges.end(); ++it)
        {
            if (ch >= GetStart(*it) && ch <= GetEnd(*it)) return true;
        }

        return false;
    }

    bool AreAnyIncluded(long start, long end)
    {
        auto it = _ranges.begin();

        while (it != _ranges.end())
        {
            long s = GetStart(*it);
            long e = GetEnd(*it);

            if (start >= s && start <= e || end >= s && end <= e) return true;

            ++it;
        }

        return false;
    }

    long GetFirst()
    {
        if (_ranges.size() == 0) return -1;

        _lastReturnedChannel = GetStart(_ranges.front());

        return _lastReturnedChannel;
    }

    long GetNext()
    {
        // Assumes ranges are sorted
        if (_lastReturnedChannel == -1) return -1;

        for (auto it = _ranges.begin(); it != _ranges.end(); ++it)
        {
            long s = GetStart(*it);
            long e = GetEnd(*it);

            _lastReturnedChannel++;

            if (s <= _lastReturnedChannel && e >= _lastReturnedChannel) return _lastReturnedChannel;

            if (s > _lastReturnedChannel)
            {
                _lastReturnedChannel = s;
                return _lastReturnedChannel;
            }
            _lastReturnedChannel--;
        }

        return -1;
    }

    long GetChannelAfter(long ch)
    {
        for (auto it = _ranges.begin(); it != _ranges.end(); ++it)
        {
            long s = GetStart(*it);
            long e = GetEnd(*it);

            ch++;
            if (s <= ch && e >= ch) return ch;
            if (s > ch)
            {
                return s;
            }
        }

        return -1;
    }

    void GetFirstRange(long& start, long& end)
    {
        if (_ranges.size() == 0)
        {
            start = -1;
            end = -1;
            return;
        }

        _lastReturnedRange = _ranges.front();
        start = GetStart(_lastReturnedRange);
        end = GetEnd(_lastReturnedRange);
    }

    void GetNextRange(long& start, long& end)
    {
        if (_lastReturnedRange == NORANGE) 
        {
            start = -1;
            end = -1;
            return;
        }

        for (auto it = _ranges.begin(); it != _ranges.end(); ++it)
        {
            if (*it == _lastReturnedRange)
            {
                ++it;
                if (it == _ranges.end())
                {
                    _lastReturnedRange = NORANGE;
                    start = -1;
                    end = -1;
                    return;
                }
                else
                {
                    _lastReturnedRange = *it;
                    start = GetStart(*it);
                    end = GetEnd(*it);
                    return;
                }
            }
        }

        _lastReturnedRange = NORANGE;
        start = -1;
        end = -1;
    }
};

class ModelTestItem;
class ModelGroupTestItem;

class PixelTestDialog: public wxDialog
{
	enum TestFunctions
	{
		OFF,
		CHASE,
		CHASE2,
		CHASE3,
		CHASE4,
		DIM,
		TWINKLE,
		SHIMMER,
		RGBW
	};

	public:

		PixelTestDialog(wxWindow* parent, OutputManager* outputManager, wxFileName networkFile, ModelManager* modelManager, wxWindowID id=wxID_ANY);
		virtual ~PixelTestDialog();
		wxTreeListCtrl* TreeListCtrl_Outputs;
		wxTreeListCtrl* TreeListCtrl_ModelGroups;
		wxTreeListCtrl* TreeListCtrl_Models;
		wxFileName _networkFile;
		ModelManager* _modelManager;
		bool _cascading;
        ModelTestItem* _lastModel;
        std::list<ModelTestItem*> _models;
        ChannelTracker _channelTracker;

        int _twinkleRatio;
		int _chaseGrouping;
		bool _checkChannelList;
		wxDateTime _starttime;
		SeqDataType _seqData;

		//(*Declarations(PixelTestDialog)
		wxRadioButton* RadioButton_Standard_Chase;
		wxRadioButton* RadioButton_Standard_Twinkle5;
		wxSlider* Slider_Standard_Background;
		wxRadioButton* RadioButton_RGB_Chase13;
		wxSlider* Slider_RGB_BG_G;
		wxPanel* Panel5;
		wxSlider* Slider_RGB_BG_R;
		wxRadioButton* RadioButton_RGB_Twinkle5;
		wxNotebook* Notebook1;
		wxRadioButton* RadioButton_RGB_Shimmer;
		wxRadioButton* RadioButton_RGB_Background;
		wxStaticText* StaticText2;
		wxPanel* Panel4;
		wxRadioButton* RadioButton_Standard_Chase14;
		wxPanel* Panel_Outputs;
		wxStaticText* StaticText6;
		wxButton* Button_Save;
		wxRadioButton* RadioButton_RGB_Twinkle10;
		wxRadioButton* RadioButton_RGBCycle_RGBW;
		wxPanel* Panel_ModelGroups;
		wxSlider* Slider_RGB_BG_B;
		wxRadioButton* RadioButton_RGB_Twinkle25;
		wxRadioButton* RadioButton_RGBCycle_ABCAll;
		wxPanel* Panel1;
		wxStaticText* StaticText1;
		xLightsTimer Timer1;
		wxSlider* Slider_RGB_H_G;
		wxStaticText* StaticText3;
		wxRadioButton* RadioButton_Standard_Shimmer;
		wxSlider* Slider_RGB_H_R;
		wxSlider* Slider_Standard_Highlight;
		wxRadioButton* RadioButton_Standard_Chase13;
		wxRadioButton* RadioButton_RGB_Alternate;
		wxRadioButton* RadioButton_RGBCycle_ABC;
		wxRadioButton* RadioButton_RGB_Twinkle50;
		wxCheckBox* CheckBox_OutputToLights;
		wxStaticText* StaticText5;
		wxRadioButton* RadioButton_RGB_Chase14;
		wxRadioButton* RadioButton_RGBCycle_ABCAllNone;
		wxRadioButton* RadioButton_Standard_Chase15;
		wxStaticText* StatusBar1;
		wxSlider* Slider_RGB_H_B;
		wxAuiNotebook* AuiNotebook1;
		wxRadioButton* RadioButton_Standard_Twinkle10;
		wxFlexGridSizer* FlexGridSizer_ModelGroups;
		wxRadioButton* RadioButton_RGB_Off;
		wxRadioButton* RadioButton_RGBCycle_Off;
		wxRadioButton* RadioButton_RGB_Chase15;
		wxRadioButton* RadioButton_Standard_Twinkle25;
		wxSlider* Slider_Speed;
		wxPanel* Panel2;
		wxRadioButton* RadioButton_Standard_Background;
		wxRadioButton* RadioButton_Standard_Twinkle50;
		wxRadioButton* RadioButton_RGB_Chase;
		wxFlexGridSizer* FlexGridSizer_Outputs;
		wxSplitterWindow* SplitterWindow1;
		wxStaticText* StaticText4;
		wxCheckBox* CheckBox_SuppressUnusedOutputs;
		wxPanel* Panel_Models;
		wxRadioButton* RadioButton_RGBCycle_MixedColors;
		wxPanel* Panel_Standard;
		wxRadioButton* RadioButton_Standard_Alternate;
		wxRadioButton* RadioButton_Standard_Off;
		wxFlexGridSizer* FlexGridSizer_Models;
		wxButton* Button_Load;
		//*)

	protected:

		OutputManager* _outputManager;
		static const long ID_TREELISTCTRL_Outputs;
		static const long ID_TREELISTCTRL_ModelGroups;
		static const long ID_TREELISTCTRL_Models;

		//(*Identifiers(PixelTestDialog)
		static const long ID_BUTTON_Load;
		static const long ID_BUTTON_Save;
		static const long ID_PANEL3;
		static const long ID_PANEL6;
		static const long ID_PANEL7;
		static const long ID_NOTEBOOK1;
		static const long ID_PANEL1;
		static const long ID_CHECKBOX_OutputToLights;
		static const long ID_CHECKBOX1;
		static const long ID_STATICTEXT2;
		static const long ID_RADIOBUTTON_Standard_Off;
		static const long ID_RADIOBUTTON_Standard_Chase;
		static const long ID_RADIOBUTTON_Standard_Chase13;
		static const long ID_RADIOBUTTON_Standard_Chase14;
		static const long ID_RADIOBUTTON_Standard_Chase15;
		static const long ID_RADIOBUTTON_Standard_Alternate;
		static const long ID_RADIOBUTTON_Standard_Twinke5;
		static const long ID_RADIOBUTTON_Standard_Twinkle10;
		static const long ID_RADIOBUTTON_Standard_Twinkle25;
		static const long ID_RADIOBUTTON_Standard_Twinkle50;
		static const long ID_RADIOBUTTON_Standard_Shimmer;
		static const long ID_RADIOBUTTON_Standard_Background;
		static const long ID_STATICTEXT3;
		static const long ID_SLIDER_Standard_Background;
		static const long ID_STATICTEXT4;
		static const long ID_SLIDER_Standard_Highlight;
		static const long ID_PANEL_Standard;
		static const long ID_STATICTEXT5;
		static const long ID_RADIOBUTTON_RGB_Off;
		static const long ID_RADIOBUTTON_RGB_Chase;
		static const long ID_RADIOBUTTON_RGB_Chase13;
		static const long ID_RADIOBUTTON_RGB_Chase14;
		static const long ID_RADIOBUTTON_RGB_Chase15;
		static const long ID_RADIOBUTTON_RGB_Alternate;
		static const long ID_RADIOBUTTON_RGB_Twinkle5;
		static const long ID_RADIOBUTTON_RGB_Twinkle10;
		static const long ID_RADIOBUTTON_RGB_Twinkle25;
		static const long ID_RADIOBUTTON_RGB_Twinkle50;
		static const long ID_RADIOBUTTON_RGB_Shimmer;
		static const long ID_RADIOBUTTON_RGB_Background;
		static const long ID_SLIDER1;
		static const long ID_SLIDER2;
		static const long ID_SLIDER3;
		static const long ID_SLIDER4;
		static const long ID_SLIDER5;
		static const long ID_SLIDER6;
		static const long ID_PANEL4;
		static const long ID_STATICTEXT6;
		static const long ID_RADIOBUTTON_RGBCycle_Off;
		static const long ID_RADIOBUTTON_RGBCycle_ABC;
		static const long ID_RADIOBUTTON_RGBCycle_ABCAll;
		static const long ID_RADIOBUTTON_RGBCycle_ABCAllNone;
		static const long ID_RADIOBUTTON_RGBCycle_MixedColors;
		static const long ID_RADIOBUTTON_RGBCycle_RGBW;
		static const long ID_PANEL5;
		static const long ID_AUINOTEBOOK1;
		static const long ID_STATICTEXT1;
		static const long ID_SLIDER_Speed;
		static const long ID_PANEL2;
		static const long ID_SPLITTERWINDOW1;
		static const long ID_STATICTEXT7;
		static const long ID_TIMER1;
		//*)

	private:

		//(*Handlers(PixelTestDialog)
		void OnButton_LoadClick(wxCommandEvent& event);
		void OnButton_SaveClick(wxCommandEvent& event);
		void OnTimer1Trigger(wxTimerEvent& event);
		void OnCheckBox_OutputToLightsClick(wxCommandEvent& event);
		void OnClose(wxCloseEvent& event);
		void OnCheckBox_SuppressUnusedOutputsClick(wxCommandEvent& event);
		void OnNotebook1PageChanged(wxNotebookEvent& event);
		//*)

		void OnTreeListCtrlCheckboxtoggled(wxTreeListEvent& event);
        void OnTreeListCtrlItemActivated(wxTreeListEvent& event);
        void OnTreeListCtrlItemSelected(wxTreeListEvent& event);
        void OnTreeListCtrlItemExpanding(wxTreeListEvent& event);

		void PopulateControllerTree(OutputManager* outputManager);
		void PopulateModelGroupTree(ModelManager* modelManager);
		void PopulateModelTree(ModelManager* modelManager);
        void AddChannel(wxTreeListCtrl* tree, wxTreeListItem parent, long absoluteChannel, long relativeChannel, char colour);
        void AddNode(wxTreeListCtrl* tree, wxTreeListItem parent, ModelTestItem* model, long node);
        char GetChannelColour(long ch);
        void AddModelGroup(wxTreeListItem parent, Model* m);
        void AddModelGroup(wxTreeListItem parent, ModelGroupTestItem* m);

        bool AreChannelsAvailable(Model* model);
        bool AreChannelsAvailable(ModelGroup* model);

		void CascadeSelected(wxTreeListCtrl* tree, wxTreeListItem& item, wxCheckBoxState state);
        void DumpSelected();

		void DestroyTreeControllerData(wxTreeListCtrl* tree, wxTreeListItem& item);
		std::list<std::string> GetModelsOnChannels(int start, int end);
		void Clear(wxTreeListCtrl* tree, wxTreeListItem& item);
		void GetCheckedItems(wxArrayInt& chArray);
		void GetCheckedItems(wxArrayInt& chArray, char col);
		void OnTimer(long curtime);
		void TestButtonsOff();
		void RollUpAll(wxTreeListCtrl* tree, wxTreeListItem start);
		void DeactivateNotClickableModels(wxTreeListCtrl* tree);
        void SetTreeTooltip(wxTreeListCtrl* tree, wxTreeListItem& item);
        void AddController(wxTreeListItem root, Output* output);
        std::string SerialiseSettings();
        void DeserialiseSettings(const std::string& settings);
        TestFunctions GetTestFunction(int notebookSelection);
        void SetCheckBoxItemFromTracker(wxTreeListCtrl* tree, wxTreeListItem item, wxCheckBoxState parentState);
        void SetSuspend();

		DECLARE_EVENT_TABLE()
};
#endif