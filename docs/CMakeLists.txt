find_package(Doxygen 1.8.17 OPTIONAL_COMPONENTS dot mscgen dia)

if (Doxygen_FOUND)
    configure_file(Doxyfile.in Doxyfile)
    doxygen_add_docs(docs CONFIG_FILE ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
    # NOTE: A neat trick to experiment with CSS changes is to link the final css file to the source one,
    # so you can then change the source file and just reload the final HTML page;
    # once you're done you can commit the changes without risk of losing them in between.
    # To do this you need to manually run something like the following after you've built the docs:
    # rm -f build/docs/html/depc.css
    #   && ./tools/devbox/devbox.sh cmake --build build -j 4 --target docs | tee build.log 
    #   && rm -f build/docs/html/depc.css
    #   && ln -r -s docs/depc.css -t build/docs/html
endif()
