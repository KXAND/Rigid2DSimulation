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
	std::shared_ptr<ParameterNameTextBox> nameTextBox;
	std::shared_ptr<DeleteParameterButton> deleteButton;
	std::shared_ptr<HorizontalControlList> nameControls;
	std::shared_ptr<ParameterValueDragBar> valueDragBar;
	std::shared_ptr<ParameterValueTextBox> valueTextBox;
	std::shared_ptr<HorizontalControlList> valueControls;
	std::shared_ptr<HorizontalSeparator> separator;
	ParameterControlRow(int id);
};
struct ParameterControlList : VerticalControlList {
	vector<std::shared_ptr<ParameterControlRow>> parameterRows;
	void Update(App& app) override;
};
struct AddParameterButton : LocalizedButton {
	AddParameterButton();
	void OnClick(App& app) override;
};

struct ParameterPanel : LocalizedFloatPanel {
	std::shared_ptr<ParameterControlList> parameterList;
	std::shared_ptr<AddParameterButton> addButton;
	std::shared_ptr<VerticalControlList> controls;
	ParameterPanel(App& app);
};

