#include <stdbool.h>
#include <stdint.h>
#include <string.h>

// 移除路径中的文件扩展名部分
char *path_remove(char *p_str)
{
    uint8_t length = strlen(p_str);
    uint8_t shift = 0;
    for (uint32_t i = 0; i < length; i++)
    {
        if (p_str[i] == '.')
        {
            if ((p_str[i + 1] == '\\') || (p_str[i + 1] == '/'))
            {
                shift = i;
            }
        }
    }
    return p_str + shift;
}

// 去除字符串首尾的空格和制表符
char *strtrim(char *p_str)
{
    char *p = p_str;
    char *q = p_str;

    // 去掉行首的空格和制表符
    while (*p == ' ' || *p == '\t')
        ++p;

    // 赋值并去掉行尾的空格和制表符
    while (*p != '\0')
    {
        *q++ = *p++;
    }
    *q = '\0'; // 确保字符串以空字符结尾

    // 去掉行尾的空格和制表符
    q--;
    while (q >= p_str && (*q == ' ' || *q == '\t'))
        --q;
    *(q + 1) = '\0';

    return p_str;
}

// 提取字符串中的中文字符
void chinese_extract(char *buff, char *p_str)
{
    char ch[4] = {0};

    uint32_t str_length = strlen(p_str);
    for (uint32_t i = 0; i < str_length; i++)
    {
        if (str_length - i < 3)
            return;

        if (is_zh(p_str[i])) // 判断是否为中文
        {
            ch[0] = p_str[i++]; // 中文占三个字符
            ch[1] = p_str[i++];
            ch[2] = p_str[i];

            if (!strstr(buff, ch)) // 判断是否已经记录过
            {
                strcat(buff, ch); // 记录
            }
        }
    }
}

// 分割字符串
void split_string(char *str, char delimiter, char *argv[], uint32_t *argc)
{
    uint32_t count = 0;
    char *start = str;
    char *end;

    while (*start != '\0')
    {
        // 跳过任何连续的分隔符
        while (*start == delimiter)
        {
            start++;
        }
        if (*start == '\0')
        {
            break;
        }

        // 找到下一个分隔符或字符串结尾
        end = start;
        while (*end != '\0' && *end != delimiter)
        {
            end++;
        }

        // 终止子字符串
        if (*end == delimiter)
        {
            *end = '\0';
            end++;
        }

        // 将子字符串的地址传给 argv
        argv[count] = start;
        count++;

        // 移动到下一个子字符串的开始位置
        start = end;
    }

    *argc = count;
}

// 分割字符串到缓冲区
void split_string_to_buff(char *buff, const char *str, const char *delimiters, const char *ignores, const char *stop, char *argv[], uint32_t *argc)
{
    uint32_t count = 0;
    memcpy(buff, str, strlen(str) + 1);

    char *start = buff;
    char *end;

    while (*start != '\0' && !strchr(stop, *start))
    {
        // 跳过任何连续的分隔符和忽略字符
        while (strchr(delimiters, *start) || strchr(ignores, *start))
        {
            start++;
        }
        if (*start == '\0' && !strchr(stop, *start))
        {
            break;
        }

        // 找到下一个分隔符或字符串结尾
        end = start;
        while (*end != '\0' && !strchr(stop, *end) && !strchr(delimiters, *end) && !strchr(ignores, *end))
        {
            end++;
        }

        // 终止子字符串
        if (*end != '\0' || !strchr(stop, *end))
        {
            *end = '\0';
            end++;
        }

        // 将子字符串的地址传给 argv
        argv[count] = start;
        count++;

        // 移动到下一个子字符串的开始位置
        start = end;
    }

    *argc = count;
}

// 字符串查找函数（指定长度）
char *strnstr(const char *haystack, const char *needle, uint32_t hay_len, uint32_t needle_len)
{
    if (needle_len == 0)
        return (char *)haystack;
    if (hay_len == 0 || hay_len < needle_len)
        return NULL;

    const char *end = haystack + hay_len - needle_len;
    for (; haystack <= end; haystack++)
    {
        if (strncmp(haystack, needle, needle_len) == 0)
        {
            return (char *)haystack;
        }
    }
    return NULL;
}

// 计算两个字符串的相同部分的长度
uint32_t strlen_common(const char *str1, const char *str2)
{
    uint32_t len1 = strlen(str1);
    uint32_t len2 = strlen(str2);
    uint32_t min_len = len1 < len2 ? len1 : len2; // 取两个字符串长度的最小值

    uint32_t common_length = 0;
    // 逐个字符比较直到遇到不同的字符或其中一个字符串结束
    for (uint32_t i = 0; i < min_len; i++)
    {
        if (str1[i] == str2[i])
        {
            common_length++;
        }
        else
        {
            break;
        }
    }
    return common_length;
}
