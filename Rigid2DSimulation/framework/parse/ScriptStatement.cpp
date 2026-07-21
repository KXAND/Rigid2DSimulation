#include "ScriptStatement.h"
#include "BracketGroup.h"

ScriptStatement::ScriptStatement(wstring const& type) : type(type) {}
wstring ToWString(ScriptStatement const& statement) {
	wstring serializedText = statement.type;
	if (statement.type == L"define") {
		serializedText += L":" + statement.functionName + L",(";
		for (auto& parameterName : statement.parameterNames) { serializedText += parameterName + L","; }
		serializedText += L"),{";
		for (auto& childStatement : statement.bodyStatements) { serializedText += L"\n" + ToWString(childStatement) + L","; }
		serializedText += L"}";
	}
	else if (statement.type == L"if") {
		serializedText += L":" + ToWString(statement.condition) + L",{";
		for (auto& childStatement : statement.thenStatements) { serializedText += L"\n" + ToWString(childStatement) + L","; }
		serializedText += L"}{";
		for (auto& childStatement : statement.elseStatements) { serializedText += L"\n" + ToWString(childStatement) + L","; }
		serializedText += L"}";
	}
	else if (statement.type == L"while") {
		serializedText += L":" + ToWString(statement.loopCondition) + L",{";
		for (auto& childStatement : statement.loopStatements) { serializedText += L"\n" + ToWString(childStatement) + L","; }
		serializedText += L"}";
	}
	else if (statement.type == L"return") { serializedText += L":" + ToWString(statement.returnExpression); }
	else if (statement.type == L"single") { serializedText += L":" + ToWString(statement.expression); }
	else if (statement.type == L"assign") {
		serializedText += L":" + ToWString(statement.leftExpression) + L"," + ToWString(statement.rightExpression);
	}
	return serializedText;
}

ScriptStatement ParseDefinition(vector<BracketGroup> const& in) {
	if (in.size() < 4) { return ScriptStatement(L"error"); }
	// 不想让这里的函数和参数也牵扯到 global 里面去，感觉完全不必要。
	if (in[1].token.type != L"var" || in[1].token.isGlobal) { return ScriptStatement(L"error"); }
	ScriptStatement p(L"define");
	p.functionName = in[1].token.variableName;
	bool comma = true;

	for (auto const& b : in[2].children) {
		if (comma) {
			p.parameterNames.push_back(b.token.variableName);
			if (b.token.type != L"var" || b.token.isGlobal) { return ScriptStatement(L"error"); }
		}
		else if (b.token.type != L",") { return ScriptStatement(L"error"); }
		comma = !comma;
	}
	p.bodyStatements = ParseStatements(in[3].children);
	return p;
}
ScriptStatement ParseReturn(vector<BracketGroup> const& in) {
	ScriptStatement p(L"return");
	auto tmp = vector<BracketGroup>(in.begin() + 1, in.end());
	p.returnExpression = tmp.empty() ? ScriptExpression(L"null") : ParseExpression(tmp);
	return p;
}
ScriptStatement ParseAssignment(vector<BracketGroup> const& in) {
	ScriptStatement p(L"assign");
	auto it = in.begin();
	for (; it != in.end() && it->token.type != L"="; ++it);
	p.leftExpression = ParseExpression(vector<BracketGroup>(in.begin(), it));
	p.rightExpression = ParseExpression(vector<BracketGroup>(it + 1, in.end()));
	return p;
}
ScriptStatement ParseIf(vector<BracketGroup> const& in) {
	if (in.size() < 3) { return ScriptStatement(L"error"); }
	if (in[1].type != 1 || in[2].type != 2) { return ScriptStatement(L"error"); }

	ScriptStatement p(L"if");
	p.condition = ParseExpression(in[1].children);
	p.thenStatements = ParseStatements(in[2].children);
	if (in.size() != 3) {
		if (in.size() != 5) { return ScriptStatement(L"error"); }
		if (in[3].token.type != L"else" || in[4].type != 2) { return ScriptStatement(L"error"); }
		p.elseStatements = ParseStatements(in[4].children);
	}
	return p;
}
ScriptStatement ParseWhile(vector<BracketGroup> const& in) {
	if (in.size() != 3) { return ScriptStatement(L"error"); }
	if (in[1].type != 1 || in[2].type != 2) { return ScriptStatement(L"error"); }
	ScriptStatement p(L"while");
	p.loopCondition = ParseExpression(in[1].children);
	p.loopStatements = ParseStatements(in[2].children);
	return p;
}

ScriptStatement ParseStatement(vector<BracketGroup> const& in) {
	if (in.empty()) { return ScriptStatement(L"error"); }
	if (in[0].token.type == L"def") { return ParseDefinition(in); }
	if (in[0].token.type == L"return") { return ParseReturn(in); }
	if (in[0].token.type == L"if") { return ParseIf(in); }
	if (in[0].token.type == L"while") { return ParseWhile(in); }
	for (auto const& b : in) {
		if (b.token.type == L"=") { return ParseAssignment(in); }
	}
	ScriptStatement p(L"single");
	p.expression = ParseExpression(in);
	return p;
}
vector<ScriptStatement> ParseStatements(vector<BracketGroup> const& in) {
	vector<BracketGroup> cur;
	vector<ScriptStatement> out;
	bool curly = false;

	for (auto const& b : in) {
		if (b.token.type == L";") {
			if (!cur.empty()) {
				auto p = ParseStatement(cur);
				if (p.type == L"error") { return {}; }
				out.push_back(p);
				cur.clear();
			}
		}
		else {
			if (b.token.type != L"else" && curly) {
				auto p = ParseStatement(cur);
				if (p.type == L"error") { return {}; }
				out.push_back(p);
				cur.clear();
			}
			cur.push_back(b);
		}
		curly = (b.type == 2);
	}

	if (curly) {
		auto p = ParseStatement(cur);
		if (p.type == L"error") { return {}; }
		out.push_back(p);
	}
	else if (!cur.empty()) {
		return {};
	}
	return out;
}

