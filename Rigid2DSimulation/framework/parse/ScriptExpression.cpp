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

bool ParseNegation(vector<PreExpression> const& preExpressions, ScriptExpression& result) {
	for (int operatorIndex = (0), expressionCount = int(preExpressions.size()); operatorIndex < expressionCount; ++operatorIndex) {
		auto const& currentItem = preExpressions[operatorIndex];
		if (!currentItem.isToken) { continue; }
		auto const& operatorType = currentItem.token.type;

		if ((operatorType == L"+" || operatorType == L"-") && (operatorIndex == 0 || preExpressions[operatorIndex - 1].isToken)) {
			bool isInvalid = operatorIndex == preExpressions.size() - 1 || preExpressions[operatorIndex + 1].isToken;
			if (isInvalid) {
				result = ScriptExpression(L"error");
				return false;
			}

			auto reducedExpressions = vector<PreExpression>(preExpressions.begin(), preExpressions.begin() + operatorIndex);
			reducedExpressions.push_back(ScriptExpression(operatorType + operatorType, { preExpressions[operatorIndex + 1].expression }));
			for (int copyIndex = (operatorIndex + 2), tailCount = int(preExpressions.size()); copyIndex < tailCount; ++copyIndex) { reducedExpressions.push_back(preExpressions[copyIndex]); }
			result = ParsePreExpression(reducedExpressions);
			return true;
		}
	}
	return false;
}
bool ParseBinary(vector<PreExpression> const& preExpressions, ScriptExpression& result, vector<wstring> const& operatorTypes) {
	for (int operatorIndex = (0), expressionCount = int(preExpressions.size()); operatorIndex < expressionCount; ++operatorIndex) {
		auto const& currentItem = preExpressions[operatorIndex];
		if (!currentItem.isToken) { continue; }
		auto const& operatorType = currentItem.token.type;

		if (find(operatorTypes.begin(), operatorTypes.end(), operatorType) != operatorTypes.end()) {
			bool isInvalid =
				operatorIndex == 0 || operatorIndex == preExpressions.size() - 1 ||
				preExpressions[operatorIndex - 1].isToken || preExpressions[operatorIndex + 1].isToken;
			if (isInvalid) {
				result = ScriptExpression(L"error");
				return false;
			}

			auto reducedExpressions = vector<PreExpression>(preExpressions.begin(), preExpressions.begin() + operatorIndex - 1);
			reducedExpressions.push_back(ScriptExpression(operatorType, { preExpressions[operatorIndex - 1].expression, preExpressions[operatorIndex + 1].expression }));
			for (int copyIndex = (operatorIndex + 2), tailCount = int(preExpressions.size()); copyIndex < tailCount; ++copyIndex) { reducedExpressions.push_back(preExpressions[copyIndex]); }
			result = ParsePreExpression(reducedExpressions);
			return true;
		}
	}
	return false;
}
ScriptExpression ParsePreExpression(vector<PreExpression> const& preExpressions) {
	if (preExpressions.empty()) { return ScriptExpression(L"error"); }
	if (preExpressions.size() == 1) {
		if (preExpressions[0].isToken) { return ScriptExpression(L"error"); }
		return preExpressions[0].expression;
	}

	ScriptExpression result;
	if (ParseNegation(preExpressions, result)) { return result; }
	if (ParseBinary(preExpressions, result, { L"*", L"/" })) { return result; }
	if (ParseBinary(preExpressions, result, { L"+", L"-" })) { return result; }
	if (ParseBinary(preExpressions, result,
		{ L"==", L"!=", L"<=", L">=", L"<", L">" })) { return result; }
	if (ParseBinary(preExpressions, result, { L"&&", L"||" })) { return result; }
	if (ParseBinary(preExpressions, result, { L"." })) { return result; }
	return ScriptExpression(L"error");
}

ScriptExpression ParseCall(vector<BracketGroup> const& groups) {
	ScriptExpression expression(L"call");
	expression.functionName = groups[0].token.variableName;
	expression.isGlobal = groups[0].token.isGlobal;

	vector<BracketGroup> argumentGroups;
	for (auto const& group : groups[1].children) {
		if (group.token.type == L",") {
			expression.arguments.push_back(ParseExpression(argumentGroups));
			argumentGroups.clear();
		}
		else {
			argumentGroups.push_back(group);
		}
	}
	if (!argumentGroups.empty()) { expression.arguments.push_back(ParseExpression(argumentGroups)); }
	return expression;
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
ScriptExpression ParseVector(vector<BracketGroup> const& groups) {
	ScriptExpression expression(L"vec");
	vector<BracketGroup> elementGroups;
	for (auto const& group : groups[0].children) {
		if (group.token.type == L",") {
			expression.elements.push_back(ParseExpression(elementGroups));
			elementGroups.clear();
		}
		else {
			elementGroups.push_back(group);
		}
	}
	if (!elementGroups.empty()) { expression.elements.push_back(ParseExpression(elementGroups)); }
	return expression;
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

