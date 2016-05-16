#include <stdio.h>
#include "al_obj.h"
#include "al_atom.h"
#include "al_list.h"
#include "al_alist.h"
#include "al_env.h"
#define __OSC_PROFILE__
#include "../libo/osc_profile.h"

int main(int ac, char **av)
{
	al_env env = al_env_create(1000000, (char *)malloc(1000000), NULL);
	OSC_PROFILE_TIMER_START(foo);
	{
		// nth on this
		al_obj app = cal_list_alloc(env, 2, cal_atom_symbol(env, "@"), cal_atom_symbol(env, "@"));
		al_obj lhs = cal_list_alloc(env, 1, cal_atom_symbol(env, "lhs"));
		al_obj rhs = cal_list_alloc(env, 2, cal_atom_symbol(env, "rhs"), cal_atom_int32(env, 0));
		al_obj al = cal_alist_alloc(env, 3, app, lhs, rhs);

		al_obj proglist = cal_list_alloc(env, 2, cal_atom_symbol(env, "prog"), al);
		al_obj prog = cal_alist_alloc(env, 1, proglist);
		al_obj e = al_obj_eval(env, prog, AL_OBJ_NULL);
		//cal_obj_println(env, e);
	}
	//printf("**************************************************\n");
	{
		// nth on string
		al_obj app = cal_list_alloc(env, 2, cal_atom_symbol(env, "@"), cal_atom_symbol(env, "@"));
		al_obj lhs = cal_list_alloc(env, 2, cal_atom_symbol(env, "lhs"), cal_atom_string(env, "fo0"));
		al_obj rhs = cal_list_alloc(env, 2, cal_atom_symbol(env, "rhs"), cal_atom_int32(env, 1));
		al_obj al = cal_alist_alloc(env, 3, app, lhs, rhs);

		al_obj proglist = cal_list_alloc(env, 2, cal_atom_symbol(env, "prog"), al);
		al_obj prog = cal_alist_alloc(env, 1, proglist);
		al_obj e = al_obj_eval(env, prog, AL_OBJ_NULL);
		//cal_obj_println(env, e);
	}
	//printf("**************************************************\n");
	{
		// lookup in context
		al_obj app = cal_list_alloc(env, 2, cal_atom_symbol(env, "@"), cal_atom_symbol(env, "@"));
		al_obj lhs = cal_list_alloc(env, 1, cal_atom_symbol(env, "lhs"));
		al_obj rhs = cal_list_alloc(env, 2, cal_atom_symbol(env, "rhs"), cal_atom_symbol(env, "this"));
		al_obj al = cal_alist_alloc(env, 3, app, lhs, rhs);

		al_obj proglist = cal_list_alloc(env, 2, cal_atom_symbol(env, "prog"), al);
		al_obj prog = cal_alist_alloc(env, 1, proglist);
		al_obj e = al_obj_eval(env, prog, AL_OBJ_NULL);
		//cal_obj_println(env, e);
	}
	//printf("**************************************************\n");
	{
		// lookup in lhs
		al_obj fn = cal_alist_alloc(env, 2, cal_list_alloc(env, 2, cal_atom_symbol(env, "hi"), cal_atom_float(env, 33.6)), cal_list_alloc(env, 2, cal_atom_symbol(env, "there"), cal_atom_double(env, 66.3)));
		al_obj app = cal_list_alloc(env, 2, cal_atom_symbol(env, "@"), cal_atom_symbol(env, "@"));
		al_obj lhs = cal_list_alloc(env, 2, cal_atom_symbol(env, "lhs"), fn);
		al_obj rhs = cal_list_alloc(env, 2, cal_atom_symbol(env, "rhs"), cal_atom_symbol(env, "hi"));
		al_obj al = cal_alist_alloc(env, 3, app, lhs, rhs);

		al_obj proglist = cal_list_alloc(env, 2, cal_atom_symbol(env, "prog"), al);
		al_obj prog = cal_alist_alloc(env, 1, proglist);
		al_obj e = al_obj_eval(env, prog, AL_OBJ_NULL);
		//cal_obj_println(env, e);
	}
	//printf("**************************************************\n");
	{
		// union with this
		//al_obj al1 = cal_alist_alloc(env, 2, cal_list_alloc(env, 2, cal_atom_symbol(env, "hi"), cal_atom_float(env, 33.6)), cal_list_alloc(env, 2, cal_atom_symbol(env, "there"), cal_atom_double(env, 66.3)));
		al_obj al2 = cal_alist_alloc(env, 2, cal_list_alloc(env, 2, cal_atom_symbol(env, "sucka"), cal_atom_true(env)), cal_list_alloc(env, 2, cal_atom_symbol(env, "hi"), cal_atom_double(env, 66.3)));
		al_obj app = cal_list_alloc(env, 2, cal_atom_symbol(env, "@"), cal_atom_symbol(env, "@"));
		al_obj lhs = cal_list_alloc(env, 1, cal_atom_symbol(env, "lhs"));
		al_obj rhs = cal_list_alloc(env, 2, cal_atom_symbol(env, "rhs"), al2);
		al_obj al = cal_alist_alloc(env, 3, app, lhs, rhs);

		al_obj proglist = cal_list_alloc(env, 2, cal_atom_symbol(env, "prog"), al);
		al_obj prog = cal_alist_alloc(env, 1, proglist);
		al_obj e = al_obj_eval(env, prog, AL_OBJ_NULL);
		//cal_obj_println(env, e);
	}
	//printf("**************************************************\n");
	{
		al_obj al1 = cal_alist_alloc(env, 2, cal_list_alloc(env, 2, cal_atom_symbol(env, "hi"), cal_atom_float(env, 33.6)), cal_list_alloc(env, 2, cal_atom_symbol(env, "there"), cal_atom_double(env, 66.3)));
		al_obj al2 = cal_alist_alloc(env, 2, cal_list_alloc(env, 2, cal_atom_symbol(env, "sucka"), cal_atom_true(env)), cal_list_alloc(env, 2, cal_atom_symbol(env, "hi"), cal_atom_double(env, 66.3)));
		al_obj app = cal_list_alloc(env, 2, cal_atom_symbol(env, "@"), cal_atom_symbol(env, "@"));
		al_obj lhs = cal_list_alloc(env, 2, cal_atom_symbol(env, "lhs"), al1);
		al_obj rhs = cal_list_alloc(env, 2, cal_atom_symbol(env, "rhs"), al2);
		al_obj al = cal_alist_alloc(env, 3, app, lhs, rhs);

		al_obj proglist = cal_list_alloc(env, 2, cal_atom_symbol(env, "prog"), al);
		al_obj prog = cal_alist_alloc(env, 1, proglist);
		al_obj e = al_obj_eval(env, prog, AL_OBJ_NULL);
		//cal_obj_println(env, e);
	}
	//printf("**************************************************\n");
	{
		al_obj app = cal_list_alloc(env, 2, cal_atom_symbol(env, "@"), cal_atom_symbol(env, "@"));
		al_obj lhs = cal_list_alloc(env, 4, cal_atom_symbol(env, "lhs"), cal_atom_int32(env, 1), cal_atom_int32(env, 2), cal_atom_int32(env, 3));
		al_obj rhs = cal_list_alloc(env, 2, cal_atom_symbol(env, "rhs"), cal_atom_int32(env, 1));
		al_obj al = cal_alist_alloc(env, 3, app, lhs, rhs);

		al_obj proglist = cal_list_alloc(env, 2, cal_atom_symbol(env, "prog"), al);
		al_obj prog = cal_alist_alloc(env, 1, proglist);
		al_obj e = al_obj_eval(env, prog, AL_OBJ_NULL);
		//cal_obj_println(env, e);
	}
	OSC_PROFILE_TIMER_STOP(foo);
	OSC_PROFILE_TIMER_PRINTF(foo);
	////printf("used %lu bytes, %lu bytes free\n", al_region_bytesUsed(env), al_region_bytesFree(env));
	return 0;
}
