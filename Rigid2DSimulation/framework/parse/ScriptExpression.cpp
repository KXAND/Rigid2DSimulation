#include "ScriptExpression.h"
#include "BracketGroup.h"

ScriptExpression::ScriptExpression(wstring const& type) : type(type) {}
ScriptExpression::ScriptExpression(double number) : number(number) { type = L"num"; }
ScriptExpression::ScriptExpression(wstring const& functionName, vector<ScriptExpression> const& arguments) :
	functionName(functionName), arguments(arguments) {
	type = L"call";
}
ScriptExpression ScriptExpression::CreateString(wstring const& text) {
	auto expression = ScriptExpression(L"str");
	expression.stringValue = text;
	return expression;
}
ScriptExpression ScriptExpression::CreateVariable(wstring const& value, bool isGlobal) {
	auto expression = ScriptExpression(L"var");
	expression.variableName = value;
	expression.isGlobal = isGlobal;
	return expression;
}

wstring ToWString(ScriptExpression const& expression) {
	wstring serializedText = L"[" + expression.type;
	if (expression.type == L"var") { serializedText += (expression.isGlobal ? L":&" : L":") + expression.variableName; }
	else if (expression.type == L"num") { serializedText += L":" + FormatFixed2(expression.number); }
	else if (expression.type == L"str") { serializedText += L":" + expression.stringValue; }
	else if (expression.type == L"access") {
		if (!expression.accessedExpression || !expression.indexExpression) { return {}; }
		serializedText += L":" + ToWString(*expression.accessedExpression) + L"," + ToWString(*expression.indexExpression);
	}
	else if (expression.type == L"vec") {
		serializedText += L":";
		for (auto const& element : expression.elements) { serializedText += ToWString(element) + L","; }
	}
	else if (expression.type == L"call") {
		serializedText += L":" + expression.functionName + L",";
		for (auto const& argument : expression.arguments) { serializedText += ToWString(argument) + L","; }
	}
	serializedText += L"]";
	return serializedText;
}

PreExpression::PreExpression(ScriptExpression const& expression) : expression(expression) {}
PreExpression::PreExpression(Token const& token) : token(token) { isToken = true; }

bool IsUnaryOperator(Token const& token) {
	// 由于 +，- 的特殊性，它们都被当成二元运算符处理。
	if (token.type.empty()) { return false; }
	auto const& operatorType = token.type;
	return operatorType == L"!";
}
bool IsBinaryOperator(Token const& token) {
	if (token.type.empty()) { return false; }
	return !iswalnum(token.type[0]) && !IsUnaryOperator(token);
}

bool ParseNegation(vector<PreExpression> const& in, ScriptExpression& out) {
	for (int i = (0), iEnd = int(in.size()); i < iEnd; ++i) {
		auto const& pre = in[i];
		if (!pre.isToken) { continue; }
		auto const& s = pre.token.type;

		if ((s == L"+" || s == L"-") && (i == 0 || in[i - 1].isToken)) {
			bool bad = i == in.size() - 1 || in[i + 1].isToken;
			if (bad) {
				out = ScriptExpression(L"error");
				return false;
			}

			auto tmp = vector<PreExpression>(in.begin(), in.begin() + i);
			tmp.push_back(ScriptExpression(s + s, { in[i + 1].expression }));
			for (int j = (i + 2), jEnd = int(in.size()); j < jEnd; ++j) { tmp.push_back(in[j]); }
			out = ParsePreExpression(tmp);
			return true;
		}
	}
	return false;
}
bool ParseBinary(vector<PreExpression> const& in, ScriptExpression& out, vector<wstring> const& mark) {
	for (int i = (0), iEnd = int(in.size()); i < iEnd; ++i) {
		auto const& pre = in[i];
		if (!pre.isToken) { continue; }
		auto const& s = pre.token.type;

		if (find(mark.begin(), mark.end(), s) != mark.end()) {
			bool bad =
				i == 0 || i == in.size() - 1 ||
				in[i - 1].isToken || in[i + 1].isToken;
			if (bad) {
				out = ScriptExpression(L"error");
				return false;
			}

			auto tmp = vector<PreExpression>(in.begin(), in.begin() + i - 1);
			tmp.push_back(ScriptExpression(s, { in[i - 1].expression, in[i + 1].expression }));
			for (int j = (i + 2), jEnd = int(in.size()); j < jEnd; ++j) { tmp.push_back(in[j]); }
			out = ParsePreExpression(tmp);
			return true;
		}
	}
	return false;
}
ScriptExpression ParsePreExpression(vector<PreExpression> const& in) {
	if (in.empty()) { return ScriptExpression(L"error"); }
	if (in.size() == 1) {
		if (in[0].isToken) { return ScriptExpression(L"error"); }
		return in[0].expression;
	}

	ScriptExpression out;
	if (ParseNegation(in, out)) { return out; }
	if (ParseBinary(in, out, { L"*", L"/" })) { return out; }
	if (ParseBinary(in, out, { L"+", L"-" })) { return out; }
	if (ParseBinary(in, out,
		{ L"==", L"!=", L"<=", L">=", L"<", L">" })) { return out; }
	if (ParseBinary(in, out, { L"&&", L"||" })) { return out; }
	if (ParseBinary(in, out, { L"." })) { return out; }
	return ScriptExpression(L"error");
}

ScriptExpression ParseCall(vector<BracketGroup> const& in) {
	ScriptExpression ex(L"call");
	ex.functionName = in[0].token.variableName;
	ex.isGlobal = in[0].token.isGlobal;

	vector<BracketGroup> tmp;
	for (auto const& b : in[1].children) {
		if (b.token.type == L",") {
			ex.arguments.push_back(ParseExpression(tmp));
			tmp.clear();
		}
		else {
			tmp.push_back(b);
		}
	}
	if (!tmp.empty()) { ex.arguments.push_back(ParseExpression(tmp)); }
	return ex;
}
ScriptExpression ParseBinaryExpressions(vector<BracketGroup> const& in) {
	vector<BracketGroup> tmp;
	vector<PreExpression> pres;
	for (auto const& b : in) {
		if (IsBinaryOperator(b.token)) {
			if (!tmp.empty()) {
				pres.push_back(ParseExpression(tmp));
				tmp.clear();
			}
			pres.push_back(b.token);
		}
		else {
			tmp.push_back(b);
		}
	}
	if (!tmp.empty()) { pres.push_back(ParseExpression(tmp)); }
	return ParsePreExpression(pres);
}
ScriptExpression ParseVector(vector<BracketGroup> const& in) {
	ScriptExpression ex(L"vec");
	vector<BracketGroup> tmp;
	for (auto const& b : in[0].children) {
		if (b.token.type == L",") {
			ex.elements.push_back(ParseExpression(tmp));
			tmp.clear();
		}
		else {
			tmp.push_back(b);
		}
	}
	if (!tmp.empty()) { ex.elements.push_back(ParseExpression(tmp)); }
	return ex;
}
ScriptExpression ParseAccess(vector<BracketGroup> const& in) {
	ScriptExpression ex(L"access");
	ex.accessedExpression = ParseExpression(vector<BracketGroup>(in.begin(), in.end() - 1));
	ex.indexExpression = ParseExpression(in.back().children);
	return ex;
}

ScriptExpression ParseExpression(vector<BracketGroup> const& in) {
	if (in.empty()) { return ScriptExpression(L"error"); }
	auto const& b = in[0];
	if (in.size() == 1) {
		if (b.type == 0) {
			if (b.token.type == L"var") { return ScriptExpression::CreateVariable(b.token.variableName, b.token.isGlobal); }
			else if (b.token.type == L"str") { return ScriptExpression::CreateString(b.token.stringValue); }
			else if (b.token.type == L"num") { return ScriptExpression(b.token.number); }
			else if (b.token.type == L"null") { return ScriptExpression(L"null"); }
			else { return ScriptExpression(L"error"); }
		}
		else if (b.type == 1) { return ParseExpression(b.children); }
		else if (b.type == 3) { return ParseVector(in); }
		else { return ScriptExpression(L"error"); }
	}

	bool call = in.size() == 2 &&
		in[0].token.type == L"var" && in[1].type == 1;
	if (call) { return ParseCall(in); }

	bool uni = in.size() == 2 && IsUnaryOperator(b.token);
	if (uni) { return ScriptExpression(b.token.type, { ParseExpression({ in[1] }) }); }

	for (auto const& br : in) {
		if (IsBinaryOperator(br.token)) { return ParseBinaryExpressions(in); }
	}

	if (in.back().type == 3) { return ParseAccess(in); }
	return ScriptExpression(L"error");
}

