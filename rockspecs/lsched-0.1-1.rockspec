package = "lsched"
version = "0.1-1"
source = {
    url = "git://github.com/montag451/lsched",
    tag = "v0.1"
}
description = {
    summary = "Very simple preemptive scheduler for Lua.",
    detailed = [[
        lsched is a very simple preemptive scheduler for Lua.
    ]],
    homepage = "http://github.com/montag451/lsched",
    license = "MIT"
}
dependencies = {
    "lua >= 5.1"
}
build = {
    type = "builtin",
    modules = {
        lsched = "lsched.c"
    }
}

