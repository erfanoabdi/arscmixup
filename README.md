# arscmixup
Android resource file Confused
//@author : angelToms
//Copyright : angelToms
//关于resources.arsc文件结构，请查看docs/resources.arsc.txt文件

//解析流程来源参照：
//这些源文件你都可以按照自己方式去实现。

//res下是resources.arsc中定义的结构体原型，有些结构体我稍微做了一些补充和改变。

//String8 是单字节表示的字
//String16 是双字节表示的字 ，一般中文都是，UTF-8的同理
//SharedBuffer 实现类似共享内存，通过获得锁来操作这片内存，这里经过改造，同步原语被我替换为了gcc内建的原语
//Unicode 提供了基于UTF-N(8、16、32)的操作
//Debug 提供了打印Hex log相关

//android-source/frameworks/base/include/androidfw/ResouceTypes.h
//android-source/frameworks/base/libs/androidfw/ResouceTypes.h


//ArscSimpleParser, 只是简单的按结构解析，不支持大端格式，
//如果在大端情况下会出现问题, 解析过程中没有进行校验。另外我这里认为字符都是以UTF-8或者单字节组成的。
//ArscComplexParser，是基本仿照源码的解析，比较复杂，支持所有情况。
//解析实现主要在dto文件夹下，dto目录下是ComplexParser的实现，dto/simple是SimpleParser的实现。

//ArscComplexParser只是关注解析，和后面的混淆res名称没有关系，混淆名字是基于ArscSimpleParser实现的。
//res里面字符串混淆，基于等长混淆，这样可以保证不用修改过多的结构。

//详细结构请看当前目录下 resources.arsc.txt文件

//暂时对于resources.arsc只混淆它的ResStringPool(string pool),位于ResTable_header后面
//并没有混淆ResTable_package的key string 和type string
//混淆采用等长变换混淆，这样文件大小保存不变，我只是简单的做了一些测试，如果你在测试中出现问题
//可能需要修改key string 和 type string ，修改了这些一定会万无一失的。但是我个人觉得没啥用途。
//对于混淆resources.arsc生成的新文件名字为resources.arsc.1,对于的res文件一同更改，保留原resources.arsc



//如果想打印更详细的日志，可以开启DEBUG_ALL宏
//ONLY_PARSE_NOT_MIXUP 宏只是解析时用，带有这个宏时，不要去用混淆功能，基于源码的混淆的实现并没有提供

//关于apk解压缩、压缩并没有使用以下目录，因为我自己实现的关于zipapk/ApkZipOp.*性能非常差
//于是改用系统的/bin下面的命令（通过system调用）
//zlib 目录是摘自安卓源码zlib-1.2.3版本的
//zip 是摘自androidfw中的
//zipapk是基于zlib和zip我自己实现的关于apk解压缩、压缩的，性能非常差，估计是实现有问题，代码保留，但是不采用

//入口
//执行本程序有两种方式：
//1 直接执行编译好的文件
//2 通过main.py脚本执行

//1 和 2 的参数格式完全一致，两种区别在于1 是用系统的zip命令执行压缩和解压缩（这就要求例如linux系统安装了zip命令）
//而2 利用python的zip 接口来进行压缩和解压缩，关于资源的混淆完全是调用arscparser内部实现

//参数格式：
//[arscparser | python main.py] -t [python| bin] -p path -o [ parse| mixup]
//[arscparser | python main.py] 代表选择1 还是 2 来作为入口
//-t [python| bin] 代表是用python的压缩和解压缩还是利用系统自带的zip命令
//-p path 传人的apk或者资源路径，如果 -o 传人的是mixup ,则path必须是包含apk的全路径，否则是包含res的当前路径
//-o [ parse| mixup] 代表是解析还是混淆

//内部提供两种解析，一种复杂的上面提过的解析，一种是按文件顺序的常规解析
//但是现在编译好的bin提过的是第二种解析方式，如果你需要使用第一种解析方式，需要修改文件加ONLY_PARSE_NOT_MIXUP宏

//输入输出
//如果是解析，输入可以是apk全路径或者包含resources.arsc的当前路径，输出log即为解析结果
//如果是混淆，输入时apk全路径，输出为新生成的apk。

混淆结果：
原结构： res -- anim， res-- color
混淆后： 是这个res --  a ,res -- b还是这个 a -- b ,a -- c 我给忘了，反正是这两个中的一个 哈哈
代码是几年前的，风格可能不是很好，请大家见谅！

