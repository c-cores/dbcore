# The Database Core

And it seems you found the Database Core. This library contains a collection of modules that simplify the caching of working data to files, which is typically useful in low memory environments.

There isn't any thorough documentation, and this library is really in its infancy.

# Why does this exist?

Mostly it was developed for use in Intel PIN tools because PIN tools share their memory space with the applications they are running.

## Why should I care?

Probably too early to care at the moment, but that's up to you.

# Linking

Lets start with a basic program which will include the table header. Every header in this library is found in the `db` directory. Every structure and function are contained within the `core` namespace. This core requires use of the standard core as well.

```c++
#include <db/table.h>

using namespace core;

int main()
{
    return 0;
}
```

This can be compiled with the following command.

```
g++ example.cpp -Ipath/to/include -Lpath/to/lib -ldbcore -lstdcore -o example
```

