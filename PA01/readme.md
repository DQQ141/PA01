# PA01 - 项目名称

## 概述

本仓库包含**PA01**项目的源代码和文档，该项目目标：工厂测试程序。项目使用KEIL进行编译，使用VSCode进行代码编写。

## 功能特性

- 工厂模式开发

## 安装与运行

### 前提条件

在开始之前，请确保您已安装以下软件：

- [KEIL MDK](https://www.keil.com/download/product/)（官网） / KEIL (软件安装可以找邬工)
- [Visual Studio Code](https://code.visualstudio.com/)
- [VSCode插件]（如C/C++、ARM等）

### 使用流程

1. **克隆仓库**

   - 打开终端或命令提示符，运行以下命令克隆仓库到本地：
     ```bash
     git clone http://192.168.2.16:1515/xieyushen/PA01.git
     ```
   - 进入项目目录：
     ```bash
     cd PA01
     ```
2. **上传代码**

   - 在本地开发环境中完成代码编写和测试。
   - 将代码推送到远程仓库的特定分支（如 `feature`或 `username`）：
     ```bash
     git checkout -b <feature>
     git add .
     git commit -m "描述提交内容的简短信息"
     git push origin <feature>
     ```
3. **申请合并到 `master`**

   - 在GitHub上打开仓库页面，点击“Pull requests”选项卡。
   - 点击“New pull request”按钮。
   - 选择您刚刚推送的分支（如 `feature`或 `username`）作为“compare”分支，选择 `master`作为“base”分支。
   - 填写PR（Pull Request）的标题和描述，说明您所做的更改和其目的。
   - 点击“Create pull request”按钮，等待审核和合并。
