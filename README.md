# AnimatedScrollBarDemo

![scrollbar_anima](https://github.com/user-attachments/assets/3ef15c63-c509-47cc-aad8-8ca70c7e07cf)

测试程序运行后，向主窗口中拖入多个文件，显示出滚动条后鼠标悬停查看效果

该测试程序的初始目的是为了测试SVN的GUI工具调用，之后在文件列表控件中增加动态滚动条

使用QVariantAnimation生成宽度数据，通知窗口重新调整布局，使用qss初始化宽度信息
