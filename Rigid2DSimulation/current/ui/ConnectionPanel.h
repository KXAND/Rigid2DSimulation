#pragma once
#include "Button.h"
#include "ControlList.h"
#include "FloatPanel.h"
#include "RichTextBox.h"
#include "RichLabel.h"
#include "Separator.h"

struct Connection;
struct ConnectionScriptEditor : RichTextBox {
	ConnectionScriptEditor();
	void Sync(App& app) override;
	void Upload(App& app) const override;
};
struct ApplyConnectionButton : LocalizedButton {
	ApplyConnectionButton();
	void OnClick(App& app) override;
};
struct DeleteConnectionButton : LocalizedButton {
	DeleteConnectionButton();
	void OnClick(App& app) override;
};

struct ConnectionEditorControls : VerticalControlList {
	Connection* con = NULL;
	std::shared_ptr<ConnectionScriptEditor> rtb;
	std::shared_ptr<ApplyConnectionButton> bt_ok;
	std::shared_ptr<DeleteConnectionButton> bt_del;
	std::shared_ptr<HorizontalControlList> clx_bt;
	std::shared_ptr<HorizontalSeparator> spy;
	ConnectionEditorControls();
	void Update(App& app) override;
	void Discard(App& app) override;
};

struct ConnectionDetailsLabel : RichLabel {
	void Sync(App& app) override;
};

struct ConnectionPanel : LocalizedFloatPanel {
	std::shared_ptr<ConnectionEditorControls> cly_con;
	std::shared_ptr<ConnectionDetailsLabel> rlb_con;
	std::shared_ptr<VerticalControlList> cly;
	ConnectionPanel(App& app);
};

