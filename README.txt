taapp
=====

author:  Thomas Atwood (tatwood.net)
date:    2010
license: see UNLICENSE.txt

This is a collection of C++ headers that provide inlined functionality for
STL containers. These classes are designed to be a subset of the standard.
For example, these containers prevent to copy or assignment of themselves even
though this is valid behavior in the standard. Additionally, no support for
C++ exceptions is provided in these containers. This is a header only library,
no link target is required for its use.

Dependencies
============

No dependencies exist other than the C standard headers.

