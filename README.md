# Timberman
Cross-platform, thread-safe C++ logging and function tracing library

# Quick start

This is a cross-platform, thread-safe library that implements robust logging and function tracing in C++ code.  The library supports two powerful features: message logging and function tracing.

`Message logging` is the ability for the application to output a debugging or informational message which is guaranteed to be logged to the output log file in a reliable and thread-safe way.  When the application finishes running, the developer has access to the complete logging file.

`Function tracing` is the ability for the application to automatically trace the entering and existing of any or all functions in the application.  The ability for a developer to debug a problem knowing precisely which functions were entered/existed and in what order they were called can significantly help the debugging process.

Both message logging and function tracing can be controlled when the Timberman system is initialized at the application start-up time.

## Basics

## Timberman API

### Constructors

```
*GetCmdOpt (int argc, char *argv [], std::string keyPrefix = "--")*
```

**Description**

Use the `GetCmdOpt` constructor to create an object that can then be probed for command line options.  It accepts

**Parameters**

    argc       [in] The original `argc` passed by the operating system to the program

    argv       [in] The original `argv` passed by the operating system to the program
           
    keyPrefix  [in] An option string denoting the characters that are to prefix to all keys in the command line,
                identifying the command line token as a key string rather than a value string.  The default key
                prefix string is two successive hyphens ('--'), but this can be changed for a given program by
                specifying a different string.

## Coding Example

```
```

# Tips / Tricks / Caveats

