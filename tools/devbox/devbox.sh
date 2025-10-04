#
# Copyright Raffaele Rossi 2023 - 2024.
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
#
script_dir=$(dirname $0)
repo_dir=$(realpath ${script_dir}/../..)
docker run \
    --rm -it \
    --privileged \
    --user $(id -u):$(id -g) \
    -v /etc/passwd:/etc/passwd:ro -v /etc/group:/etc/group:ro \
    -v ${repo_dir}:${repo_dir} \
    -w ${repo_dir} \
    -e CCACHE_DIR=${repo_dir}/.cache/ccache \
    -e CONAN_USER_HOME=${repo_dir} \
    depc-devbox \
    "$@"
