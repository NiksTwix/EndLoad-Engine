#pragma once
#include "..\Definitions\CommandsInfo.hpp"
#include "..\ErrorHandling\ErrorHandler.hpp"

namespace ELScript
{
	class ALUDecoder
	{
		private:
			bool IsAssignOperator(const Token& t) {
				if (t.type != TokenType::OPERATOR) return false;
				return OpCodeMap::IsAssign(t.value.strVal);
			}

			void ProcessDelayedAssignment(std::stack<Token>& operatorStack,
				std::vector<Command>& commands) {
				if (operatorStack.size() < 2) return;

				Token opToken = operatorStack.top();
				operatorStack.pop();
				Token varToken = operatorStack.top();
				operatorStack.pop();

				if (opToken.value.strVal == "=") {
					// ������� ������������
					commands.push_back(Command(OpCode::STORE, varToken.value.strVal, opToken.line));
				}
				else {
					std::string baseOp = opToken.value.strVal.substr(0, opToken.value.strVal.size() - 1);
					if (baseOp == "+") commands.push_back(Command(OpCode::ADD, Value(), opToken.line));
					else if (baseOp == "-") commands.push_back(Command(OpCode::SUB, Value(), opToken.line));
					else if (baseOp == "*") commands.push_back(Command(OpCode::MUL, Value(), opToken.line));
					else if (baseOp == "/") commands.push_back(Command(OpCode::DIV, Value(), opToken.line));
					else if (baseOp == "%") commands.push_back(Command(OpCode::MOD, Value(), opToken.line));

					commands.push_back(Command(OpCode::STORE, varToken.value.strVal, opToken.line));
				}
			}

			void AssignHandler(std::stack<Token>& operatorStack, std::vector<Command>& commands,
				const std::vector<Token>& operators) {
				if (operators.size() < 2) return;

				const Token& varToken = operators[0];
				const Token& opToken = operators[1];

				if (opToken.value.strVal == "=") {
					// ������� ������������ - �����������
					operatorStack.push(varToken);  // ������� ����������
					operatorStack.push(opToken);   // ����� ��������
				}
				else {
					// ��������� ������������ - ���� �����������
					operatorStack.push(varToken);
					operatorStack.push(opToken);
				}
			}
			void ApplyOperator(const Token& opToken, std::vector<Command>& commands,
				const std::string& varName = "") {
				std::string op = opToken.value.strVal;
				// ������ ������� ���������, �� ������������!
				if (OpCodeMap::HasOpCode(op)) {
					commands.push_back(Command(OpCodeMap::GetOpCode(op), Value(), opToken.line));
				}
			}
			void ProcessParenthesis(std::stack<Token>& operatorStack, std::vector<Command>& commands) {
				// ����������� ��� ��������� �� ����������� ������
				while (!operatorStack.empty()) {
					Token top = operatorStack.top();
					operatorStack.pop();

					if (top.type == TokenType::DELIMITER && top.value.strVal == "(") {
						break; // ����� ����������� ������
					}

					ApplyOperator(top, commands);
				}
			}
			void ProcessOperator(const Token& opToken, std::stack<Token>& operatorStack,
				std::vector<Command>& commands) {

				static const std::unordered_map<std::string, int> precedence = {
					{"||", 2}, {"&&", 2}, {"!", 2}, // �������� ��������� ! ��� ���������� ������
					{"==", 3}, {"!=", 3}, {"<", 3}, {">", 3}, {"<=", 3}, {">=", 3},
					{"+", 4}, {"-", 4},
					{"*", 5}, {"/", 5}, {"%", 5},
					{"(", 0} // ������ ���������
				};

				std::string currentOp = opToken.value.strVal;

				// ���������� ��������� ������������ - ��� �������������� ��������!
				if (IsAssignOperator(opToken)) {
					return;
				}

				// ����������� ��������� � higher or equal precedence
				while (!operatorStack.empty()) {
					Token top = operatorStack.top();

					// ��������������� �� ����������� ������
					if (top.type == TokenType::DELIMITER && top.value.strVal == "(") {
						break;
					}

					// ���������, ���� �� �������� � ������� �����������
					if (precedence.count(top.value.strVal) > 0 &&
						precedence.count(currentOp) > 0 &&
						precedence.at(currentOp) <= precedence.at(top.value.strVal)) {
						ApplyOperator(top, commands);
						operatorStack.pop();
					}
					else {
						break;
					}
				}

				operatorStack.push(opToken);
			}
			bool IsUnaryOperator(const Token& token, const std::vector<Token>& tokens, size_t index) {
				if (index < tokens.size() - 1 && (token.value.strVal == "-" || token.value.strVal == "!") && tokens[index + 1].value.strVal != "=") {
					// ������� ����: ������ ��������� ��� ����� ���������/������
					return index == 0 ||
						tokens[index - 1].type == TokenType::OPERATOR ||
						(tokens[index - 1].type == TokenType::DELIMITER &&
							tokens[index - 1].value.strVal == "(");
				}
				return false;
			}

			bool IsArrayAssign(std::vector<Token>& tokens, int index) 
			{
				for (size_t i = index; i < tokens.size(); i++) {
					Token& t = tokens[i];
					if (i + 1 < tokens.size() && t.type == TokenType::IDENTIFIER && tokens[i + 1].value.strVal == "[")
					{
						for (int j = i; j < tokens.size(); j++) 
						{
							if (j + 1 < tokens.size() && tokens[j].value.strVal == "]" && tokens[j + 1].value.strVal == "=") return true;
						}
					}
				}
				return false;
			}

			void ParseIndexAssign(std::vector<Token>& tokens, size_t& i, std::vector<Command>& commands) 
			{
				Token t = tokens[i];
				std::string arrayName = t.value.strVal;
				Token assignOp;
				int assign_index = -1;
				for (int j = i; j < tokens.size(); j++)
				{
					if (IsAssignOperator(tokens[j]))
					{
						assignOp = tokens[j];
						assign_index = j;
					}
				}
				// ���������� ���, �������� ������������ � '['
				// �������� ��������� ������� (���������� ������)
				std::vector<Token> indexExpr;
				int bracketDepth = 0;

				while (i < tokens.size()) {
					if (tokens[i].value.strVal == "[") bracketDepth++;
					else if (tokens[i].value.strVal == "]") bracketDepth--;
					else if (bracketDepth > 0) {
						indexExpr.push_back(tokens[i]);
					}
					if (IsAssignOperator(tokens[i]))
					{
						break;
					}
					i++;
				}
				// i ������ �� ������ ����� ']'

				// �������� ��������� ������ ����� (��������)
				std::vector<Token> rightExpr;

				while (i < tokens.size()) {
					rightExpr.push_back(tokens[i]);
					i++;
				}

				auto indexCommands = ExpressionHandler(indexExpr);
				auto valueCommands = ExpressionHandler(rightExpr);

				// ���������� ��� ��� ������������ �� �������:
				// 1. ��������� �������� (������ �����)
				commands.insert(commands.end(), valueCommands.begin(), valueCommands.end());
				// 2. ��������� ������
				commands.insert(commands.end(), indexCommands.begin(), indexCommands.end());
				// 3. ��������� ������
				commands.push_back(Command(OpCode::LOAD, arrayName, t.line));
				// 4. �����������: stack -> [value, index, array] -> (������)
				commands.push_back(Command(OpCode::SET_BY, assignOp.value.strVal, t.line)); // ������� �������� ��� ��������� += � �.�.

			}
			
			std::shared_ptr<std::unordered_map<std::string, Value>> ParseDict(std::vector<Token>& tokens, size_t& i, size_t line) {
				std::unordered_map<std::string, Value> dict;
				return std::make_shared<std::unordered_map<std::string, Value>>(dict);
			}
			std::vector<Command> ParseArray(std::vector<Token>& tokens, size_t& i, size_t line) {
				std::vector<std::vector<Token>> expressions;
				i++; //������� [

				std::vector<Token> temp_tokens;

				std::vector<Command> commands;

				for (; i < tokens.size(); i++) 
				{
					Token& t = tokens[i];

					if (t.value.strVal == ",") 
					{
						if (!temp_tokens.empty()) expressions.push_back(temp_tokens);
						temp_tokens.clear();
						continue;
					}
					if (t.value.strVal == "]") 
					{
						if (!temp_tokens.empty())expressions.push_back(temp_tokens);
						temp_tokens.clear();
						break;
					}
					temp_tokens.push_back(t);
				}

				for (int j = expressions.size() - 1; j >= 0; j--) 
				{
					auto commands_ = ExpressionHandler(expressions[j]);
					commands.insert(commands.end(), commands_.begin(), commands_.end());
				}
				commands.push_back(Command(OpCode::ARRAY_MAKE, (int)expressions.size()));
				return commands;
			}

			std::vector<Command> ParseDictionary(std::vector<Token>& tokens, size_t& i, size_t line) {
				std::vector<Command> commands;
				i++; // ���������� '['

				std::vector<std::pair<std::vector<Token>, std::vector<Token>>> pairs; // ���� [����, ��������]

				// ������ ���� ����:��������
				while (i < tokens.size() && tokens[i].value.strVal != "]") {
					// ������ ���� (�� ���������)
					std::vector<Token> key_tokens;
					while (i < tokens.size() && tokens[i].value.strVal != ":") {
						key_tokens.push_back(tokens[i]);
						i++;
					}

					if (i >= tokens.size()) 
					{
						ErrorHandlerManager::RaiseError(EHMessage(EHMessageType::Error, "[ALUDecoder] Expected ':'"));
					}  
					i++; // ���������� ':'   

					// ������ �������� (�� ������� ��� ])
					std::vector<Token> value_tokens;
					while (i < tokens.size() && tokens[i].value.strVal != "," && tokens[i].value.strVal != "]") {
						value_tokens.push_back(tokens[i]);
						i++;
					}

					pairs.push_back({ key_tokens, value_tokens });

					if (i < tokens.size() && tokens[i].value.strVal == ",") {
						i++; // ���������� ','
					}
				}

				if (i >= tokens.size() || tokens[i].value.strVal != "]") {
					throw std::runtime_error("Expected ']'");
				}
				i++; // ���������� ']'

				// ���������� ��� � �������� �������
				for (auto it = pairs.rbegin(); it != pairs.rend(); ++it) {
					// ����
					auto key_cmds = ExpressionHandler(it->first);
					commands.insert(commands.end(), key_cmds.begin(), key_cmds.end());
					// ��������
					auto value_cmds = ExpressionHandler(it->second);
					commands.insert(commands.end(), value_cmds.begin(), value_cmds.end());

					
				}

				commands.push_back(Command(OpCode::DICT_MAKE, Value((int)pairs.size()), line));
				return commands;
			}
			bool IsLikelyDictionary(std::vector<Token>& tokens, size_t start_index) {
				size_t j = start_index;
				while (j < tokens.size() && tokens[j].value.strVal != "]") {
					// ���� ������� ��������� ':' � ��� ������ ����� �������
					if (tokens[j].value.strVal == ":") {
						return true;
					}
					// ���� ������� ������� �� ������ ��������� � ��� ������ ������
					if (tokens[j].value.strVal == ",") {
						return false;
					}
					j++;
				}
				return false;
			}
	public:
		std::vector<Command> ExpressionHandler(std::vector<Token> tokens) {
			std::vector<Command> commands;
			std::stack<Token> operatorStack;

			

			for (size_t i = 0; i < tokens.size(); i++) {
				Token& t = tokens[i];

				// ���������, �� �������� �� ��� �����������: �������, ���� �� ����� �������������� '['
				if (t.type == TokenType::IDENTIFIER && IsArrayAssign(tokens,i)) 
				{
					// ��� ������������ �������� �������! a[index] = value
					ParseIndexAssign(tokens, i, commands);		//��� ������� ���� ��������, �� ��� ��������� �� ������� ��� �����
					continue;
				}

				// ��������� ������������
				if (t.type == TokenType::IDENTIFIER &&
					i + 1 < tokens.size() && IsAssignOperator(tokens[i + 1])) {
					

					// ��� ��������� ����������: ����� ��������� ����������
					if (tokens[i + 1].value.strVal != "=")commands.push_back(Command(OpCode::LOAD, t.value.strVal, t.line));
					// ����������� �������� �� �����
					operatorStack.push(t);
					operatorStack.push(tokens[i + 1]);
					i++;

					continue;
				}
				else if (t.type == TokenType::IDENTIFIER) {

					if (i + 1 < tokens.size() && tokens[i + 1].value.strVal == "[") //�������� � ��� ��������
					{
						std::string arrayName = t.value.strVal;
						i += 2;// ���������� ��� ������� � ����������� ������ [
						// �������� ��������� ������ ������ (������) �� ����������� ]
						std::vector<Token> indexExpr;
						int bracketDepth = 1;

						while (i < tokens.size() && bracketDepth > 0) {
							if (tokens[i].value.strVal == "[") bracketDepth++;
							else if (tokens[i].value.strVal == "]") bracketDepth--;
							else {
								indexExpr.push_back(tokens[i]);
							}
							i++;
						}
						i--; // ������������ ������, �.�. ���� while �������� ����� ']'
						// ���������� ������������ ��������� �������
						auto indexCommands = ExpressionHandler(indexExpr);
						// ���������� ���:

						commands.insert(commands.end(), indexCommands.begin(), indexCommands.end()); // ��������� ������, ����� �� ����
						commands.push_back(Command(OpCode::LOAD, arrayName, t.line)); // ��������� ������ (��� ��� �����)

						commands.push_back(Command(OpCode::GET_BY, Value(), t.line)); // GET_INDEX: stack -> [array, index] -> value
						continue;
					}

					// ���������, �� �������� �� ��������� ����� ����������� ������� - ������� ������ �������
					else if (i + 1 < tokens.size() && tokens[i + 1].value.strVal == "(") {
						// ��� ����� �������!
						std::string funcName = t.value.strVal;
						// ���������� ��� ������� � ����������� ������
						i += 2;

						// �������� ��������� �� ����������� ������
						std::vector<Token> args;
						std::vector<Token> currentArg;
						std::vector<std::vector<Command>> inversed_args;
						int parenDepth = 1; // ��������� ��������� ������

						while (i < tokens.size() && parenDepth > 0) {
							if (i < tokens.size() - 1 && tokens[i].type == TokenType::IDENTIFIER && tokens[i + 1].value.strVal == "(") 
							{
								std::vector<Token> inner_function_call;
								inner_function_call.push_back(tokens[i]);
								inner_function_call.push_back(tokens[i+1]);
								i += 2;
								int start = parenDepth;
								parenDepth++;
								for (; i < tokens.size(); i++) 
								{
									Token t = tokens[i];
									if (tokens[i].value.strVal == "(") parenDepth++;
									else if (tokens[i].value.strVal == ")" && parenDepth  == start + 1)
									{
										inner_function_call.push_back(tokens[i]);
										parenDepth--;
										break;
									}
									else if (tokens[i].value.strVal == ")")parenDepth--;
									inner_function_call.push_back(tokens[i]);
								}
								auto f_call = ExpressionHandler(inner_function_call);
								inversed_args.push_back(f_call);
								i++;
							}
							if (tokens[i].value.strVal == "(") parenDepth++;
							else if (tokens[i].value.strVal == ")") parenDepth--;
							else if (tokens[i].value.strVal == "," && parenDepth == 1) {
								// ����������� ���������� - ������������ ��������� ��������
								if (!currentArg.empty()) {
									auto argCommands = ExpressionHandler(currentArg);
									inversed_args.push_back(argCommands);
									currentArg.clear();
								}
							}
							else {
								currentArg.push_back(tokens[i]);
							}
							i++;
						}
						i-=1;	//������������ ������ ��������
						// ������������ ��������� ��������
						if (!currentArg.empty()) {
							auto argCommands = ExpressionHandler(currentArg);
							inversed_args.push_back(argCommands);
						}

						for (int i = inversed_args.size() - 1; i >= 0; i--) 
						{
							commands.insert(commands.end(), inversed_args[i].begin(), inversed_args[i].end());
						}
						// ������ ��������� � ����� � �������: [argN, argN-1, ..., arg1]
						// ��������� ����� �������
						commands.push_back(Command(OpCode::CALL, funcName, t.line));
						continue;
					}
					else {
						// ������� ����������
						commands.push_back(Command(OpCode::LOAD, t.value, t.line));
					}
				}
				// ��������
				if (t.type == TokenType::LITERAL) {
					commands.push_back(Command(OpCode::PUSH, t.value, t.line));
				}
				// ���������
				else if (t.type == TokenType::OPERATOR) {
					if (IsUnaryOperator(t, tokens, i)) {
						Token unaryToken = t;
						if (unaryToken.value.strVal == "-") unaryToken.value.strVal = "-u";
						ProcessOperator(unaryToken, operatorStack, commands);
					}
					else {
						ProcessOperator(t, operatorStack, commands);
					}
				}

				// ������
				else if (t.type == TokenType::DELIMITER) {
					if (t.value.strVal == "(") {
						operatorStack.push(t);
					}
					else if (t.value.strVal == ")") {
						ProcessParenthesis(operatorStack, commands);
					}
					else if (t.value.strVal == "[")
					{
						bool is_dict = IsLikelyDictionary(tokens, i + 1);

						std::vector<Command> commands1;
						if (is_dict) {
							commands1 = ParseDictionary(tokens, i, t.line);
						}
						else {
							commands1 = ParseArray(tokens, i, t.line);
						}

						commands.insert(commands.end(), commands1.begin(), commands1.end());
					}
				}
				else if (t.type == TokenType::KEYWORD) 
				{
					if (t.value.strVal == "meta")
					{
						if (i < tokens.size() - 2 && tokens[i + 1].value.strVal == ":" && tokens[i + 2].type == TokenType::IDENTIFIER) 
						{
							commands.push_back(Command(OpCode::LOADM, tokens[i + 2].value, t.line));
							i += 2;
						}
						else 
						{
							ErrorHandlerManager::RaiseError(EHMessage(EHMessageType::Error, "[ALUDecoder] invalid meta variable."));
						}
					}
					
				}
			}
			while (!operatorStack.empty()) {
				Token top = operatorStack.top();

				// ���� ��� �������� ������������ - ������������ ��������
				if (IsAssignOperator(top)) {
					ProcessDelayedAssignment(operatorStack, commands);
				}
				// ���� ��� ������� ��������
				else if (top.type == TokenType::OPERATOR && !IsAssignOperator(top)) {
					ApplyOperator(top, commands);
					operatorStack.pop();
				}
				// ���� ��� ����������� ������ - ������, �� ����������
				else if (top.type == TokenType::DELIMITER && top.value.strVal == "(") {
					operatorStack.pop(); // ���������� ������ - ����������
				}
				else {
					// ���-�� unexpected - ������ �������
					operatorStack.pop();
				}
			}
			return commands;
		}
	};
}