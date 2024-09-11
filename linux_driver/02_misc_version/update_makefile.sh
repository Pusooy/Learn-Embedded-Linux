#!/bin/bash

# 找到当前目录下名称最短的 .c 文件
shortest_c_file=$(find . -maxdepth 1 -name "*.c" -exec basename {} \; | awk 'length < min_length || NR == 1 { min_length = length; shortest = $0 } END { print shortest }')

# 检查是否找到 .c 文件
if [ -z "$shortest_c_file" ]; then
  echo "No .c file found in the current directory."
  exit 1
fi

# 获取文件名（不带扩展名）
file_name="${shortest_c_file%.c}"

# 修改 Makefile 中的 obj-m 值
sed -i "s/^obj-m.*/obj-m += ${file_name}.o/" Makefile

echo "Makefile has been updated with obj-m += ${file_name}.o"