#pragma once
#include "button.h"
#include "textbox.h"
#include "dragbar.h"
#include "ctrl_list.h"
#include "separator.h"
#include "float_panel.h"

struct TbParNm : Textbox {
	int id = 0;
	TbParNm(int id);
	void Sync(App& app) override;
	void OnDone(App& app) const override;
};
struct BtDelPar : BtLan {
	int id = 0;
	BtDelPar(int id);
	void OnClick(App& app) override;
};
struct DbXParVal : DragbarX {
	int id = 0;
	DbXParVal(int id);
	void Sync(App& app) override;
	void Upload(App& app) const override;
};
struct TbParVal : Textbox {
	int id = 0;
	TbParVal(int id);
	void Sync(App& app) override;
	void OnDone(App& app) const override;
};

struct ClyPar : CtrlListY {
	std::shared_ptr<TbParNm> tb_nm;
	std::shared_ptr<BtDelPar> bt_del;
	std::shared_ptr<CtrlListX> clx_nm;
	std::shared_ptr<DbXParVal> dbx_val;
	std::shared_ptr<TbParVal> tb_val;
	std::shared_ptr<CtrlListX> clx_val;
	std::shared_ptr<SeparatorY> spy;
	ClyPar(int id);
};
struct ClyPars : CtrlListY {
	vector<std::shared_ptr<ClyPar>> mbs;
	void Update(App& app) override;
};
struct BtAddPar : BtLan {
	BtAddPar();
	void OnClick(App& app) override;
};

struct FPnPar : FPnLan {
	std::shared_ptr<ClyPars> cly_pars;
	std::shared_ptr<BtAddPar> bt_add_par;
	std::shared_ptr<CtrlListY> cly;
	FPnPar(App& app);
};

