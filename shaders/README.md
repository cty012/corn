# Compiling and Embedding Shaders

See https://github.com/bkaradzic/bgfx for information on how to obtain the `shaderc` tool.

## Compiling Shaders

### Windows (Direct3D 12):
Compiling vertex shaders:
```shell
.\shaderc.exe -f [INPUT_SOURCE_FILE] -o [OUTPUT_BINARY_FILE] --type v --platform windows --profile s_5_0
```
Compiling fragment shaders:
```shell
.\shaderc.exe -f [INPUT_SOURCE_FILE] -o [OUTPUT_BINARY_FILE] --type f --platform windows --profile s_5_0
```

### MacOS (Metal):
Compiling vertex shaders:
```shell
./shaderc.exe -f [INPUT_SOURCE_FILE] -o [OUTPUT_BINARY_FILE] --type v --platform osx --profile metal
```
Compiling fragment shaders:
```shell
./shaderc.exe -f [INPUT_SOURCE_FILE] -o [OUTPUT_BINARY_FILE] --type f --platform osx --profile metal
```

### Linux (Vulkan):
Compiling vertex shaders:
```shell
./shaderc.exe -f [INPUT_SOURCE_FILE] -o [OUTPUT_BINARY_FILE] --type v --platform linux --profile spirv
```
Compiling fragment shaders:
```shell
./shaderc.exe -f [INPUT_SOURCE_FILE] -o [OUTPUT_BINARY_FILE] --type f --platform linux --profile spirv
```

## Embedding Shaders

Use the python script `dump.py` to extract the hex dump of compiled shaders for embedding:
```shell
python dump.py [INPUT_BINARY_FILE] [OUTPUT_HEXDUMP_FILE]
```
