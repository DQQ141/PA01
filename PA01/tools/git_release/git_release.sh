#!/bin/bash

# 获取脚本所在目录
script_dir=$(dirname "$(readlink -f "$0")")

# 初始化输出文件
output_file="$script_dir/release.md"
> "$output_file" # 清空文件

# 获取所有tag列表（按时间降序排序）
tags=$(git tag --sort=-creatordate)

# 遍历每个tag并生成内容
for tag in $tags; do
    commit_hash=$(git rev-list -n 1 "$tag")
    tagger_info=$(git show "$tag" --pretty=format:"%an <%ae>" --no-patch)
    tag_date=$(git show "$tag" --pretty=format:"%ad" --date=iso --no-patch)
    commit_message=$(git tag -l --format="%(contents)" "$tag")

    # 追加到文件
    cat <<EOF >> "$output_file"

## $tag
**Commit:** $commit_hash  
**Tagger:** $tagger_info  
**Date:** $tag_date  
**Instructions:**  
$(echo "$commit_message" | sed 's/^/  - /')

---
EOF
done

echo "Release notes written to $output_file"
