xkb-perm-layout
===============

Utility to switch to default layout when X window switching

Build
-----

```bash
$ make
```

Run (as daemon)
---------------

```bash
$ ./build/xkb-perm-layout
```

!!! Warning !!! about run
-------------------------

Now it has bug
(see [issue #1](https://github.com/unclechu/xkb-perm-layout/issues/1)), —
when you close window daemon falls.

You can use temporarly solution for autorestart after fall:

```bash
$ ./xkb-perm-layout-daemon.sh ./build/xkb-perm-layout
```

Author
------

[Viacheslav Lotsmanov](https://github.com/unclechu)

License
-------

[GNU/GPLv3](./LICENSE)
