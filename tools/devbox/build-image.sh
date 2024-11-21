#
# Copyright Raffaele Rossi 2023 - 2024.
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
#
script_dir=$(dirname $0)
docker build \
    -f ${script_dir}/devbox.Dockerfile \
    --rm --tag depc-devbox \
    ${script_dir}
