/*
 * shared.h
 * jkcompiler
 * Keilan Jackson, Ramsey Kant
 *
 * Contains function definitions and structures of all parts of the compiler
 */

#ifndef SHARED_H
#define SHARED_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define K 1024
#define MAX_ERROR_SIZE 1 * K
#define MAX_NEW_CHAR_SIZE 255
#define MAX_USRDEF_HASHKEYS 8
#define MAX_SYMTAB_HASHKEYS 26
#define PROGRAMNAME "opc"

#define PRIMITIVE_TYPE_NAME_INTEGER "integer"
#define PRIMITIVE_TYPE_NAME_REAL "real"
#define PRIMITIVE_TYPE_NAME_BOOLEAN "boolean"
#define PRIMITIVE_TYPE_NAME_POINTER "pointer"
#define PRIMITIVE_TYPE_NAME_UNKNOWN "unknown"

#define BOOLEAN_VALUE_TRUE "true"
#define BOOLEAN_VALUE_FALSE "false"

#define OP_PLUS 1
#define OP_MINUS 2
#define OP_OR 3
#define OP_STAR 4
#define OP_SLASH 5
#define OP_MOD 6
#define OP_AND 7
#define OP_EQUAL 8
#define OP_NOTEQUAL 9
#define OP_LT 10
#define OP_GT 11
#define OP_LE 12
#define OP_GE 13
// Ops used for code generation
#define OP_ASSIGN 14
#define OP_BRANCH 15
#define OP_GOTO 16
#define OP_CALLER_ASSIGN 17 // Unused
#define OP_PARAM_ASSIGN 18
#define OP_FUNC_CALL 19
#define OP_FUNC_RETURN 20
#define OP_MEM_ACCESS 21
#define OP_NEW_OBJ 22
#define OP_PRINT 23

// Sizes for code generation
#define SIZE_BYTE 1
#define SIZE_HALFWORD 2
#define SIZE_WORD 4


/* Macro that checks for a malloc error */
#define CHECK_MEM_ERROR(name) {if (name == NULL) { \
				 printf("Memory allocation error\n"); \
				 exit(1); \
				} \
                              }

/* Macro that iterates to the end of a linked list whose next
 * element pointer is named "next"
 */
#define GOTO_END_OF_LIST(name) {while (name->next != NULL) \
                                  name = name->next; \
                               }

/*
 * Macro for grammar logging
 */
#if 0
	#define GLOG(x) printf x
#else
	#define GLOG(x) (void)0
#endif

/*
 * Macro for semantic logging
 */
#if 0
	#define SLOG(x) printf x
#else
	#define SLOG(x) (void)0
#endif

/*
 * Macro for IR/optimization/CFG/TAC logging
 */
#if 0
	#define IRLOG(x) printf x
#else
	#define IRLOG(x) (void)0
#endif

/*
 * Macro for Machine Code Generation
 */
#if 0
	#define MCLOG(x) printf x
#else
	#define MCLOG(x) (void)0
#endif


/* ================================================================
 * Global structures
 * ================================================================
 */

extern int error_flag;

struct args_t {
    /** Print the user defined data types and symbol table */
    int verbose;
    /** Exit after performing semantic analysis */
    int exit_after_sem;
    /** Print the Intermediate Representation */
    int printIr;
    /** Print the Lower-level Intermediate Representation */
    int printLir;
    /** Apply constant folding */
    int optimize_cf;
    /** Apply redundant expression elimination */
    int optimize_ree;
    /** Apply value numbering */
    int optimize_vn;
    /** Apply global redundancy elimination */
    int optimize_gre;
};


/* ================================================================
 * Datatype declarations
 * ================================================================
 */


struct identifier_list_t;
struct identifier_list_t {
    char *id;
    struct identifier_list_t *next;
};

struct unsigned_number_t;
struct range_t {
    struct unsigned_number_t *min;
    struct unsigned_number_t *max;
};

struct array_type_t {
    struct range_t *r;
    struct type_denoter_t *td;
    char *inner_type_name;
};

#define TYPE_DENOTER_T_ARRAY_TYPE 1
#define TYPE_DENOTER_T_CLASS_TYPE 2
#define TYPE_DENOTER_T_IDENTIFIER 3
struct type_denoter_t {
    int type; /* 1 - array_type
	     * 2 - class_type
	     * 3 - base_type
	     */
    char *name;
    union {
        struct array_type_t *at;
        struct class_list_t *cl;
        char *id;
    } data;
    int size; // Size of the type in bytes
};

/* ---------------------------------------------------------------- */



/* ----------------------------------------------------------------
 * Everything required for the variable_declaration_list
 * ----------------------------------------------------------------
 */
struct variable_declaration_t {
    struct identifier_list_t *il;
    struct type_denoter_t *tden;
    int line_number;
};

struct variable_declaration_list_t;
struct variable_declaration_list_t {
    struct variable_declaration_t *vd;
    struct variable_declaration_list_t *next;
};
/* ---------------------------------------------------------------- */


/* ----------------------------------------------------------------
 * Everything required for the func_declaration_list
 * ----------------------------------------------------------------
 */
struct formal_parameter_section_t {
    struct identifier_list_t *il;
    char *id; // Type
    int is_var;
};

struct formal_parameter_section_list_t {
    struct formal_parameter_section_t *fps;
    struct formal_parameter_section_list_t *next;
};

struct function_heading_t {
    char *id;
    char *res;
    struct formal_parameter_section_list_t *fpsl; /* == formal_parameter_list */
};

struct function_declaration_t {
    struct function_heading_t *fh;
    struct function_block_t *fb;
    int line_number;
};

struct func_declaration_list_t;
struct func_declaration_list_t {
    struct function_declaration_t *fd;
    struct func_declaration_list_t *next;
};
/* ---------------------------------------------------------------- */



/* ----------------------------------------------------------------
 * Everything required for the statement_sequence
 * ----------------------------------------------------------------
 */
struct expression_t;
struct actual_parameter_t {
    struct expression_t *e1;
    struct expression_t *e2;
    struct expression_t *e3;
};

struct actual_parameter_list_t;
struct actual_parameter_list_t {
    struct actual_parameter_t *ap;
    struct actual_parameter_list_t *next;
};

struct function_designator_t {
    char *id;
    struct actual_parameter_list_t *apl;
};

/* This is a temporary data structure used to hold the value and type of
   an expression. It is included (inherited) by a bunch of other data
   structures */
struct expression_data_t {
    float val;
    char *type;
	struct tac_data_t *tacData; // Data used for tac generation
	struct three_address_t *tac;// Data used for code generation
};

struct unsigned_number_t {
    int ui;
    struct expression_data_t *expr;
};

struct variable_access_t;
#define PRIMARY_T_VARIABLE_ACCESS 1
#define PRIMARY_T_UNSIGNED_CONSTANT 2
#define PRIMARY_T_FUNCTION_DESIGNATOR 3
#define PRIMARY_T_EXPRESSION 4
#define PRIMARY_T_PRIMARY 5
struct primary_t {
    int type; /* 1 - variable_access
	     * 2 - unsigned_constant
	     * 3 - function_designator
	     * 4 - expression
	     * 5 - primary
	     */
    union {
        struct variable_access_t *va;
        struct unsigned_number_t *un; /* == unsigned_constant */
        struct function_designator_t *fd;
        struct expression_t *e;
        struct primary_data_t {
            int not;
            struct primary_t *next;
        } p;
    } data;
    struct expression_data_t *expr;
};

#define FACTOR_T_SIGNFACTOR 1
#define FACTOR_T_PRIMARY 2
struct factor_t {
    int type; /* 1 - sign/factor
	     * 2 - primary
	     */
    union {
        struct factor_data_t {
            int *sign;
            struct factor_t *next;
        } f;
        struct primary_t *p;
    } data;
    struct expression_data_t *expr;
};

struct term_t;
struct term_t {
    struct factor_t *f;
    int mulop;
    struct expression_data_t *expr;
    struct term_t *next;
};

struct simple_expression_t;
struct simple_expression_t {
    struct term_t *t;
    int addop;
    struct expression_data_t *expr;
    struct simple_expression_t *next;
};

struct expression_t {
    struct simple_expression_t *se1;
    int relop;
    struct simple_expression_t *se2;
    struct expression_data_t *expr;
    struct block_t *block; // Temporary pointer for building CFG
};

struct index_expression_list_t;
struct index_expression_list_t {
    struct expression_t *e;
    struct index_expression_list_t *next;
    struct expression_data_t *expr;
};

struct indexed_variable_t {
    struct variable_access_t *va;
    struct index_expression_list_t *iel;
    struct expression_data_t *expr;
};

struct attribute_designator_t {
    struct variable_access_t *va;
    char *id;
};

struct method_designator_t {
    struct variable_access_t *va;
    struct function_designator_t *fd;
};

#define VARIABLE_ACCESS_T_IDENTIFIER 1
#define VARIABLE_ACCESS_T_INDEXED_VARIABLE 2
#define VARIABLE_ACCESS_T_ATTRIBUTE_DESIGNATOR 3
#define VARIABLE_ACCESS_T_METHOD_DESIGNATOR 4
struct variable_access_t {
    int type; /* 1 - identifier
	     * 2 - indexed_variable
	     * 3 - attribute_designator
	     * 4 - method_designator
	     */
    union {
        char *id;
        struct indexed_variable_t *iv;
        struct attribute_designator_t *ad;
        struct method_designator_t *md;
    } data;
    char *recordname;          /* This is a temporary field used to collect
				a verbose description of the data type
				that is validated */
    struct expression_data_t *expr;
    char *tacName;
};

struct object_instantiation_t {
    char *id;
    struct actual_parameter_list_t *apl;
};

struct assignment_statement_t {
    struct variable_access_t *va;
    struct expression_t *e;
    struct object_instantiation_t *oe;
    struct block_t *block; // Temporary pointer for building CFG
};

struct statement_t;
struct if_statement_t {
    struct expression_t *e;
    struct statement_t *s1;
    struct statement_t *s2;
    struct block_t *block; // Temporary pointer for building CFG
};

struct while_statement_t {
    struct expression_t *e;
    struct statement_t *s;
    struct block_t *block; // Temporary pointer for building CFG
};

struct print_statement_t {
    struct variable_access_t *va;
    struct block_t *block; // Temp pointer For building CFG
};

struct function_block_t {
    struct variable_declaration_list_t *vdl;
    struct statement_sequence_t *ss;
};

struct statement_sequence_t;
#define STATEMENT_T_ASSIGNMENT 1
#define STATEMENT_T_SEQUENCE 2
#define STATEMENT_T_IF 3
#define STATEMENT_T_WHILE 4
#define STATEMENT_T_PRINT 5
struct statement_t {
    int type; /* 1 - assignment_statement
	     * 2 - statement_sequence
	     * 3 - if_statement
	     * 4 - while_statement
	     * 5 - print_statement
	     */

    union {
        struct assignment_statement_t *as;
        struct statement_sequence_t *ss;
        struct if_statement_t *is;
        struct while_statement_t *ws;
        struct print_statement_t *ps;
    } data;
    int line_number;
    struct block_t *block; // Temporary pointer for building CFG
};

struct statement_sequence_t {
    struct statement_t *s;
    struct statement_sequence_t *next;
    struct block_t *block; // Temporary pointer for building CFG
};

/* ---------------------------------------------------------------- */



/* ----------------------------------------------------------------
 * Everything required for the program
 * ----------------------------------------------------------------
 */
struct program_heading_t {
    char *id;
    struct identifier_list_t *il;
};

struct class_identification_t {
    char *id;
    char *extend;
    int line_number;
};

struct class_block_t {
    struct variable_declaration_list_t *vdl; /* == variable_declaration_part */
    struct func_declaration_list_t *fdl;
};

struct class_list_t;
struct class_list_t {
    struct class_identification_t *ci;
    struct class_block_t *cb;
    struct class_list_t *next;
};

struct program_t {
    struct program_heading_t *ph;
    struct class_list_t *cl;
};



/* ----------------------------------------------------------------
 * Function declarations
 * ----------------------------------------------------------------
 */
char *op_str(int i_op);
int makekey(char* lexeme, int max_hashkeys);
void print_tabs(int numOfTabs);
char *stringtolower(char *str);
#endif

