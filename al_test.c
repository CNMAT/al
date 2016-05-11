#include "al_obj.h"
#include "al_atom.h"
#include "al_list.h"
#include "al_region.h"

int main(int ac, char **av)
{
	al_region r = al_region_allocWithBytes(5000, (char *)malloc(5000));
	al_obj i = cal_atom_int32(r, 10);
	al_obj s = cal_atom_string(r, "foo");
	al_obj S = cal_atom_symbol(r, "/bar");
	al_obj l = cal_list_alloc(r, 3, S, s, i);
	for(int i = 0; i < 3; i++){
		al_obj o = cal_list_nth(r, l, i);
		size_t len = cal_atom_nformat(r, NULL, 0, o);
		char buf[len + 1];
		cal_atom_nformat(r, buf, len + 1, o);
		printf("%s ", buf);
	}
	printf("\n");
	return 0;
}
