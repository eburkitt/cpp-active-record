# Installation

# Download the source

```
git clone git://github.com/joeyates/cpp-active-record.git
```

# Install Dependencies

- [Boost](http://www.boost.org/)
- [GoogleTest](http://code.google.com/p/googletest/)
- SQLite3

Ubuntu:

```shell
sudo apt-get install \
  libboost-all-dev \
  libgtest-dev \       # Test use the gtest library
  libsqlite3-dev \
  xutils-dev           # The build system requires `makedepend`
```

Complie gtest:

```shell
# cd /usr/src/gtest && cmake . && make && mv libg* /usr/lib/ && cd -
```

# Install the library

```shell
./configure
make
make install
```
