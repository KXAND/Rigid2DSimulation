#pragma once
#include "vptr.h"
#include "bra.h"

// ����ռ����ռ��̫���ˣ�����ʱ��̬���Խ�������������鷳�ˡ�
struct expr {
	wstring typ = L"null";
	// vec �� access ��ʵ�����Էŵ� call �Ŀ���
	// ����ʵ�ֵ�ʱ���һ���ϣ���ֿ�������ȻҲûɶ���ɾ����ˡ�
	// num, var, str, call, null, vec, access

	bool global = false;
	double num = 0;
	wstring str;
	wstring var;
	wstring fun;
	vector<expr> pars;
	vector<expr> vec;
	vptr<expr> getLocalizedString, ind;

	expr() = default;
	expr(wstring const& typ);
	expr(double num);
	expr(wstring const& fun, vector<expr> const& pars);
	static expr mkstr(wstring const& str);
	static expr mkvar(wstring const& var, bool gl);
};
wstring tw(expr const& ex);

struct pre_expr {
	bool istk = false;
	expr ex;
	token tk;

	pre_expr() = default;
	pre_expr(expr const& ex);
	pre_expr(token const& tk);
};

bool is_uni_operator(token const& tk);
bool is_bin_operator(token const& tk);

bool parsing_neg(vector<pre_expr> const& in, expr& out);
bool parsing_bin(vector<pre_expr> const& in, expr& out, vector<wstring> const& mark);
expr parsing_pre_expr(vector<pre_expr> const& in);

expr parsing_call(vector<bra> const& in);
expr parsing_bins(vector<bra> const& in);
expr parsing_vec(vector<bra> const& in);
expr parsing_access(vector<bra> const& in);
expr parsing_expr(vector<bra> const& in);

