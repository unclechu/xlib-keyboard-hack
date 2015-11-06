xlib-keyboard-hack
==================

Xorg hack to do something when modifier key is pressed and do something
different when the modifier key is released. Or do something when one key
is pressed and do something different when other key is pressed
(depends on mode, start with `--help` to see more info).

For example remap arrow keys to home/end/page-up/page-down
if there's no this keys on a keyboard, and remap arrow keys
to normal when modifier key is released.

Useful for slim keyboards with limited keys.

Build
-----

```bash
$ make
```

Run (as daemon)
---------------

```bash
$ ./build/xlib-keyboard-hack -m
```
or
```bash
$ ./build/xlib-keyboard-hack -t
```

More usage info
---------------

```bash
$ ./build/xlib-keyboard-hack -h
```

You need to create two bash-scripts with on-action and off-action.

Also you need to put number of key to on-key configuration file,
and put number of key to off-key configuration file too
if you're using `--toggle` mode.

Start app with `--help` to see paths of this files.

Author
------

[Viacheslav Lotsmanov](https://github.com/unclechu)

License
-------

[GNU/GPLv3](./LICENSE)
