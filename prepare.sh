#!/bin/sh
if test ! -d phanapi; then
    git clone "https://github.com/ronsaldo/phanapi" phanapi
fi

phanapi/scripts/make_headers.py definitions/api.xml include/AGPU || exit 1
phanapi/scripts/make_headers_cpp.py definitions/api.xml include/AGPU || exit 1
phanapi/scripts/make_implementation_stubs_cpp.py definitions/api.xml include/AGPU || exit 1
phanapi/scripts/make_icdloader.py definitions/api.xml implementations/Loader || exit 1
phanapi/scripts/make_pharo_bindings.py definitions/api.xml tonel || exit 1

