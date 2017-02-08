#pragma once
#include "ImGuiWidget.h"

class WindowMenuBarWidget : public ImGuiWidget
{
private:
	void DrawMenu();
public:
	virtual bool DrawWidget() override;
};