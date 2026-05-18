# Flower 项目配置

## 自动加载 Skills

会话开始或每次 `/clear` 后，自动加载以下 skills（按优先级）：

1. **web-access** — 联网操作入口，搜索、网页抓取、CDP 浏览器自动化
2. **planning-with-files** — 复杂任务的文件规划与进度跟踪
3. **simplify** — 代码审查，保证质量和效率
4. **security-review** — 分支变更安全审查

加载方式：对应场景触发时直接调用对应 skill，无需等待用户手动 / 触发。

## 注意事项

- 联网操作一律通过 web-access skill 处理
- 复杂的多步骤任务（5+ 工具调用）先走 planning-with-files 规划
- 代码修改完成后主动走 simplify 审查
- 涉及安全敏感的变更走 security-review
