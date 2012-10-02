/*
 * semantic.h
 *
 */

#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "symtab.h"

void semantic_analysis(struct program_t *p);

bool compatible_types(struct type_denoter_t *t1, struct type_denoter_t *t2);
bool compatible_arrays(struct array_type_t *a1, struct array_type_t *a2);
bool compatible_classes(struct class_list_t *c1, struct class_list_t *c2);
bool compatible_class_assignment(struct class_list_t *lhs, struct class_list_t *rhs);

<<<<<<< HEAD
void verify_statements_in_sequence(struct statement_sequence_t *ss);
void verify_identifiers_in_variable_access(struct variable_access_t *va);

void check_variable_list_types_defined(struct variable_declaration_list_t *vdl);
=======
void process_variable_declaration_list(struct variable_declaration_list_t *vdl);
>>>>>>> Invaild variable names (true/false)
void check_variable_declared_in_parent(struct class_list_t *cl);

int identifier_list_size(struct identifier_list_t *il);
bool check_variable_valid_name(char *id);

#endif /* SEMANTIC_H */
