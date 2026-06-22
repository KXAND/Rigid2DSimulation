#pragma once
#include "dvec.h"

struct App;
struct Control {
	dVector2 tl;
	dRect vp;

	virtual ~Control() {}
	virtual void Update(App& app) {}
	// ������֮ǰҪִ�� Discard��
	virtual void Discard(App& app) {}
	virtual void PreUpdate(App& app) {}

	virtual int GetW() const { return 0; }
	virtual int GetH() const { return 0; }
};
// Delegate ���Զ� Discard ���ĳ�Ա���������� CtrlList ��ͬ��һ�㡣
// Delegate Ҫ�� c һֱ����Ч�ģ���ʹ���� c �����ǵ���һ֡��
struct Delegate : Control {
	Control* c = NULL;
	void set_c(App& app, Control* _c);
	int GetW() const override;
	int GetH() const override;

	void Update(App& app) override;
	void Discard(App& app) override;
	void PreUpdate(App& app) override;
};

