bprintf
=======

bprintf is intended to be a better printf. Better in this context means type-safe, extensible and performant.

There are many attempts to fix printf's lack of type-safety notably boost::format.

However, all alternatives I've found have usually is beaten quite severely by printf in terms of performance.

With bprintf I want the performance to be at least as good as with printf.

In addition bprintf adopts the formatting concept from C# in order to provide extensibility for your own types.

Sample
```c++
    bprintf (
        "Hi there %0% %1%!\n%0% you are %2:000% years old\n"
      , "John"
      , std::string ("Doe")
      , 34.5
      );
```
