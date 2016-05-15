#include "al_time.h"
#include "al_osctime.h"
#include "al_atom.h"

al_time al_time_add(al_region r, al_time t1, al_time t2)
{
	return (al_time)al_osctime_add(r, (al_osctime)t1, (al_osctime)t2);
}

al_time al_time_subtract(al_region r, al_time lhs, al_time rhs)
{
	return (al_time)al_osctime_subtract(r, (al_osctime)lhs, (al_osctime)rhs);
}

al_obj al_time_compare(al_region r, al_time t1, al_time t2)
{
	return cal_atom_int32(r, al_osctime_compare(r, (al_osctime)t1, (al_osctime)t2));
}

al_obj al_time_toFloat(al_region r, al_time t)
{
	return cal_atom_double(r, al_osctime_toFloat(r, (al_time)t));
}

al_time al_time_fromFloat(al_region r, al_obj f)
{
	al_c_double v = cal_atom_getDouble(r, f);
	if(!al_c_error(v)){
		return (al_time)al_osctime_fromFloat(r, al_c_value(v));
	}else{
		return AL_OSCTIME_NULL;
	}
}

al_obj al_time_toString(al_region r, al_time t)
{
	return cal_atom_string(r, al_osctime_toISO8601(r, (al_osctime)t));
}

al_time al_time_fromString(al_region r, al_obj s)
{
	if(cal_obj_getType(r, s) != AL_OBJ_TYPE_ATOM){
		return AL_OSCTIME_NULL;
	}
	al_c_ptr p = cal_atom_getPtr(r, s);
	if(!al_c_error(p)){
		return (al_time)al_osctime_fromISO8601(r, al_c_value(p));
	}else{
		return AL_OSCTIME_NULL;
	}
}
