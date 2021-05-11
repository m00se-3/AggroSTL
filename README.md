# AggroSTL

A C++20 template library which provides an alternative to the Standard Template Library. This library does not replace the STL, but provides
 an optimized utility for writing memory efficient and safe code. This library takes advangate of C++20 Concepts in order to provide
 a clean template API that is easy to debug.  

## Dependence on the STL  

Currently the AggroSTL relies on some parts of the STL for building its API, namely the Concepts library and some parts of the Utility library.
 These dependencies will be gradually replaced with our own code as the AggroSTL matures and develops.  

That being said, the AggroSTL trys to emulate the STL's API where it can for easy migration and compatibility with existing algorithms.  

## Why Call it AggroSTL?  

Because "aggro" is part of my gamer tag. With this being a game development focused library, I thought it was fitting.  

## Header Only  

With this library being primarily template-based, it only contains ".hpp" files with some test programs. You only need to **#include** the library  
files into your project.  
