#include <bits/stdc++.h>
using namespace std;
#define fst first
#define snd second

stringstream codeStream;
vector<string> codeBuffer;
map<string, int> labelMap;
vector<pair<int, string>> labelVec;

struct DataType {
    int type; // 0: int, 1: float, 2: char
    string value;
    DataType() : type(0), value("") {}
    DataType(int t, string v) : type(t), value(v) {}
};
map<string, DataType> variables;

void generateLabels() { 
    // Gera as etiquetas e armazena suas posições
    int idx = 1;
    labelVec.push_back({0, codeBuffer[0]});
    while (codeBuffer[idx] != "halt") {
        if (codeBuffer[idx] == ";") {
            idx++;
            if (!labelMap.count(codeBuffer[idx])) {
                labelMap[codeBuffer[idx]] = idx;
                labelVec.push_back({idx, codeBuffer[idx]});
            }
        }
        idx++;
    }
}

string getVarValue(int idx) { 
    // Obtém o valor de uma variável a partir do nome
    string varName = "";
    for (char ch : codeBuffer[idx]) varName += ch;
    if (!variables.count(varName)) {
        cerr << "Erro: Variável não encontrada" << endl;
        exit(EXIT_FAILURE);
    }
    return variables[varName].value;
}

void printCommand(int idx) { 
    // Imprime um valor ou string no console
    if (codeBuffer[idx][0] == '"') {
        string output = "";
        for (int i = 1; codeBuffer[idx][i] != '"'; i++) {
            output += codeBuffer[idx][i];
        }
        cout << output << endl;
    } else {
        string varValue = getVarValue(idx);
        cout << varValue << endl;
    }
}

void inputCommand(string varName) { 
    // Lê uma entrada e armazena na variável
    if (!variables.count(varName)) {
        string inputVal;
        cin >> inputVal;
        if (inputVal.size() == 1 && isalpha(inputVal[0])) {
            variables[varName].type = 2;
            variables[varName].value = inputVal;
        } else {
            bool isFloat = inputVal.find('.') != string::npos;
            variables[varName].type = isFloat ? 1 : 0;
            variables[varName].value = inputVal;
        }
    }
}

bool handleInput(int idx) { 
    // Manipula a entrada da variável, se já existir, retorna falso
    string varName = codeBuffer[idx];
    if (!variables.count(varName)) {
        inputCommand(varName);
        return true;
    }
    return false;
}

int findLabelPosition(string lbl) { 
    // Encontra a posição de uma etiqueta
    int left = 0, right = labelVec.size() - 1, mid;
    while (left <= right) {
        mid = (left + right) / 2;
        if (lbl == labelVec[mid].snd) return mid;
        if (lbl < labelVec[mid].snd) right = mid - 1;
        else left = mid + 1;
    }
    return -1;
}

int jumpToLabel(string lbl) { 
    // Salta para a posição de uma etiqueta
    sort(labelVec.begin(), labelVec.end());
    int labelIdx = findLabelPosition(lbl);
    return labelIdx;
}

bool evaluateCondition(int idx) { 
    // Avalia uma condição de comparação
    string operand1 = "", condition = "", operand2 = "";
    int i = 0;
    while (!strchr("<>!=:", codeBuffer[idx][i])) operand1 += codeBuffer[idx][i++];
    while (strchr("<>!=:", codeBuffer[idx][i])) condition += codeBuffer[idx][i++];
    operand2 = codeBuffer[idx].substr(i);

    if (variables.count(operand1) && variables.count(operand2)) {
        if (condition == ">") return stod(variables[operand1].value) > stod(variables[operand2].value);
        if (condition == "<") return stod(variables[operand1].value) < stod(variables[operand2].value);
        if (condition == ">=") return stod(variables[operand1].value) >= stod(variables[operand2].value);
        if (condition == "<=") return stod(variables[operand1].value) <= stod(variables[operand2].value);
        if (condition == "==") return stod(variables[operand1].value) == stod(variables[operand2].value);
        if (condition == "!=") return stod(variables[operand1].value) != stod(variables[operand2].value);
    } else {
        cerr << "Erro: Condição inválida ou variável não encontrada" << endl;
        exit(EXIT_FAILURE);
    }
    return false;
}

string calculateExpression(vector<string> tokens, vector<int> ops) {
    // Placeholder para implementação futura da avaliação de expressões
}

void handleExpression(int idx, string varName, int tokenPos) { 
    // Avalia e atribui o resultado de uma expressão à variável
    string expression = codeBuffer[idx].substr(tokenPos + 1);
    if (expression.find_first_of("+-") == string::npos) {
        if (!variables.count(varName)) {
            if (variables.count(expression)) {
                variables[varName] = variables[expression];
            } else {
                variables[varName].type = (expression.find('.') == string::npos) ? 0 : 1;
                variables[varName].value = expression;
            }
        } else {
            if (variables.count(expression)) {
                variables[varName].value = variables[expression].value;
            } else {
                variables[varName].value = expression;
            }
        }
    } else {
        vector<string> tokens;
        vector<int> positions;
        string current = "";
        for (int i = 0; i < expression.size(); i++) {
            if (expression[i] == '+' || expression[i] == '-') {
                tokens.push_back(current);
                current = "";
                positions.push_back(i);
            } else {
                current += expression[i];
            }
        }
        tokens.push_back(current);
        string result = calculateExpression(tokens, positions);
        variables[varName].value = result;
    }
}

void interpreter() { 
    // Interpreta e executa os comandos
    int idx = 0;
    while (codeBuffer[idx] != "halt") {
        if (codeBuffer[idx] == "rem") {
            while (codeBuffer[idx] != ";" && codeBuffer[idx] != ":") idx++;
        } else if (codeBuffer[idx] == "print") {
            printCommand(++idx);
        } else if (codeBuffer[idx] == "input") {
            if (!handleInput(++idx)) {
                cerr << "Erro: Variável já existe" << endl;
                exit(EXIT_FAILURE);
            }
        } else if (codeBuffer[idx] == "goto") {
            int lblIdx = jumpToLabel(codeBuffer[++idx]);
            if (lblIdx != -1) idx = lblIdx;
            else {
                cerr << "Erro: Salto inválido na linha " << idx << endl;
                exit(EXIT_FAILURE);
            }
        }
        idx++;
    }
}
