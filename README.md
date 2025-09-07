# hobbes

This is a fork of https://github.com/morganstanley/hobbes . I very much doubt that my changes will be incorporated into original Hobbes, but just in case I'll use branch kmatveev-main as a main branch. README for original Hobbes was renamed to README-original.md .

Following improvements have been made:
- Support for modern versions of LLVM. Original Hobbes supports LLVM starting from version 3, handling incompatibilities via preprocessor directives. Between versions 14 and 17 LLVM moved from typed pointers to [opaque pointers ](https://llvm.org/docs/OpaquePointers.html) which is a big change. To simplify code, support for older versions of LLVM was removed. This fork of Hobbes is built and tested against LLVM 20, however earlier versions might also work.
- Support for Windows via MINGW compiler. Original Hobbes was available for Linux and Mac OS, and those OSes are also supported.

Roadmap:
- Support for Windows via MSVC and Clang compilers