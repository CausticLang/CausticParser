(
    cd ./src/ast/protobuf/
    protoc --c_out . *.proto
) || exit 1
