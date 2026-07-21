#pragma once
#include "Button.h"
#include "TextBox.h"
#include "DragBar.h"
#include "ControlList.h"
#include "Separator.h"
#include "FloatPanel.h"

struct TbParNm : TextBox {
	int id = 0;
	TbParNm(int id);
	void Sync(App& app) override;
	void OnDone(App& app) const override;
};
struct BtDelPar : LocalizedButton {
	int id = 0;
	BtDelPar(int id);
	void OnClick(App& app) override;
};
struct DbXParVal : HorizontalDragBar {
	int id = 0;
	DbXParVal(int id);
	void Sync(App& app) override;
	void Upload(App& app) const override;
};
struct TbParVal : TextBox {
	int id = 0;
	TbParVal(int id);
	void Sync(App& app) override;
	void OnDone(App& app) const override;
};

struct ClyPar : VerticalControlList {
	std::shared_ptr<TbParNm> tb_nm;
	std::shared_ptr<BtDelPar> bt_del;
	std::shared_ptr<HorizontalControlList> clx_nm;
	std::shared_ptr<DbXParVal> dbx_val;
	std::shared_ptr<TbParVal> tb_val;
	std::shared_ptr<HorizontalControlList> clx_val;
	std::shared_ptr<HorizontalSeparator> spy;
	ClyPar(int id);
};
struct ClyPars : VerticalControlList {
	vector<std::shared_ptr<ClyPar>> mbs;
	void Update(App& app) override;
};
struct BtAddPar : LocalizedButton {
	BtAddPar();
	void OnClick(App& app) override;
};

struct FPnPar : LocalizedFloatPanel {
	std::shared_ptr<ClyPars> cly_pars;
	std::shared_ptr<BtAddPar> bt_add_par;
	std::shared_ptr<VerticalControlList> cly;
	FPnPar(App& app);
};

