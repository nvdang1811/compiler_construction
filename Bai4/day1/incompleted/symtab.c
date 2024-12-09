/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"

void freeObject(Object* obj);
void freeScope(Scope* scope);
void freeObjectList(ObjectNode *objList);
void freeReferenceList(ObjectNode *objList);

SymTab* symtab;
Type* intType;
Type* charType;

/******************* Type utilities ******************************/

Type* makeIntType(void) {
  Type* type = (Type*) malloc(sizeof(Type));
  type->typeClass = TP_INT;
  return type;
}

Type* makeCharType(void) {
  Type* type = (Type*) malloc(sizeof(Type));
  type->typeClass = TP_CHAR;
  return type;
}

Type* makeArrayType(int arraySize, Type* elementType) {
  Type* type = (Type*) malloc(sizeof(Type));
  type->typeClass = TP_ARRAY;
  type->arraySize = arraySize;
  type->elementType = elementType;
  return type;
}


/**
 * @brief Duplicates a given type.
 * 
 * @param type The type to duplicate.
 * @return Type* A pointer to the duplicated type.
 */
Type* duplicateType(Type* type) {
  // Allocate memory for the new type
  Type * t = (Type*) malloc(sizeof(Type));
  // Copy the type class
  t->typeClass = type->typeClass;
  // If the type is an array, copy the array size and recursively duplicate the element type
  if (type->typeClass == TP_ARRAY) {
    t->arraySize = type->arraySize;
    t->elementType = duplicateType(type->elementType);
  }
  return t;
}

/**
 * @brief Compares two types for equality.
 * 
 * @param type1 The first type to compare.
 * @param type2 The second type to compare.
 * @return int 1 if the types are equal, 0 otherwise.
 */
int compareType(Type* type1, Type* type2) {
  // Check if the type classes are the same
  if (type1->typeClass == type2->typeClass) {
    // If both types are arrays, compare their sizes and element types
    if (type1->typeClass == TP_ARRAY) {
      if (type1->arraySize == type2->arraySize) {
        return compareType(type1->elementType, type2->elementType);
      } else {
        return 0;
      }
    }
    return 1;
  }
  return 0;
}

/**
 * @brief Frees the memory allocated for a type.
 * 
 * @param type The type to free.
 */
void freeType(Type* type) {
  // Free the type based on its type class
  switch (type->typeClass) {
    case TP_INT:
    case TP_CHAR:
      free(type);
      break;
    case TP_ARRAY:
      freeType(type->elementType);
      free(type);
      break;
  }
}

/******************* Constant utility ******************************/

/**
 * @brief Creates an integer constant.
 * 
 * @param i The integer value.
 * @return ConstantValue* A pointer to the created constant.
 */
ConstantValue* makeIntConstant(int i) {
  // Allocate memory for the constant and set its value
  ConstantValue *c = (ConstantValue*)malloc(sizeof(ConstantValue));
  c->type = TP_INT;
  c->intValue = i;
  return c;
}

/**
 * @brief Creates a character constant.
 * 
 * @param ch The character value.
 * @return ConstantValue* A pointer to the created constant.
 */
ConstantValue* makeCharConstant(char ch) {
  // Allocate memory for the constant and set its value
  ConstantValue *c = (ConstantValue*)malloc(sizeof(ConstantValue));
  c->type = TP_CHAR;
  c->charValue = ch;
  return c;
}

/**
 * @brief Duplicates a constant value.
 * 
 * @param v The constant value to duplicate.
 * @return ConstantValue* A pointer to the duplicated constant value.
 */
ConstantValue* duplicateConstantValue(ConstantValue* v) {
  // Allocate memory for the new constant value and copy its type and value
  ConstantValue *newConstValue = (ConstantValue *)malloc(sizeof(ConstantValue));
  newConstValue->type = v->type;
  if (v->type == TP_INT) {
    newConstValue->intValue = v->intValue;
  } else if (v->type == TP_CHAR) {
    newConstValue->charValue = v->charValue;
  }
  return newConstValue;
}

/******************* Object utilities ******************************/

/**
 * @brief Creates a new scope.
 * 
 * @param owner The owner of the scope.
 * @param outer The outer scope.
 * @return Scope* A pointer to the created scope.
 */
Scope* createScope(Object* owner, Scope* outer) {
  // Allocate memory for the scope and initialize its fields
  Scope* scope = (Scope*) malloc(sizeof(Scope));
  scope->objList = NULL;
  scope->owner = owner;
  scope->outer = outer;
  return scope;
}

/**
 * @brief Creates a program object.
 * 
 * @param programName The name of the program.
 * @return Object* A pointer to the created program object.
 */
Object* createProgramObject(char *programName) {
  // Allocate memory for the program object and initialize its fields
  Object* program = (Object*) malloc(sizeof(Object));
  strcpy(program->name, programName);
  program->kind = OBJ_PROGRAM;
  program->progAttrs = (ProgramAttributes*) malloc(sizeof(ProgramAttributes));
  program->progAttrs->scope = createScope(program, NULL);
  symtab->program = program;

  return program;
}

/**
 * @brief Creates a constant object.
 * 
 * @param name The name of the constant.
 * @return Object* A pointer to the created constant object.
 */
Object* createConstantObject(char *name) {
  // Allocate memory for the constant object and initialize its fields
  Object* obj = (Object*) malloc(sizeof(Object));
  strcpy(obj->name, name);
  obj->kind = OBJ_TYPE;
  obj->typeAttrs = (TypeAttributes*) malloc(sizeof(TypeAttributes));
  return obj;
}

/**
 * @brief Creates a type object.
 * 
 * @param name The name of the type.
 * @return Object* A pointer to the created type object.
 */
Object* createTypeObject(char *name) {
  // Allocate memory for the type object and initialize its fields
  Object* obj = (Object*) malloc(sizeof(Object));
  strcpy(obj->name, name);
  obj->kind = OBJ_TYPE;
  obj->typeAttrs = (TypeAttributes*) malloc(sizeof(TypeAttributes));
  return obj;
}

/**
 * @brief Creates a variable object.
 * 
 * @param name The name of the variable.
 * @return Object* A pointer to the created variable object.
 */
Object* createVariableObject(char *name) {
  // Allocate memory for the variable object and initialize its fields
  Object* obj = (Object*) malloc(sizeof(Object));
  strcpy(obj->name, name);
  obj->kind = OBJ_VARIABLE;
  obj->varAttrs = (VariableAttributes*) malloc(sizeof(VariableAttributes));
  obj->varAttrs->scope = symtab->currentScope;
  return obj;
}

/**
 * @brief Creates a function object.
 * 
 * @param name The name of the function.
 * @return Object* A pointer to the created function object.
 */
Object* createFunctionObject(char *name) {
  // Allocate memory for the function object and initialize its fields
  Object* obj = (Object*) malloc(sizeof(Object));
  strcpy(obj->name, name);
  obj->kind = OBJ_FUNCTION;
  obj->funcAttrs = (FunctionAttributes*) malloc(sizeof(FunctionAttributes));
  obj->funcAttrs->paramList = NULL;
  obj->funcAttrs->scope = createScope(obj, symtab->currentScope);
  return obj;
}

/**
 * @brief Creates a procedure object.
 * 
 * @param name The name of the procedure.
 * @return Object* A pointer to the created procedure object.
 */
Object* createProcedureObject(char *name) {
  // Allocate memory for the procedure object and initialize its fields
  Object* obj = (Object*) malloc(sizeof(Object));
  strcpy(obj->name, name);
  obj->kind = OBJ_PROCEDURE;
  obj->procAttrs = (ProcedureAttributes*) malloc(sizeof(ProcedureAttributes));
  obj->procAttrs->paramList = NULL;
  obj->procAttrs->scope = createScope(obj, symtab->currentScope);
  return obj;
}

/**
 * @brief Creates a parameter object.
 * 
 * @param name The name of the parameter.
 * @param kind The kind of the parameter.
 * @param owner The owner of the parameter.
 * @return Object* A pointer to the created parameter object.
 */
Object* createParameterObject(char *name, enum ParamKind kind, Object* owner) {
  // Allocate memory for the parameter object and initialize its fields
  Object* obj = (Object*) malloc(sizeof(Object));
  strcpy(obj->name, name);
  obj->kind = OBJ_PARAMETER;
  obj->paramAttrs = (ParameterAttributes*) malloc(sizeof(ParameterAttributes));
  obj->paramAttrs->kind = kind;
  obj->paramAttrs->function = owner;
  return obj;
}

/**
 * @brief Frees the memory allocated for an object.
 * 
 * @param obj The object to free.
 */
void freeObject(Object* obj) {
  // Free the object based on its kind
  switch (obj->kind) {
    case OBJ_CONSTANT:
      free(obj->constAttrs->value);
      free(obj->constAttrs);
      break;
    case OBJ_TYPE:
      free(obj->typeAttrs->actualType);
      free(obj->typeAttrs);
      break;
    case OBJ_VARIABLE:
      free(obj->varAttrs->type);
      free(obj->varAttrs);
      break;
    case OBJ_FUNCTION:
      freeReferenceList(obj->funcAttrs->paramList);
      freeType(obj->funcAttrs->returnType);
      freeScope(obj->funcAttrs->scope);
      free(obj->funcAttrs);
      break;
    case OBJ_PROCEDURE:
      freeReferenceList(obj->procAttrs->paramList);
      freeScope(obj->procAttrs->scope);
      free(obj->procAttrs);
      break;
    case OBJ_PROGRAM:
      freeScope(obj->progAttrs->scope);
      free(obj->progAttrs);
      break;
    case OBJ_PARAMETER:
      freeType(obj->paramAttrs->type);
      free(obj->paramAttrs);
  }
  free(obj);
}

/**
 * @brief Frees the memory allocated for a scope.
 * 
 * @param scope The scope to free.
 */
void freeScope(Scope* scope) {
  // Free the object list and the scope itself
  freeObjectList(scope->objList);
  free(scope);
}

/**
 * @brief Frees the memory allocated for an object list.
 * 
 * @param objList The object list to free.
 */
void freeObjectList(ObjectNode *objList) {
  // Iterate through the list and free each object and node
  ObjectNode* list = objList;

  while (list != NULL) {
    ObjectNode* node = list;
    list = list->next;
    freeObject(node->object);
    free(node);
  }
}

/**
 * @brief Frees the memory allocated for a reference list.
 * 
 * @param objList The reference list to free.
 */
void freeReferenceList(ObjectNode *objList) {
  // Iterate through the list and free each node
  ObjectNode* list = objList;

  while (list != NULL) {
    ObjectNode* node = list;
    list = list->next;
    free(node);
  }
}

/**
 * @brief Adds an object to an object list.
 * 
 * @param objList The object list to add to.
 * @param obj The object to add.
 */
void addObject(ObjectNode **objList, Object* obj) {
  // Create a new node for the object
  ObjectNode* node = (ObjectNode*) malloc(sizeof(ObjectNode));
  node->object = obj;
  node->next = NULL;
  // If the list is empty, set the new node as the head
  if ((*objList) == NULL) 
    *objList = node;
  else {
    // Otherwise, iterate to the end of the list and add the new node
    ObjectNode *n = *objList;
    while (n->next != NULL) 
      n = n->next;
    n->next = node;
  }
}

/**
 * @brief Finds an object in an object list by name.
 * 
 * @param objList The object list to search.
 * @param name The name of the object to find.
 * @return Object* A pointer to the found object, or NULL if not found.
 */
Object* findObject(ObjectNode *objList, char *name) {
  // Iterate through the list and compare names
  while (objList != NULL) {
    if (strcmp(objList->object->name, name) == 0) 
      return objList->object;
    else objList = objList->next;
  }
  return NULL;
}

/******************* others ******************************/

void initSymTab(void) {
  Object* obj;
  Object* param;

  symtab = (SymTab*) malloc(sizeof(SymTab));
  symtab->globalObjectList = NULL;
  
  obj = createFunctionObject("READC");
  obj->funcAttrs->returnType = makeCharType();
  addObject(&(symtab->globalObjectList), obj);

  obj = createFunctionObject("READI");
  obj->funcAttrs->returnType = makeIntType();
  addObject(&(symtab->globalObjectList), obj);

  obj = createProcedureObject("WRITEI");
  param = createParameterObject("i", PARAM_VALUE, obj);
  param->paramAttrs->type = makeIntType();
  addObject(&(obj->procAttrs->paramList),param);
  addObject(&(symtab->globalObjectList), obj);

  obj = createProcedureObject("WRITEC");
  param = createParameterObject("ch", PARAM_VALUE, obj);
  param->paramAttrs->type = makeCharType();
  addObject(&(obj->procAttrs->paramList),param);
  addObject(&(symtab->globalObjectList), obj);

  obj = createProcedureObject("WRITELN");
  addObject(&(symtab->globalObjectList), obj);

  intType = makeIntType();
  charType = makeCharType();
}

void cleanSymTab(void) {
  freeObject(symtab->program);
  freeObjectList(symtab->globalObjectList);
  free(symtab);
  freeType(intType);
  freeType(charType);
}

void enterBlock(Scope* scope) {
  symtab->currentScope = scope;
}

void exitBlock(void) {
  symtab->currentScope = symtab->currentScope->outer;
}

void declareObject(Object* obj) {
  if (obj->kind == OBJ_PARAMETER) {
    Object* owner = symtab->currentScope->owner;
    switch (owner->kind) {
    case OBJ_FUNCTION:
      addObject(&(owner->funcAttrs->paramList), obj);
      break;
    case OBJ_PROCEDURE:
      addObject(&(owner->procAttrs->paramList), obj);
      break;
    default:
      break;
    }
  }
 
  addObject(&(symtab->currentScope->objList), obj);
}


