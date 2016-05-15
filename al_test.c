#include <stdio.h>
#include "al_obj.h"
#include "al_atom.h"
#include "al_list.h"
#include "al_alist.h"
#include "al_region.h"
#include "al_env.h"

int main(int ac, char **av)
{
	al_region r = al_region_allocWithBytes(1000000, (char *)malloc(1000000));
	char *p = al_region_getPtr(r);
	{
		// nth on this
		al_obj app = cal_list_alloc(r, 2, cal_atom_symbol(r, "@"), cal_atom_symbol(r, "@"));
		al_obj lhs = cal_list_alloc(r, 1, cal_atom_symbol(r, "lhs"));
		al_obj rhs = cal_list_alloc(r, 2, cal_atom_symbol(r, "rhs"), cal_atom_int32(r, 0));
		al_obj al = cal_alist_alloc(r, 3, app, lhs, rhs);

		al_obj proglist = cal_list_alloc(r, 2, cal_atom_symbol(r, "prog"), al);
		al_obj prog = cal_alist_alloc(r, 1, proglist);
		al_obj e = al_obj_eval(r, prog, AL_OBJ_NULL);
		cal_obj_println(r, e);
		al_region_setPtr(r, p);
	}
	printf("**************************************************\n");
	{
		// nth on string
		al_obj app = cal_list_alloc(r, 2, cal_atom_symbol(r, "@"), cal_atom_symbol(r, "@"));
		al_obj lhs = cal_list_alloc(r, 2, cal_atom_symbol(r, "lhs"), cal_atom_string(r, "fo0"));
		al_obj rhs = cal_list_alloc(r, 2, cal_atom_symbol(r, "rhs"), cal_atom_int32(r, 1));
		al_obj al = cal_alist_alloc(r, 3, app, lhs, rhs);

		al_obj proglist = cal_list_alloc(r, 2, cal_atom_symbol(r, "prog"), al);
		al_obj prog = cal_alist_alloc(r, 1, proglist);
		al_obj e = al_obj_eval(r, prog, AL_OBJ_NULL);
		cal_obj_println(r, e);
		al_region_setPtr(r, p);
	}
	printf("**************************************************\n");
	{
		// lookup in context
		al_obj app = cal_list_alloc(r, 2, cal_atom_symbol(r, "@"), cal_atom_symbol(r, "@"));
		al_obj lhs = cal_list_alloc(r, 1, cal_atom_symbol(r, "lhs"));
		al_obj rhs = cal_list_alloc(r, 2, cal_atom_symbol(r, "rhs"), cal_atom_symbol(r, "this"));
		al_obj al = cal_alist_alloc(r, 3, app, lhs, rhs);

		al_obj proglist = cal_list_alloc(r, 2, cal_atom_symbol(r, "prog"), al);
		al_obj prog = cal_alist_alloc(r, 1, proglist);
		al_obj e = al_obj_eval(r, prog, AL_OBJ_NULL);
		cal_obj_println(r, e);
		al_region_setPtr(r, p);
	}
	printf("**************************************************\n");
	{
		// lookup in lhs
		al_obj fn = cal_alist_alloc(r, 2, cal_list_alloc(r, 2, cal_atom_symbol(r, "hi"), cal_atom_float(r, 33.6)), cal_list_alloc(r, 2, cal_atom_symbol(r, "there"), cal_atom_double(r, 66.3)));
		al_obj app = cal_list_alloc(r, 2, cal_atom_symbol(r, "@"), cal_atom_symbol(r, "@"));
		al_obj lhs = cal_list_alloc(r, 2, cal_atom_symbol(r, "lhs"), fn);
		al_obj rhs = cal_list_alloc(r, 2, cal_atom_symbol(r, "rhs"), cal_atom_symbol(r, "hi"));
		al_obj al = cal_alist_alloc(r, 3, app, lhs, rhs);

		al_obj proglist = cal_list_alloc(r, 2, cal_atom_symbol(r, "prog"), al);
		al_obj prog = cal_alist_alloc(r, 1, proglist);
		al_obj e = al_obj_eval(r, prog, AL_OBJ_NULL);
		cal_obj_println(r, e);
		al_region_setPtr(r, p);
	}
	printf("**************************************************\n");
	{
		// union with this
		//al_obj al1 = cal_alist_alloc(r, 2, cal_list_alloc(r, 2, cal_atom_symbol(r, "hi"), cal_atom_float(r, 33.6)), cal_list_alloc(r, 2, cal_atom_symbol(r, "there"), cal_atom_double(r, 66.3)));
		al_obj al2 = cal_alist_alloc(r, 2, cal_list_alloc(r, 2, cal_atom_symbol(r, "sucka"), cal_atom_true(r)), cal_list_alloc(r, 2, cal_atom_symbol(r, "hi"), cal_atom_double(r, 66.3)));
		al_obj app = cal_list_alloc(r, 2, cal_atom_symbol(r, "@"), cal_atom_symbol(r, "@"));
		al_obj lhs = cal_list_alloc(r, 1, cal_atom_symbol(r, "lhs"));
		al_obj rhs = cal_list_alloc(r, 2, cal_atom_symbol(r, "rhs"), al2);
		al_obj al = cal_alist_alloc(r, 3, app, lhs, rhs);

		al_obj proglist = cal_list_alloc(r, 2, cal_atom_symbol(r, "prog"), al);
		al_obj prog = cal_alist_alloc(r, 1, proglist);
		al_obj e = al_obj_eval(r, prog, AL_OBJ_NULL);
		cal_obj_println(r, e);
		al_region_setPtr(r, p);
	}
	printf("**************************************************\n");
	{
		al_obj al1 = cal_alist_alloc(r, 2, cal_list_alloc(r, 2, cal_atom_symbol(r, "hi"), cal_atom_float(r, 33.6)), cal_list_alloc(r, 2, cal_atom_symbol(r, "there"), cal_atom_double(r, 66.3)));
		al_obj al2 = cal_alist_alloc(r, 2, cal_list_alloc(r, 2, cal_atom_symbol(r, "sucka"), cal_atom_true(r)), cal_list_alloc(r, 2, cal_atom_symbol(r, "hi"), cal_atom_double(r, 66.3)));
		al_obj app = cal_list_alloc(r, 2, cal_atom_symbol(r, "@"), cal_atom_symbol(r, "@"));
		al_obj lhs = cal_list_alloc(r, 2, cal_atom_symbol(r, "lhs"), al1);
		al_obj rhs = cal_list_alloc(r, 2, cal_atom_symbol(r, "rhs"), al2);
		al_obj al = cal_alist_alloc(r, 3, app, lhs, rhs);

		al_obj proglist = cal_list_alloc(r, 2, cal_atom_symbol(r, "prog"), al);
		al_obj prog = cal_alist_alloc(r, 1, proglist);
		al_obj e = al_obj_eval(r, prog, AL_OBJ_NULL);
		cal_obj_println(r, e);
		al_region_setPtr(r, p);
	}
	printf("**************************************************\n");
	{
		al_obj app = cal_list_alloc(r, 2, cal_atom_symbol(r, "@"), cal_atom_symbol(r, "@"));
		al_obj lhs = cal_list_alloc(r, 4, cal_atom_symbol(r, "lhs"), cal_atom_int32(r, 1), cal_atom_int32(r, 2), cal_atom_int32(r, 3));
		al_obj rhs = cal_list_alloc(r, 2, cal_atom_symbol(r, "rhs"), cal_atom_int32(r, 1));
		al_obj al = cal_alist_alloc(r, 3, app, lhs, rhs);

		al_obj proglist = cal_list_alloc(r, 2, cal_atom_symbol(r, "prog"), al);
		al_obj prog = cal_alist_alloc(r, 1, proglist);
		al_obj e = al_obj_eval(r, prog, AL_OBJ_NULL);
		cal_obj_println(r, e);
		al_region_setPtr(r, p);
	}

	//printf("used %lu bytes, %lu bytes free\n", al_region_bytesUsed(r), al_region_bytesFree(r));
	return 0;
}
