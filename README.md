# PS5 Baidu Launcher

一个小型 ELF 安装器：在 PS5「媒体」分类注册“百度”网页快捷方式，目标为 `https://www.baidu.com/`。

它使用系统内置网页窗口，不包含浏览器内核、本地 HTTP 服务、DNS 修改或全屏补丁。安装器完成后退出。

## 使用

1. 从本仓库 Actions 中下载成功构建的 `baidu-launcher` artifact，解压获得 `baidu-launcher.elf`。
2. 在已经完成破解、具备文件写入和应用注册权限的 PS5 上，通过 Payload Manager 或兼容 ELF loader 加载一次。
3. 成功通知出现后，回到「媒体」分类打开“百度”。需要正常联网。

目标环境为用户的 PS5 13.60 WebKit 破解，但尚未实机验证。编译成功不代表确认固件兼容。重启后入口是否仍可使用，取决于系统和破解环境，需实测。

安装路径为 `/user/app/BDUL00001/sce_sys/`。独立 title ID 为 `BDUL00001`。相同资源可重复执行并重试注册；发现同 ID 的不同资源时拒绝覆盖。没有后台常驻进程。

如果注册接口的按标题安装符号不可用，会使用参考项目的 `sceAppInstUtilAppInstallAll` 回退方式，此方式可能同时注册其他已暂存的应用目录。

失败时查看 payload 标准输出中的十六进制错误码。不要为此升级系统。删除入口可尝试主界面系统提供的删除功能；本项目不自动删除文件。

## 编译

Linux / WSL 中按 [PS5 Payload SDK](https://github.com/ps5-payload-dev/sdk#quick-start) 配置 clang-18、lld-18、make、Python 3 并解压 SDK：

```sh
export PS5_PAYLOAD_SDK=/opt/ps5-payload-sdk
make
```

输出 `baidu-launcher.elf`，图标和参数已嵌入，无需额外复制资源。GitHub Actions 使用 SDK 最新发布包，并在日志记录其 SHA-256；不同日期的 SDK 可能不同。

## 来源与许可

应用注册逻辑改编自 [owendswang/ps5-web-file-manager](https://github.com/owendswang/ps5-web-file-manager)，参考提交 `3659cbbc40022e32229f9919f045018a988f2ef5` 的 `src/app_installer.c`、通知 ABI 和参数布局。感谢原作者及 ps5-payload-dev 社区。

本项目采用 GPL-3.0-or-later，保留上游 LICENSE。图标为本项目生成的通用字母图标，不是百度官方图标。本项目非 Sony 或百度官方产品。
