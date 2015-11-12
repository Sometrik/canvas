canvas
======

A C++ canvas implementation.

Currently this library is used to create OpenGL textures with an interface which is based on HTML5 Canvas.

On Windows, rendering is done using GDI+, Cairo is used on Linux and Quartz2D is used on iOS.

Development of an Android version is in progress.

Fluent Style
============

Fluent programming style is supported in this implementation, even though it's not available in the original HTML5 Canvas:

    context->strokeStyle("#ff0000").moveTo(10, 10).lineTo(20, 20).stroke();

Example
=======

    auto context = new ContextCairo(width, height);
    context->moveTo(10, 10);
    context->lineTo(20, 20);
    context->strokeStyle = "#ff0000";
    context->stroke();
