#pragma once
#include "Font.h"
#include "Control.h"

typedef function<wstring(int, App&)> DropListItemNameProvider;
wstring GetDefaultDropListItemName(int id, App &app);

struct DropList : Control {
	DVector2 gap;
	double depth = 0;
	wstring text;
	int width = 0, height = 0;

	// 即使某些设计中 selectedIndex 可以取 -1，默认值还是设成 0 吧。
	// 这里 selectedIndex 能取什么值还没想清楚的，能不能取 -1 之类，要实践中得到。
	int selectedIndex = 0;
	int itemHeight = 0;
	int itemCount = 0;
	DropListItemNameProvider itemNameProvider = GetDefaultDropListItemName;

	double signRadius = 0;
	int signMargin = 0;
	int textMarginX = 0;
	Color textColor, markColor, editColor, frameColor, normalColor, invalidColor, hoveredColor;

	// -1 代表没有悬浮在任何选项上面。
	int hoveredItemIndex = -1;
	int textWidth = 0;
	bool edit = false;
	bool enabled = true;
	bool hovered = false;

	DropList(int width = 0, DropListItemNameProvider const &itemNameProvider = GetDefaultDropListItemName);
	int GetWidth() const override;
	int GetHeight() const override;

	DVector2 GetBoxTopLeft() const;
	DVector2 GetItemsTopLeft() const;
	void RenderMain(App& app);
	void RenderItems(App& app);
	void Render(App& app);
	virtual void Sync(App& app) {}
	// 现在默认的是 Upload 的校正不改变控件本身值，只改变目标值。
	virtual void Upload(App& app) const {}

	void Update(App& app) override;
	void Discard(App& app) override;
	void PreUpdate(App& app) override;
};
struct LocalizedDropList : DropList {
	wstring textId;
	LocalizedDropList() = default;
	LocalizedDropList(wstring const& id, int width, DropListItemNameProvider const& itemNameProvider = GetDefaultDropListItemName);
	void Update(App& app) override;
};


