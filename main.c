#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include<memory.h>
/*
For the language grammar, please refer to Grammar section on the github page:
  https://github.com/lightbulb12294/CSI2P-II-Mini1#grammar
*/

#define MAX_LENGTH 200
typedef enum {
	ASSIGN, ADD, SUB, MUL, DIV, REM, PREINC, PREDEC, POSTINC, POSTDEC, IDENTIFIER, CONSTANT, LPAR, RPAR, PLUS, MINUS, END
} Kind;
typedef enum {
	STMT, EXPR, ASSIGN_EXPR, ADD_EXPR, MUL_EXPR, UNARY_EXPR, POSTFIX_EXPR, PRI_EXPR
} GrammarState;
typedef struct TokenUnit {
	Kind kind;
	int val; // record the integer value or variable name
	struct TokenUnit* next;
} Token;
typedef struct ASTUnit {
	Kind kind;
	int val; // record the integer value or variable name
	struct ASTUnit* lhs, * mid, * rhs;
} AST;
typedef enum { LOAD_, STORE_, ADD_, SUB_, MUL_, DIV_, REM_, CONS_, ASSIGN_ ,ID_,PLUS_,MINUS_,PREINC_,PREDEC_,POSTINC_,POSTDEC_
} ASM_TYPE;
typedef struct node_
{
	ASM_TYPE type;
	char op[100];
	struct node_* next;
}node;

node* make_node(char* op_, ASM_TYPE type_)
{
	node* root = (node*)malloc(sizeof(node));
	root->type = type_;
	strcpy(root->op, op_);
	root->next = NULL;
	return root;
}
void insert(node** root,char* op_, ASM_TYPE type_)
{
	node* now = *root;
	if (now == NULL)*root = make_node(op_, type_);
	else
	{
		while (now->next != NULL)now = now->next;
		now->next = make_node(op_, type_);
	}
}
void show(node* root)
{
	node* now = root;
	while (now != NULL)
	{
		//for(int i=0;i<strlen(root->op);i++)printf("%c",root->op[i]);
		//printf("\n");
		//printf("%d\n",strlen(root->op));
		//if(now->type == ASSIGN_)printf("assign:");
		//if(now->type == ADD_)printf("add:");
		printf("%s\n",now->op);
		now = now->next;
	}
	return;
}
int reg_arr[256];
int post_inc_check[3];
int post_dec_check[3];
int expr_at_reg[3];
int id_at_reg[3];
void reset()
{
    memset(reg_arr,0,sizeof(reg_arr));
    memset(post_dec_check,0,sizeof(post_dec_check));
    memset(post_inc_check,0,sizeof(post_inc_check));
    memset(expr_at_reg,0,sizeof(expr_at_reg));
    memset(id_at_reg,0,sizeof(id_at_reg));
}
int find_avai_reg()
{
	for (int i = 0; i < 257; i++)
	{
		if (reg_arr[i] == 0)return i;
	}
	return -1;
}
int get_expr(AST* node)  //return [0] [4] [8]
{
	if (node->mid != NULL)
	{
		node = node->mid;
		while (node->kind == LPAR)node = node->mid;
	}
	if (node->val == 'x')return 0;
	else if (node->val == 'y')return 4;
	else return 8;
}
void post_check(node** ASM_head)
{
    for(int i=0;i<3;i++)
    {
        if(post_inc_check[i])
        {
            int reg = id_at_reg[i];
            char op_1[100] = "";
            char inc[10] = "";
            char reg_[10] = "";
            itoa(post_inc_check[i],inc,10);
            itoa(reg,reg_,10);
            strcat(op_1,"add ");
            strcat(op_1,"r");
            strcat(op_1,reg_);
            strcat(op_1," r");
            strcat(op_1,reg_);
            strcat(op_1," ");
            strcat(op_1,inc);
            insert(ASM_head,op_1,POSTINC_);
            char op_2[100] = "";
            char store_reg[10] = "";
            itoa(i*4,store_reg,10);
            strcat(op_2,"store ");
            strcat(op_2,"[");
            strcat(op_2,store_reg);
            strcat(op_2,"]");
            strcat(op_2," r");
            strcat(op_2,reg_);
            insert(ASM_head,op_2,POSTINC_);
        }
        if(post_dec_check[i])
        {
            int reg = id_at_reg[i];
            char op_1[100] = "";
            char dec[10] = "";
            char reg_[10] = "";
            itoa(post_dec_check[i],dec,10);
            itoa(reg,reg_,10);
            strcat(op_1,"sub ");
            strcat(op_1,"r");
            strcat(op_1,reg_);
            strcat(op_1," r");
            strcat(op_1,reg_);
            strcat(op_1," ");
            strcat(op_1,dec);
            insert(ASM_head,op_1,POSTDEC_);
            char op_2[100] = "";
            char store_reg[10] = "";
            itoa(i*4,store_reg,10);
            strcat(op_2,"store ");
            strcat(op_2,"[");
            strcat(op_2,store_reg);
            strcat(op_2,"]");
            strcat(op_2," r");
            strcat(op_2,reg_);
            insert(ASM_head,op_2,POSTDEC_);
        }


    }
}


//node* make_node(char* op_, ASM_TYPE type_);
//void insert(node** root, char* op_, ASM_TYPE type_);
//node* head = NULL;
//void show(node* head);
/// utility interfaces

// err marco should be used when a expression error occurs.
#define err(x) {\
	puts("Compile Error!");\
	if (DEBUG) {\
		fprintf(stderr, "Error at line: %d\n", __LINE__);\
		fprintf(stderr, "Error message: %s\n", x);\
	}\
	exit(0);\
}
// You may set DEBUG=1 to debug. Remember setting back to 0 before submit.
#define DEBUG 0
// Split the input char array into token linked list.
Token* lexer(const char* in);
// Create a new token.
Token* new_token(Kind kind, int val);
// Translate a token linked list into array, return its length.
size_t token_list_to_arr(Token** head);
// Parse the token array. Return the constructed AST.
AST* parser(Token* arr, size_t len);
// Parse the token array. Return the constructed AST.
AST* parse(Token* arr, int l, int r, GrammarState S);
// Create a new AST node.
AST* new_AST(Kind kind, int val);
// Find the location of next token that fits the condition(cond). Return -1 if not found. Search direction from start to end.
int findNextSection(Token* arr, int start, int end, int (*cond)(Kind));
// Return 1 if kind is ASSIGN.
int condASSIGN(Kind kind);
// Return 1 if kind is ADD or SUB.
int condADD(Kind kind);
// Return 1 if kind is MUL, DIV, or REM.
int condMUL(Kind kind);
// Return 1 if kind is RPAR.
int condRPAR(Kind kind);
// Check if the AST is semantically right. This function will call err() automatically if check failed.
void semantic_check(AST* now);
// Generate ASM code.
int codegen(AST* root,node** ASM_head);
// Free the whole AST.
void freeAST(AST* now);

/// debug interfaces

// Print token array.
void token_print(Token* in, size_t len);
// Print AST tree.
void AST_print(AST* head);
AST* AST_head;

char input[MAX_LENGTH];

int main() {
	while (fgets(input, MAX_LENGTH, stdin) != NULL) {
		Token* content = lexer(input);
		size_t len = token_list_to_arr(&content);
		if (len == 0) continue;
		AST* ast_root = parser(content, len);
		semantic_check(ast_root);
		AST_print(ast_root);
		AST_head = ast_root;
		node* ASM_head = NULL;
		codegen(ast_root,&ASM_head);
		post_check(&ASM_head);
		show(ASM_head);
		reset();
		free(content);
		freeAST(ast_root);
	}
	return 0;
}

Token* lexer(const char* in) {
	Token* head = NULL;
	Token** now = &head;
	for (int i = 0; in[i]; i++) {
		if (isspace(in[i])) // ignore space characters
			continue;
		else if (isdigit(in[i])) {
			(*now) = new_token(CONSTANT, atoi(in + i));
			while (in[i + 1] && isdigit(in[i + 1])) i++;
		}
		else if ('x' <= in[i] && in[i] <= 'z') // variable
			(*now) = new_token(IDENTIFIER, in[i]);
		else switch (in[i]) {
		case '=':
			(*now) = new_token(ASSIGN, 0);
			break;
		case '+':
			if (in[i + 1] && in[i + 1] == '+') {
				i++;
				// In lexer scope, all "++" will be labeled as PREINC.
				(*now) = new_token(PREINC, 0);
			}
			// In lexer scope, all single "+" will be labeled as PLUS.
			else (*now) = new_token(PLUS, 0);
			break;
		case '-':
			if (in[i + 1] && in[i + 1] == '-') {
				i++;
				// In lexer scope, all "--" will be labeled as PREDEC.
				(*now) = new_token(PREDEC, 0);
			}
			// In lexer scope, all single "-" will be labeled as MINUS.
			else (*now) = new_token(MINUS, 0);
			break;
		case '*':
			(*now) = new_token(MUL, 0);
			break;
		case '/':
			(*now) = new_token(DIV, 0);
			break;
		case '%':
			(*now) = new_token(REM, 0);
			break;
		case '(':
			(*now) = new_token(LPAR, 0);
			break;
		case ')':
			(*now) = new_token(RPAR, 0);
			break;
		case ';':
			(*now) = new_token(END, 0);
			break;
		default:
			err("Unexpected character.");
		}
		now = &((*now)->next);
	}
	return head;
}

Token* new_token(Kind kind, int val) {
	Token* res = (Token*)malloc(sizeof(Token));
	res->kind = kind;
	res->val = val;
	res->next = NULL;
	return res;
}

size_t token_list_to_arr(Token** head) {
	size_t res;
	Token* now = (*head), * del;
	for (res = 0; now != NULL; res++) now = now->next;
	now = (*head);
	if (res != 0) (*head) = (Token*)malloc(sizeof(Token) * res);
	for (int i = 0; i < res; i++) {
		(*head)[i] = (*now);
		del = now;
		now = now->next;
		free(del);
	}
	return res;
}

AST* parser(Token* arr, size_t len) {
	for (int i = 1; i < len; i++) {
		// correctly identify "ADD" and "SUB"
		if (arr[i].kind == PLUS || arr[i].kind == MINUS) {
			switch (arr[i - 1].kind) {
			case PREINC:
			case PREDEC:
			case IDENTIFIER:
			case CONSTANT:
			case RPAR:
				arr[i].kind = (Kind)(arr[i].kind - PLUS + ADD);
			default: break;
			}
		}
	}
	return parse(arr, 0, len - 1, STMT);
}

AST* parse(Token* arr, int l, int r, GrammarState S) {
	AST* now = NULL;
	if (l > r)
		err("Unexpected parsing range.");
	int nxt;
	switch (S) {
	case STMT:
		if (l == r && arr[l].kind == END)
			return NULL;
		else if (arr[r].kind == END)
			return parse(arr, l, r - 1, EXPR);
		else err("Expected \';\' at the end of line.");
	case EXPR:
		return parse(arr, l, r, ASSIGN_EXPR);
	case ASSIGN_EXPR:
		if ((nxt = findNextSection(arr, l, r, condASSIGN)) != -1) {
			now = new_AST(arr[nxt].kind, 0);
			now->lhs = parse(arr, l, nxt - 1, UNARY_EXPR);
			now->rhs = parse(arr, nxt + 1, r, ASSIGN_EXPR);
			return now;
		}
		return parse(arr, l, r, ADD_EXPR);
	case ADD_EXPR:
		if ((nxt = findNextSection(arr, r, l, condADD)) != -1) {
			now = new_AST(arr[nxt].kind, 0);
			now->lhs = parse(arr, l, nxt - 1, ADD_EXPR);
			now->rhs = parse(arr, nxt + 1, r, MUL_EXPR);
			return now;
		}
		return parse(arr, l, r, MUL_EXPR);
	case MUL_EXPR:
		// TODO: Implement MUL_EXPR.
		// hint: Take ADD_EXPR as reference.
		if ((nxt = findNextSection(arr, r, l, condMUL)) != -1)
		{
			now = new_AST(arr[nxt].kind, 0);
			now->lhs = parse(arr, l, nxt - 1, MUL_EXPR);
			now->rhs = parse(arr, nxt + 1, r, UNARY_EXPR);
			return now;
		}
		return parse(arr, l, r, UNARY_EXPR);
	case UNARY_EXPR:
		// TODO: Implement UNARY_EXPR.
		// hint: Take POSTFIX_EXPR as reference.
		if (arr[l].kind == PREINC || arr[l].kind == PREDEC||arr[l].kind==PLUS||arr[l].kind==MINUS)
		{
			now = new_AST(arr[l].kind, 0);
			now->mid = parse(arr, l + 1, r, UNARY_EXPR);
			return now;
		}
		return parse(arr, l, r, POSTFIX_EXPR);
	case POSTFIX_EXPR:
		if (arr[r].kind == PREINC || arr[r].kind == PREDEC) {
			// translate "PREINC", "PREDEC" into "POSTINC", "POSTDEC"
			now = new_AST((Kind)(arr[r].kind - PREINC + POSTINC), 0);
			now->mid = parse(arr, l, r - 1, POSTFIX_EXPR);
			return now;
		}
		return parse(arr, l, r, PRI_EXPR);
	case PRI_EXPR:
		if (findNextSection(arr, l, r, condRPAR) == r) {
			now = new_AST(LPAR, 0);
			now->mid = parse(arr, l + 1, r - 1, EXPR);
			return now;
		}
		if (l == r) {
			if (arr[l].kind == IDENTIFIER || arr[l].kind == CONSTANT)
				return new_AST(arr[l].kind, arr[l].val);
			err("Unexpected token during parsing.");
		}
		err("No token left for parsing.");
	default:
		err("Unexpected grammar state.");
	}
}

AST* new_AST(Kind kind, int val) {
	AST* res = (AST*)malloc(sizeof(AST));
	res->kind = kind;
	res->val = val;
	res->lhs = res->mid = res->rhs = NULL;
	return res;
}

int findNextSection(Token* arr, int start, int end, int (*cond)(Kind)) {
	int par = 0;
	int d = (start < end) ? 1 : -1;
	for (int i = start; (start < end) ? (i <= end) : (i >= end); i += d) {
		if (arr[i].kind == LPAR) par++;
		if (arr[i].kind == RPAR) par--;
		if (par == 0 && cond(arr[i].kind) == 1) return i;
	}
	return -1;
}

int condASSIGN(Kind kind) {
	return kind == ASSIGN;
}

int condADD(Kind kind) {
	return kind == ADD || kind == SUB;
}

int condMUL(Kind kind) {
	return kind == MUL || kind == DIV || kind == REM;
}

int condRPAR(Kind kind) {
	return kind == RPAR;
}

void semantic_check(AST* now) {
	if (now == NULL) return;
	// Left operand of '=' must be an identifier or identifier with one or more parentheses.
	if (now->kind == ASSIGN) {
		AST* tmp = now->lhs;
		while (tmp->kind == LPAR) tmp = tmp->mid;
		if (tmp->kind != IDENTIFIER)
			err("Lvalue is required as left operand of assignment.");
	}
	// Operand of INC/DEC must be an identifier or identifier with one or more parentheses.
	// TODO: Implement the remaining semantic_check code.
	// hint: Follow the instruction above and ASSIGN-part code to implement.
	// hint: Semantic of each node needs to be checked recursively (from the current node to lhs/mid/rhs node).
	while (now->kind == LPAR)now = now->mid;
	if (now->kind == PREDEC || now->kind == PREINC || now->kind == POSTDEC || now->kind == POSTINC)
	{
		AST* tmp = now->mid;
		while (tmp->kind == LPAR)tmp = tmp->mid;
		if (tmp->kind != IDENTIFIER)
		{
			err("Lvalue is required as left operand of assignment.");
		}
	}
	if (now->lhs != NULL)semantic_check(now->lhs);
	if (now->rhs != NULL)semantic_check(now->rhs);
}




int codegen(AST* root,node** ASM_head) {
	// TODO: Implement your codegen in your own way.
	// You may modify the function parameter or the return type, even the whole structure as you wish.
	//static node* head = NULL;
	static char load[] = "load ";
	static char store[] = "store ";
	static char add[] = "add ";
	static char sub[] = "sub ";
	static char rem[] = "rem ";
	static char mul[] = "mul ";
	static char div[] = "div ";
	static char blank[] = " ";

    if(root==NULL)return 0;
    if(root->kind == LPAR)
    {
        return codegen(root->mid,ASM_head);
    }
	if (root->kind == ASSIGN)
	{
		int left = get_expr(root->lhs);
		int reg = codegen(root->rhs,ASM_head);
        id_at_reg[left/4] = reg;
		char reg_str[10]="";
		itoa(reg, reg_str, 10);
		char expr[10]="";
		itoa(left, expr, 10);
		char op[100] = "";
		strcat(op, store);
		strcat(op, "[");
		strcat(op, expr);
		strcat(op, "]");
		strcat(op, " r");
		strcat(op, reg_str);
		strcat(op,"\0");
		//printf("%s\n",op);
		insert(ASM_head,op,ASSIGN_);
		//reg_arr[reg] = 0;
		if(root != AST_head)return reg;
		else return 0;
	}
	if(root->kind == ADD)
	{
		int reg1 = codegen(root->lhs,ASM_head);
		int reg2 = codegen(root->rhs,ASM_head);
		int reg3 = find_avai_reg();
		reg_arr[reg3] = 1;
		char reg3_[10] = "";
		itoa(reg3,reg3_,10);
		char op[100]="";
		char l_reg[10]="";
		itoa(reg1, l_reg, 10);
		char r_reg[10]="";
		itoa(reg2, r_reg, 10);

		strcat(op, add);
		strcat(op, "r");
		strcat(op, reg3_);
		strcat(op, " r");
		strcat(op, l_reg);
		strcat(op, " r");
		strcat(op, r_reg);
		strcat(op,"\0");
		insert(ASM_head,op, ADD_);

		return reg3;
	}
    if(root->kind == SUB)
	{
		int reg1 = codegen(root->lhs,ASM_head);
		int reg2 = codegen(root->rhs,ASM_head);
		int reg3 = find_avai_reg();
		reg_arr[reg3] = 1;
		char reg3_[10] = "";
		itoa(reg3,reg3_,10);
		char op[100]="";
		char l_reg[10]="";
		itoa(reg1, l_reg, 10);
		char r_reg[10]="";
		itoa(reg2, r_reg, 10);

		strcat(op, sub);
		strcat(op, " r");
		strcat(op, reg3_);
		strcat(op, " r");
		strcat(op, l_reg);
		strcat(op, " r");
		strcat(op, r_reg);
		strcat(op,"\0");
		//printf("%s\n",op);
		insert(ASM_head,op, SUB_);
		return reg3;
	}
	if(root->kind == MUL)
	{
		int reg1 = codegen(root->lhs,ASM_head);
		int reg2 = codegen(root->rhs,ASM_head);
        int reg3 = find_avai_reg();
        reg_arr[reg3] = 1;
		char reg3_[10] = "";
		itoa(reg3,reg3_,10);
		char op[100]="";
		char l_reg[10]="";
		itoa(reg1, l_reg, 10);
		char r_reg[10]="";
		itoa(reg2, r_reg, 10);

		strcat(op, mul);
		strcat(op, " r");
		strcat(op, reg3_);
		strcat(op, " r");
		strcat(op, l_reg);
		strcat(op, " r");
		strcat(op, r_reg);
		strcat(op,"\0");
		//printf("%s\n",op);
		insert(ASM_head,op, MUL_);
		return reg3;
	}
	if(root->kind == DIV)
	{
		int reg1 = codegen(root->lhs,ASM_head);
		int reg2 = codegen(root->rhs,ASM_head);
        int reg3 = find_avai_reg();
        reg_arr[reg3] = 1;
		char reg3_[10] = "";
		itoa(reg3,reg3_,10);
		char op[100]="";
		char l_reg[10]="";
		itoa(reg1, l_reg, 10);
		char r_reg[10]="";
		itoa(reg2, r_reg, 10);

		strcat(op, div);
		strcat(op, " r");
		strcat(op, reg3_);
		strcat(op, " r");
		strcat(op, l_reg);
		strcat(op, " r");
		strcat(op, r_reg);
		strcat(op,"\0");
		//printf("%s\n",op);
		insert(ASM_head,op, DIV_);
		return reg3;
	}
    if(root->kind == REM)
	{
		int reg1 = codegen(root->lhs,ASM_head);
		int reg2 = codegen(root->rhs,ASM_head);
        int reg3 = find_avai_reg();
        reg_arr[reg3] = 1;
		char reg3_[10] = "";
		itoa(reg3,reg3_,10);
		char op[100]="";
		char l_reg[10]="";
		itoa(reg1, l_reg, 10);
		char r_reg[10]="";
		itoa(reg2, r_reg, 10);

		strcat(op, rem);
		strcat(op, " r");
		strcat(op, reg3_);
		strcat(op, " r");
		strcat(op, l_reg);
		strcat(op, " r");
		strcat(op, r_reg);
		strcat(op,"\0");
		//printf("%s\n",op);
		insert(ASM_head,op, REM_);
		return reg3;
	}
    if(root->kind == CONSTANT)
    {
        int num = root->val;
        int reg = find_avai_reg();
        reg_arr[reg] = 1;
        char op[100]="";
        char reg_1[10]="";
        itoa(num,reg_1,10);
        char reg_2[10]="";
        itoa(reg,reg_2,10);
        if(num >= 0)
        {
            strcat(op,add);
            strcat(op,"r");
            strcat(op,reg_2);
            strcat(op," 0 ");
            strcat(op,reg_1);
            insert(ASM_head,op,CONS_);
        }
        else
        {
            strcat(op,sub);
            strcat(op,"r");
            strcat(op,reg_2);
            strcat(op," 0 ");
            strcat(op,"r");
            strcat(op,reg_1);
            insert(ASM_head,op,CONS_);
        }
        return reg;
    }
    if(root->kind == IDENTIFIER)
    {
        int id = get_expr(root);
        int reg = find_avai_reg();
        id_at_reg[id/4] = reg;
        reg_arr[reg]=1;
        char reg_[10]="";
        char op[100]="";
        char id_[10]="";
        itoa(id,id_,10);
        itoa(reg,reg_,10);
        strcat(op,load);
        strcat(op,"r");
        strcat(op,reg_);
        strcat(op," [");
        strcat(op,id_);
        strcat(op,"]");
        insert(ASM_head,op,ID_);
        return reg;
    }
    if(root->kind == PLUS)
    {
        return codegen(root->mid,ASM_head);
    }
    if(root->kind == MINUS)
    {
        int mid = codegen(root->mid,ASM_head);
        int reg = find_avai_reg();
        reg_arr[reg] = 1;
        char reg_[10]="";
        char op[100]="";
        char mid_[10] = "";
        itoa(mid,mid_,10);
        itoa(reg,reg_,10);
        strcat(op,sub);
        strcat(op,"r");
        strcat(op,reg_);
        strcat(op," 0 ");
        strcat(op,"r");
        strcat(op,mid_);
        insert(ASM_head,op,MINUS_);
        return reg;
    }
    if(root->kind == PREINC)
    {
        int mid = codegen(root->mid,ASM_head);
        char op_1[100] = "";
        char mid_[10] = "";
        itoa(mid,mid_,10);
        strcat(op_1,add);
        strcat(op_1,"r");
        strcat(op_1,mid_);
        strcat(op_1," r");
        strcat(op_1,mid_);
        strcat(op_1," 1");
        insert(ASM_head,op_1,PREINC_);
        char op_2[100]="";
        char store_[10]="";
        itoa(get_expr(root),store_,10);
        strcat(op_2,store);
        strcat(op_2,"[");
        strcat(op_2,store_);
        strcat(op_2,"]");
        strcat(op_2," r");
        strcat(op_2,mid_);
        insert(ASM_head,op_2,PREINC_);
        return mid;
    }
    if(root->kind == PREDEC)
    {
        int mid = codegen(root->mid,ASM_head);
        char op_1[100] = "";
        char mid_[10] = "";
        itoa(mid,mid_,10);
        strcat(op_1,sub);
        strcat(op_1,"r");
        strcat(op_1,mid_);
        strcat(op_1," r");
        strcat(op_1,mid_);
        strcat(op_1," 1");
        insert(ASM_head,op_1,PREDEC_);
        char op_2[100]="";
        char store_[10]="";
        itoa(get_expr(root),store_,10);
        strcat(op_2,store);
        strcat(op_2,"[");
        strcat(op_2,store_);
        strcat(op_2,"]");
        strcat(op_2," r");
        strcat(op_2,mid_);
        insert(ASM_head,op_2,PREDEC_);
        return mid;
    }
    if(root->kind == POSTINC)
    {
        int id = get_expr(root);
        id/=4;
        post_inc_check[id]++;
        return codegen(root->mid,ASM_head);
    }
    if(root->kind == POSTDEC)
    {
        int id = get_expr(root);
        id/=4;
        post_dec_check[id]++;
        return codegen(root->mid,ASM_head);
    }
	return 0;
}

void freeAST(AST* now) {
	if (now == NULL) return;
	freeAST(now->lhs);
	freeAST(now->mid);
	freeAST(now->rhs);
	free(now);
}

void token_print(Token* in, size_t len) {
	const static char KindName[][20] = {
		"Assign", "Add", "Sub", "Mul", "Div", "Rem", "Inc", "Dec", "Inc", "Dec", "Identifier", "Constant", "LPar", "RPar", "Plus", "Minus", "End"
	};
	const static char KindSymbol[][20] = {
		"'='", "'+'", "'-'", "'*'", "'/'", "'%'", "\"++\"", "\"--\"", "\"++\"", "\"--\"", "", "", "'('", "')'", "'+'", "'-'"
	};
	const static char format_str[] = "<Index = %3d>: %-10s, %-6s = %s\n";
	const static char format_int[] = "<Index = %3d>: %-10s, %-6s = %d\n";
	for (int i = 0; i < len; i++) {
		switch (in[i].kind) {
		case LPAR:
		case RPAR:
		case PREINC:
		case PREDEC:
		case ADD:
		case SUB:
		case MUL:
		case DIV:
		case REM:
		case ASSIGN:
		case PLUS:
		case MINUS:
			printf(format_str, i, KindName[in[i].kind], "symbol", KindSymbol[in[i].kind]);
			break;
		case CONSTANT:
			printf(format_int, i, KindName[in[i].kind], "value", in[i].val);
			break;
		case IDENTIFIER:
			printf(format_str, i, KindName[in[i].kind], "name", (char*)(&(in[i].val)));
			break;
		case END:
			printf("<Index = %3d>: %-10s\n", i, KindName[in[i].kind]);
			break;
		default:
			puts("=== unknown token ===");
		}
	}
}

void AST_print(AST* head) {
	static char indent_str[MAX_LENGTH] = "";
	static int indent = 0;
	char* indent_now = indent_str + indent;
	const static char KindName[][20] = {
		"Assign", "Add", "Sub", "Mul", "Div", "Rem", "PreInc", "PreDec", "PostInc", "PostDec", "Identifier", "Constant", "Parentheses", "Parentheses", "Plus", "Minus"
	};
	const static char format[] = "%s\n";
	const static char format_str[] = "%s, <%s = %s>\n";
	const static char format_val[] = "%s, <%s = %d>\n";
	if (head == NULL) return;
	indent_str[indent - 1] = '-';
	printf("%s", indent_str);
	indent_str[indent - 1] = ' ';
	if (indent_str[indent - 2] == '`')
		indent_str[indent - 2] = ' ';
	switch (head->kind) {
	case ASSIGN:
	case ADD:
	case SUB:
	case MUL:
	case DIV:
	case REM:
	case PREINC:
	case PREDEC:
	case POSTINC:
	case POSTDEC:
	case LPAR:
	case RPAR:
	case PLUS:
	case MINUS:
		printf(format, KindName[head->kind]);
		break;
	case IDENTIFIER:
		printf(format_str, KindName[head->kind], "name", (char*)&(head->val));
		break;
	case CONSTANT:
		printf(format_val, KindName[head->kind], "value", head->val);
		break;
	default:
		puts("=== unknown AST type ===");
	}
	indent += 2;
	strcpy(indent_now, "| ");
	AST_print(head->lhs);
	strcpy(indent_now, "` ");
	AST_print(head->mid);
	AST_print(head->rhs);
	indent -= 2;
	(*indent_now) = '\0';
}