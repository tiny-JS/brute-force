#ifndef TINYJS_DEFINE_H
#define TINYJS_DEFINE_H

#include <stdlib.h>
#include <string.h>

//源文件行数最大限制
//#define MAX_SOURCE_LINE_SIZE    4096
#define MAX_IDENT_SIZE          256 // Maximum identifier size
#define COMMENTIDENT            '#' //汇编语言注释符号
#define IDSTRING 				"JSE0"
// ---- opcode ------------------

#define INSTR_MOV               0

#define INSTR_ADD               1
#define INSTR_SUB               2
#define INSTR_MUL               3
#define INSTR_DIV               4
#define INSTR_MOD               5
#define INSTR_EXP               6
#define INSTR_NEG               7
#define INSTR_INC               8
#define INSTR_DEC               9

#define INSTR_AND               10
#define INSTR_OR                11
#define INSTR_XOR               12
#define INSTR_NOT               13
#define INSTR_SHL               14
#define INSTR_SHR               15

#define INSTR_CONCAT            16
#define INSTR_GETCHAR           17
#define INSTR_SETCHAR           18

#define INSTR_JMP               19
#define INSTR_JE                20
#define INSTR_JNE               21
#define INSTR_JG                22
#define INSTR_JL                23
#define INSTR_JGE               24
#define INSTR_JLE               25

#define INSTR_PUSH              26
#define INSTR_POP               27

#define INSTR_CALL              28
#define INSTR_RET               29
#define INSTR_CALLHOST          30

#define INSTR_PAUSE             31
#define INSTR_EXIT              32

//#define MAX_INSTR_LOOKUP_COUNT  256 //指令查找表最大数量
//#define MAX_INSTR_MNEMONIC_SIZE 16 //指令助记符字符串最大值

//操作数类型位域掩码
#define OP_FLAG_TYPE_INT                    1
#define OP_FLAG_TYPE_REG                    128
#define OP_FLAG_TYPE_FLOAT                  2
#define OP_FLAG_TYPE_STRING                 4
#define OP_FLAG_TYPE_MEM_REF                8
#define OP_FLAG_TYPE_FUNC_NAME              32
#define OP_FLAG_TYPE_LINE_LABEL             16
#define OP_FLAG_TYPE_HOST_API_CALL          64

//指令操作数类型
#define OP_TYPE_INT                 0           // Integer literal value
#define OP_TYPE_FLOAT               1           // Floating-point literal value
#define OP_TYPE_STRING_INDEX        2           // String literal value
#define OP_TYPE_ABS_STACK_INDEX     3           // Absolute array index
#define OP_TYPE_REL_STACK_INDEX     4           // Relative array index
#define OP_TYPE_INSTR_INDEX         5           // Instruction index
#define OP_TYPE_FUNC_INDEX          6           // Function index
#define OP_TYPE_HOST_API_CALL_INDEX 7           // Host API call index
#define OP_TYPE_REG                 8           // Register

//main函数
#define MAIN_FUNC_NAME              "_main"

// ---- Error Strings ---------------------------------------------------------------------

// The following macros are used to represent assembly-time error strings
#define ERROR_MSSG_INVALID_INPUT	\
			"Invalid input"

#define ERROR_MSSG_LOCAL_SETSTACKSIZE	\
			"SetStackSize can only appear in the global scope"

#define ERROR_MSSG_INVALID_STACK_SIZE	\
			"Invalid stack size"

#define ERROR_MSSG_MULTIPLE_SETSTACKSIZES	\
			"Multiple instances of SetStackSize illegal"

#define ERROR_MSSG_IDENT_EXPECTED	\
			"Identifier expected"

#define ERROR_MSSG_INVALID_ARRAY_SIZE	\
			"Invalid array size"

#define ERROR_MSSG_IDENT_REDEFINITION	\
			"Identifier redefinition"

#define ERROR_MSSG_UNDEFINED_IDENT	\
			"Undefined identifier"

#define ERROR_MSSG_NESTED_FUNC	\
			"Nested functions illegal"

#define ERROR_MSSG_FUNC_REDEFINITION	\
			"Function redefinition"

#define ERROR_MSSG_UNDEFINED_FUNC	\
			"Undefined function"

#define ERROR_MSSG_GLOBAL_PARAM	\
			"Parameters can only appear inside functions"

#define ERROR_MSSG_MAIN_PARAM	\
			"_Main () function cannot accept parameters"

#define ERROR_MSSG_GLOBAL_LINE_LABEL	\
			"Line labels can only appear inside functions"

#define ERROR_MSSG_LINE_LABEL_REDEFINITION	\
			"Line label redefinition"

#define ERROR_MSSG_UNDEFINED_LINE_LABEL	\
			"Undefined line label"

#define ERROR_MSSG_GLOBAL_INSTR	\
			"Instructions can only appear inside functions"

#define ERROR_MSSG_INVALID_INSTR	\
			"Invalid instruction"

#define ERROR_MSSG_INVALID_OP	\
			"Invalid operand"

#define ERROR_MSSG_INVALID_STRING	\
			"Invalid string"

#define ERROR_MSSG_INVALID_ARRAY_NOT_INDEXED	\
			"Arrays must be indexed"

#define ERROR_MSSG_INVALID_ARRAY	\
			"Invalid array"

#define ERROR_MSSG_INVALID_ARRAY_INDEX	\
			"Invalid array index"


//-------------------------------------------------------------------------------------------------------
//										类型
//-------------------------------------------------------------------------------------------------------
//$ Boolean
typedef enum {
	false,
	true
} Boolean;

//$ 文件头部
typedef struct
{
//    uint id; #暂时不需要
//    float version; #暂时不需要
	int iStackSize;         //堆栈大小
	int iGlobalDateSize;    //脚本中全局数据的大小
	Boolean iIsMainFuncPresent; //_main()是否存在
	ssize_t iMainFuncIndex;     //_main()函数的索引
} scriptHeader;

//$ 链表
typedef struct _linkedListNode
{
	void *pData;
	struct _linkedListNode *pNext;
} linkedListNode;

typedef struct
{
	linkedListNode *pHead,
			*pTail;
	int iNodeCount;
} linkedList;

//$ 表节点数据相关
//> 函数
typedef struct
{
	int iIndex;                  //函数表索引
	char *ptrName;   //名称
	int iEntryPoint;             //入口
	int iParamCount;             //实参
	int iLocalDataSize;          //局部数据大小
} funcNode;

//> 标签
typedef struct
{
	int iIndex;                     //标签表索引
	char *ptrIdent;  //标签名字
	int iTargetIndex;               //标签首指令索引
	int iFuncIndex;              //标签所处函数
} labelNode;

//> 符号
typedef struct
{
	int iIndex;                   //符号表索引
	char *ptrIdent;  //标识符
	int iSize;                   //变量大小, 数组是N、其他为1
	int iStackIndex;             //堆栈索引
	int iFuncIndex;              //符号作用域(声明)函数
} symbolNode;

//> 指令
typedef int opTypes;
typedef struct
{
	int iIndex;        //指令索引
	char *ptrMnemonic;    //助记符
	int iOpcode;          //操作码
	int iOpCount;         //操作数个数
	opTypes *opList;      //操作数列表指针
} instrLookup;

//$ 指令操作数结构
typedef struct
{
	int iType;
	union {
		int iIntLiteral;         //整型字面量
		float fFloatLiteral;     //浮点型字面量
		int   iStringTableIndex; //字符串表索引
		int   iStackIndex;       //堆栈索引
		int   iInstrIndex;       //指令索引
		int   iFuncIndex;        //函数索引
		int   iHostAPICallIndex; //主应用程序API调用索引
		int   iReg; //寄存器码值
	};
	int iOffsetIndex;  //偏移量索引
} op;

//$ 指令结构
typedef struct
{
	int iOpcode;       //操作码
	int iOpcount;      //操作数个数统计
	op *pOpList;          //指向操作数列表的指针
} instr;

//$ 全局编译信息
typedef struct
{
	//--- 源代码信息相关
	char *ptrSourceFileName;     //源代码文件名
	char **ptrSourceCode;        //源代码数据
	size_t iSourceCodeLine;      //源代码数据行数

	char *ptrOutputFileName;     //输出文件名
	//--- 输出文件头部结构信息相关
	Boolean setStackSizeFound;   //是否已经调用setStackSize false|true
	int VERSION_MAJOR;           //主版本号
	int VERSION_MINOR;           //次版本号
	scriptHeader headerInfo;     //头部信息

	//--- stream
	instr *pInstrStream;         //指令流
	int iCurrInstrIndex;         //当前指令索引

	//--- 语法分析表
	linkedList *stringTable;      //字符串表
	linkedList *funcTable;        //函数表
	linkedList *symbolTable;      //符号表
	linkedList *labelTable;       //标签表
	linkedList *hostAPICallTable; //主程序API调用表
	linkedList *instrTable;       //指令表
} global;

//$ lexer结构
//> token
typedef enum {
	TOKEN_TYPE_INT,             //整型字面量
	TOKEN_TYPE_FLOAT,           //浮点型字面量
	TOKEN_TYPE_STRING,          //字符串型字面量, 不包括引号. 引号认定为分隔属性字
	TOKEN_TYPE_QUOTE,           //双引号
	TOKEN_TYPE_IDENT,           //标识符
	TOKEN_TYPE_COLON,           //冒号
	TOKEN_TYPE_OPEN_BRACKET,    //左中括号
	TOKEN_TYPE_CLOSE_BRACKET,   //右中括号
	TOKEN_TYPE_COMMA,           //逗号
	TOKEN_TYPE_OPEN_BRACE,      //左大括号
	TOKEN_TYPE_CLOSE_BRACE,     //右大括号
	TOKEN_TYPE_NEWLINE,         //换行
	TOKEN_TYPE_INSTR,           //指令
	TOKEN_TYPE_SETSTACKSIZE,    //setStackSize指示符
	TOKEN_TYPE_VAR,             //var指示符
	TOKEN_TYPE_FUNC,            //fun指示符
	TOKEN_TYPE_PARAM,           //param指示符
	TOKEN_TYPE_REG_RETVAL,      //_retVal指示符
	TOKEN_TYPE_INVALID,         //不符合属性字的错误编码
	END_OF_TOKEN_STREAM         //到达属性字流的尾部
} token;

//> token状态
typedef enum {
	LEX_STATE_NO_STRING,
	LEX_STATE_IN_STRING,
	LEX_STATE_END_STRING,
} lexerState;

//> lexer
typedef struct {
	size_t iCurrSourceLine;
	size_t iIndex0;
	size_t iIndex1;
	token currToken;
	char *ptrCurrLexeme;
	lexerState iCurrLexState; //当前词法分析器状态
} lexer;

//> lexer状态
typedef struct {
	//指令相关
	int iInstrStreamSize;
	instrLookup currInstr;
	//函数相关
	funcNode *pCurrFunc;
	int iCurrFuncIndex;
	Boolean iIsFuncActive;
	char *ptrCurrFuncName;
	int iCurrFuncParamCount;
	int iCurrFuncLocalDataSize;

	//椎栈设置
	Boolean iIsSetStackSizeFound;

	//指令流设置
	int iCurrInstrIndex;

	//词法分析器相关
	lexer lexer;
} state;

#endif //TINYJS_DEFINE_H
