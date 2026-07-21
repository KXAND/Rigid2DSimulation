#pragma once
#include "Label.h"
#include "Button.h"
#include "TextBox.h"
#include "ControlList.h"
#include "FloatPanel.h"
#include "RichLabel.h"
#include "Separator.h"

struct SaveButton : LocalizedButton {
	SaveButton();
	void OnClick(App& app) override;
};
struct LoadButton : LocalizedButton {
	LoadButton();
	void OnClick(App& app) override;
};
struct GlobalDetailsLabel : RichLabel {
	void Sync(App& app) override;
};

struct GlobalPanel : LocalizedFloatPanel {
	std::shared_ptr<TextBox> tb_save;
	std::shared_ptr<SaveButton> bt_save;
	std::shared_ptr<HorizontalControlList> clx_save;
	std::shared_ptr<TextBox> tb_load;
	std::shared_ptr<LoadButton> bt_load;
	std::shared_ptr<HorizontalControlList> clx_load;
	std::shared_ptr<HorizontalSeparator> spy;
	std::shared_ptr<GlobalDetailsLabel> rlb_global;
	std::shared_ptr<VerticalControlList> cly;
	GlobalPanel(App& app);
};

