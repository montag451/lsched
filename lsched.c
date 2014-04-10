#include <stdlib.h>
#include <assert.h>

#include <lua.h>
#include <lauxlib.h>

#define LSCHED_SCHEDULER_TNAME "lsched.Scheduler"
#define DEFAULT_COUNT_HOOK 100

static int print(lua_State* L)
{
    int nargs = lua_gettop(L);
    int i;

    luaL_checkstack(L, nargs + 1, "too many values to print");
    lua_getglobal(L, "print");
    if (lua_isnil(L, -1)) {
        return 0;
    }
    for (i = 1; i <= nargs; ++i) {
        lua_pushvalue(L, i);
    }
    lua_call(L, nargs, 0);
    return 0;
}

static void scheduler_hook(lua_State* L, lua_Debug* ar)
{
    (void)ar;
    lua_yield(L, 0);
}

static int scheduler_new_thread(lua_State* L)
{
    lua_State* th;
    int is_count;
    int count;
    int func_idx;
    int func_nargs;

    luaL_checkudata(L, 1, LSCHED_SCHEDULER_TNAME);
    count = lua_tointegerx(L, 2, &is_count);
    func_idx = is_count ? 3 : 2;
    luaL_checktype(L, func_idx, LUA_TFUNCTION);
    func_nargs = lua_gettop(L) - func_idx;
    th = lua_newthread(L);
    lua_insert(L, func_idx);
    luaL_checkstack(th, func_nargs + 1, "too many arguments for thread");
    lua_xmove(L, th, func_nargs + 1);
    lua_getuservalue(L, 1);
    lua_pushvalue(L, -2);
    lua_rawseti(L, -2, lua_rawlen(L, -2) + 1);
    lua_sethook(th, scheduler_hook, LUA_MASKCOUNT, is_count ? count : DEFAULT_COUNT_HOOK);
    return 0;
}

static int scheduler_run(lua_State* L)
{
    lua_State* th;
    int status;
    int nthreads;
    int i;
    int nargs;
    int nres;

    luaL_checkudata(L, 1, LSCHED_SCHEDULER_TNAME);
    lua_getuservalue(L, 1);
    for (;;) {
        nthreads = lua_rawlen(L, -1);
        if (!nthreads) {
            break;
        }
        for (i = 1; i <= nthreads; ++i) {
            lua_rawgeti(L, -1, i);
            th = lua_tothread(L, -1);
            assert(th != NULL);
            lua_pop(L, 1); /* remove thread from stack */
            nargs = lua_status(th) == LUA_OK ? lua_gettop(th) - 1 : 0;
            status = lua_resume(th, L, nargs);
            nthreads = lua_rawlen(L, -1);
            if (status != LUA_YIELD) {
                /* remove thread from list */
                int pos = i;
                for (; pos < nthreads; pos++) {
                    lua_rawgeti(L, -1, pos + 1);
                    lua_rawseti(L, -2, pos);
                }
                lua_pushnil(L);
                lua_rawseti(L, -2, pos);
                --i;
                --nthreads;
                if (status != LUA_OK) {
                    lua_pushcfunction(L, print);
                    lua_pushstring(L, "*** thread terminated with error ***\n");
                    luaL_traceback(L, th, lua_tostring(th, -1), 1);
                    lua_concat(L, 2);
                    lua_pcall(L, 1, 0, 0);
                }
                continue;
            }
            nres = lua_gettop(th);
            lua_pop(th, nres);
        }
    }
    return 0;
}

static const luaL_Reg lsched_scheduler_meth[] = {
    {"new_thread", scheduler_new_thread},
    {"run", scheduler_run},
    {NULL, NULL}
};

static void create_scheduler_meta(lua_State* L)
{
    luaL_newmetatable(L, LSCHED_SCHEDULER_TNAME);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, lsched_scheduler_meth, 0);
    lua_pop(L, 1);
}

static int lsched_new(lua_State* L)
{
    lua_newuserdata(L, 0);
    lua_newtable(L);
    lua_setuservalue(L, -2);
    luaL_setmetatable(L, LSCHED_SCHEDULER_TNAME);
    return 1;
}

static const luaL_Reg lschedlib[] = {
    {"new", lsched_new},
    {NULL, NULL}
};

int luaopen_lsched(lua_State* L)
{
    luaL_newlib(L, lschedlib);
    create_scheduler_meta(L);
    return 1;
}

