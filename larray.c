/*
** Lua arrays
** See Copyright Notice in lua.h
*/

#define larray_c
#define LUA_CORE

#include "lprefix.h"


#include <math.h>
#include <limits.h>

#include "lua.h"

#include "ldebug.h"
#include "ldo.h"
#include "lgc.h"
#include "lmem.h"
#include "lobject.h"
#include "lstate.h"
#include "lstring.h"
#include "ltable.h"
#include "larray.h"
#include "lvm.h"

void luaA_resize (lua_State *L, Array *a, unsigned int newsize) {
  luaG_runerror(L, "luaA_resize not implemented!");
#if 0
  unsigned int i;
  Table newt;  /* to keep the new hash part */
  unsigned int oldasize = t->sizearray;
  TValue *newarray;
  /* create new hash part with appropriate size into 'newt' */
  setnodevector(L, &newt, nhsize);
  if (newasize < oldasize) {  /* will array shrink? */
    t->sizearray = newasize;  /* pretend array has new size... */
    exchangehashpart(t, &newt);  /* and new hash */
    /* re-insert into the new hash the elements from vanishing slice */
    for (i = newasize; i < oldasize; i++) {
      if (!isempty(&t->array[i]))
        luaH_setint(L, t, i + 1, &t->array[i]);
    }
    t->sizearray = oldasize;  /* restore current size... */
    exchangehashpart(t, &newt);  /* and hash (in case of errors) */
  }
  /* allocate new array */
  newarray = luaM_reallocvector(L, t->array, oldasize, newasize, TValue);
  if (newarray == NULL && newasize > 0) {  /* allocation failed? */
    freehash(L, &newt);  /* release new hash part */
    luaM_error(L);  /* raise error (with array unchanged) */
  }
  /* allocation ok; initialize new part of the array */
  exchangehashpart(t, &newt);  /* 't' has the new hash ('newt' has the old) */
  t->array = newarray;  /* set new array part */
  t->sizearray = newasize;
  for (i = oldasize; i < newasize; i++)  /* clear new slice of the array */
     setempty(&t->array[i]);
  /* re-insert elements from old hash part into new parts */
  reinsert(L, &newt, t);  /* 'newt' now has the old hash */
  freehash(L, &newt);  /* free old hash part */
#endif
}


/*
** }=============================================================
*/


Array *luaA_new (lua_State *L) {
  GCObject *o = luaC_newobj(L, LUA_TARRAY, sizeof(Array));
  Array *a = gco2a(o);
  a->array = NULL;
  a->sizearray = 0;
  return a;
}


void luaA_free (lua_State *L, Array *a) {
  luaM_freearray(L, a->array, a->sizearray);
  luaM_free(L, a);
}


const TValue *luaA_getint (Array *a, lua_Integer key) {
  /* (1 <= key && key <= t->sizearray) */
  if (l_castS2U(key) - 1u < a->sizearray)
    return &a->array[key - 1];
  else
    return luaH_emptyobject;
}

const TValue *luaA_get (lua_State *L, Array *a, const TValue *key) {
  if (ttypetag(key) == LUA_TNUMINT) {
    lua_Integer ikey = ivalue(key);
    if (l_castS2U(ikey) - 1u < a->sizearray)
      return &a->array[ikey - 1];
    else
      return luaH_emptyobject;
  } else {
    /* TODO: the error message could be improved */
    luaG_runerror(L, "attempt to index array with a non-integer value");
  }
}

void luaA_setint (lua_State *L, Array *a, lua_Integer key, TValue *value) {
  if (l_castS2U(key) - 1u < a->sizearray) {
    /* set value! */
    TValue* val = &a->array[key - 1];
    val->value_ = value->value_;
    val->tt_ = value->tt_;
    checkliveness(L,val);
  } else {
    /* TODO: this error message could be improved! */    
    luaG_runerror(L, "index out of bounds");
  }
}

void luaA_set (lua_State *L, Array *a, const TValue* key, TValue *value) {
  if (ttypetag(key) == LUA_TNUMINT) {
    lua_Integer ikey = ivalue(key);
    luaA_setint(L, a, ikey, value);
  } else {
    /* TODO: the error message could be improved */
    luaG_runerror(L, "attempt to index array with a non-integer value");
  }
}