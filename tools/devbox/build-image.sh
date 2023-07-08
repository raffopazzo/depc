script_dir=$(dirname $0)
docker build \
    -f ${script_dir}/devbox.Dockerfile \
    --rm --tag depc-devbox-gcc13 \
    ${script_dir}
