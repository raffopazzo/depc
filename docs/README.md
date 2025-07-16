At some point in December 2024 I decided to move documentatation from markdown files to Doxygen
but I have not yet found the time to host the result somewhere on the internet.

So for now you can find high-level non-API documentation in `.dox` files.
Or, if you feel proactive, you can build the full documentation locally via the `docs` CMake target,
for example`cmake --build build --target docs` and you should get a `build/docs/html/index.html` file
that you can open with your favourite browser.
