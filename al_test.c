#include <stdio.h>
#include "al.h"
#include "al_obj.h"
#include "al_atom.h"
#include "al_list.h"
#include "al_alist.h"
#include "al_env.h"
//#define __OSC_PROFILE__
#include "../libo/osc_profile.h"

#include "oscal_parse.h"

al_obj shithole(al_env env, al_obj o, al_obj context)
{
	printf("hi from %s\n", __func__);
	cal_obj_println(env, o);
	return cal_alist_alloc(env, 1, cal_list_alloc(env, 3, cal_atom_symbol(env, "value"), cal_atom_symbol(env, "shithole"), cal_atom_int32(env, 666)));
}

void test(al_env env, char *_prog, al_obj stdlib)
{
	al_obj prog = oscal_parse(env, _prog);
	printf("prog:\n\n%s\n\nparsed:\n\n", _prog);
	cal_obj_println(env, prog);
	al_obj e = al_alist_eval(env, prog, stdlib);
	printf("\neval:\n\n");
	cal_obj_println(env, e);
	printf("\n");
	printf("**************************************************\n");
}

int main(int ac, char **av)
{
	al_env env = al_env_create(8000000, (char *)malloc(8000000), NULL);
	al_obj stdlib = al_stdlib(env);

	test(env, "{prog : @eql}", stdlib);
	test(env, "{prog : @eql!@1}", stdlib);
	test(env, "{prog : @eqa}", stdlib);
	test(env, "{prog : @0}", stdlib);
	test(env, "{prog : {a : 10, b : 3}@0}", stdlib);
	test(env, "{prog : {a : 10, b : 3}@0!@1}", stdlib);
	test(env, "{prog : @eql!@1!@argnames}", stdlib);
	test(env, "{prog : \"cnmat\"@3}", stdlib);
	test(env, "{prog : \"cnmat\"@10}", stdlib); // doesn't work
	test(env, "{prog : \"cnmat\"@[0, 2, 4]}", stdlib); // starts with second element of the list
	test(env, "{prog : [5, 4, 3, 2, 1]@2}", stdlib);
	test(env, "{prog : [5, 4, 3, 2, 1]@[4, 3, 2, 1, 0]}", stdlib);
	test(env, "{prog : @length!@1 !@ {list : [1, 2, 3, 4, 5]}}", stdlib);
	test(env, "{prog : @length!@1 !@ {list : \"foo\"}}", stdlib); // doesn't work
	test(env, "{prog : @eql!@1 !@ {obj1 : 1, obj2 : 1}}", stdlib);

	test(env, "{prog : @nth!@1 !@ {list : [1, 2, 3]}}", stdlib); 
	test(env, "{gorp : @nth!@1 !@ {list : [1, 2, 3]}, foo : (@eval!@1 !@! {prog : @gorp!@1}) !@ {n : 2}}", stdlib);

	test(env, "{prog : @nth!@1 !@ {list : [1, 2, 3], n : 2}}", stdlib);
	test(env, "{prog : @nth!@1 !@ {list : \"foO\", n : 2}}", stdlib);
	test(env, "{prog : @union!@1 !@ {rhs : {foo : [1, 2, 3], bar : [a, b, c]}, lhs : {fool : 10, bar : \"xxx\"}}}", stdlib);
	test(env, "{prog : @intersection!@1 !@ {rhs : {foo : [1, 2, 3], bar : [a, b, c]}, lhs : {fool : 10, bar : \"xxx\"}}}", stdlib);
	test(env, "{prog : @rcomplement!@1 !@ {rhs : {foo : [1, 2, 3], bar : [a, b, c]}, lhs : {fool : 10, bar : \"xxx\"}}}", stdlib);

	test(env, "{a : 3, b : 4, prog : @if!@1 !@ {test : @eql!@1 !@! {obj1 : @a!@1, obj2 : @b!@1}, then : true, else : false}}", stdlib);
	test(env, "{a : 3, b : 3, prog : @if!@1 !@ {test : @eql!@1 !@! {obj1 : @a!@1, obj2 : @b!@1}, then : true, else : false}}", stdlib);

	printf("used %d bytes, %d bytes free\n", al_region_bytesUsed(al_env_getRegion(env)), al_region_bytesFree(al_env_getRegion(env)));
	return 0;
}
