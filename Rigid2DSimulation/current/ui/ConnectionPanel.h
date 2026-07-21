#pragma once
#include "Button.h"
#include "ControlList.h"
#include "FloatPanel.h"
#include "RichTextBox.h"
#include "RichLabel.h"
#include "Separator.h"

struct Connection;
struct RTbCon : RichTextBox {
	RTbCon();
	void Sync(App& app) override;
	void Upload(App& app) const override;
};
struct BtConOK : LocalizedButton {
	BtConOK();
	void OnClick(App& app) override;
};
struct BtConDel : LocalizedButton {
	BtConDel();
	void OnClick(App& app) override;
};

struct ClyCon : VerticalControlList {
	Connection* con = NULL;
	std::shared_ptr<RTbCon> rtb;
	std::shared_ptr<BtConOK> bt_ok;
	std::shared_ptr<BtConDel> bt_del;
	std::shared_ptr<HorizontalControlList> clx_bt;
	std::shared_ptr<HorizontalSeparator> spy;
	ClyCon();
	void Update(App& app) override;
	void Discard(App& app) override;
};

struct RLbCon : RichLabel {
	void Sync(App& app) override;
};

struct ConnectionPanel : LocalizedFloatPanel {
	std::shared_ptr<ClyCon> cly_con;
	std::shared_ptr<RLbCon> rlb_con;
	std::shared_ptr<VerticalControlList> cly;
	ConnectionPanel(App& app);
};

