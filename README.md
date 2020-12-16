# dbus-c++

This is a library for d-bus communication in C++.
You can generate adaptors and proxies from an introspection file and use it with `ecore`, `glib` or `Boost.Asio`.

# Build and install the library

```
./bootstrap
mkdir build
cd build
../configure
make
sudo make install
```

## Disable components

To disable ecore and glib support, call `../configure --disable-ecore --disable-glib`
For other options, see `../configure --help`

## Debugging support

To compile debugging code configure the project with the `--enable-debug` option.
Then at runtime you may export the environment variable `DBUSXX_VERBOSE` to any value.

# Use the library

## Code generation

Use dbusxx-xml2cpp to generate the code from your introspection xml file

TODO: describe how to use it with autotools / qmake / cmake /...

## Use the needed integration

Dbus-c++ installs .pc files for pkg-config depending on the configured integrations.
If you want to use this library with Boost.Asio, you need dbus-c++-asio-1.pc

# Bugs

* autogen.sh does not work from a sub directory
