# Source
Source game engine. Support Android iOS pc ps5 switch
由于我只看过Unity和Unreal的源码，所以这个引擎可以基本认为是Unity和Unrael的混合体。
不会采纳Unity的Libaray，会使用Unreal的uasset，但是源文件需要和meta文件放在一起，这里的meta文件其实是uasset文件。
这样的话，工程会大很多，所以会提供一个虚拟路径，也就是说可以做一个映射，原始文件和meta文件不在一起，但是要求工程结构保持一致，或者可配置，这样的话，可以做到每个工种之间不得尽量少感知别人的文件。仓库管理也会方便一些。
meta文件需要自己手动删除，新增的原始文件会自动生成meta文件，程序可以自定义导入规则。
不支持脚本，这块始终没有想好，不太想为性能放弃太多的东西，引擎这件事提高了一个门槛。
序列化采用Unreal的方式。
UI采用Unreal的方式。
xmake进行构建
nanite lumen
RTTI
UMG