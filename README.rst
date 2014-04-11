Very simple preemptive scheduler for Lua.
=========================================

``lsched`` is a very simple preemptive scheduler for Lua.

License: MIT (see LICENSE)

Dependencies
------------

Lua = 5.2

Compilation
-----------

To compile, just type the following command in the terminal::

    make

If you are on a debian-like system and you have installed all the required
dependencies, it should work as-is. If you are out of luck, you can tweak the
compilation process using the following variables:

- LUA_CFLAGS

For example, if the Lua development headers are not in a common location, you
can try::

    make LUA_CFLAGS="-I/path/to/lua/headers"

Documentation
-------------

The first thing to do is to create a scheduler::

    local lsched = require('lsched')
    local sched = lsched.new()

Then you can start adding threads to the scheduler::

    sched:new_thread(function(msg) while true do print(msg) end end, 'Hello world')
    -- it's also possible to specify the number of instructions a thread will
    -- execute before releasing control to another thread. To do that pass an
    -- integer as the first argument to the new_thread() method.
    sched:new_thread(10, function() while true do print('foo') end end)

Finally run the scheduler::

    sched:run()

When all threads managed by the scheduler terminate or die, the run() method returns.
It is possible to add new threads to the scheduler will it is running.

