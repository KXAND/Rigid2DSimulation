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
struct RTbBody : RichTextBox {
	RTbBody();
	void Sync(App& app) override;
	void Upload(App& app) const override;
};
struct BtBodyOK : BtLan {
	BtBodyOK();
	void OnClick(App& app) override;
};
struct BtBodyDel : BtLan {
	BtBodyDel();
	void OnClick(App& app) override;
};

struct LbBodyCon : Label {
	int id = 0;
	LbBodyCon(int id);
	void Sync(App& app) override;
};
struct CbBodyCon : CbLan {
	int id = 0;
	CbBodyCon(int id);
	void Sync(App& app) override;
	void Upload(App& app) const override;
};
struct ClXBodyCon : CtrlListX {
	std::shared_ptr<LbBodyCon> lb;
	std::shared_ptr<CbBodyCon> cb;
	ClXBodyCon(int id);
};
struct ClYBodyCons : CtrlListY {
	vector<std::shared_ptr<ClXBodyCon>> mbs;
	void Update(App& app) override;
};

struct RLbBody : RichLabel {
	void Sync(App& app) override;
};

// 我觉得 delegate 没有啥价值了。
// 其实 Panel 的 set_c 方法也没啥价值了吧。
struct ClyBody : CtrlListY {
	Body* bo = NULL;
	std::shared_ptr<ClYBodyCons> cly_cons;
	std::shared_ptr<RTbBody> rtb;
	std::shared_ptr<BtBodyOK> bt_ok;
	std::shared_ptr<BtBodyDel> bt_del;
	std::shared_ptr<CtrlListX> clx_bt;
	std::shared_ptr<SeparatorY> spy;
	ClyBody();
	void Update(App& app) override;
	void Discard(App& app) override;
};

struct FPnBody : FPnLan {
	std::shared_ptr<ClyBody> cly_body;
	std::shared_ptr<RLbBody> rlb_body;
	std::shared_ptr<CtrlListY> cly;
	FPnBody(App& app);
};

