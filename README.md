# AggroSTL

A C++20 template library which provides an alternative to the Standard Template Library. This library does not completely replace the STL, but has several memory efficient alternatives to their STL counterparts. This library takes advangate of C++20 Concepts in order to provide a clean template API that is (relatively) easy to debug.  

One of the big goals of this library is to give the programmer as much control over their memory as possible. The data containers in this library are very flexible in thisregard. Right now the main container we've optimized with this goal is **aggro::darray**, the replacement for **std::vector**. It gives the programmer the ability to decidehow much memory it allocates when it needs to expand, and programmers can change this *at run-time!* This is in contrast with std::vector which gives you zero control over that and can sometimes be unpredictable with it's memory consumption.  

Another goal of this library is performance. The AggroSTL tries to achieve CPU and memory efficiency that is equal to or better than the Standard Template Library. Although the AggroSTL is still young and far from perfect, so far the benchmark tests we have been running look promising.  

The final goal of this library is to avoid the use of exceptions. To that end, we have created the **optional_ref** template which can be used to eliminate the need to resort to exceptions when reference returning functions fail. However, this feature is still experimental so use carefully.  

## Dependence on the Standard Template Library  

Currently the AggroSTL relies on some parts of the STL for building its API, namely the Concepts library. Some of these dependencies will be gradually replaced with our own code as the AggroSTL matures and develops. Keep in mind, however, this library will never completely replace the STL. It will simply give game developers additional tools created specifically with their needs in mind.  

That being said, the AggroSTL trys to emulate the STL's API where it can for easy migration and compatibility with existing algorithms.  

## Why Call it AggroSTL?  

Because "aggro" is part of my gamer tag. With this being a game development focused library, I thought it was fitting.  

## Header Only  

With this library being primarily template-based, it only contains ".hpp" files with some test programs. You only need to **#include** the library files into your project.  
