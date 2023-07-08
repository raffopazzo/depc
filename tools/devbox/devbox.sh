script_dir=$(dirname $0)
repo_dir=$(realpath ${script_dir}/../..)
docker run \
    --rm -it \
    --user $(id -u):$(id -g) \
    -v /etc/passwd:/etc/passwd:ro -v /etc/group:/etc/group:ro \
    -v ${repo_dir}:${repo_dir} \
    -w ${repo_dir} \
    -e CONAN_USER_HOME=${repo_dir} \
    depc-devbox \
    "$@"
