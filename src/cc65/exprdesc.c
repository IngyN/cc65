/*****************************************************************************/
/*                                                                           */
/*                                exprdesc.c                                 */
/*                                                                           */
/*                      Expression descriptor structure                      */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2002-2004 Ullrich von Bassewitz                                       */
/*               Römerstraße 52                                              */
/*               D-70794 Filderstadt                                         */
/* EMail:        uz@cc65.org                                                 */
/*                                                                           */
/*                                                                           */
/* This software is provided 'as-is', without any expressed or implied       */
/* warranty.  In no event will the authors be held liable for any damages    */
/* arising from the use of this software.                                    */
/*                                                                           */
/* Permission is granted to anyone to use this software for any purpose,     */
/* including commercial applications, and to alter it and redistribute it    */
/* freely, subject to the following restrictions:                            */
/*                                                                           */
/* 1. The origin of this software must not be misrepresented; you must not   */
/*    claim that you wrote the original software. If you use this software   */
/*    in a product, an acknowledgment in the product documentation would be  */
/*    appreciated but is not required.                                       */
/* 2. Altered source versions must be plainly marked as such, and must not   */
/*    be misrepresented as being the original software.                      */
/* 3. This notice may not be removed or altered from any source              */
/*    distribution.                                                          */
/*                                                                           */
/*****************************************************************************/



/* cc65 */
#include "datatype.h"
#include "symentry.h"
#include "exprdesc.h"



/*****************************************************************************/
/*				     Code                                    */
/*****************************************************************************/



ExprDesc* ED_Init (ExprDesc* Expr)
/* Initialize an ExprDesc */
{
    Expr->Sym   = 0;
    Expr->Type  = 0;
    Expr->Val   = 0;
    Expr->Flags = 0;
    Expr->Test  = 0;
    Expr->Name  = 0;
    return Expr;
}



ExprDesc* ED_MakeConstAbs (ExprDesc* Expr, long Value, type* Type)
/* Make Expr an absolute const with the given value and type. */
{
    Expr->Sym   = 0;
    Expr->Type  = Type;
    Expr->Val   = Value;
    Expr->Flags = E_LOC_ABS | E_RTYPE_RVAL;
    Expr->Test  = 0;
    Expr->Name  = 0;
    return Expr;
}



ExprDesc* ED_MakeConstAbsInt (ExprDesc* Expr, long Value)
/* Make Expr a constant integer expression with the given value */
{
    Expr->Sym   = 0;
    Expr->Type  = type_int;
    Expr->Val   = Value;
    Expr->Flags = E_LOC_ABS | E_RTYPE_RVAL;
    Expr->Test  = 0;
    Expr->Name  = 0;
    return Expr;
}



ExprDesc* ED_MakeRValExpr (ExprDesc* Expr)
/* Convert Expr into a rvalue which is in the primary register without an
 * offset.
 */
{
    Expr->Sym   = 0;
    Expr->Val   = 0;    /* No offset */
    Expr->Flags = (Expr->Flags & ~(E_MASK_LOC|E_MASK_RTYPE)) | (E_LOC_EXPR|E_RTYPE_RVAL);
    Expr->Test  = 0;
    Expr->Name  = 0;
    return Expr;
}



ExprDesc* ED_MakeLValExpr (ExprDesc* Expr)
/* Convert Expr into a lvalue which is in the primary register without an
 * offset.
 */
{
    Expr->Sym   = 0;
    Expr->Val   = 0;    /* No offset */
    Expr->Flags = (Expr->Flags & ~(E_MASK_LOC|E_MASK_RTYPE)) | (E_LOC_EXPR|E_RTYPE_LVAL);
    Expr->Test  = 0;
    Expr->Name  = 0;
    return Expr;
}



int ED_IsConst (const ExprDesc* Expr)
/* Return true if the expression denotes a constant of some sort. This can be a
 * numeric constant, the address of a global variable (maybe with offset) or
 * similar.
 */
{
    return ED_IsRVal (Expr) && (Expr->Flags & E_LOC_CONST) != 0;
}



int ED_IsConstAbs (const ExprDesc* Expr)
/* Return true if the expression denotes a constant absolute value. This can be
 * a numeric constant, cast to any type.
 */
{
    return ED_IsConst (Expr) && ED_IsLocAbs (Expr);
}



int ED_IsConstAbsInt (const ExprDesc* Expr)
/* Return true if the expression is a constant (numeric) integer. */
{
    return (Expr->Flags & (E_MASK_LOC|E_MASK_RTYPE)) == (E_LOC_ABS|E_RTYPE_RVAL) &&
           IsClassInt (Expr->Type);
}



int ED_IsNullPtr (const ExprDesc* Expr)
/* Return true if the given expression is a NULL pointer constant */
{
    return (Expr->Flags & (E_MASK_LOC|E_MASK_RTYPE)) == (E_LOC_ABS|E_RTYPE_RVAL) &&
           Expr->Val == 0                                                        &&
           IsClassInt (Expr->Type);
}



int ED_IsBool (const ExprDesc* Expr)
/* Return true of the expression can be treated as a boolean, that is, it can
 * be an operand to a compare operation.
 */
{
    /* Either ints, floats, or pointers can be used in a boolean context */
    return IsClassInt (Expr->Type)   ||
           IsClassFloat (Expr->Type) ||
           IsClassPtr (Expr->Type);
}



void PrintExprDesc (FILE* F, ExprDesc* E)
/* Print an ExprDesc */
{
    unsigned Flags;
    char     Sep;

    fprintf (F, "Symbol:   %s\n", E->Sym? E->Sym->Name : "(none)");
    if (E->Type) {
        fprintf (F, "Type:     ");
        PrintType (F, E->Type);
        fprintf (F, "\nRaw type: ");
        PrintRawType (F, E->Type);
    } else {
        fprintf (F, "Type:     (unknown)\n"
                    "Raw type: (unknown)\n");
    }
    fprintf (F, "Value:    0x%08lX\n", E->Val);

    Flags = E->Flags;
    Sep   = '(';
    fprintf (F, "Flags:    0x%04X ", Flags);
    if (Flags & E_LOC_ABS) {
        fprintf (F, "%cE_LOC_ABS", Sep);
        Flags &= ~E_LOC_ABS;
        Sep = ',';
    }
    if (Flags & E_LOC_GLOBAL) {
        fprintf (F, "%cE_LOC_GLOBAL", Sep);
        Flags &= ~E_LOC_GLOBAL;
        Sep = ',';
    }
    if (Flags & E_LOC_STATIC) {
        fprintf (F, "%cE_LOC_STATIC", Sep);
        Flags &= ~E_LOC_STATIC;
        Sep = ',';
    }
    if (Flags & E_LOC_REGISTER) {
        fprintf (F, "%cE_LOC_REGISTER", Sep);
        Flags &= ~E_LOC_REGISTER;
        Sep = ',';
    }
    if (Flags & E_LOC_STACK) {
        fprintf (F, "%cE_LOC_STACK", Sep);
        Flags &= ~E_LOC_STACK;
        Sep = ',';
    }
    if (Flags & E_LOC_PRIMARY) {
        fprintf (F, "%cE_LOC_PRIMARY", Sep);
        Flags &= ~E_LOC_PRIMARY;
        Sep = ',';
    }
    if (Flags & E_LOC_EXPR) {
        fprintf (F, "%cE_LOC_EXPR", Sep);
        Flags &= ~E_LOC_EXPR;
        Sep = ',';
    }
    if (Flags & E_LOC_LITERAL) {
        fprintf (F, "%cE_LOC_LITERAL", Sep);
        Flags &= ~E_LOC_LITERAL;
        Sep = ',';
    }
    if (Flags & E_RTYPE_LVAL) {
        fprintf (F, "%cE_RTYPE_LVAL", Sep);
        Flags &= ~E_RTYPE_LVAL;
        Sep = ',';
    }
    if (Flags) {
        fprintf (F, "%c,0x%04X", Sep, Flags);
        Sep = ',';
    }
    if (Sep != '(') {
        fputc (')', F);
    }
    fputc ('\n', F);

    fprintf (F, "\nTest:     ");
    if (E->Test & E_CC) {
        fprintf (F, "E_CC ");
    }
    if (E->Test & E_FORCETEST) {
        fprintf (F, "E_FORCETEST ");
    }

    fprintf (F, "\nName:     0x%08lX\n", E->Name);
}



type* ReplaceType (ExprDesc* Expr, const type* NewType)
/* Replace the type of Expr by a copy of Newtype and return the old type string */
{
    type* OldType = Expr->Type;
    Expr->Type = TypeDup (NewType);
    return OldType;
}



