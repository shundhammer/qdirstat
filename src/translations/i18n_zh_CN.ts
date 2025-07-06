<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="zh_CN">
<context>
    <name>Cleanup</name>
    <message>
        <location filename="../Cleanup.cpp" line="232"/>
        <source>ERROR: Shell %1 is not executable</source>
        <translation>错误：Shell %1 不可执行</translation>
    </message>
    <message>
        <location filename="../Cleanup.cpp" line="236"/>
        <source>Using fallback %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../Cleanup.cpp" line="265"/>
        <source>No usable shell - aborting cleanup action</source>
        <translation>没有可用的 shell - 中止清理操作</translation>
    </message>
</context>
<context>
    <name>CleanupConfigPage</name>
    <message>
        <location filename="../cleanup-config-page.ui" line="14"/>
        <source>Form</source>
        <translation>清理配置窗口</translation>
    </message>
    <message>
        <location filename="../cleanup-config-page.ui" line="45"/>
        <source>Move the current cleanup action to the top of the list.</source>
        <translation>将当前清理操作移至列表顶部。</translation>
    </message>
    <message>
        <location filename="../cleanup-config-page.ui" line="48"/>
        <location filename="../cleanup-config-page.ui" line="62"/>
        <location filename="../cleanup-config-page.ui" line="90"/>
        <location filename="../cleanup-config-page.ui" line="104"/>
        <location filename="../cleanup-config-page.ui" line="131"/>
        <location filename="../cleanup-config-page.ui" line="145"/>
        <source>...</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../cleanup-config-page.ui" line="59"/>
        <source>Move the current cleanup action one position up in the list.</source>
        <translation>将当前清理操作在列表中向上移动一个位置。</translation>
    </message>
    <message>
        <location filename="../cleanup-config-page.ui" line="86"/>
        <source>Create a new cleanup action.
You can create as many as you like.</source>
        <translation>创建新的清理操作。
您可以创建任意数量的清理操作。</translation>
    </message>
    <message>
        <location filename="../cleanup-config-page.ui" line="101"/>
        <source>Remove the current cleanup action.</source>
        <translation>删除当前的清理操作。</translation>
    </message>
    <message>
        <location filename="../cleanup-config-page.ui" line="128"/>
        <source>Move the current cleanup action one position down in the list.</source>
        <translation>将当前清理操作在列表中向下移动一个位置。</translation>
    </message>
    <message>
        <location filename="../cleanup-config-page.ui" line="142"/>
        <source>Move the current cleanup action to the bottom of the list.</source>
        <translation>将当前清理操作移至列表底部。</translation>
    </message>
    <message>
        <location filename="../cleanup-config-page.ui" line="165"/>
        <source>Uncheck to disable this cleanup action completely.</source>
        <translation>取消选中可完全禁用此清理操作。</translation>
    </message>
    <message>
        <location filename="../cleanup-config-page.ui" line="168"/>
        <source>&amp;Active</source>
        <translation>激活(&amp;A)</translation>
    </message>
    <message>
        <location filename="../cleanup-config-page.ui" line="185"/>
        <source>&amp;Title:</source>
        <translation>标题(&amp;T):</translation>
    </message>
    <message>
        <location filename="../cleanup-config-page.ui" line="201"/>
        <source>Check this if a confirmation dialog should be
opened before executing the command.
There will be only one confirmation dialog,
even if multiple items are selected or if
the command is executed recursively.</source>
        <translation>如果在执行命令之前需要打开确认对话框，请勾选此项。
即使选择了多个项目或命令以递归方式执行，也只会显示一个确认对话框。</translation>
    </message>
    <message>
        <location filename="../cleanup-config-page.ui" line="208"/>
        <source>As&amp;k For Confirmation</source>
        <translation>询问确认(&amp;K)</translation>
    </message>
    <message>
        <location filename="../cleanup-config-page.ui" line="221"/>
        <source>Enter a title for this cleanup action.
This will show up in the menus.
You can add a hotkey with &quot;&amp;&quot;.</source>
        <translation>输入此清理操作的标题。
该标题将显示在菜单中。
您可以使用“&amp;”添加热键。</translation>
    </message>
    <message>
        <location filename="../cleanup-config-page.ui" line="248"/>
        <source>These are the macros you can use in the command.
Both %p and %n always contain only one item.
If multiple items are selected, the command will be
executed multiple times.</source>
        <translation>这些是您可以在命令中使用的宏。
%p 和 %n 始终只包含一个项目。
如果选择了多个项目，则该命令将被执行多次。</translation>
    </message>
    <message>
        <location filename="../cleanup-config-page.ui" line="254"/>
        <source>%p  Full Path
%n  File / Directory Name Without Path
%d  Directory Name With Full Path
</source>
        <translation>%p 完整路径
%n 不含路径的文件/目录名称
%d 包含完整路径的目录名称
</translation>
    </message>
    <message>
        <location filename="../cleanup-config-page.ui" line="288"/>
        <source>&amp;Command Line:</source>
        <translation>命令行(&amp;C):</translation>
    </message>
    <message>
        <location filename="../cleanup-config-page.ui" line="298"/>
        <source>Enter the command to be executed here.
Each command will be executed with a shell.

If multiple items are selected, the command
will be executed once for each selected item.</source>
        <translation>在此处输入要执行的命令。
每个命令都将通过 shell 执行。

如果选择了多个项目，则该命令将针对每个选定项目执行一次。</translation>
    </message>
    <message>
        <location filename="../cleanup-config-page.ui" line="315"/>
        <source>Select if the command should be executed recursively
for each directory in the selected branch.
The command will be called separately for each directory level.</source>
        <translation>选择是否对所选分支中的每个目录递归执行该命令。
该命令将针对每个目录级别单独调用。</translation>
    </message>
    <message>
        <location filename="../cleanup-config-page.ui" line="320"/>
        <source>&amp;Recurse Into Subdirectories</source>
        <translation>递归到子目录(&amp;R)</translation>
    </message>
    <message>
        <location filename="../cleanup-config-page.ui" line="333"/>
        <source>This specifies if and how the directory tree should be
re-read from disk after the cleanup action is finished.</source>
        <translation>这指定了清理操作完成后是否以及如何从磁盘重新读取目录树。</translation>
    </message>
    <message>
        <location filename="../cleanup-config-page.ui" line="341"/>
        <source>No Refresh</source>
        <translation>不刷新</translation>
    </message>
    <message>
        <location filename="../cleanup-config-page.ui" line="346"/>
        <source>Refresh This Item</source>
        <translation>刷新此项目</translation>
    </message>
    <message>
        <location filename="../cleanup-config-page.ui" line="351"/>
        <source>Refresh This Item&apos;s Parent</source>
        <translation>刷新此项目的父级</translation>
    </message>
    <message>
        <location filename="../cleanup-config-page.ui" line="356"/>
        <source>Assume Item Has Been Deleted</source>
        <translation>假设项目已被删除</translation>
    </message>
    <message>
        <location filename="../cleanup-config-page.ui" line="370"/>
        <source>R&amp;efresh Policy:</source>
        <translation>刷新策略(&amp;E):</translation>
    </message>
    <message>
        <location filename="../cleanup-config-page.ui" line="416"/>
        <source>Works For...</source>
        <translation>应用到...</translation>
    </message>
    <message>
        <location filename="../cleanup-config-page.ui" line="424"/>
        <source>Check this if the command can be applied to directories.</source>
        <translation>检查该命令是否可以应用于目录。</translation>
    </message>
    <message>
        <location filename="../cleanup-config-page.ui" line="427"/>
        <source>&amp;Directories</source>
        <translation>目录(&amp;D)</translation>
    </message>
    <message>
        <location filename="../cleanup-config-page.ui" line="434"/>
        <source>Check this if the command can be applied to files.</source>
        <translation>如果命令可以应用于文件，请检查此项。</translation>
    </message>
    <message>
        <location filename="../cleanup-config-page.ui" line="437"/>
        <source>&amp;Files</source>
        <translation>文件(&amp;F)</translation>
    </message>
    <message>
        <location filename="../cleanup-config-page.ui" line="444"/>
        <source>Check this if the command can be applied to the &lt;Files&gt; entries in the tree.
Note that they don&apos;t really have a counterpart on the disk.</source>
        <translation>如果该命令可以应用于树状图中的 &lt;Files&gt; 条目，请选中此项。
请注意，它们实际上在磁盘上没有对应项。</translation>
    </message>
    <message>
        <location filename="../cleanup-config-page.ui" line="448"/>
        <source>&lt;Files&gt; Pse&amp;udo Entries</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../cleanup-config-page.ui" line="479"/>
        <source>Output Window</source>
        <translation>输出窗口</translation>
    </message>
    <message>
        <location filename="../cleanup-config-page.ui" line="493"/>
        <source>&amp;Policy:</source>
        <translation>策略(&amp;P):</translation>
    </message>
    <message>
        <location filename="../cleanup-config-page.ui" line="509"/>
        <source>Select if and when to open a window for the output of the command.
If there is an error, the output window is always opened unless
&quot;Never Show&quot; is selected here.
&quot;Show After Timeout&quot; is a useful setting for most cases.</source>
        <translation>选择是否以及何时打开命令输出窗口。
如果出现错误，则输出窗口始终会打开，除非
在此处选择“从不显示”。
“超时后显示”在大多数情况下都是一个有用的设置。</translation>
    </message>
    <message>
        <location filename="../cleanup-config-page.ui" line="519"/>
        <source>Show Always</source>
        <translation>总是显示</translation>
    </message>
    <message>
        <location filename="../cleanup-config-page.ui" line="524"/>
        <source>Show If Error Output</source>
        <translation>有错误输出时显示</translation>
    </message>
    <message>
        <location filename="../cleanup-config-page.ui" line="529"/>
        <source>Show After Timeout</source>
        <translation>超时后显示</translation>
    </message>
    <message>
        <location filename="../cleanup-config-page.ui" line="534"/>
        <source>Never Show</source>
        <translation>从不显示</translation>
    </message>
    <message>
        <location filename="../cleanup-config-page.ui" line="548"/>
        <source>Ti&amp;meout:</source>
        <translation>超时(&amp;M):</translation>
    </message>
    <message>
        <location filename="../cleanup-config-page.ui" line="564"/>
        <source>Set the timeout for the &quot;Show After Timeout&quot; setting.
Don&apos;t underestimate your computer: It can get more
done in half a second than you might think.
On the other hand, waiting for half a second feels
longer than you might think.</source>
        <translation>设置“超时后显示”设置的超时时间。
不要低估你的电脑：它在半秒内完成的任务可能比你想象的要多。
另一方面，等待半秒的时间感觉起来比你想象的要长。</translation>
    </message>
    <message>
        <location filename="../cleanup-config-page.ui" line="571"/>
        <source> sec</source>
        <translation> 秒</translation>
    </message>
    <message>
        <location filename="../cleanup-config-page.ui" line="587"/>
        <source>Check this to automatically close the
output window if there was no error.
If you are interested in the output of your
command, uncheck this.</source>
        <translation>勾选此项，如果没有错误，则自动关闭输出窗口。
如果您对命令的输出感兴趣，请取消勾选此项。</translation>
    </message>
    <message>
        <location filename="../cleanup-config-page.ui" line="593"/>
        <source>Auto C&amp;lose If No Error</source>
        <translation>如果没有错误则自动关闭(&amp;L)</translation>
    </message>
    <message>
        <location filename="../cleanup-config-page.ui" line="630"/>
        <source>Shell</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../cleanup-config-page.ui" line="644"/>
        <source>&amp;Shell:</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../cleanup-config-page.ui" line="660"/>
        <source>Select a shell here or enter your favourite one.

Each command is executed with a shell and the &quot;-c&quot; option.
Shells may behave differently for wildcards and special characters.

If you are setting this up for other users, better stay on the safe side
and select a shell with a well-known behaviour like /bin/bash.</source>
        <translation>在此处选择一个 Shell 或输入您喜欢的 Shell。

每个命令都通过 Shell 和“-c”选项执行。
Shell 对通配符和特殊字符的处理方式可能有所不同。

如果您要为其他用户设置此 Shell，最好谨慎起见，
并选择具有常见行为的 Shell，例如 /bin/bash。</translation>
    </message>
    <message>
        <location filename="../cleanup-config-page.ui" line="673"/>
        <source>$SHELL (Login Shell)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../cleanup-config-page.ui" line="678"/>
        <source>/bin/bash</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../cleanup-config-page.ui" line="683"/>
        <source>/bin/sh</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../cleanup-config-page.ui" line="693"/>
        <source>Each command is executed with a shell
and the &apos;-c&apos; option.</source>
        <translation>每个命令都通过 shell 和“-c”选项执行。</translation>
    </message>
</context>
<context>
    <name>ConfigDialog</name>
    <message>
        <location filename="../config-dialog.ui" line="14"/>
        <source>QDirStat Configuration</source>
        <translation>QDirstat 配置</translation>
    </message>
    <message>
        <location filename="../config-dialog.ui" line="48"/>
        <source>OK</source>
        <translation>确定</translation>
    </message>
    <message>
        <location filename="../config-dialog.ui" line="55"/>
        <source>Apply</source>
        <translation>应用</translation>
    </message>
    <message>
        <location filename="../config-dialog.ui" line="62"/>
        <source>Cancel</source>
        <translation>取消</translation>
    </message>
</context>
<context>
    <name>ExcludeRulesConfigPage</name>
    <message>
        <location filename="../exclude-rules-config-page.ui" line="14"/>
        <source>Form</source>
        <translation>排除规则配置窗口</translation>
    </message>
    <message>
        <location filename="../exclude-rules-config-page.ui" line="45"/>
        <source>Move the current exclude rule to the top of the list.</source>
        <translation>将当前排除规则移至列表顶部。</translation>
    </message>
    <message>
        <location filename="../exclude-rules-config-page.ui" line="48"/>
        <location filename="../exclude-rules-config-page.ui" line="62"/>
        <location filename="../exclude-rules-config-page.ui" line="90"/>
        <location filename="../exclude-rules-config-page.ui" line="104"/>
        <location filename="../exclude-rules-config-page.ui" line="131"/>
        <location filename="../exclude-rules-config-page.ui" line="145"/>
        <source>...</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../exclude-rules-config-page.ui" line="59"/>
        <source>Move the current exclude rule one position up in the list.</source>
        <translation>将当前排除规则在列表中上移一个位置。</translation>
    </message>
    <message>
        <location filename="../exclude-rules-config-page.ui" line="86"/>
        <source>Create a new exclude rule.
You can create as many as you like.</source>
        <translation>创建新的排除规则。
您可以创建任意数量的排除规则。</translation>
    </message>
    <message>
        <location filename="../exclude-rules-config-page.ui" line="101"/>
        <source>Remove the current exclude rule.</source>
        <translation>删除当前的排除规则。</translation>
    </message>
    <message>
        <location filename="../exclude-rules-config-page.ui" line="128"/>
        <source>Move the current exclude rule one position down in the list.</source>
        <translation>将当前排除规则在列表中向下移动一个位置。</translation>
    </message>
    <message>
        <location filename="../exclude-rules-config-page.ui" line="142"/>
        <source>Move the current exclude rule to the bottom of the list.</source>
        <translation>将当前排除规则移至列表底部。</translation>
    </message>
    <message>
        <location filename="../exclude-rules-config-page.ui" line="162"/>
        <source>Exclude Directories With &amp;Pattern</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../exclude-rules-config-page.ui" line="188"/>
        <source>P&amp;attern Syntax</source>
        <translation>表达式语法(&amp;a)</translation>
    </message>
    <message>
        <location filename="../exclude-rules-config-page.ui" line="194"/>
        <source>Fixed &amp;String</source>
        <translation>固定字符串(&amp;S)</translation>
    </message>
    <message>
        <location filename="../exclude-rules-config-page.ui" line="201"/>
        <source>&amp;Wildcards</source>
        <translation>通配符(&amp;W)</translation>
    </message>
    <message>
        <location filename="../exclude-rules-config-page.ui" line="208"/>
        <source>&amp;Regular Expression</source>
        <translation>正则表达式(&amp;R)</translation>
    </message>
    <message>
        <location filename="../exclude-rules-config-page.ui" line="231"/>
        <source>&amp;Match Against</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../exclude-rules-config-page.ui" line="237"/>
        <source>&amp;Directory Name Without Path</source>
        <translation>不包含路径的目录名(&amp;D)</translation>
    </message>
    <message>
        <location filename="../exclude-rules-config-page.ui" line="244"/>
        <source>F&amp;ull Path</source>
        <translation>完整路径(&amp;u)</translation>
    </message>
    <message>
        <location filename="../exclude-rules-config-page.ui" line="251"/>
        <source>Any &amp;File in that Directory</source>
        <translation>该目录下的任何文件(&amp;F)</translation>
    </message>
</context>
<context>
    <name>FileAgeStatsWindow</name>
    <message>
        <location filename="../file-age-stats-window.ui" line="14"/>
        <location filename="../file-age-stats-window.ui" line="32"/>
        <source>File Age</source>
        <translation>文件时间</translation>
    </message>
    <message>
        <location filename="../file-age-stats-window.ui" line="29"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Year of the modification time of the file&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;&lt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;文件修改时间的年份&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;&lt;</translation>
    </message>
    <message>
        <location filename="../file-age-stats-window.ui" line="62"/>
        <source>&amp;Refresh</source>
        <translation>刷新(&amp;R)</translation>
    </message>
    <message>
        <location filename="../file-age-stats-window.ui" line="72"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Locate the files from that year&lt;/p&gt;&lt;p&gt;(only if no more than 1000)&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;查找该年份的文件&lt;/p&gt;&lt;p&gt;（仅限不超过 1000 个）&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../file-age-stats-window.ui" line="75"/>
        <source>&amp;Locate</source>
        <translation>定位(&amp;L)</translation>
    </message>
    <message>
        <location filename="../file-age-stats-window.ui" line="98"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;If checked: If a different directory is selected in the main window, automatically update the content of this window with data from that other directory of the main window.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;如果选中：如果在主窗口中选择了不同的目录，则使用主窗口的另一个目录中的数据自动更新此窗口的内容。&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../file-age-stats-window.ui" line="101"/>
        <source>&amp;Sync with Main Window</source>
        <translation>与主窗口同步(&amp;S)</translation>
    </message>
    <message>
        <location filename="../file-age-stats-window.ui" line="130"/>
        <source>&amp;Close</source>
        <translation>关闭(&amp;C)</translation>
    </message>
</context>
<context>
    <name>FileDetailsView</name>
    <message>
        <location filename="../file-details-view.ui" line="14"/>
        <source>Details</source>
        <translation>详情</translation>
    </message>
    <message>
        <location filename="../file-details-view.ui" line="50"/>
        <source>myfile.txt</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../file-details-view.ui" line="85"/>
        <location filename="../file-details-view.ui" line="628"/>
        <location filename="../file-details-view.ui" line="802"/>
        <source>Type:</source>
        <translation>类型:</translation>
    </message>
    <message>
        <location filename="../file-details-view.ui" line="92"/>
        <source>File</source>
        <translation>文件</translation>
    </message>
    <message>
        <location filename="../file-details-view.ui" line="104"/>
        <source>MIME Category:</source>
        <translation>MIME 类别:</translation>
    </message>
    <message>
        <location filename="../file-details-view.ui" line="111"/>
        <source>Documents</source>
        <translation>文档</translation>
    </message>
    <message>
        <location filename="../file-details-view.ui" line="123"/>
        <source>Size:</source>
        <translation>大小:</translation>
    </message>
    <message>
        <location filename="../file-details-view.ui" line="130"/>
        <location filename="../file-details-view.ui" line="275"/>
        <source>42.0 kB</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../file-details-view.ui" line="151"/>
        <source>System File</source>
        <translation>系统文件</translation>
    </message>
    <message>
        <location filename="../file-details-view.ui" line="163"/>
        <source>Package:</source>
        <translation>包:</translation>
    </message>
    <message>
        <location filename="../file-details-view.ui" line="170"/>
        <source>none</source>
        <translation>无</translation>
    </message>
    <message>
        <location filename="../file-details-view.ui" line="192"/>
        <location filename="../file-details-view.ui" line="616"/>
        <source>User:</source>
        <translation>用户:</translation>
    </message>
    <message>
        <location filename="../file-details-view.ui" line="199"/>
        <location filename="../file-details-view.ui" line="520"/>
        <source>kilroy</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../file-details-view.ui" line="211"/>
        <location filename="../file-details-view.ui" line="532"/>
        <source>Group:</source>
        <translation>组:</translation>
    </message>
    <message>
        <location filename="../file-details-view.ui" line="218"/>
        <location filename="../file-details-view.ui" line="539"/>
        <source>users</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../file-details-view.ui" line="230"/>
        <location filename="../file-details-view.ui" line="551"/>
        <source>Permissions:</source>
        <translation>权限:</translation>
    </message>
    <message>
        <location filename="../file-details-view.ui" line="237"/>
        <source>rw-r--r--  0644</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../file-details-view.ui" line="249"/>
        <location filename="../file-details-view.ui" line="499"/>
        <location filename="../file-details-view.ui" line="570"/>
        <location filename="../file-details-view.ui" line="950"/>
        <location filename="../file-details-view.ui" line="1166"/>
        <source>Last Modified:</source>
        <translation>修改时间:</translation>
    </message>
    <message>
        <location filename="../file-details-view.ui" line="256"/>
        <location filename="../file-details-view.ui" line="506"/>
        <location filename="../file-details-view.ui" line="577"/>
        <source>31.06.2018 09:18</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../file-details-view.ui" line="268"/>
        <location filename="../file-details-view.ui" line="693"/>
        <location filename="../file-details-view.ui" line="969"/>
        <location filename="../file-details-view.ui" line="1185"/>
        <location filename="../file-details-view.ui" line="1385"/>
        <source>Allocated:</source>
        <translation>占用:</translation>
    </message>
    <message>
        <location filename="../file-details-view.ui" line="282"/>
        <source>...</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../file-details-view.ui" line="294"/>
        <source>Link Target:</source>
        <translation>链接目标:</translation>
    </message>
    <message>
        <location filename="../file-details-view.ui" line="308"/>
        <source>Broken Symlink! </source>
        <translation>已断开的链接!</translation>
    </message>
    <message>
        <location filename="../file-details-view.ui" line="394"/>
        <source>doc/</source>
        <translation>项目名</translation>
    </message>
    <message>
        <location filename="../file-details-view.ui" line="435"/>
        <location filename="../file-details-view.ui" line="874"/>
        <location filename="../file-details-view.ui" line="1090"/>
        <location filename="../file-details-view.ui" line="1373"/>
        <source>Total Size:</source>
        <translation>总大小:</translation>
    </message>
    <message>
        <location filename="../file-details-view.ui" line="442"/>
        <source>4.0 kiB</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../file-details-view.ui" line="449"/>
        <source>17</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../file-details-view.ui" line="461"/>
        <location filename="../file-details-view.ui" line="912"/>
        <location filename="../file-details-view.ui" line="1128"/>
        <location filename="../file-details-view.ui" line="1309"/>
        <source>Files:</source>
        <translation>文件数:</translation>
    </message>
    <message>
        <location filename="../file-details-view.ui" line="468"/>
        <source>15</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../file-details-view.ui" line="480"/>
        <location filename="../file-details-view.ui" line="931"/>
        <location filename="../file-details-view.ui" line="1147"/>
        <source>Subdirs:</source>
        <translation>子目录数:</translation>
    </message>
    <message>
        <location filename="../file-details-view.ui" line="487"/>
        <location filename="../file-details-view.ui" line="1323"/>
        <source>2</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../file-details-view.ui" line="513"/>
        <source>Directory</source>
        <translation>目录</translation>
    </message>
    <message>
        <location filename="../file-details-view.ui" line="558"/>
        <source>rwxr-xr-x  0755</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../file-details-view.ui" line="591"/>
        <source>
Directory</source>
        <translation>
目录</translation>
    </message>
    <message>
        <location filename="../file-details-view.ui" line="604"/>
        <source>Own Size:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../file-details-view.ui" line="642"/>
        <source>
Subtree</source>
        <translation>
子树</translation>
    </message>
    <message>
        <location filename="../file-details-view.ui" line="650"/>
        <location filename="../file-details-view.ui" line="700"/>
        <source>512.0 MiB</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../file-details-view.ui" line="662"/>
        <location filename="../file-details-view.ui" line="893"/>
        <location filename="../file-details-view.ui" line="1109"/>
        <source>Items:</source>
        <translation>项目数:</translation>
    </message>
    <message>
        <location filename="../file-details-view.ui" line="767"/>
        <source>superfoomatic</source>
        <translation>项目名</translation>
    </message>
    <message>
        <location filename="../file-details-view.ui" line="809"/>
        <source>Package</source>
        <translation>包</translation>
    </message>
    <message>
        <location filename="../file-details-view.ui" line="821"/>
        <source>Version:</source>
        <translation>版本:</translation>
    </message>
    <message>
        <location filename="../file-details-view.ui" line="828"/>
        <source>42.0</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../file-details-view.ui" line="840"/>
        <source>Architecture:</source>
        <translation>架构:</translation>
    </message>
    <message>
        <location filename="../file-details-view.ui" line="847"/>
        <source>super128</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../file-details-view.ui" line="861"/>
        <location filename="../file-details-view.ui" line="1077"/>
        <source>
Installed Files</source>
        <translation>
已安装文件</translation>
    </message>
    <message>
        <location filename="../file-details-view.ui" line="881"/>
        <location filename="../file-details-view.ui" line="976"/>
        <location filename="../file-details-view.ui" line="1097"/>
        <location filename="../file-details-view.ui" line="1192"/>
        <source>240.0 MB</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../file-details-view.ui" line="900"/>
        <location filename="../file-details-view.ui" line="1116"/>
        <source>317</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../file-details-view.ui" line="919"/>
        <location filename="../file-details-view.ui" line="1135"/>
        <source>280</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../file-details-view.ui" line="938"/>
        <location filename="../file-details-view.ui" line="1154"/>
        <source>37</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../file-details-view.ui" line="957"/>
        <location filename="../file-details-view.ui" line="1173"/>
        <source>2019-04-28 16:02</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../file-details-view.ui" line="1026"/>
        <source>Packages Summary</source>
        <translation>软件包摘要</translation>
    </message>
    <message>
        <location filename="../file-details-view.ui" line="1056"/>
        <source>Packages:</source>
        <translation>包:</translation>
    </message>
    <message>
        <location filename="../file-details-view.ui" line="1063"/>
        <source>42</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../file-details-view.ui" line="1244"/>
        <source>9</source>
        <translation>项目名</translation>
    </message>
    <message>
        <location filename="../file-details-view.ui" line="1258"/>
        <source>Selected Items</source>
        <translation>已选择的项目</translation>
    </message>
    <message>
        <location filename="../file-details-view.ui" line="1316"/>
        <source>7</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../file-details-view.ui" line="1330"/>
        <location filename="../file-details-view.ui" line="1392"/>
        <source>512.0 MB</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../file-details-view.ui" line="1342"/>
        <source>Directories:</source>
        <translation>目录数:</translation>
    </message>
    <message>
        <location filename="../file-details-view.ui" line="1354"/>
        <source>Files in Subtrees:</source>
        <translation>子目录文件数:</translation>
    </message>
    <message>
        <location filename="../file-details-view.ui" line="1361"/>
        <source>84</source>
        <translation></translation>
    </message>
</context>
<context>
    <name>FileSizeStatsWindow</name>
    <message>
        <location filename="../file-size-stats-window.ui" line="14"/>
        <source>File Size Statistics</source>
        <translation>文件大小统计</translation>
    </message>
    <message>
        <location filename="../file-size-stats-window.ui" line="23"/>
        <source>File size statistics</source>
        <translation>文件大小统计</translation>
    </message>
    <message>
        <location filename="../file-size-stats-window.ui" line="37"/>
        <source>&amp;Histogram</source>
        <translation>直方图(&amp;H)</translation>
    </message>
    <message>
        <location filename="../file-size-stats-window.ui" line="48"/>
        <source>&amp;Options &gt;&gt;</source>
        <translation>选项(&amp;P) &gt;&gt;</translation>
    </message>
    <message>
        <location filename="../file-size-stats-window.ui" line="112"/>
        <source>A&amp;uto</source>
        <translation>自动(&amp;u)</translation>
    </message>
    <message>
        <location filename="../file-size-stats-window.ui" line="141"/>
        <location filename="../file-size-stats-window.ui" line="180"/>
        <source>P</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../file-size-stats-window.ui" line="151"/>
        <source>&amp;Start Percentile</source>
        <translation>开始百分比(&amp;C)</translation>
    </message>
    <message>
        <location filename="../file-size-stats-window.ui" line="161"/>
        <source>&amp;End Percentile</source>
        <translation>结束百分比(&amp;C)</translation>
    </message>
    <message>
        <location filename="../file-size-stats-window.ui" line="202"/>
        <source>&amp;Percentiles</source>
        <translation>百分比(&amp;P)</translation>
    </message>
    <message>
        <location filename="../file-size-stats-window.ui" line="227"/>
        <source>Interesting only</source>
        <translation>简略</translation>
    </message>
    <message>
        <location filename="../file-size-stats-window.ui" line="232"/>
        <source>Show all</source>
        <translation>全部显示</translation>
    </message>
    <message>
        <location filename="../file-size-stats-window.ui" line="359"/>
        <source>&amp;Buckets</source>
        <translation>桶(&amp;B)</translation>
    </message>
    <message>
        <location filename="../file-size-stats-window.ui" line="369"/>
        <source>H&amp;elp</source>
        <translation>帮助(&amp;e)</translation>
    </message>
    <message>
        <location filename="../file-size-stats-window.ui" line="805"/>
        <source>&lt;h3&gt;Statistics Basics&lt;/h3&gt;</source>
        <translation>&lt;h3&gt;基础统计信息&lt;/h3&gt;</translation>
    </message>
    <message>
        <location filename="../file-size-stats-window.ui" line="812"/>
        <source>Median, Quartiles and Percentiles</source>
        <translation>二分、四分和百分比</translation>
    </message>
    <message>
        <location filename="../file-size-stats-window.ui" line="815"/>
        <source>Basic statistics terms explained in a way that everybody can understand.</source>
        <translation>以每个人都能理解的方式解释基本统计术语。</translation>
    </message>
    <message>
        <location filename="../file-size-stats-window.ui" line="822"/>
        <source>Histograms in General</source>
        <translation>直方图概述</translation>
    </message>
    <message>
        <location filename="../file-size-stats-window.ui" line="825"/>
        <source>What is a histogram? What is it good for?</source>
        <translation>什么是直方图？它有什么用处？</translation>
    </message>
    <message>
        <location filename="../file-size-stats-window.ui" line="848"/>
        <source>&lt;h3&gt;QDirStat File Size Statistics&lt;/h3&gt;</source>
        <translation>&lt;h3&gt;QDirStat 文件大小统计&lt;/h3&gt;</translation>
    </message>
    <message>
        <location filename="../file-size-stats-window.ui" line="855"/>
        <source>The File Size Histogram</source>
        <translation>文件大小直方图</translation>
    </message>
    <message>
        <location filename="../file-size-stats-window.ui" line="858"/>
        <source>QDirStat&apos;s &quot;File Size&quot; histogram in particular: What is it? What does it tell you?</source>
        <translation>QDirStat 的“文件大小”直方图具体是什么？它能告诉你什么？</translation>
    </message>
    <message>
        <location filename="../file-size-stats-window.ui" line="865"/>
        <source>The Overflow Area</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../file-size-stats-window.ui" line="868"/>
        <source>Explaining the &quot;Cut off&quot; panel with the pie charts at the right side of the histogram.</source>
        <translation>使用直方图右侧的饼图解释“饼图”面板。</translation>
    </message>
    <message>
        <location filename="../file-size-stats-window.ui" line="875"/>
        <source>Histogram Options</source>
        <translation>直方图</translation>
    </message>
    <message>
        <location filename="../file-size-stats-window.ui" line="878"/>
        <source>Explanation of the sliders, input fields and buttons in the panel that opens when you click the &quot;Options&quot; button below the histogram.</source>
        <translation>当您单击直方图下方的“选项”按钮时，打开面板中的滑块、输入字段和按钮的说明。</translation>
    </message>
    <message>
        <location filename="../file-size-stats-window.ui" line="901"/>
        <source>The Percentiles Table</source>
        <translation>百分比</translation>
    </message>
    <message>
        <location filename="../file-size-stats-window.ui" line="904"/>
        <source>Explanation of the table on the &quot;Percentiles&quot; page in this dialog.</source>
        <translation>此对话框中“百分比”页面上的表格的解释。</translation>
    </message>
    <message>
        <location filename="../file-size-stats-window.ui" line="911"/>
        <source>The Buckets Table</source>
        <translation>桶</translation>
    </message>
    <message>
        <location filename="../file-size-stats-window.ui" line="914"/>
        <source>Explanation of the table on the &quot;Buckets&quot; table in this dialog.</source>
        <translation>此对话框中“桶”表的解释。</translation>
    </message>
    <message>
        <location filename="../file-size-stats-window.ui" line="937"/>
        <source>&lt;i&gt;All links are opened in an external browser.&lt;/i&gt;</source>
        <translation>&lt;i&gt;所有链接均在外部浏览器中打开。&lt;/i&gt;</translation>
    </message>
    <message>
        <location filename="../file-size-stats-window.ui" line="992"/>
        <source>&amp;Close</source>
        <translation>关闭(&amp;C)</translation>
    </message>
</context>
<context>
    <name>FileTypeStatsWindow</name>
    <message>
        <location filename="../file-type-stats-window.ui" line="14"/>
        <source>File Type Statistics</source>
        <translation>文件类型统计</translation>
    </message>
    <message>
        <location filename="../file-type-stats-window.ui" line="23"/>
        <source>File type statistics</source>
        <translation>文件类型统计</translation>
    </message>
    <message>
        <location filename="../file-type-stats-window.ui" line="50"/>
        <source>&amp;Refresh</source>
        <translation>刷新(&amp;R)</translation>
    </message>
    <message>
        <location filename="../file-type-stats-window.ui" line="57"/>
        <source>File &amp;Type</source>
        <translation>文件类型(&amp;T)</translation>
    </message>
    <message>
        <location filename="../file-type-stats-window.ui" line="83"/>
        <source>&amp;Close</source>
        <translation>关闭(&amp;C)</translation>
    </message>
    <message>
        <location filename="../file-type-stats-window.ui" line="92"/>
        <source>&amp;Locate</source>
        <translation>定位(&amp;L)</translation>
    </message>
    <message>
        <location filename="../file-type-stats-window.ui" line="97"/>
        <source>Si&amp;ze Statistics</source>
        <translation>大小统计(&amp;Z)</translation>
    </message>
    <message>
        <location filename="../file-type-stats-window.ui" line="100"/>
        <source>F2</source>
        <translation></translation>
    </message>
</context>
<context>
    <name>FilesystemsWindow</name>
    <message>
        <location filename="../filesystems-window.ui" line="14"/>
        <source>Filesystems</source>
        <translation>文件系统</translation>
    </message>
    <message>
        <location filename="../filesystems-window.ui" line="32"/>
        <source>&amp;Mounted Filesystems</source>
        <translation>已挂载的文件系统(&amp;M)</translation>
    </message>
    <message>
        <location filename="../filesystems-window.ui" line="78"/>
        <source>Re&amp;fresh</source>
        <translation>刷新(&amp;R)</translation>
    </message>
    <message>
        <location filename="../filesystems-window.ui" line="85"/>
        <source>&amp;Read</source>
        <translation>读取(&amp;R)</translation>
    </message>
    <message>
        <location filename="../filesystems-window.ui" line="111"/>
        <source>&amp;Close</source>
        <translation>关闭(&amp;C)</translation>
    </message>
</context>
<context>
    <name>FindFilesDialog</name>
    <message>
        <location filename="../find-files-dialog.ui" line="14"/>
        <source>Find</source>
        <translation>查找</translation>
    </message>
    <message>
        <location filename="../find-files-dialog.ui" line="51"/>
        <source>Find in the Scanned Tree</source>
        <translation>在已扫描的树状图中查找</translation>
    </message>
    <message>
        <location filename="../find-files-dialog.ui" line="91"/>
        <source>&amp;Name:</source>
        <translation>名称(&amp;N):</translation>
    </message>
    <message>
        <location filename="../find-files-dialog.ui" line="116"/>
        <source>search text or pattern</source>
        <translation>搜索文件或表达式</translation>
    </message>
    <message>
        <location filename="../find-files-dialog.ui" line="129"/>
        <source>Match &amp;Mode:</source>
        <translation>匹配模式(&amp;M):</translation>
    </message>
    <message>
        <location filename="../find-files-dialog.ui" line="158"/>
        <source>Auto</source>
        <translation>自动</translation>
    </message>
    <message>
        <location filename="../find-files-dialog.ui" line="163"/>
        <source>Contains</source>
        <translation>包含</translation>
    </message>
    <message>
        <location filename="../find-files-dialog.ui" line="168"/>
        <source>Starts with</source>
        <translation>开始于</translation>
    </message>
    <message>
        <location filename="../find-files-dialog.ui" line="173"/>
        <source>Exact match</source>
        <translation>完全匹配</translation>
    </message>
    <message>
        <location filename="../find-files-dialog.ui" line="178"/>
        <source>Wildcard</source>
        <translation>通配符</translation>
    </message>
    <message>
        <location filename="../find-files-dialog.ui" line="183"/>
        <source>Regular Expression</source>
        <translation>正则表达式</translation>
    </message>
    <message>
        <location filename="../find-files-dialog.ui" line="204"/>
        <source>Case Sensiti&amp;ve</source>
        <translation>区分大小写(&amp;V)</translation>
    </message>
    <message>
        <location filename="../find-files-dialog.ui" line="241"/>
        <source>Find &amp;Files</source>
        <translation>查找文件(&amp;F)</translation>
    </message>
    <message>
        <location filename="../find-files-dialog.ui" line="251"/>
        <source>Find &amp;Directories</source>
        <translation>查找目录(&amp;D)</translation>
    </message>
    <message>
        <location filename="../find-files-dialog.ui" line="261"/>
        <source>Find &amp;Both, Files and Directories</source>
        <translation>查找文件和目录(&amp;B)</translation>
    </message>
    <message>
        <location filename="../find-files-dialog.ui" line="281"/>
        <source>Find Symbolic &amp;Links</source>
        <translation>查找符号链接(&amp;L)</translation>
    </message>
    <message>
        <location filename="../find-files-dialog.ui" line="319"/>
        <source>In the &amp;Whole Tree</source>
        <translation>在整个树状图中(&amp;W)</translation>
    </message>
    <message>
        <location filename="../find-files-dialog.ui" line="326"/>
        <source>In the Current S&amp;ubtree</source>
        <translation>在当前子树中(&amp;U)</translation>
    </message>
    <message>
        <location filename="../find-files-dialog.ui" line="378"/>
        <source>/home/kilroy/tmp</source>
        <translation></translation>
    </message>
</context>
<context>
    <name>GeneralConfigPage</name>
    <message>
        <location filename="../general-config-page.ui" line="14"/>
        <source>Form</source>
        <translation>一般配置窗口</translation>
    </message>
    <message>
        <location filename="../general-config-page.ui" line="50"/>
        <source>Show &amp;URL in window title</source>
        <translation>窗口标题显示URL(&amp;U)</translation>
    </message>
    <message>
        <location filename="../general-config-page.ui" line="57"/>
        <source>Use ho&amp;ver effect in treemap</source>
        <translation>在树状图中使用悬停效果(&amp;V)</translation>
    </message>
    <message>
        <location filename="../general-config-page.ui" line="64"/>
        <source>&amp;Cross filesystem boundaries while reading</source>
        <translation>读取时允许跨越文件系统(&amp;C)</translation>
    </message>
    <message>
        <location filename="../general-config-page.ui" line="71"/>
        <source>Use &amp;bold font for dominant tree items</source>
        <translation>对主要树状图项目使用粗体字体(&amp;B)</translation>
    </message>
    <message>
        <location filename="../general-config-page.ui" line="78"/>
        <source>Use &amp;ISO date and time format (&quot;2024-12-28  17:38&quot;)</source>
        <translation>使用ISO标准的日期和时间格式(&quot;2024-12-28  17:38&quot;)(&amp;I)</translation>
    </message>
    <message>
        <location filename="../general-config-page.ui" line="102"/>
        <source>Tree icon &amp;theme:</source>
        <translation>树状图图标主题(&amp;T):</translation>
    </message>
    <message>
        <location filename="../general-config-page.ui" line="116"/>
        <source>Medium</source>
        <translation>中</translation>
    </message>
    <message>
        <location filename="../general-config-page.ui" line="121"/>
        <source>Small</source>
        <translation>小</translation>
    </message>
    <message>
        <location filename="../general-config-page.ui" line="142"/>
        <source>Tree update &amp;interval while reading:</source>
        <translation>读取时树状图的更新时间间隔(&amp;i):</translation>
    </message>
    <message>
        <location filename="../general-config-page.ui" line="152"/>
        <source> millisec</source>
        <translation> 毫秒</translation>
    </message>
    <message>
        <location filename="../general-config-page.ui" line="181"/>
        <source>Timeout for status bar &amp;messages:</source>
        <translation>状态栏消息提示超时时间(&amp;M):</translation>
    </message>
    <message>
        <location filename="../general-config-page.ui" line="191"/>
        <source> sec</source>
        <translation> 秒</translation>
    </message>
    <message>
        <location filename="../general-config-page.ui" line="243"/>
        <source>Most options on this page take effect only after restarting the program.</source>
        <translation>此页面上的大多数选项只有重新启动程序后才会生效。</translation>
    </message>
</context>
<context>
    <name>LocateFileTypeWindow</name>
    <message>
        <location filename="../locate-file-type-window.ui" line="14"/>
        <source>Locate Files by Type</source>
        <translation>通过类型定位文件</translation>
    </message>
    <message>
        <location filename="../locate-file-type-window.ui" line="29"/>
        <source>Directories with File Type</source>
        <translation>目录及文件类型</translation>
    </message>
    <message>
        <location filename="../locate-file-type-window.ui" line="59"/>
        <source>&amp;Refresh</source>
        <translation>刷新(&amp;R)</translation>
    </message>
    <message>
        <location filename="../locate-file-type-window.ui" line="85"/>
        <source>&amp;Close</source>
        <translation>关闭(&amp;C)</translation>
    </message>
</context>
<context>
    <name>LocateFilesWindow</name>
    <message>
        <location filename="../locate-files-window.ui" line="14"/>
        <location filename="../locate-files-window.ui" line="29"/>
        <source>Locate Files</source>
        <translation>定位文件</translation>
    </message>
    <message>
        <location filename="../locate-files-window.ui" line="59"/>
        <source>&amp;Refresh</source>
        <translation>刷新(&amp;R)</translation>
    </message>
    <message>
        <location filename="../locate-files-window.ui" line="82"/>
        <source>42 Results</source>
        <translation>42 条结果</translation>
    </message>
    <message>
        <location filename="../locate-files-window.ui" line="108"/>
        <source>&amp;Close</source>
        <translation>关闭(&amp;C)</translation>
    </message>
</context>
<context>
    <name>MainWindow</name>
    <message>
        <location filename="../main-window.ui" line="15"/>
        <source>QDirStat</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="69"/>
        <source>Bookmark / Un-Bookmark</source>
        <translation>添加/删除书签</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="83"/>
        <source>Ctrl+D</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="96"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;/work/src/&lt;a href=&quot;/work/src/qdirstat&quot;&gt;qdirstat&lt;/a&gt;/&lt;a href=&quot;/work/src/qdirstat/src&quot;&gt;src&lt;/a&gt;/&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="181"/>
        <source>&amp;File</source>
        <translation>文件(&amp;F)</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="201"/>
        <source>&amp;View</source>
        <translation>视图(&amp;V)</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="205"/>
        <source>E&amp;xpand Tree to Level</source>
        <translation>展开树状图级别到(&amp;x)...</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="216"/>
        <source>Tree&amp;map</source>
        <translation>树状图(&amp;m)</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="241"/>
        <source>&amp;Edit</source>
        <translation>编辑(&amp;E)</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="251"/>
        <source>&amp;Go</source>
        <translation>转到(&amp;G)</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="258"/>
        <source>To &amp;Bookmark</source>
        <translation>转到书签(&amp;B)</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="274"/>
        <source>&amp;Help</source>
        <translation>帮助(&amp;H)</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="278"/>
        <source>Problems and &amp;Solutions</source>
        <translation>问题与解决方案(&amp;S)</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="303"/>
        <source>&amp;Clean Up</source>
        <translation>清理(&amp;C)</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="310"/>
        <source>&amp;Discover</source>
        <translation>搜寻(&amp;D)</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="330"/>
        <source>Main Toolbar</source>
        <translation>主工具栏</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="360"/>
        <source>&amp;Quit</source>
        <translation>退出(&amp;E)</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="363"/>
        <source>Exit application</source>
        <translation>退出应用</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="366"/>
        <source>Ctrl+Q</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="375"/>
        <source>&amp;Open Directory...</source>
        <translation>打开目录(&amp;O)...</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="378"/>
        <source>Open a directory to scan.</source>
        <translation>打开要扫描的目录。</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="381"/>
        <source>Ctrl+O</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="386"/>
        <source>&amp;Close All Tree Levels</source>
        <translation>关闭所有树状图层级(&amp;C)</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="389"/>
        <source>Close all branches of the directory tree.</source>
        <translation>关闭目录树的所有分支。</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="394"/>
        <source>Level &amp;0</source>
        <translation>第 0 级(&amp;0)</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="397"/>
        <source>Expand (open) the directory tree to level 0.</source>
        <translation>展开目录树状图到第 0 级。</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="402"/>
        <source>Level &amp;1</source>
        <translation>第 1 级(&amp;1)</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="405"/>
        <source>Expand (open) the directory tree to level 1.</source>
        <translation>展开目录树状图到第 1 级。</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="408"/>
        <source>Ctrl+1</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="413"/>
        <source>Level &amp;2</source>
        <translation>第 2 级(&amp;2)</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="416"/>
        <source>Expand (open) the directory tree to level 2.</source>
        <translation>展开目录树状图到第 2 级。</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="419"/>
        <source>Ctrl+2</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="424"/>
        <source>Level &amp;3</source>
        <translation>第 3 级(&amp;3)</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="427"/>
        <source>Expand (open) the directory tree to level 3.</source>
        <translation>展开目录树状图到第 3 级。</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="430"/>
        <source>Ctrl+3</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="435"/>
        <source>Level &amp;4</source>
        <translation>第 4 级(&amp;4)</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="438"/>
        <source>Expand (open) the directory tree to level 4.</source>
        <translation>展开目录树状图到第 4 级。</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="441"/>
        <source>Ctrl+4</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="446"/>
        <source>Level &amp;5</source>
        <translation>第 5 级(&amp;5)</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="449"/>
        <source>Expand (open) the directory tree to level 5.</source>
        <translation>展开目录树状图到第 5 级。</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="452"/>
        <source>Ctrl+5</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="457"/>
        <source>Level &amp;6</source>
        <translation>第 6 级(&amp;6)</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="460"/>
        <source>Expand (open) the directory tree to level 6.</source>
        <translation>展开目录树状图到第 6 级。</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="463"/>
        <source>Ctrl+6</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="468"/>
        <source>Level &amp;7</source>
        <translation>第 7 级(&amp;7)</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="471"/>
        <source>Expand (open) the directory tree to level 7.</source>
        <translation>展开目录树状图到第 7 级。</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="474"/>
        <source>Ctrl+7</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="479"/>
        <source>Level &amp;8</source>
        <translation>第 8 级(&amp;8)</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="482"/>
        <source>Expand (open) the directory tree to level 8.</source>
        <translation>展开目录树状图到第 8 级。</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="485"/>
        <source>Ctrl+8</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="490"/>
        <source>Level &amp;9</source>
        <translation>第 9 级(&amp;9)</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="493"/>
        <source>Expand (open) the directory tree to level 9.</source>
        <translation>展开目录树状图到第 9 级。</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="496"/>
        <source>Ctrl+9</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="505"/>
        <source>&amp;Stop Reading</source>
        <translation>停止读取(&amp;S)</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="508"/>
        <source>Stop reading directories.</source>
        <translation>停止读取目录。</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="511"/>
        <source>Ctrl+S</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="520"/>
        <source>&amp;Write To Cache File...</source>
        <translation>写到缓存文件(&amp;W)...</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="523"/>
        <source>Write the current directory tree to a cache file.</source>
        <translation>写当前目录树到缓存文件。</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="532"/>
        <source>&amp;Read Cache File...</source>
        <translation>读取缓存文件(&amp;R)...</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="535"/>
        <source>Read a directory tree from a cache file.</source>
        <translation>从缓存文件读取目录树。</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="544"/>
        <source>Refresh &amp;All</source>
        <translation>全部刷新(&amp;A)</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="547"/>
        <source>Re-read the entire directory tree from disk.</source>
        <translation>从磁盘重新读取整个目录树。</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="550"/>
        <source>F5</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="559"/>
        <source>&amp;Copy Path</source>
        <translation>复制路径(&amp;C)</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="562"/>
        <source>Copy the full path of the current item to the system clipboard for use in other applications.</source>
        <translation>将当前项目的完整路径复制到系统剪贴板以供其他应用程序使用。</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="565"/>
        <source>Ctrl+C</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="574"/>
        <source>Zoom &amp;In</source>
        <translation>放大(&amp;I)</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="577"/>
        <source>Zoom the treemap in (enlarge) one level.</source>
        <translation>将树状图放大一级。</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="580"/>
        <source>Ctrl++</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="589"/>
        <source>Zoom &amp;Out</source>
        <translation>缩小(&amp;O)</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="592"/>
        <source>Zoom the treemap out one level.</source>
        <translation>将树状图缩小一级。</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="595"/>
        <source>Ctrl+-</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="604"/>
        <source>&amp;Reset Zoom</source>
        <translation>重置缩放(&amp;R)</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="607"/>
        <source>Reset the treemap zoom factor to normal.</source>
        <translation>将树状图缩放比例重置为正常。</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="610"/>
        <source>Ctrl+0</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="615"/>
        <source>Re&amp;build</source>
        <translation>重建(&amp;b)</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="618"/>
        <source>Rebuild the treemap.</source>
        <translation>重建树状图。</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="629"/>
        <source>&amp;Show Treemap</source>
        <translation>显示树状图(&amp;S)</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="632"/>
        <source>Enable or disable showing the treemap view.</source>
        <translation>启用或禁用显示树状图视图。</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="635"/>
        <source>F9</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="644"/>
        <source>&amp;Up One Level</source>
        <translation>向上一级(&amp;U)</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="647"/>
        <source>Up One Level</source>
        <translation>向上一级</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="650"/>
        <source>Alt+Up</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="659"/>
        <source>To Tople&amp;vel</source>
        <translation>转到顶级(&amp;v)</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="662"/>
        <source>Navigate to the toplevel directory of this tree.</source>
        <translation>导航到此树状图的顶层目录。</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="665"/>
        <source>Alt+Home</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="674"/>
        <source>A&amp;bout QDirStat...</source>
        <translation>关于 QDirStat(&amp;A)...</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="677"/>
        <location filename="../MainWindowHelp.cpp" line="47"/>
        <source>About QDirStat</source>
        <translation>关于 QDirStat</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="682"/>
        <source>About &amp;Qt...</source>
        <translation>关于 Qt(&amp;Q)...</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="685"/>
        <source>About Qt...</source>
        <translation>关于 Qt...</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="688"/>
        <source>About Qt</source>
        <translation>关于 Qt</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="693"/>
        <source>Re&amp;fresh Selected</source>
        <translation>刷新所选(&amp;f)</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="696"/>
        <source>Reread the selected branch from disk.</source>
        <translation>从磁盘重新读取选定的分支。</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="699"/>
        <source>F6</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="704"/>
        <source>Read &amp;Excluded Directory</source>
        <translation>读取排除目录(&amp;E)</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="707"/>
        <source>Read a directory that was excluded by an exclude rule.</source>
        <translation>读取被排除规则排除的目录。</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="716"/>
        <source>Continue Reading at &amp;Mount Point</source>
        <translation>在挂载点继续读取(&amp;M)</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="719"/>
        <source>Continue Reading at Mount Point</source>
        <translation>在挂载点继续读取</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="728"/>
        <source>C&amp;onfigure QDirStat</source>
        <translation>QDirStat 配置(&amp;o)</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="731"/>
        <source>Configure QDirStat</source>
        <translation>QDirStat 配置</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="739"/>
        <source>Verbose Selection</source>
        <translation>详细选择</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="742"/>
        <source>Switch verbose logging of selecting and unselecting items on or off.</source>
        <translation>打开或关闭选择和取消选择项目的详细日志记录。</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="745"/>
        <source>Shift+F7</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="754"/>
        <source>Move to T&amp;rash</source>
        <translation>删除到回收站(&amp;R)</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="757"/>
        <source>Move the selected items to the trash bin.</source>
        <translation>将选定的项目移至回收站。</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="760"/>
        <source>Del</source>
        <translation>删除</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="765"/>
        <source>Dump Selection to Log</source>
        <translation>将选定内容转储到日志</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="768"/>
        <source>F7</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="773"/>
        <source>File &amp;Type Statistics</source>
        <translation>文件类型统计信息(&amp;T)</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="776"/>
        <source>File Type Statistics</source>
        <translation>文件类型统计信息</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="779"/>
        <source>F3</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="784"/>
        <source>QDirStat &amp;Help</source>
        <translation>QDirStat 帮助(&amp;H)</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="787"/>
        <location filename="../main-window.ui" line="790"/>
        <source>QDirStat Help</source>
        <translation>QDirStat 帮助</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="793"/>
        <source>https://github.com/shundhammer/qdirstat/blob/master/README.md</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="796"/>
        <source>F1</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="801"/>
        <source>File &amp;Size Statistics</source>
        <translation>文件大小统计(&amp;S)</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="804"/>
        <source>F2</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="815"/>
        <location filename="../main-window.ui" line="818"/>
        <source>Show &amp;Details Panel</source>
        <translation>显示详情面板(&amp;D)</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="826"/>
        <source>Layout &amp;1 (Short)</source>
        <translation>样式一(Short)(&amp;1)</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="829"/>
        <source>L1</source>
        <translation>样式一</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="832"/>
        <source>Alt+1</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="843"/>
        <source>Layout &amp;2 (Classic)</source>
        <translation>样式二(Classic)(&amp;2)</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="846"/>
        <source>L2</source>
        <translation>样式二</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="849"/>
        <source>Alt+2</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="857"/>
        <source>Layout &amp;3 (Full)</source>
        <translation>样式三(Full)(&amp;3)</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="860"/>
        <source>L3</source>
        <translation>样式三</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="863"/>
        <source>Alt+3</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="868"/>
        <source>&amp;Donate...</source>
        <translation>捐赠(&amp;D)...</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="874"/>
        <source>Make a donation to support the QDirStat project</source>
        <translation>捐款支持 QDirStat 项目</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="879"/>
        <source>What&apos;s &amp;New in this Release...</source>
        <translation>此版本中的新功能(&amp;N)...</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="882"/>
        <source>Show release notes in external web browser</source>
        <translation>在外部 Web 浏览器中显示发行说明</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="892"/>
        <source>Show Installed &amp;Packages...</source>
        <translation>显示已安装的软件包(&amp;P)...</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="895"/>
        <source>Show Installed Packages</source>
        <translation>显示已安装的软件包</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="898"/>
        <source>Ctrl+P</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="903"/>
        <source>The &amp;Packages View...</source>
        <translation>包视图(&amp;P)...</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="906"/>
        <source>The Packages View...</source>
        <translation>包视图...</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="909"/>
        <source>The Packages View</source>
        <translation>包视图</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="912"/>
        <source>https://github.com/shundhammer/qdirstat/blob/master/doc/Pkg-View.md</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="917"/>
        <source>Show &amp;Unpackaged Files...</source>
        <translation>显示已解包的文件(&amp;U)...</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="920"/>
        <source>Show only files that do NOT belong to an installed software package</source>
        <translation>仅显示不属于已安装软件包的文件</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="923"/>
        <source>Ctrl+U</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="928"/>
        <source>The &amp;Unpackaged Files View...</source>
        <translation>已解包文件视图(&amp;U)...</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="931"/>
        <source>The Unpackaged Files View...</source>
        <translation>已解包文件视图...</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="934"/>
        <source>The Unpackaged Files View</source>
        <translation>已解包文件视图</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="937"/>
        <source>https://github.com/shundhammer/qdirstat/blob/master/doc/Unpkg-View.md</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="942"/>
        <source>Show Mounted &amp;Filesystems</source>
        <translation>显示已挂载的文件系统(&amp;F)</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="945"/>
        <source>Ctrl+M</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="950"/>
        <source>&amp;Largest Files</source>
        <translation>最大文件(&amp;L)</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="953"/>
        <source>Largest Files</source>
        <translation>最大文件</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="958"/>
        <source>&amp;Newest Files</source>
        <translation>最新文件(&amp;N)</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="961"/>
        <source>Newest Files</source>
        <translation>最新文件</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="966"/>
        <source>&amp;Oldest Files</source>
        <translation>最旧文件(&amp;O)</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="969"/>
        <source>Oldest Files</source>
        <translation>最旧文件</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="974"/>
        <source>Files with Multiple &amp;Hard Links</source>
        <translation>具有多个硬链接的文件(&amp;H)</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="977"/>
        <source>Files with Multiple Hard Links</source>
        <translation>具有多个硬链接的文件</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="982"/>
        <source>&amp;Broken Symbolic LInks</source>
        <translation>损坏的符号链接(&amp;B)</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="985"/>
        <source>Broken Symbolic LInks</source>
        <translation>损坏的符号链接</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="990"/>
        <source>&amp;Sparse Files</source>
        <translation>稀疏文件(&amp;S)</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="993"/>
        <source>Sparse Files</source>
        <translation>稀疏文件</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="998"/>
        <source>&amp;Btrfs Size Reporting...</source>
        <translation>Btrfs 大小报告(&amp;B)...</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="1001"/>
        <source>https://github.com/shundhammer/qdirstat/blob/master/doc/Btrfs-Free-Size.md</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="1006"/>
        <source>Files Shadowed by a &amp;Mount...</source>
        <translation>被挂载操作覆盖的文件(&amp;S)...</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="1009"/>
        <source>Files Shadowed by a Mount...</source>
        <translation>被挂载操作覆盖的文件...</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="1012"/>
        <source>Files Shadowed by a Mount</source>
        <translation>被挂载操作覆盖的文件</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="1015"/>
        <source>https://github.com/shundhammer/qdirstat/blob/master/doc/Shadowed-by-Mount.md</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="1020"/>
        <source>QDirStat for &amp;Headless Servers...</source>
        <translation>无头服务器的 QDirStat(&amp;H)...</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="1023"/>
        <source>QDirStat for Headless Servers...</source>
        <translation>无头服务器的 QDirStat...</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="1026"/>
        <source>QDirStat for Headless Servers</source>
        <translation>无头服务器的 QDirStat</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="1029"/>
        <source>https://github.com/shundhammer/qdirstat/blob/master/doc/QDirStat-for-Servers.md</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="1034"/>
        <source>Can&apos;t Move a Directory to &amp;Trash...</source>
        <translation>无法将目录移动到回收站(&amp;T)...</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="1037"/>
        <source>https://github.com/shundhammer/qdirstat/blob/master/doc/Troubleshooting.md#cant-move-a-directory-to-trash</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="1042"/>
        <source>File &amp;Age Statistics</source>
        <translation>文件时间统计(&amp;A)</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="1045"/>
        <source>F4</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="1050"/>
        <source>Files from &amp;Year</source>
        <translation>来自某年的文件(&amp;Y)</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="1059"/>
        <source>&amp;Back</source>
        <translation>返回(&amp;B)</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="1062"/>
        <source>Go back to the previous directory</source>
        <translation>返回前一个目录</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="1065"/>
        <source>Alt+Left</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="1074"/>
        <source>&amp;Forward</source>
        <translation>前进(&amp;F)</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="1077"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Go forward again to the next directory (after going back).&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;再次前进到下一个目录（返回后）。&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="1080"/>
        <source>Alt+Right</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="1085"/>
        <source>The File &amp;Age Statistics...</source>
        <translation>文件时间统计(&amp;A)...</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="1088"/>
        <source>https://github.com/shundhammer/qdirstat/blob/master/doc/File-Age-Stats.md</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="1093"/>
        <source>&amp;Find...</source>
        <translation>查找(&amp;F)...</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="1096"/>
        <source>Find Files or Directories in the Scanned Tree</source>
        <translation>在扫描树中查找文件或目录</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="1099"/>
        <source>Ctrl+F</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="1104"/>
        <source>Select &amp;All</source>
        <translation>全选(&amp;A)</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="1107"/>
        <source>Select all Bookmarks in the Tree</source>
        <translation>选择树中的所有书签</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="1112"/>
        <source>The &amp;Treemap Graphics...</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="1115"/>
        <source>The colored graphics in the main window explained</source>
        <translation>主窗口中的彩色图形说明</translation>
    </message>
    <message>
        <location filename="../main-window.ui" line="1118"/>
        <source>https://github.com/shundhammer/qdirstat/blob/master/doc/Treemap.md</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="525"/>
        <source>Finished. Elapsed time: %1</source>
        <translation>已完成。耗时：%1</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="544"/>
        <source>Aborted. Elapsed time: %1</source>
        <translation>已中断，耗时:%1</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="596"/>
        <location filename="../MainWindow.cpp" line="807"/>
        <location filename="../MainWindow.cpp" line="842"/>
        <location filename="../MainWindow.cpp" line="945"/>
        <source>Error</source>
        <translation>错误</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="597"/>
        <source>Could not open directory %1</source>
        <translation>无法打开目录 %1</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="624"/>
        <source>Following symbolic link</source>
        <translation>跟随符号链接</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="625"/>
        <source>%1 is a symbolic link to %2</source>
        <translation>%1 是 %2 的符号链接</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="638"/>
        <source>Broken symbolic link</source>
        <translation>损坏的符号链接</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="639"/>
        <source>%1 is a broken symbolic link to %2</source>
        <translation>%1 是指向 %2 的损坏符号链接</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="662"/>
        <source>Select directory to scan</source>
        <translation>选择要扫描的目录</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="709"/>
        <source>Reading package database...</source>
        <translation>正在读取包数据库...</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="790"/>
        <source>Reading aborted.</source>
        <translation>读取已中断。</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="808"/>
        <source>Can&apos;t read cache file &quot;%1&quot;</source>
        <translation>无法读取缓存文件“%1”</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="817"/>
        <source>Select QDirStat cache file</source>
        <translation>选择 QDirStat 缓存文件</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="829"/>
        <source>Enter name for QDirStat cache file</source>
        <translation>输入 QDirStat 缓存文件的名称</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="837"/>
        <source>Directory tree written to file %1</source>
        <translation>目录树已写入文件 %1</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="843"/>
        <source>ERROR writing cache file &quot;%1&quot;</source>
        <translation>写入缓存文件“%1”时出错</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="854"/>
        <source> [root]</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="871"/>
        <source>Reading... %1</source>
        <translation>正在读取...%1</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="886"/>
        <source>  [Permission Denied]</source>
        <translation>  [没有权限]</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="888"/>
        <source>  [Read Error]</source>
        <translation>  [读取错误]</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="910"/>
        <source>%1 items selected (%2 total)</source>
        <translation>已选择 %1 个项目（共 %2 个）</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="925"/>
        <source>Starting cleanup action %1</source>
        <translation>开始清理操作 %1</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="937"/>
        <source>Cleanup action finished successfully.</source>
        <translation>清理操作已成功完成。</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="939"/>
        <source>Cleanup action finished with %1 errors.</source>
        <translation>清理操作已完成，但出现 %1 个错误。</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="945"/>
        <source>Not implemented!</source>
        <translation>未实现！</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="958"/>
        <source>Copied to system clipboard: %1</source>
        <translation>已复制到系统剪贴板：%1</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="962"/>
        <source>No current item</source>
        <translation>没有当前项目</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="1095"/>
        <source>Bookmarked %1</source>
        <translation>已添加书签 %1</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="1101"/>
        <source>Un-bookmarked %1</source>
        <translation>已删除书签 %1</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="1144"/>
        <source>Moved to trash: %1</source>
        <translation>移至回收站：%1</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="1146"/>
        <source>Move to trash failed for %1</source>
        <translation>将 %1 移至回收站失败</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="1243"/>
        <source>Some directories could not be read.</source>
        <translation>部分目录无法读取。</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="1244"/>
        <source>You might not have sufficient permissions.</source>
        <translation>您可能没有足够的权限。</translation>
    </message>
    <message>
        <location filename="../MainWindowHelp.cpp" line="27"/>
        <source>Qt-based directory statistics -- showing where all your disk space has gone  and trying to help you to clean it up.</source>
        <translation>基于 Qt 的目录统计——显示所有磁盘空间的去向并尝试帮助您清理它。</translation>
    </message>
    <message>
        <location filename="../MainWindowHelp.cpp" line="32"/>
        <source>Contact: </source>
        <translation>联系： </translation>
    </message>
    <message>
        <location filename="../MainWindowHelp.cpp" line="35"/>
        <source>License: GPL V2 (GNU General Public License Version 2)</source>
        <translation>许可证：GPL V2（GNU 通用公共许可证版本 2）</translation>
    </message>
    <message>
        <location filename="../MainWindowHelp.cpp" line="37"/>
        <source>This is free Open Source software, provided to you hoping that it might be useful for you. It does not cost you anything, but on the other hand there is no warranty or promise of anything.</source>
        <translation>这是免费的开源软件，希望对您有所帮助。它无需您支付任何费用，但同时也不提供任何担保或承诺。</translation>
    </message>
    <message>
        <location filename="../MainWindowHelp.cpp" line="41"/>
        <source>This software was made with the best intentions and greatest care, but still there is the off chance that something might go wrong which might damage data on your computer. Under no circumstances will the authors of this program be held responsible for anything like that. Use this program at your own risk.</source>
        <translation>本软件的开发初衷是尽善尽美，精心设计，但仍存在一定的风险，可能会出现错误，损坏您计算机上的数据。在任何情况下，本程序的作者均不承担任何责任。使用本程序的风险由您自行承担。</translation>
    </message>
    <message>
        <location filename="../MainWindowHelp.cpp" line="57"/>
        <source>QDirStat is Free Open Source Software.</source>
        <translation>QDirStat 是开源免费的软件。</translation>
    </message>
    <message>
        <location filename="../MainWindowHelp.cpp" line="59"/>
        <source>If you find it useful, please consider donating.
</source>
        <translation>如果您发现它有用，请考虑捐赠。
</translation>
    </message>
    <message>
        <location filename="../MainWindowHelp.cpp" line="60"/>
        <source>You can donate any amount of your choice:</source>
        <translation>您可以捐赠任意金额：</translation>
    </message>
    <message>
        <location filename="../MainWindowHelp.cpp" line="67"/>
        <source>(external browser window)</source>
        <translation>（外部浏览器窗口）</translation>
    </message>
    <message>
        <location filename="../MainWindowHelp.cpp" line="71"/>
        <source>Donate</source>
        <translation>捐赠</translation>
    </message>
    <message>
        <location filename="../MainWindowUnpkg.cpp" line="64"/>
        <source>Reading file lists...</source>
        <translation>正在读取文件列表...</translation>
    </message>
</context>
<context>
    <name>MessagePanel</name>
    <message>
        <location filename="../message-panel.ui" line="20"/>
        <source>Form</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../message-panel.ui" line="38"/>
        <source>Dummy Placeholder</source>
        <translation></translation>
    </message>
</context>
<context>
    <name>MimeCategoryConfigPage</name>
    <message>
        <location filename="../mime-category-config-page.ui" line="14"/>
        <source>Form</source>
        <translation>MIME 类别配置窗口</translation>
    </message>
    <message>
        <location filename="../mime-category-config-page.ui" line="43"/>
        <source>MIME C&amp;ategory</source>
        <translation>MIME 类别(&amp;C)</translation>
    </message>
    <message>
        <location filename="../mime-category-config-page.ui" line="72"/>
        <source>Edit the name of the current category.</source>
        <translation>编辑当前类别的名称。</translation>
    </message>
    <message>
        <location filename="../mime-category-config-page.ui" line="84"/>
        <source>Create a new category.</source>
        <translation>创建新类别。</translation>
    </message>
    <message>
        <location filename="../mime-category-config-page.ui" line="87"/>
        <location filename="../mime-category-config-page.ui" line="101"/>
        <source>...</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../mime-category-config-page.ui" line="98"/>
        <source>Remove the current category.</source>
        <translation>删除当前类别。</translation>
    </message>
    <message>
        <location filename="../mime-category-config-page.ui" line="131"/>
        <source>Select a color for the current category.</source>
        <translation>为当前类别选择一种颜色。</translation>
    </message>
    <message>
        <location filename="../mime-category-config-page.ui" line="134"/>
        <source>C&amp;olor</source>
        <translation>颜色(&amp;O)</translation>
    </message>
    <message>
        <location filename="../mime-category-config-page.ui" line="147"/>
        <source>Enter color RGB code directly.</source>
        <translation>直接输入颜色RGB代码。</translation>
    </message>
    <message>
        <location filename="../mime-category-config-page.ui" line="150"/>
        <source>#RRGGBB</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../mime-category-config-page.ui" line="172"/>
        <source>Preview Treemap</source>
        <translation>预览树状图</translation>
    </message>
    <message>
        <location filename="../mime-category-config-page.ui" line="197"/>
        <source>&amp;Patterns (Case Insensitive)</source>
        <translation>模式（不区分大小写）(&amp;P)</translation>
    </message>
    <message>
        <location filename="../mime-category-config-page.ui" line="213"/>
        <location filename="../mime-category-config-page.ui" line="242"/>
        <source>Add filename patterns here,
each one on a separate line.
Use * or ? as wildcards.</source>
        <translation>在此处添加文件名模式，
每个模式占一行。
使用 * 或 ? 作为通配符。</translation>
    </message>
    <message>
        <location filename="../mime-category-config-page.ui" line="226"/>
        <source>Patterns (Case &amp;Sensitive)</source>
        <translation>模式（区分大小写）(&amp;S)</translation>
    </message>
</context>
<context>
    <name>OpenDirDialog</name>
    <message>
        <location filename="../open-dir-dialog.ui" line="14"/>
        <source>Select Directory</source>
        <translation>选择要读取的目录</translation>
    </message>
    <message>
        <location filename="../open-dir-dialog.ui" line="35"/>
        <source>Open Directory to Read</source>
        <translation>打开要读取的目录</translation>
    </message>
    <message>
        <location filename="../open-dir-dialog.ui" line="80"/>
        <source>Places and
&amp;Mounted Filesystems</source>
        <translation>位置和已挂载的文件系统(&amp;M)</translation>
    </message>
    <message>
        <location filename="../open-dir-dialog.ui" line="106"/>
        <source>
&amp;Path</source>
        <translation>
目录(&amp;P)</translation>
    </message>
    <message>
        <location filename="../open-dir-dialog.ui" line="130"/>
        <source>Up</source>
        <translation>转到上一级</translation>
    </message>
    <message>
        <location filename="../open-dir-dialog.ui" line="189"/>
        <source>Normal mounts only,
no bind mounts,
no multiple mounts,
no network mounts
(NFS, Samba)

This is temporary
just for this program run.</source>
        <translation>仅限普通挂载，
不支持绑定挂载，
不支持多重挂载，
不支持网络挂载
（NFS、Samba）

此设置仅供本程序运行，
仅供临时使用。</translation>
    </message>
    <message>
        <location filename="../open-dir-dialog.ui" line="199"/>
        <source>Cross &amp;Filesystems</source>
        <translation>跨文件系统(&amp;F)</translation>
    </message>
</context>
<context>
    <name>OpenPkgDialog</name>
    <message>
        <location filename="../open-pkg-dialog.ui" line="14"/>
        <source>Select Packages</source>
        <translation>选择包</translation>
    </message>
    <message>
        <location filename="../open-pkg-dialog.ui" line="44"/>
        <source>Open Installed Packages</source>
        <translation>打开已安装的包</translation>
    </message>
    <message>
        <location filename="../open-pkg-dialog.ui" line="67"/>
        <source>Show Packages Mat&amp;ching...</source>
        <translation>显示匹配的包(&amp;C)...</translation>
    </message>
    <message>
        <location filename="../open-pkg-dialog.ui" line="118"/>
        <source>&amp;Pkg:/</source>
        <translation>包名(&amp;P):/</translation>
    </message>
    <message>
        <location filename="../open-pkg-dialog.ui" line="128"/>
        <source>pkgname</source>
        <translation>包名</translation>
    </message>
    <message>
        <location filename="../open-pkg-dialog.ui" line="145"/>
        <source>Match &amp;Mode:</source>
        <translation>匹配模式(&amp;M):</translation>
    </message>
    <message>
        <location filename="../open-pkg-dialog.ui" line="165"/>
        <source>Auto</source>
        <translation>自动</translation>
    </message>
    <message>
        <location filename="../open-pkg-dialog.ui" line="170"/>
        <source>Contains</source>
        <translation>包含</translation>
    </message>
    <message>
        <location filename="../open-pkg-dialog.ui" line="175"/>
        <source>Starts with</source>
        <translation>开始于</translation>
    </message>
    <message>
        <location filename="../open-pkg-dialog.ui" line="180"/>
        <source>Exact match</source>
        <translation>完全匹配</translation>
    </message>
    <message>
        <location filename="../open-pkg-dialog.ui" line="185"/>
        <source>Wildcard</source>
        <translation>通配符</translation>
    </message>
    <message>
        <location filename="../open-pkg-dialog.ui" line="190"/>
        <source>Regular Expression</source>
        <translation>正则表达式</translation>
    </message>
    <message>
        <location filename="../open-pkg-dialog.ui" line="221"/>
        <source>&amp;All Installed Packages</source>
        <translation>所有已安装的软件包(&amp;A)</translation>
    </message>
</context>
<context>
    <name>OutputWindow</name>
    <message>
        <location filename="../output-window.ui" line="14"/>
        <source>Cleanup Output</source>
        <translation>清空输出</translation>
    </message>
    <message>
        <location filename="../output-window.ui" line="107"/>
        <source>&lt;!DOCTYPE HTML PUBLIC &quot;-//W3C//DTD HTML 4.0//EN&quot; &quot;http://www.w3.org/TR/REC-html40/strict.dtd&quot;&gt;
&lt;html&gt;&lt;head&gt;&lt;meta name=&quot;qrichtext&quot; content=&quot;1&quot; /&gt;&lt;style type=&quot;text/css&quot;&gt;
p, li { white-space: pre-wrap; }
&lt;/style&gt;&lt;/head&gt;&lt;body style=&quot; font-family:&apos;DejaVu Sans Mono&apos;; font-size:10pt; font-weight:400; font-style:normal;&quot;&gt;
&lt;p style=&quot; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;This is where output of the cleanup action will appear.&lt;/p&gt;
&lt;p style=&quot;-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;&lt;br /&gt;&lt;/p&gt;
&lt;p style=&quot; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;&lt;span style=&quot; color:#ffffff;&quot;&gt;cd /some/where/on/the/disk&lt;/span&gt;&lt;/p&gt;
&lt;p style=&quot; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;&lt;span style=&quot; color:#ffffff;&quot;&gt;cleanup --force junkfile&lt;/span&gt;&lt;/p&gt;
&lt;p style=&quot; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;&lt;span style=&quot; font-weight:600; color:#aa0000;&quot;&gt;This is an error message.&lt;/span&gt;&lt;/p&gt;
&lt;p style=&quot; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;This is normal program output.&lt;/p&gt;
&lt;p style=&quot;-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;&lt;br /&gt;&lt;/p&gt;
&lt;p style=&quot; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;12345678901234567890123456789012345678901234567890123456789012345678901234567890&lt;/p&gt;
&lt;p style=&quot; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;         10        20        30        40        50        60        70       80&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished">&lt;!DOCTYPE HTML PUBLIC &quot;-//W3C//DTD HTML 4.0//EN&quot; &quot;http://www.w3.org/TR/REC-html40/strict.dtd&quot;&gt;
&lt;html&gt;&lt;head&gt;&lt;meta name=&quot;qrichtext&quot; content=&quot;1&quot; /&gt;&lt;style type=&quot;text/css&quot;&gt;
p, li { white-space: pre-wrap; }
&lt;/style&gt;&lt;/head&gt;&lt;body style=&quot; font-family:&apos;DejaVu Sans Mono&apos;; font-size:10pt; font-weight:400; font-style:normal;&quot;&gt;
&lt;p style=&quot; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;清理操作的输出将在此处显示。&lt;/p&gt;
&lt;p style=&quot;-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;&lt;br /&gt;&lt;/p&gt;
&lt;p style=&quot; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;&lt;span style=&quot; color:#ffffff;&quot;&gt;cd /some/where/on/the/disk&lt;/span&gt;&lt;/p&gt;
&lt;p style=&quot; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;&lt;span style=&quot; color:#ffffff;&quot;&gt;清理 --force junkfile&lt;/span&gt;&lt;/p&gt;
&lt;p style=&quot; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;&lt;span style=&quot; font-weight:600; color:#aa0000;&quot;&gt;这是一条错误消息。&lt;/span&gt;&lt;/p&gt;
&lt;p style=&quot; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;这是正常的程序输出。&lt;/p&gt;
&lt;p style=&quot;-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;&lt;br /&gt;&lt;/p&gt;
&lt;p style=&quot; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890&lt;/p&gt;
&lt;p style=&quot; margin-top:0px;下边距：0px；左边距：0px；右边距：0px；-qt-block-indent：0；文本缩进：0px；&quot;&gt; 10 20 30 40 50 60 70 80&lt;/p&gt;&lt;/body&gt;&lt;/html</translation>
    </message>
    <message>
        <location filename="../output-window.ui" line="134"/>
        <location filename="../output-window.ui" line="145"/>
        <location filename="../output-window.ui" line="156"/>
        <source>...</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../output-window.ui" line="180"/>
        <source>&amp;Kill Process</source>
        <translation>终止进程(&amp;K)</translation>
    </message>
    <message>
        <location filename="../output-window.ui" line="200"/>
        <source>&amp;Auto Close</source>
        <translation>自动关闭(&amp;A)</translation>
    </message>
    <message>
        <location filename="../output-window.ui" line="210"/>
        <source>&amp;Close</source>
        <translation>关闭(&amp;C)</translation>
    </message>
    <message>
        <location filename="../output-window.ui" line="223"/>
        <source>Zoom In</source>
        <translation>放大</translation>
    </message>
    <message>
        <location filename="../output-window.ui" line="226"/>
        <source>Make the font of the output window larger.</source>
        <translation>增大输出窗口字体大小。</translation>
    </message>
    <message>
        <location filename="../output-window.ui" line="229"/>
        <source>Ctrl++</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../output-window.ui" line="238"/>
        <source>Zoom Out</source>
        <translation>缩小</translation>
    </message>
    <message>
        <location filename="../output-window.ui" line="241"/>
        <source>Make the font of the output window smaller.</source>
        <translation>减小输出窗口字体大小。</translation>
    </message>
    <message>
        <location filename="../output-window.ui" line="244"/>
        <source>Ctrl+-</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../output-window.ui" line="253"/>
        <source>Reset Zoom</source>
        <translation>重置缩放</translation>
    </message>
    <message>
        <location filename="../output-window.ui" line="256"/>
        <source>Use standard font size for the output window.</source>
        <translation>对输出窗口使用标准字体大小。</translation>
    </message>
    <message>
        <location filename="../output-window.ui" line="259"/>
        <source>Ctrl+0</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../output-window.ui" line="268"/>
        <source>Kill Process</source>
        <translation>终止进程</translation>
    </message>
    <message>
        <location filename="../output-window.ui" line="271"/>
        <source>Forcefully kill the cleanup process.</source>
        <translation>强制终止清理进程。</translation>
    </message>
    <message>
        <location filename="../OutputWindow.cpp" line="201"/>
        <source>Process finished.</source>
        <translation>进程成功处理完成。</translation>
    </message>
    <message>
        <location filename="../OutputWindow.cpp" line="214"/>
        <source>Process crashed.</source>
        <translation>进程崩溃。</translation>
    </message>
    <message>
        <location filename="../OutputWindow.cpp" line="219"/>
        <source>Process crashed. Exit code: %1</source>
        <translation>进程崩溃。退出代码：%1</translation>
    </message>
    <message>
        <location filename="../OutputWindow.cpp" line="251"/>
        <source>Error: Process failed to start.</source>
        <translation>错误：进程启动失败。</translation>
    </message>
    <message>
        <location filename="../OutputWindow.cpp" line="258"/>
        <source>Error: Process timed out.</source>
        <translation>错误：进程超时。</translation>
    </message>
    <message>
        <location filename="../OutputWindow.cpp" line="262"/>
        <source>Error reading output from the process.</source>
        <translation>读取进程输出时出错。</translation>
    </message>
    <message>
        <location filename="../OutputWindow.cpp" line="266"/>
        <source>Error writing data to the process.</source>
        <translation>将数据写入进程时出错。</translation>
    </message>
    <message>
        <location filename="../OutputWindow.cpp" line="270"/>
        <source>Unknown error.</source>
        <translation>未知错误。</translation>
    </message>
    <message>
        <location filename="../OutputWindow.cpp" line="385"/>
        <source>Process killed.</source>
        <translation>进程被终止。</translation>
    </message>
    <message>
        <location filename="../OutputWindow.cpp" line="386"/>
        <source>Killed %1 processes.</source>
        <translation>已终止 %1 进程。</translation>
    </message>
</context>
<context>
    <name>PanelMessage</name>
    <message>
        <location filename="../panel-message.ui" line="14"/>
        <source>Message</source>
        <translation>消息</translation>
    </message>
    <message>
        <location filename="../panel-message.ui" line="116"/>
        <source>Some directories could not be read.</source>
        <translation>部分目录无法读取。</translation>
    </message>
    <message>
        <location filename="../panel-message.ui" line="132"/>
        <source>You might not have sufficient permissions.</source>
        <translation>您可能没有足够的权限。</translation>
    </message>
    <message>
        <location filename="../panel-message.ui" line="148"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;&lt;a href=&quot;details&quot;&gt;&lt;span style=&quot; text-decoration: underline; color:#1d96b9;&quot;&gt;Details...&lt;/span&gt;&lt;/a&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;&lt;a href=&quot;details&quot;&gt;&lt;span style=&quot; text-decoration: underline; color:#1d96b9;&quot;&gt;详细信息...&lt;/span&gt;&lt;/a&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
</context>
<context>
    <name>QDirStat::BookmarksManager</name>
    <message>
        <location filename="../BookmarksManager.cpp" line="115"/>
        <source>No Bookmarks</source>
        <translation>无书签</translation>
    </message>
</context>
<context>
    <name>QDirStat::BucketsTableModel</name>
    <message>
        <location filename="../BucketsTableModel.cpp" line="107"/>
        <source>Start</source>
        <translation>开始</translation>
    </message>
    <message>
        <location filename="../BucketsTableModel.cpp" line="108"/>
        <source>End</source>
        <translation>结束</translation>
    </message>
    <message>
        <location filename="../BucketsTableModel.cpp" line="109"/>
        <source>Files</source>
        <translation>文件</translation>
    </message>
</context>
<context>
    <name>QDirStat::CleanupCollection</name>
    <message>
        <location filename="../CleanupCollection.cpp" line="345"/>
        <source>&lt;h3&gt;%1&lt;/h3&gt;for &lt;b&gt;directory&lt;/b&gt; %2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../CleanupCollection.cpp" line="347"/>
        <source>&lt;h3&gt;%1&lt;/h3&gt;for file %2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../CleanupCollection.cpp" line="379"/>
        <source>&lt;i&gt;(%1 items total)&lt;/i&gt;</source>
        <translation>&lt;i&gt;（共 %1 项）&lt;/i&gt;</translation>
    </message>
    <message>
        <location filename="../CleanupCollection.cpp" line="382"/>
        <source>&lt;h3&gt;%1&lt;/h3&gt; for:&lt;br&gt;
%2&lt;br&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../CleanupCollection.cpp" line="386"/>
        <source>Please Confirm</source>
        <translation>请确认</translation>
    </message>
    <message>
        <location filename="../CleanupCollection.cpp" line="410"/>
        <source>&lt;b&gt;Directory &lt;font color=blue&gt;%1&lt;/font&gt;&lt;/b&gt;</source>
        <translation>&lt;b&gt;目录 &lt;font color=blue&gt;%1&lt;/font&gt;&lt;/b&gt;</translation>
    </message>
    <message>
        <location filename="../CleanupCollection.cpp" line="412"/>
        <source>&lt;b&gt;Directory&lt;/b&gt; %1</source>
        <translation>&lt;b&gt;目录&lt;/b&gt; %1</translation>
    </message>
</context>
<context>
    <name>QDirStat::CleanupConfigPage</name>
    <message>
        <location filename="../CleanupConfigPage.cpp" line="135"/>
        <source>Error</source>
        <translation>错误</translation>
    </message>
    <message>
        <location filename="../CleanupConfigPage.cpp" line="136"/>
        <source>Recursive operation cannot be used with
refresh policy &quot;Assume Deleted&quot;.</source>
        <translation>递归操作不能与刷新策略“假定已删除”一起使用。</translation>
    </message>
    <message>
        <location filename="../CleanupConfigPage.cpp" line="253"/>
        <source>Really delete cleanup &quot;%1&quot;?</source>
        <translation>确定清理并删除“%1”吗？</translation>
    </message>
</context>
<context>
    <name>QDirStat::ConfigDialog</name>
    <message>
        <location filename="../ConfigDialog.cpp" line="42"/>
        <source>Cleanup Actions</source>
        <translation>清理</translation>
    </message>
    <message>
        <location filename="../ConfigDialog.cpp" line="46"/>
        <source>MIME Categories</source>
        <translation>MIME 类别</translation>
    </message>
    <message>
        <location filename="../ConfigDialog.cpp" line="50"/>
        <source>Exclude Rules</source>
        <translation>排除规则</translation>
    </message>
    <message>
        <location filename="../ConfigDialog.cpp" line="54"/>
        <source>General</source>
        <translation>常规</translation>
    </message>
</context>
<context>
    <name>QDirStat::DirTreeModel</name>
    <message>
        <location filename="../DirTreeModel.cpp" line="486"/>
        <source>Name</source>
        <translation>名称</translation>
    </message>
    <message>
        <location filename="../DirTreeModel.cpp" line="487"/>
        <source>Subtree Percentage</source>
        <translation>子树百分比</translation>
    </message>
    <message>
        <location filename="../DirTreeModel.cpp" line="488"/>
        <source>%</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../DirTreeModel.cpp" line="489"/>
        <source>Size</source>
        <translation>大小</translation>
    </message>
    <message>
        <location filename="../DirTreeModel.cpp" line="490"/>
        <source>Items</source>
        <translation>项目</translation>
    </message>
    <message>
        <location filename="../DirTreeModel.cpp" line="491"/>
        <source>Files</source>
        <translation>文件</translation>
    </message>
    <message>
        <location filename="../DirTreeModel.cpp" line="492"/>
        <source>Subdirs</source>
        <translation>子目录</translation>
    </message>
    <message>
        <location filename="../DirTreeModel.cpp" line="493"/>
        <source>Last Modified</source>
        <translation>修改时间</translation>
    </message>
    <message>
        <location filename="../DirTreeModel.cpp" line="494"/>
        <source>Oldest File</source>
        <translation>最旧文件</translation>
    </message>
    <message>
        <location filename="../DirTreeModel.cpp" line="495"/>
        <source>User</source>
        <translation>用户</translation>
    </message>
    <message>
        <location filename="../DirTreeModel.cpp" line="496"/>
        <source>Group</source>
        <translation>组</translation>
    </message>
    <message>
        <location filename="../DirTreeModel.cpp" line="497"/>
        <source>Permissions</source>
        <translation>权限</translation>
    </message>
    <message>
        <location filename="../DirTreeModel.cpp" line="498"/>
        <source>Perm.</source>
        <translation>八进制权限.</translation>
    </message>
    <message>
        <location filename="../DirTreeModel.cpp" line="678"/>
        <source>[%1 Read Jobs]</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../DirTreeModel.cpp" line="696"/>
        <source>[Excluded]</source>
        <translation>[不包括]</translation>
    </message>
    <message>
        <location filename="../DirTreeModel.cpp" line="883"/>
        <source>%1 / %2 Links (allocated: %3)</source>
        <translation>%1 / %2 链接（已分配：%3）</translation>
    </message>
    <message>
        <location filename="../DirTreeModel.cpp" line="890"/>
        <source>%1 / %2 Links</source>
        <translation>%1 / %2 链接</translation>
    </message>
    <message>
        <location filename="../DirTreeModel.cpp" line="899"/>
        <source>%1 (allocated: %2)</source>
        <translation>%1 （已分配：%2）</translation>
    </message>
</context>
<context>
    <name>QDirStat::DirTreeView</name>
    <message>
        <location filename="../DirTreeView.cpp" line="139"/>
        <source>View in</source>
        <translation>查看</translation>
    </message>
    <message>
        <location filename="../DirTreeView.cpp" line="167"/>
        <source> (allocated: %1)</source>
        <translation> （已分配：%1）</translation>
    </message>
</context>
<context>
    <name>QDirStat::DiscoverActions</name>
    <message>
        <location filename="../DiscoverActions.cpp" line="41"/>
        <source>Largest Files in %1</source>
        <translation>%1 中最大的文件</translation>
    </message>
    <message>
        <location filename="../DiscoverActions.cpp" line="49"/>
        <source>Newest Files in %1</source>
        <translation>%1 中的最新文件</translation>
    </message>
    <message>
        <location filename="../DiscoverActions.cpp" line="57"/>
        <source>Oldest Files in %1</source>
        <translation>%1 中最旧的文件</translation>
    </message>
    <message>
        <location filename="../DiscoverActions.cpp" line="65"/>
        <source>Files with Multiple Hard Links in %1</source>
        <translation>%1 中具有多个硬链接的文件</translation>
    </message>
    <message>
        <location filename="../DiscoverActions.cpp" line="72"/>
        <source>Checking symlinks...</source>
        <translation>检查符号链接...</translation>
    </message>
    <message>
        <location filename="../DiscoverActions.cpp" line="74"/>
        <source>Broken Symbolic Links in %1</source>
        <translation>%1 中的符号链接已损坏</translation>
    </message>
    <message>
        <location filename="../DiscoverActions.cpp" line="82"/>
        <source>Sparse Files in %1</source>
        <translation>%1 中的稀疏文件</translation>
    </message>
    <message>
        <location filename="../DiscoverActions.cpp" line="89"/>
        <source>Files from %1 in %2</source>
        <translation>来自 %2 中 %1 的文件</translation>
    </message>
    <message>
        <location filename="../DiscoverActions.cpp" line="98"/>
        <source>Files from %1/%2 in %3</source>
        <translation>来自 %3 中 %1/%2 的文件</translation>
    </message>
    <message>
        <location filename="../DiscoverActions.cpp" line="162"/>
        <source>Search Results for &quot;%1&quot;</source>
        <translation>“%1”的搜索结果</translation>
    </message>
</context>
<context>
    <name>QDirStat::ExcludeRulesConfigPage</name>
    <message>
        <location filename="../ExcludeRulesConfigPage.cpp" line="218"/>
        <source>Really delete exclude rule &quot;%1&quot;?</source>
        <translation>确实删除排除规则“%1”吗？</translation>
    </message>
</context>
<context>
    <name>QDirStat::FileAgeStatsWindow</name>
    <message>
        <location filename="../FileAgeStatsWindow.cpp" line="80"/>
        <source>Year</source>
        <translation>时间</translation>
    </message>
    <message>
        <location filename="../FileAgeStatsWindow.cpp" line="81"/>
        <source>Files</source>
        <translation>文件</translation>
    </message>
    <message>
        <location filename="../FileAgeStatsWindow.cpp" line="82"/>
        <source>Files %</source>
        <translation>文件占用百分比 %</translation>
    </message>
    <message>
        <location filename="../FileAgeStatsWindow.cpp" line="83"/>
        <location filename="../FileAgeStatsWindow.cpp" line="86"/>
        <source>%</source>
        <translation>占用百分比 %</translation>
    </message>
    <message>
        <location filename="../FileAgeStatsWindow.cpp" line="84"/>
        <source>Size</source>
        <translation>大小</translation>
    </message>
    <message>
        <location filename="../FileAgeStatsWindow.cpp" line="85"/>
        <source>Size %</source>
        <translation>大小占用百分比 %</translation>
    </message>
    <message>
        <location filename="../FileAgeStatsWindow.cpp" line="154"/>
        <source>File Age Statistics for %1</source>
        <translation>%1 的文件时间统计信息</translation>
    </message>
</context>
<context>
    <name>QDirStat::FileDetailsView</name>
    <message>
        <location filename="../FileDetailsView.cpp" line="235"/>
        <location filename="../FileDetailsView.cpp" line="241"/>
        <location filename="../FileDetailsView.cpp" line="260"/>
        <location filename="../FileDetailsView.cpp" line="264"/>
        <source>%1 / %2 Links</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../FileDetailsView.cpp" line="342"/>
        <source>Pseudo Directory</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../FileDetailsView.cpp" line="342"/>
        <source>Directory</source>
        <translation>目录</translation>
    </message>
    <message>
        <location filename="../FileDetailsView.cpp" line="345"/>
        <source>Mount Point</source>
        <translation>挂载点</translation>
    </message>
    <message>
        <location filename="../FileDetailsView.cpp" line="370"/>
        <location filename="../FileDetailsView.cpp" line="493"/>
        <location filename="../FileDetailsView.cpp" line="573"/>
        <source>[Reading]</source>
        <translation>[读取中]</translation>
    </message>
    <message>
        <location filename="../FileDetailsView.cpp" line="372"/>
        <source>[Not Read]</source>
        <translation>[未读取]</translation>
    </message>
    <message>
        <location filename="../FileDetailsView.cpp" line="373"/>
        <location filename="../FileDetailsView.cpp" line="495"/>
        <source>[Permission Denied]</source>
        <translation>[没有权限]</translation>
    </message>
    <message>
        <location filename="../FileDetailsView.cpp" line="374"/>
        <location filename="../FileDetailsView.cpp" line="496"/>
        <location filename="../FileDetailsView.cpp" line="578"/>
        <source>[Read Error]</source>
        <translation>[读取错误]</translation>
    </message>
    <message>
        <location filename="../FileDetailsView.cpp" line="497"/>
        <source>[Aborted]</source>
        <translation>[已中止]</translation>
    </message>
</context>
<context>
    <name>QDirStat::FileSizeStatsWindow</name>
    <message>
        <location filename="../FileSizeStatsWindow.cpp" line="166"/>
        <source>File Size Statistics for %1</source>
        <translation>%1 的文件大小统计信息</translation>
    </message>
    <message>
        <location filename="../FileSizeStatsWindow.cpp" line="168"/>
        <source>File Size Statistics for %1 in %2</source>
        <translation>%2 中 %1 的文件大小统计信息</translation>
    </message>
    <message>
        <location filename="../FileSizeStatsWindow.cpp" line="230"/>
        <source>Percentile</source>
        <translation>百分位</translation>
    </message>
    <message>
        <location filename="../FileSizeStatsWindow.cpp" line="231"/>
        <source>Decile</source>
        <translation>十分位</translation>
    </message>
    <message>
        <location filename="../FileSizeStatsWindow.cpp" line="232"/>
        <source>Quartile</source>
        <translation>千分位</translation>
    </message>
    <message>
        <location filename="../FileSizeStatsWindow.cpp" line="233"/>
        <source>%1-Quantile</source>
        <translation>%1-分位</translation>
    </message>
    <message>
        <location filename="../FileSizeStatsWindow.cpp" line="236"/>
        <source>Value</source>
        <translation>值</translation>
    </message>
    <message>
        <location filename="../FileSizeStatsWindow.cpp" line="236"/>
        <source>Name</source>
        <translation>名称</translation>
    </message>
    <message>
        <location filename="../FileSizeStatsWindow.cpp" line="240"/>
        <source>Sum %1(n-1)..%2(n)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../FileSizeStatsWindow.cpp" line="241"/>
        <source>Cumulative Sum</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../FileSizeStatsWindow.cpp" line="284"/>
        <source>Min</source>
        <translation>最小</translation>
    </message>
    <message>
        <location filename="../FileSizeStatsWindow.cpp" line="285"/>
        <source>Max</source>
        <translation>最大</translation>
    </message>
    <message>
        <location filename="../FileSizeStatsWindow.cpp" line="286"/>
        <source>Median</source>
        <translation>二分之一</translation>
    </message>
    <message>
        <location filename="../FileSizeStatsWindow.cpp" line="287"/>
        <source>1. Quartile</source>
        <translation>四分之一</translation>
    </message>
    <message>
        <location filename="../FileSizeStatsWindow.cpp" line="288"/>
        <source>3. Quartile</source>
        <translation>四分之三</translation>
    </message>
    <message>
        <location filename="../FileSizeStatsWindow.cpp" line="432"/>
        <source>&amp;Options &gt;&gt;</source>
        <translation>选项(&amp;P) &gt;&gt;</translation>
    </message>
    <message>
        <location filename="../FileSizeStatsWindow.cpp" line="437"/>
        <source>&lt;&lt; &amp;Options</source>
        <translation>&lt;&lt; 选项(&amp;P)</translation>
    </message>
</context>
<context>
    <name>QDirStat::FileTypeStats</name>
    <message>
        <location filename="../FileTypeStats.cpp" line="28"/>
        <source>Other</source>
        <translation>其他</translation>
    </message>
</context>
<context>
    <name>QDirStat::FileTypeStatsWindow</name>
    <message>
        <location filename="../FileTypeStatsWindow.cpp" line="93"/>
        <source>Name</source>
        <translation>名称</translation>
    </message>
    <message>
        <location filename="../FileTypeStatsWindow.cpp" line="94"/>
        <source>Number</source>
        <translation>计数</translation>
    </message>
    <message>
        <location filename="../FileTypeStatsWindow.cpp" line="95"/>
        <source>Total Size</source>
        <translation>总大小</translation>
    </message>
    <message>
        <location filename="../FileTypeStatsWindow.cpp" line="96"/>
        <source>Percentage</source>
        <translation>百分比</translation>
    </message>
    <message>
        <location filename="../FileTypeStatsWindow.cpp" line="145"/>
        <source>File Type Statistics for %1</source>
        <translation>%1 的文件类型统计信息</translation>
    </message>
    <message>
        <location filename="../FileTypeStatsWindow.cpp" line="249"/>
        <source>Other (Top %1)</source>
        <translation>其他（前 %1）</translation>
    </message>
    <message>
        <location filename="../FileTypeStatsWindow.cpp" line="249"/>
        <source>Other</source>
        <translation>其他</translation>
    </message>
</context>
<context>
    <name>QDirStat::HeaderTweaker</name>
    <message>
        <location filename="../HeaderTweaker.cpp" line="120"/>
        <source>&amp;Hide</source>
        <translation>隐藏(&amp;H)</translation>
    </message>
    <message>
        <location filename="../HeaderTweaker.cpp" line="123"/>
        <source>Show &amp;All Hidden Columns</source>
        <translation>显示所有隐藏列(&amp;A)</translation>
    </message>
    <message>
        <location filename="../HeaderTweaker.cpp" line="126"/>
        <source>A&amp;uto Size</source>
        <translation>自动调整大小(&amp;U)</translation>
    </message>
    <message>
        <location filename="../HeaderTweaker.cpp" line="132"/>
        <source>&amp;Auto Size</source>
        <translation>自动调整大小(&amp;A)</translation>
    </message>
    <message>
        <location filename="../HeaderTweaker.cpp" line="135"/>
        <source>&amp;Interactive Size</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../HeaderTweaker.cpp" line="138"/>
        <source>&amp;Reset to Defaults</source>
        <translation>重置为默认值(&amp;R)</translation>
    </message>
    <message>
        <location filename="../HeaderTweaker.cpp" line="161"/>
        <source>Column &quot;%1&quot;</source>
        <translation>列“%1”</translation>
    </message>
    <message>
        <location filename="../HeaderTweaker.cpp" line="168"/>
        <source>&amp;All Columns</source>
        <translation>所有列(&amp;A)</translation>
    </message>
    <message>
        <location filename="../HeaderTweaker.cpp" line="181"/>
        <source>Hi&amp;dden Columns</source>
        <translation>隐藏列(&amp;D)</translation>
    </message>
    <message>
        <location filename="../HeaderTweaker.cpp" line="188"/>
        <source>Show Column &quot;%1&quot;</source>
        <translation>显示列“%1”</translation>
    </message>
</context>
<context>
    <name>QDirStat::HistogramView</name>
    <message>
        <location filename="../HistogramDraw.cpp" line="94"/>
        <source>File Size</source>
        <translation>文件大小</translation>
    </message>
    <message>
        <location filename="../HistogramDraw.cpp" line="115"/>
        <source>Min</source>
        <translation>最小</translation>
    </message>
    <message>
        <location filename="../HistogramDraw.cpp" line="123"/>
        <source>Max</source>
        <translation>最大</translation>
    </message>
    <message>
        <location filename="../HistogramDraw.cpp" line="158"/>
        <source>Q1: %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../HistogramDraw.cpp" line="159"/>
        <source>Q3: %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../HistogramDraw.cpp" line="160"/>
        <source>Median: %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../HistogramDraw.cpp" line="197"/>
        <source>Files (n): %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../HistogramDraw.cpp" line="304"/>
        <source>Q1 (1st Quartile)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../HistogramDraw.cpp" line="308"/>
        <source>Q3 (3rd Quartile)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../HistogramDraw.cpp" line="314"/>
        <source>Median</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../HistogramOverflowPanel.cpp" line="42"/>
        <source>Cut off</source>
        <translation>饼图</translation>
    </message>
    <message>
        <location filename="../HistogramOverflowPanel.cpp" line="56"/>
        <source>Min (P0) .. P%1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../HistogramOverflowPanel.cpp" line="57"/>
        <location filename="../HistogramOverflowPanel.cpp" line="66"/>
        <source>%1 .. %2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../HistogramOverflowPanel.cpp" line="65"/>
        <source>P%1 .. Max (P100)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../HistogramOverflowPanel.cpp" line="87"/>
        <source>%1% of all files</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../HistogramOverflowPanel.cpp" line="89"/>
        <source>1 file total</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../HistogramOverflowPanel.cpp" line="90"/>
        <source>%1 files total</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../HistogramOverflowPanel.cpp" line="125"/>
        <source>%1% of disk space</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../HistogramOverflowPanel.cpp" line="126"/>
        <source>%1 total</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>QDirStat::ListEditor</name>
    <message>
        <location filename="../ListEditor.cpp" line="210"/>
        <source>Please Confirm</source>
        <translation>请确认</translation>
    </message>
</context>
<context>
    <name>QDirStat::LocateFileTypeWindow</name>
    <message>
        <location filename="../LocateFileTypeWindow.cpp" line="73"/>
        <source>Number</source>
        <translation>计数</translation>
    </message>
    <message>
        <location filename="../LocateFileTypeWindow.cpp" line="74"/>
        <source>Total Size</source>
        <translation>总大小</translation>
    </message>
    <message>
        <location filename="../LocateFileTypeWindow.cpp" line="75"/>
        <source>Directory</source>
        <translation>目录</translation>
    </message>
    <message>
        <location filename="../LocateFileTypeWindow.cpp" line="106"/>
        <source>Directories with %1 Files below %2</source>
        <translation>包含 %1 个文件的目录，位于 %2 之下</translation>
    </message>
</context>
<context>
    <name>QDirStat::LocateFilesWindow</name>
    <message>
        <location filename="../LocateFilesWindow.cpp" line="99"/>
        <source>Size</source>
        <translation>大小</translation>
    </message>
    <message>
        <location filename="../LocateFilesWindow.cpp" line="100"/>
        <source>Last Modified</source>
        <translation>修改时间</translation>
    </message>
    <message>
        <location filename="../LocateFilesWindow.cpp" line="101"/>
        <source>Path</source>
        <translation>路径</translation>
    </message>
    <message>
        <location filename="../LocateFilesWindow.cpp" line="170"/>
        <source>Limited to %1 Results</source>
        <translation>限制为 %1 个结果</translation>
    </message>
    <message>
        <location filename="../LocateFilesWindow.cpp" line="174"/>
        <source>%1 Results</source>
        <translation>%1 条结果</translation>
    </message>
</context>
<context>
    <name>QDirStat::MimeCategorizer</name>
    <message>
        <location filename="../MimeCategorizer.cpp" line="338"/>
        <source>Junk</source>
        <translation>垃圾文件</translation>
    </message>
    <message>
        <location filename="../MimeCategorizer.cpp" line="347"/>
        <source>Compressed Archives</source>
        <translation>压缩文件</translation>
    </message>
    <message>
        <location filename="../MimeCategorizer.cpp" line="379"/>
        <source>Uncompressed Archives</source>
        <translation>未压缩文件</translation>
    </message>
    <message>
        <location filename="../MimeCategorizer.cpp" line="387"/>
        <source>Compressed Files</source>
        <translation>压缩文件</translation>
    </message>
    <message>
        <location filename="../MimeCategorizer.cpp" line="401"/>
        <source>Images</source>
        <translation>图片</translation>
    </message>
    <message>
        <location filename="../MimeCategorizer.cpp" line="422"/>
        <source>Uncompressed Images</source>
        <translation>未压缩的图像</translation>
    </message>
    <message>
        <location filename="../MimeCategorizer.cpp" line="436"/>
        <source>Videos</source>
        <translation>视频</translation>
    </message>
    <message>
        <location filename="../MimeCategorizer.cpp" line="469"/>
        <source>Music</source>
        <translation>音乐</translation>
    </message>
    <message>
        <location filename="../MimeCategorizer.cpp" line="501"/>
        <source>Documents</source>
        <translation>文档</translation>
    </message>
    <message>
        <location filename="../MimeCategorizer.cpp" line="552"/>
        <source>Source Files</source>
        <translation>源文件</translation>
    </message>
    <message>
        <location filename="../MimeCategorizer.cpp" line="562"/>
        <source>Object or Generated Files</source>
        <translation>对象或生成的文件</translation>
    </message>
    <message>
        <location filename="../MimeCategorizer.cpp" line="575"/>
        <source>Libraries</source>
        <translation>库</translation>
    </message>
</context>
<context>
    <name>QDirStat::MimeCategoryConfigPage</name>
    <message>
        <location filename="../MimeCategoryConfigPage.cpp" line="156"/>
        <source>Category Color</source>
        <translation>类别颜色</translation>
    </message>
    <message>
        <location filename="../MimeCategoryConfigPage.cpp" line="253"/>
        <source>Really delete category &quot;%1&quot;?</source>
        <translation>确实删除类别“%1”吗？</translation>
    </message>
</context>
<context>
    <name>QDirStat::PanelMessage</name>
    <message>
        <location filename="../PanelMessage.cpp" line="45"/>
        <source>Details...</source>
        <translation>详情...</translation>
    </message>
</context>
<context>
    <name>QDirStat::PathSelector</name>
    <message>
        <location filename="../PathSelector.cpp" line="64"/>
        <source>Your home directory</source>
        <translation>您的主目录</translation>
    </message>
</context>
<context>
    <name>QDirStat::ShowUnpkgFilesDialog</name>
    <message>
        <location filename="../ShowUnpkgFilesDialog.cpp" line="111"/>
        <source>Really reset all values to default?</source>
        <translation>确定将所有值重置为默认？</translation>
    </message>
    <message>
        <location filename="../ShowUnpkgFilesDialog.cpp" line="113"/>
        <source>Please Confirm</source>
        <translation>请确认</translation>
    </message>
</context>
<context>
    <name>QDirStat::UnreadableDirsWindow</name>
    <message>
        <location filename="../UnreadableDirsWindow.cpp" line="76"/>
        <source>Directory</source>
        <translation>目录</translation>
    </message>
    <message>
        <location filename="../UnreadableDirsWindow.cpp" line="77"/>
        <source>User</source>
        <translation>用户</translation>
    </message>
    <message>
        <location filename="../UnreadableDirsWindow.cpp" line="78"/>
        <source>Group</source>
        <translation>组</translation>
    </message>
    <message>
        <location filename="../UnreadableDirsWindow.cpp" line="79"/>
        <source>Permissions</source>
        <translation>权限</translation>
    </message>
    <message>
        <location filename="../UnreadableDirsWindow.cpp" line="80"/>
        <source>Perm.</source>
        <translation>八进制权限.</translation>
    </message>
</context>
<context>
    <name>QObject</name>
    <message>
        <location filename="../Exception.cpp" line="39"/>
        <source>%1( &quot;%2&quot; ) failed: %3</source>
        <translation>%1(“%2”)失败：%3</translation>
    </message>
    <message>
        <location filename="../Exception.cpp" line="46"/>
        <source>%1( &quot;%2&quot; ) failed</source>
        <translation>%1( &quot;%2&quot; ) 失败</translation>
    </message>
    <message>
        <location filename="../FileAgeStatsWindow.cpp" line="348"/>
        <source>Jan.</source>
        <translation>一月</translation>
    </message>
    <message>
        <location filename="../FileAgeStatsWindow.cpp" line="349"/>
        <source>Feb.</source>
        <translation>二月</translation>
    </message>
    <message>
        <location filename="../FileAgeStatsWindow.cpp" line="350"/>
        <source>Mar.</source>
        <translation>三月</translation>
    </message>
    <message>
        <location filename="../FileAgeStatsWindow.cpp" line="351"/>
        <source>Apr.</source>
        <translation>四月</translation>
    </message>
    <message>
        <location filename="../FileAgeStatsWindow.cpp" line="352"/>
        <source>May</source>
        <translation>五月</translation>
    </message>
    <message>
        <location filename="../FileAgeStatsWindow.cpp" line="353"/>
        <source>Jun.</source>
        <translation>六月</translation>
    </message>
    <message>
        <location filename="../FileAgeStatsWindow.cpp" line="354"/>
        <source>Jul.</source>
        <translation>七月</translation>
    </message>
    <message>
        <location filename="../FileAgeStatsWindow.cpp" line="355"/>
        <source>Aug.</source>
        <translation>八月</translation>
    </message>
    <message>
        <location filename="../FileAgeStatsWindow.cpp" line="356"/>
        <source>Sep.</source>
        <translation>九月</translation>
    </message>
    <message>
        <location filename="../FileAgeStatsWindow.cpp" line="357"/>
        <source>Oct.</source>
        <translation>十月</translation>
    </message>
    <message>
        <location filename="../FileAgeStatsWindow.cpp" line="358"/>
        <source>Nov.</source>
        <translation>十一月</translation>
    </message>
    <message>
        <location filename="../FileAgeStatsWindow.cpp" line="359"/>
        <source>Dec.</source>
        <translation>十二月</translation>
    </message>
    <message>
        <location filename="../FileInfo.cpp" line="518"/>
        <source>&lt;Files&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../FileInfo.cpp" line="524"/>
        <source>&lt;Ignored&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../FileTypeStatsWindow.cpp" line="470"/>
        <source>&lt;No Extension&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../FileTypeStatsWindow.cpp" line="472"/>
        <source>&lt;Other&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../FormatUtil.cpp" line="72"/>
        <source>Bytes</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../FormatUtil.cpp" line="73"/>
        <source>kB</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../FormatUtil.cpp" line="74"/>
        <source>MB</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../FormatUtil.cpp" line="75"/>
        <source>GB</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../FormatUtil.cpp" line="76"/>
        <source>TB</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../FormatUtil.cpp" line="77"/>
        <source>PB</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../FormatUtil.cpp" line="78"/>
        <source>EB</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../FormatUtil.cpp" line="79"/>
        <source>ZB</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../FormatUtil.cpp" line="80"/>
        <source>YB</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../FormatUtil.cpp" line="125"/>
        <source>%1 Bytes</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../FormatUtil.cpp" line="178"/>
        <source>Directory</source>
        <translation>目录</translation>
    </message>
    <message>
        <location filename="../FormatUtil.cpp" line="179"/>
        <source>Character Device</source>
        <translation>字符设备</translation>
    </message>
    <message>
        <location filename="../FormatUtil.cpp" line="180"/>
        <source>Block Device</source>
        <translation>块设备</translation>
    </message>
    <message>
        <location filename="../FormatUtil.cpp" line="181"/>
        <source>Named Pipe</source>
        <translation>命名管道</translation>
    </message>
    <message>
        <location filename="../FormatUtil.cpp" line="182"/>
        <source>Symbolic Link</source>
        <translation>符号链接</translation>
    </message>
    <message>
        <location filename="../FormatUtil.cpp" line="183"/>
        <source>Socket</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../FormatUtil.cpp" line="184"/>
        <source>File</source>
        <translation>文件</translation>
    </message>
    <message>
        <location filename="../FormatUtil.cpp" line="270"/>
        <source>sec</source>
        <translation>秒</translation>
    </message>
    <message>
        <location filename="../HistogramItems.cpp" line="50"/>
        <source>Bucket #%1:
%2 Files
%3 .. %4</source>
        <translation>存储桶 #%1：
%2 个文件
%3 .. %4</translation>
    </message>
    <message>
        <location filename="../HistogramItems.cpp" line="108"/>
        <source>Percentile P%1</source>
        <translation>百分位数 P%</translation>
    </message>
    <message>
        <location filename="../PathSelector.cpp" line="194"/>
        <source>Used: %1</source>
        <translation>已使用：%1</translation>
    </message>
    <message>
        <location filename="../PathSelector.cpp" line="197"/>
        <source>Free for users: %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../PathSelector.cpp" line="200"/>
        <source>Free for root: %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../RpmPkgManager.cpp" line="268"/>
        <source>RPM is very slow.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../RpmPkgManager.cpp" line="269"/>
        <source>Open a shell window and run:&lt;br&gt;%1</source>
        <translation>打开一个 shell 窗口并运行：&lt;br&gt;%1</translation>
    </message>
    <message>
        <location filename="../StdCleanup.cpp" line="48"/>
        <source>Open File &amp;Manager Here</source>
        <translation>在此打开文件管理器(&amp;M)</translation>
    </message>
    <message>
        <location filename="../StdCleanup.cpp" line="66"/>
        <source>Open &amp;Terminal Here</source>
        <translation>在此打开终端(&amp;T)</translation>
    </message>
    <message>
        <location filename="../StdCleanup.cpp" line="84"/>
        <source>Check File T&amp;ype</source>
        <translation>检查文件类型(&amp;Y)</translation>
    </message>
    <message>
        <location filename="../StdCleanup.cpp" line="101"/>
        <source>&amp;Compress</source>
        <translation>压缩(&amp;C)</translation>
    </message>
    <message>
        <location filename="../StdCleanup.cpp" line="116"/>
        <source>make cl&amp;ean</source>
        <translation>清理(&amp;E)</translation>
    </message>
    <message>
        <location filename="../StdCleanup.cpp" line="131"/>
        <source>git cle&amp;an</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../StdCleanup.cpp" line="148"/>
        <source>Delete J&amp;unk Files</source>
        <translation>删除垃圾文件(&amp;N)</translation>
    </message>
    <message>
        <location filename="../StdCleanup.cpp" line="165"/>
        <source>&amp;Delete (no way to undelete!)</source>
        <translation>删除（无法恢复删除！）(&amp;D)</translation>
    </message>
    <message>
        <location filename="../StdCleanup.cpp" line="183"/>
        <source>Clear Directory C&amp;ontents</source>
        <translation>清除目录内容(&amp;O)</translation>
    </message>
    <message>
        <location filename="../StdCleanup.cpp" line="201"/>
        <source>echoargs</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../StdCleanup.cpp" line="217"/>
        <source>Output on stdout and stderr</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../StdCleanup.cpp" line="233"/>
        <source>Segfaulter</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../StdCleanup.cpp" line="248"/>
        <source>Nonexistent command</source>
        <translation>不存在的命令</translation>
    </message>
    <message>
        <location filename="../StdCleanup.cpp" line="263"/>
        <source>Sleepy echoargs</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>ShowUnpkgFilesDialog</name>
    <message>
        <location filename="../show-unpkg-files-dialog.ui" line="14"/>
        <location filename="../show-unpkg-files-dialog.ui" line="29"/>
        <source>Show Unpackaged Files</source>
        <translation>显示已解包的文件</translation>
    </message>
    <message>
        <location filename="../show-unpkg-files-dialog.ui" line="54"/>
        <source>Starting &amp;Directory:</source>
        <translation>起始目录(&amp;D):</translation>
    </message>
    <message>
        <location filename="../show-unpkg-files-dialog.ui" line="109"/>
        <source>E&amp;xclude Directories:</source>
        <translation>排除目录(&amp;X):</translation>
    </message>
    <message>
        <location filename="../show-unpkg-files-dialog.ui" line="147"/>
        <source>I&amp;gnore Patterns:</source>
        <translation>忽略表达式(&amp;G):</translation>
    </message>
</context>
<context>
    <name>UnreadableDirsWindow</name>
    <message>
        <location filename="../unreadable-dirs-window.ui" line="14"/>
        <source>Unreadable Directories</source>
        <translation>无法读取的目录</translation>
    </message>
    <message>
        <location filename="../unreadable-dirs-window.ui" line="50"/>
        <source>Directories that could not be Read</source>
        <translation>无法读取的目录</translation>
    </message>
    <message>
        <location filename="../unreadable-dirs-window.ui" line="91"/>
        <source>Total: 0</source>
        <translation>总计：0</translation>
    </message>
    <message>
        <location filename="../unreadable-dirs-window.ui" line="117"/>
        <source>&amp;Close</source>
        <translation>关闭(&amp;C)</translation>
    </message>
</context>
</TS>
