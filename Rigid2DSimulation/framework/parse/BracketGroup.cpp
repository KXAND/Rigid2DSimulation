#include "BracketGroup.h"

wstring ToWString(BracketGroup const& group) {
    wstring serializedText;
    if (group.type == 0) { serializedText += ToWString(group.token); }
    else {
        serializedText += std::to_wstring(group.type) + L" {";
        for (auto const& childGroup : group.children) { serializedText += ToWString(childGroup) + L", "; }
        serializedText += L"}";
    }
    return serializedText;
}

vector<BracketGroup> AnalyzeBrackets(vector<Token> const& tokens) {
    vector<BracketGroup> groups;
    int parenthesisDepth = 0, braceDepth = 0, bracketDepth = 0;
    BracketGroup group;
    vector<Token> pendingTokens;
    bool isBalanced = true;
    for (auto const& token : tokens) {
        if (token.type == L"(") {
            ++parenthesisDepth;
            if (!isBalanced) { pendingTokens.push_back(token); }
            isBalanced = false;
            continue;
        }
        else if (token.type == L"{") {
            ++braceDepth;
            if (!isBalanced) { pendingTokens.push_back(token); }
            isBalanced = false;
            continue;
        }
        else if (token.type == L"[") {
            ++bracketDepth;
            if (!isBalanced) { pendingTokens.push_back(token); }
            isBalanced = false;
            continue;
        }
        else if (token.type == L")") { --parenthesisDepth; group.type = 1; }
        else if (token.type == L"}") { --braceDepth; group.type = 2; }
        else if (token.type == L"]") { --bracketDepth; group.type = 3; }
        else { group.type = 0; }

        isBalanced = parenthesisDepth == 0 && braceDepth == 0 && bracketDepth == 0;
        if (isBalanced) {
            if (group.type) {
                group.children = AnalyzeBrackets(pendingTokens);
                pendingTokens.clear();
            }
            else {
                group.token = token;
            }
            groups.push_back(group);
            group = {};
            // 其它不相关的地方需要是零值，后面为了省事有时会利用这一点。
        }
        else {
            pendingTokens.push_back(token);
        }
    }
    return isBalanced ? groups : vector<BracketGroup>();
}

