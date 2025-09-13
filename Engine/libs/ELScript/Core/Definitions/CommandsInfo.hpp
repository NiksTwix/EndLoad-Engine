#pragma once
#include <unordered_map>
#include <string>
#include <vector>
#include <stack>
#include <unordered_set>

namespace ELScript
{
    constexpr int MaxCallStackDepth = 65535;
    enum ValueType {VOID, NUMBER, STRING, BOOL, ARRAY, DICT};

    struct Value {
        ValueType type;

        union {
            double numberVal;
            bool boolVal; 
        };
        std::string strVal;   // ��� ����� 
        std::shared_ptr<std::vector<Value>> arrayVal; // ��� ��������
        std::shared_ptr<std::unordered_map<std::string,Value>> dictVal; // ��� ��������
        // ������������
        Value() : type(ValueType::VOID), numberVal(0) {  } 

        Value(double v) : type(ValueType::NUMBER), numberVal(v) {}
        Value(int v) : type(ValueType::NUMBER), numberVal(v) {}

        Value(bool v) : type(ValueType::BOOL), boolVal(v) {}

        Value(const std::string& s) : type(ValueType::STRING) {
            new (&strVal) std::string(s); 
        }

        Value(const char* s) : type(ValueType::STRING) {
            new (&strVal) std::string(s);
        }

        Value(std::shared_ptr<std::vector<Value>> arr) : type(ValueType::ARRAY), arrayVal(arr) {}
        Value(std::shared_ptr<std::unordered_map<std::string, Value>> map) : type(ValueType::DICT), dictVal(map) {}
        // ������� ���� (����� �����!)
        // ����������� �����������
        Value(const Value& other) : type(other.type) {
            switch (type) {
            case ValueType::NUMBER: numberVal = other.numberVal; break;
            case ValueType::BOOL:   boolVal = other.boolVal; break;
            case ValueType::STRING: new (&strVal) std::string(other.strVal); break; // �������� ������
            case ValueType::ARRAY:  arrayVal = other.arrayVal; break; // �������� shared_ptr
            case ValueType::DICT:  dictVal = other.dictVal; break; // �������� shared_ptr
            case ValueType::VOID:   break; // ������ �� ������
            }
        }

        // �������� ������������
        Value& operator=(const Value& other) {
            // ���� ������� ������ ��� ������ ������, � ����� ����������
            if (this == &other) return *this; // ������ �� ����������������
            destroyCurrent();

            type = other.type;
            switch (type) {
            case ValueType::NUMBER: numberVal = other.numberVal; break;
            case ValueType::BOOL:   boolVal = other.boolVal; break;
            case ValueType::STRING: new (&strVal) std::string(other.strVal); break;
            case ValueType::ARRAY:  arrayVal = other.arrayVal; break;
            case ValueType::DICT:  dictVal = other.dictVal; break;
            case ValueType::VOID:   break;
            }
            return *this;
        }
        Value(Value&& other) noexcept : type(other.type) {
            switch (type) {
            case ValueType::NUMBER: numberVal = other.numberVal; break;
            case ValueType::BOOL:   boolVal = other.boolVal; break;
            case ValueType::STRING: new (&strVal) std::string(std::move(other.strVal)); break;
            case ValueType::ARRAY:  arrayVal = std::move(other.arrayVal); break; // �����: ���������� shared_ptr
            case ValueType::DICT:  dictVal = std::move(other.dictVal); break;
            case ValueType::VOID:   break;
            }
            // ����� ����������� ����� �������� �������� ������ � ���������� ���������!
            other.type = ValueType::VOID; // ����� ���������� other ������ �� �����
        }

        // �������� ������������� ������������
        Value& operator=(Value&& other) noexcept {
            if (this != &other) {
                destroyCurrent(); // ���������� ������� ������
                type = other.type;
                switch (type) {
                case ValueType::NUMBER: numberVal = other.numberVal; break;
                case ValueType::BOOL:   boolVal = other.boolVal; break;
                case ValueType::STRING: new (&strVal) std::string(std::move(other.strVal)); break;
                case ValueType::ARRAY:  arrayVal = std::move(other.arrayVal); break;
                case ValueType::DICT:  dictVal = std::move(other.dictVal); break;
                case ValueType::VOID:   break;
                }
                other.type = ValueType::VOID;
            }
            return *this;
        }

        std::string ToString() const 
        {
            std::string result;
            switch (type)
            {
            case ELScript::VOID:
                return "";
            case ELScript::NUMBER:
                return std::to_string(numberVal);
            case ELScript::STRING:
                return strVal;
            case ELScript::BOOL:
                return boolVal ? "true" : "false";
            case ELScript::ARRAY:
                for (auto l : *arrayVal)
                {
                    result += l.ToString() + " ";
                }
                return result;
            case ELScript::DICT:
                for (auto l : *dictVal)
                {
                    result += l.first + ":" + l.second.ToString() + " ";
                }
                return result;
            default:
                return "";
            }
        }
        static std::string GetTypeString(ValueType type) 
        {
            std::string result;
            switch (type)
            {
            case ELScript::VOID:
                return "void";
            case ELScript::NUMBER:
                return "number";
            case ELScript::STRING:
                return "string";
            case ELScript::BOOL:
                return "bool";
            case ELScript::ARRAY:
                return "array";
            case ELScript::DICT:
                return "dict";
            default:
                return "void";
            }
        }
        // ����������
        ~Value() {
            destroyCurrent();
        }

    private:
        void destroyCurrent() {
            // ���� �������� ���������� ������ ��� ������
            if (type == ValueType::STRING) {
                strVal.~basic_string();
            }
            // ��� shared_ptr � ���������� ���������� �������� �� �����
        }
    };

    // ���� ������� (���������)
    enum class TokenType {
        NOP,
        IDENTIFIER,// ����������, �������� �����
        OPERATOR,  // +, -, =, *, /, >, <
        LITERAL,   // �����, ������, true/false
        DELIMITER,
        OP_END, //����� ����� ��������
        SCOPE_START,
        SCOPE_END,
        TYPE_MARKER,
        KEYWORD
    };

    // ��������� ������
    struct Token {
        TokenType type;
        Value value;
        size_t line; // ��� ������
        size_t depth;  // ������� �����������
    };

    struct CommandNode
    {
        std::vector<Token> tokens;
        std::vector<CommandNode> children;
        int line;
    };

    enum class OpCode
    {
        NOP = 1,
        LABEL, //�����
        EXIT,   //����� �� ���������

        PUSH,
        POP,
        CALL,
        RET,
        ADD,
        SUB,
        MUL,
        DIV,
        MOD,//%
        NEG,//- NEGATIVE
        //Logic
        EQUAL,
        EQUAL_N,
        GREATER,
        LESS,
        EGREATER,
        ELESS,
        AND,
        OR,
        NOT,
        //Variables
        STORE,
        LOAD,
        LOADM,  //LOAD META
        //IF A- Absolute, R-relative
        JMPA,
        JMPA_IF_N,
        JMPA_IF,

        PRINT,

        //Special
        EXCEPT_HANDLING, // ������ ������ ��� �������� ����, ������� �� ����� ����� ���� ��������� ����������, value � Command ������ � ���� �������� �����
        DECLARE,
        DECLARED,   //�������� �� ������� ���������� � current scope
        FUNC_DECLARE,
        FUNC_DECLARED,
        SCOPESTR,   //������ ����� ������� ��������� ����������
        SCOPEEND,    //����� ������� ��������� ����������

        GET_TYPE_STR,
        CONVERT_TYPE,        //����������� �������� � number,string,bool

        //������ � ���������
        ARRAY_MAKE,
        GET_BY,
        SET_BY,
        ARRAY_PUSH_BACK,
        ARRAY_POP_BACK,
        ARRAY_INSERT_INDEX,
        ARRAY_ERASE_INDEX,
        ARRAY_SIZE,

        //������ � ���-������
        DICT_MAKE,
        DICT_SIZE,
        DICT_ERASE,
    };
    
    struct Command 
    {
        OpCode code = OpCode::NOP;
        Value operand;  //����������� �������� �������, ��������, ��� ����������� ����� � ��
        int line; // ������ ��������� ����
        Command(OpCode code, Value operand)
        {
            this->code = code;
            this->operand = operand;
        }
        Command(OpCode code)
        {
            this->code = code;
        }
        Command(OpCode code, Value operand,int line)
        {
            this->code = code;
            this->operand = operand;
            this->line = line;
        }
        Command() 
        {
            this->code = OpCode::NOP;
        }
    };

   
    class OpCodeMap 
    {
    private:
        static std::unordered_map<std::string, OpCode> GetOpCodeMap() 
        {
            static const std::unordered_map<std::string, OpCode> opCodeMap = {
                // ��������������
                {"+", OpCode::ADD},
                {"-", OpCode::SUB},
                {"*", OpCode::MUL},
                {"/", OpCode::DIV},
                {"%", OpCode::MOD},
                {"-u", OpCode::NEG}, // ������� �����

                // ����������
                {"==", OpCode::EQUAL},
                {"!=", OpCode::EQUAL_N}, // + NOT �����
                {">", OpCode::GREATER},
                {"<", OpCode::LESS},
                {">=", OpCode::EGREATER},
                {"<=", OpCode::ELESS},
                {"&&", OpCode::AND},
                {"||", OpCode::OR},
                {"!", OpCode::NOT},

                // ������������ (�������������� ��������)
                {"=", OpCode::STORE},
                {"+=", OpCode::ADD},
                {"-=", OpCode::SUB},
                {"*=", OpCode::MUL},
                {"/=", OpCode::DIV},
                {"%=", OpCode::MOD}
            };
            return opCodeMap;
        }
        static std::unordered_set<std::string> GetAssignOpCodeMap()
        {
            static const std::unordered_set<std::string> assignOps = {
                     "=", "+=", "-=", "*=", "/=", "%="
            };
            return assignOps;
        }

        static std::unordered_map<std::string, OpCode> GetStringOpCodeMap() 
        {
            static const std::unordered_map<std::string, OpCode> opcodeMap = {
                {"nop", OpCode::NOP},
                {"label", OpCode::LABEL},
                {"exit", OpCode::EXIT},
                {"push", OpCode::PUSH},
                {"pop", OpCode::POP},
                {"call", OpCode::CALL},
                {"ret", OpCode::RET},
                {"add", OpCode::ADD},
                {"sub", OpCode::SUB},
                {"mul", OpCode::MUL},
                {"div", OpCode::DIV},
                {"mod", OpCode::MOD},
                {"neg", OpCode::NEG},
                {"equal", OpCode::EQUAL},
                {"equal_n", OpCode::EQUAL_N},
                {"greater", OpCode::GREATER},
                {"less", OpCode::LESS},
                {"egreater", OpCode::EGREATER},
                {"eless", OpCode::ELESS},
                {"and", OpCode::AND},
                {"or", OpCode::OR},
                {"not", OpCode::NOT},
                {"store", OpCode::STORE},
                {"load", OpCode::LOAD},
                {"loadm", OpCode::LOADM},
                {"jmpa", OpCode::JMPA},
                {"jmpa_if_n", OpCode::JMPA_IF_N},
                {"jmpa_if", OpCode::JMPA_IF},
                {"print", OpCode::PRINT},
                {"except_handling", OpCode::EXCEPT_HANDLING},
                {"declare", OpCode::DECLARE},
                {"declared", OpCode::DECLARED},
                {"func_declare", OpCode::FUNC_DECLARE},
                {"func_declared", OpCode::FUNC_DECLARED},
                {"scopestr", OpCode::SCOPESTR},
                {"scopeend", OpCode::SCOPEEND},

                {"convert_type", OpCode::CONVERT_TYPE},
                {"get_type_str", OpCode::GET_TYPE_STR},
                {"get_by", OpCode::GET_BY},
                {"set_by", OpCode::SET_BY},
                {"array_push_back", OpCode::ARRAY_PUSH_BACK},
                {"array_pop_back", OpCode::ARRAY_POP_BACK},
                {"array_insert_index", OpCode::ARRAY_INSERT_INDEX},
                {"array_erase_index", OpCode::ARRAY_ERASE_INDEX},
                {"array_size", OpCode::ARRAY_SIZE},
                {"dict_size", OpCode::DICT_SIZE},
                {"dict_erase", OpCode::DICT_ERASE},
            };
            return opcodeMap;
        }
        static std::unordered_map<std::string, ValueType> GetStringTypeMap()
        {
            static const std::unordered_map<std::string, ValueType> typeMap = {
                {"string",ValueType::STRING},
                {"bool",ValueType::BOOL},
                {"number",ValueType::NUMBER},
                {"array",ValueType::ARRAY},
                {"void",ValueType::VOID},
            };
            return typeMap;
        }
    public:
        static OpCode GetOpCode(std::string token) 
        {
            if (GetOpCodeMap().count(token) > 0) return GetOpCodeMap().at(token);
            return OpCode::NOP;
        }
        static OpCode GetStringOpCode(std::string token)
        {
            if (GetStringOpCodeMap().count(token) > 0) return GetStringOpCodeMap().at(token);
            return OpCode::NOP;
        }
        static ValueType GetStringType(std::string token)
        {
            if (GetStringTypeMap().count(token) > 0) return GetStringTypeMap().at(token);
            return ValueType::VOID;
        }
        static bool HasOpCode(std::string token) 
        {
            return GetOpCodeMap().count(token) > 0;
        }
        static bool IsAssign(std::string token) 
        {
            return GetAssignOpCodeMap().count(token) > 0;
        }
    };
}