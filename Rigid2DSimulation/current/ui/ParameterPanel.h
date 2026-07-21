#pragma once
#include "Button.h"
#include "TextBox.h"
#include "DragBar.h"
#include "ControlList.h"
#include "Separator.h"
#include "FloatPanel.h"

struct ParameterNameTextBox : TextBox {
	int id = 0;
	ParameterNameTextBox(int id);
	void Sync(App& app) override;
	void OnDone(App& app) const override;
};
struct DeleteParameterButton : LocalizedButton {
	int id = 0;
	DeleteParameterButton(int id);
	void OnClick(App& app) override;
};
struct ParameterValueDragBar : HorizontalDragBar {
	int id = 0;
	ParameterValueDragBar(int id);
	void Sync(App& app) override;
	void Upload(App& app) const override;
};
struct ParameterValueTextBox : TextBox {
	int id = 0;
	ParameterValueTextBox(int id);
	void Sync(App& app) override;
	void OnDone(App& app) const override;
};

struct ParameterControlRow : VerticalControlList {
	std::shared_ptr<ParameterNameTextBox> tb_nm;
	std::shared_ptr<DeleteParameterButton> bt_del;
	std::shared_ptr<HorizontalControlList> clx_nm;
	std::shared_ptr<ParameterValueDragBar> dbx_val;
	std::shared_ptr<ParameterValueTextBox> tb_val;
	std::shared_ptr<HorizontalControlList> clx_val;
	std::shared_ptr<HorizontalSeparator> spy;
	ParameterControlRow(int id);
};
struct ParameterControlList : VerticalControlList {
	vector<std::shared_ptr<ParameterControlRow>> mbs;
	void Update(App& app) override;
};
struct AddParameterButton : LocalizedButton {
	AddParameterButton();
	void OnClick(App& app) override;
};

struct ParameterPanel : LocalizedFloatPanel {
	std::shared_ptr<ParameterControlList> cly_pars;
	std::shared_ptr<AddParameterButton> bt_add_par;
	std::shared_ptr<VerticalControlList> cly;
	ParameterPanel(App& app);
};

