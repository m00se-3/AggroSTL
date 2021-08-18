# AggroSTL

A C++20 template library which provides an alternative to the Standard Template Library. This library does not completely replace the STL, but has several memory efficient alternatives to their STL counterparts. This library takes advangate of C++20 Concepts in order to provide a clean template API that is (relatively) easy to debug.  

 One of the big goals of this library is to give the programmer as much control over their memory as possible. The data containers in this library are very flexible in this regard. Right now the main container we've optimized with this goal is **aggro::darray**, the replacement for **std::vector**. It gives the programmer the ability to decide how much memory it allocates when it needs to expand, and programmers can change this *at run-time!* This is in contrast with std::vector which gives you zero control over that.

 This library also tries to acheive minimal CPU overhead. One small way it does this is by only abstracting iterators when and where it needs to. If a simple pointer provides all the functionality needed, it just uses that.  

Another goal of this library is to avoid the use of exceptions. To that end, we have created the **optional_ref** template which can be used to eliminate the need to resort to exceptions when reference returning functions fail. However, this feature is still experimental so use carefully.  

## Dependence on the Standard Template Library  

Currently the AggroSTL relies on some parts of the STL for building its API, namely the Concepts library. These dependencies will be gradually replaced with our own code as the AggroSTL matures and develops.  

That being said, the AggroSTL trys to emulate the STL's API where it can for easy migration and compatibility with existing algorithms.  

## Why Call it AggroSTL?  

Because "aggro" is part of my gamer tag. With this being a game development focused library, I thought it was fitting.  

## Header Only  

With this library being primarily template-based, it only contains ".hpp" files with some test programs. You only need to **#include** the library  
files into your project.  
