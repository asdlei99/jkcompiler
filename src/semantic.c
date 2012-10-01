/*
 * semantic.c
 *
 * Implements all functions that participate in semantic analysis.
 */


#include "shared.h"
#include "semantic.h"
#include "rulefuncs.h"
#include "usrdef.h"
#include "symtab.h"

/* -----------------------------------------------------------------------
 * Carries out semantic analysis on a program
 * -----------------------------------------------------------------------
 */
void semantic_analysis(struct program_t *p) {
	bool foundProgramClass = false; // Determined if the main Program class is present in the class list
    struct class_list_t *temp_cl;
    temp_cl = p->cl;
    
    // Initial fix up all base & subclass class structures
    while (temp_cl != NULL) {
		// Found the main program class
		if(strcmp(temp_cl->ci->id, p->ph->id) == 0)
			foundProgramClass = true;

    	// If temp_cl->extend != null, check if extend is in p->cl
		// Check if the class has a base class
		struct scope_t *childScope = symtab_lookup_class(temp_cl->ci->id);
		struct class_list_t *childClass = (struct class_list_t*)childScope->ptr;
		if(childClass->ci->extend != NULL) {
			struct scope_t *baseScope = symtab_lookup_class(childClass->ci->extend);
			// Base Class doesn't exist
			if(baseScope == NULL) {
				error_extending_missing_class(temp_cl->ci->line_number, temp_cl->ci->id, childClass->ci->extend);
			} else {
				// Add subclass into the scope of the base class & Set the parent pointer of the subclass to base class
				symtab_set_current_scope(baseScope);
				symtab_enter_scope();
				symtab_insert_scope(childScope);
				symtab_exit_scope(baseScope->ptr);
			}
		}
		temp_cl = temp_cl->next;
    }

	// Main program class wasn't found in the initial class fixup, error
	if(!foundProgramClass)
		error_missing_program_class();
    
    // Now that the class list is fixed, check deeper into the program
    temp_cl = p->cl;
    while (temp_cl != NULL) {

        // Fix up type_denoters and set the appropriate type (determine if class or identifier)
    	fix_type_denoters();

    	// Process the variable declaration list
    	//check_variable_list_types_defined(temp_cl->cb->vdl);
		
        // Process the func_declaration_list


        // Advance to the next class
        temp_cl = temp_cl->next;
    }
}

/*
 * Checks for compatible types.
 */
bool compatible_types(struct type_denoter_t *t1, struct type_denoter_t *t2){
	// Handle primitive types
	if(t1->type == TYPE_DENOTER_T_IDENTIFIER && t2->type == TYPE_DENOTER_T_IDENTIFIER) {
		if(strcmp(t1->data.id,t2->data.id) == 0) {
			return true;
		}
	}
	// Array types
	if(t1->type == TYPE_DENOTER_T_ARRAY_TYPE && t2->type == TYPE_DENOTER_T_ARRAY_TYPE) {
		return compatible_arrays(t1->data.at, t2->data.at);
	}
	// Class types
	if(t1->type == TYPE_DENOTER_T_CLASS_TYPE && t2->type == TYPE_DENOTER_T_CLASS_TYPE) {
		return compatible_classes(t1->data.cl, t2->data.cl);
	}
	return false;
}

/*
 *  Check array compatibility.  Arrays are compatible if they have the same number of elements
 *  and the elements are of compatible types.
 */
bool compatible_arrays(struct array_type_t *a1, struct array_type_t *a2) {
	int size1 = a1->r->max - a1->r->min;
	int size2 = a2->r->max - a2->r->min;
	return (size1 == size2) && compatible_types(a1->td, a2->td);
}

/*
 * Check class compatibility.  Classes are compatible if all of their fields, in order,
 * are compatible.
 */

bool compatible_classes(struct class_list_t *c1, struct class_list_t *c2) {
	// Pointer to first variable declaration in c1 and c2
	struct variable_declaration_list_t *temp_vdl1 = c1->cb->vdl;
	struct variable_declaration_list_t *temp_vdl2 = c2->cb->vdl;

	// Iterate through c1's variable declarations
	while(temp_vdl1 != NULL) {
		// Make sure the lists are the same size
		int same_size = (identifier_list_size(temp_vdl1->vd->il) == identifier_list_size(temp_vdl2->vd->il));
		// Make sure the types are compatible
		int types_compat = compatible_types(temp_vdl1->vd->tden, temp_vdl2->vd->tden);
		if(!same_size || !types_compat)
			return false;
		temp_vdl1 = temp_vdl1->next;
		temp_vdl2 = temp_vdl2->next;
		// If one list is bigger than the other, not compatible
		if((temp_vdl1 == NULL && temp_vdl2 != NULL) || (temp_vdl1 != NULL && temp_vdl2 == NULL))
			return false;
	}
	return true;
}

/*
 * Checks if an assignment of one class to another is compatible.  An assignment is
 * compatible if the rhs is a descendant of the lhs (lhs is an ancestor)
 */
bool compatible_class_assignment(struct class_list_t *lhs, struct class_list_t *rhs) {
	// Look up the classes in the symbol table
	struct scope_t *left = symtab_lookup_class(lhs->ci->id);
	struct scope_t *right = symtab_lookup_class(rhs->ci->id);
	// If the classes are both in the symbol table and are the same they are compatible
	if(right == left) {
		return true;
	}
	// If the right scope is an ancestor of the left, they are compatible
	while(right != NULL) {
		if(right == left)
			return true;
		right = right->outer;
	}
	// Not compatible
	return false;
}

/*
 * Checks that all types are defined for a variable_declaration_list_t.  Prints
 * appropriate error messages
 */
void check_variable_list_types_defined(struct variable_declaration_list_t *vdl) {
	// Process the variable_declaration_list
	struct variable_declaration_list_t *temp_vdl = vdl;
	// for each variable_declaration_list
	while(temp_vdl != NULL) {
		// Check if the type is defined
		struct type_denoter_t *temp_vtden = temp_vdl->vd->tden;
		// Look up the name for class
		if(temp_vtden->type == TYPE_DENOTER_T_CLASS_TYPE && usrdef_lookup_td(temp_vtden) == NULL)
			error_type_not_defined(temp_vdl->vd->line_number,temp_vtden->name);
		// Look up the array type's type denoter for arrays
		if(temp_vtden->type == TYPE_DENOTER_T_ARRAY_TYPE && usrdef_lookup_td(temp_vtden->data.at->td) == NULL)
			error_type_not_defined(temp_vdl->vd->line_number, temp_vtden->data.at->td->name);
		// Look up for other types
		if(temp_vtden->type == TYPE_DENOTER_T_IDENTIFIER && usrdef_lookup_name(temp_vtden) == NULL)
			error_type_not_defined(temp_vdl->vd->line_number,temp_vtden->name);
		temp_vdl = temp_vdl->next;
	}
}

/*
 * Fix up type_denoters and set the appropriate type (determine if class or identifier)
 */
void fix_type_denoters() {
	struct type_denoter_list_t *temp_tdl = usrdef_types;
	while(temp_tdl != NULL) {
		// See if it is an identifier that may need to be fixed to a class
		if(temp_tdl->tden->type == TYPE_DENOTER_T_IDENTIFIER) {
			struct scope_t *class_scope = symtab_lookup_class(temp_tdl->tden->name);
			// Class found update the type and data
			if(class_scope != NULL) {
				temp_tdl->tden->type = TYPE_DENOTER_T_CLASS_TYPE;
				temp_tdl->tden->data.cl = (struct class_list_t *) class_scope->ptr;
			}
		}
		temp_tdl = temp_tdl->next;
	}
}

/*
 * Returns the number of elements in an identifier list
 */
int identifier_list_size(struct identifier_list_t *il) {
	int count = 0;
	struct identifier_list_t *temp_il = il;
	while(temp_il != NULL) {
		count++;
		temp_il = temp_il->next;
	}
	return count;
}
