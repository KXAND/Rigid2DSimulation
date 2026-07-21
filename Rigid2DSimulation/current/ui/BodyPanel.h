#pragma once
#include "Label.h"
#include "Button.h"
#include "ControlList.h"
#include "FloatPanel.h"
#include "RichTextBox.h"
#include "CheckBox.h"
#include "Separator.h"
#include "RichLabel.h"

struct Body;
struct BodyScriptEditor : RichTextBox {
	BodyScriptEditor();
	void Sync(App& app) override;
	void Upload(App& app) const override;
};
struct ApplyBodyButton : LocalizedButton {
	ApplyBodyButton();
	void OnClick(App& app) override;
};
struct DeleteBodyButton : LocalizedButton {
	DeleteBodyButton();
	void OnClick(App& app) override;
};

struct BodyConnectionLabel : Label {
	int id = 0;
	BodyConnectionLabel(int id);
	void Sync(App& app) override;
};
struct SelectBodyConnectionCheckBox : LocalizedCheckBox {
	int id = 0;
	SelectBodyConnectionCheckBox(int id);
	void Sync(App& app) override;
	void Upload(App& app) const override;
};
struct BodyConnectionControlRow : HorizontalControlList {
	std::shared_ptr<BodyConnectionLabel> lb;
	std::shared_ptr<SelectBodyConnectionCheckBox> cb;
	BodyConnectionControlRow(int id);
};
struct BodyConnectionControlList : VerticalControlList {
	vector<std::shared_ptr<BodyConnectionControlRow>> mbs;
	void Update(App& app) override;
};

struct BodyDetailsLabel : RichLabel {
	void Sync(App& app) override;
};

// 我觉得 delegate 没有啥价值了。
// 其实 Panel 的 set_c 方法也没啥价值了吧。
struct BodyEditorControls : VerticalControlList {
	Body* bo = NULL;
	std::shared_ptr<BodyConnectionControlList> cly_cons;
	std::shared_ptr<BodyScriptEditor> rtb;
	std::shared_ptr<ApplyBodyButton> bt_ok;
	std::shared_ptr<DeleteBodyButton> bt_del;
	std::shared_ptr<HorizontalControlList> clx_bt;
	std::shared_ptr<HorizontalSeparator> spy;
	BodyEditorControls();
	void Update(App& app) override;
	void Discard(App& app) override;
};

struct BodyPanel : LocalizedFloatPanel {
	std::shared_ptr<BodyEditorControls> cly_body;
	std::shared_ptr<BodyDetailsLabel> rlb_body;
	std::shared_ptr<VerticalControlList> cly;
	BodyPanel(App& app);
};

