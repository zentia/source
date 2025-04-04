# Texture2D
- 纹理在vulkan中并没有一个结构体来表示，只是一种概念，shader中，`sampler2D`表示一个二维纹理。

# VKImage
- 图像对象，存储元数据`metadata`，但是不存储高度、宽度、格式。
- 类似于`VkBuffer`对象，是一个句柄`handle`，不包含实际的像素数据，需要通过`vkBindImageMemory`函数将VKImage与实际的`VkDeviceMemory`对象关联
- 完成关联之后，就可以使用`vkCmdCopyImage`、`vkDestroyImage`、`vkFreeMemory`等函数了。

# VKImageView
- 是对`VKImage`的一个视图或描述。它指定了如何从底层的`VKImage`中读取数据，包括格式、范围等。通常需要用`VKImageView`对`VKImage`进行采样操作。

# VkSampler
- 描述采样的方式，它定义了纹理采样时的过滤方式、边界模式、各向异性过滤等参数。通常与`VkImageView`一起用于纹理采样操作。

# VkDeviceMemory
- 设备的内存对象，用于存储`VkImage`或其他vulkan对象的实际内存。需要显示的为图像数据分配GPU内存，完成数据从CPU到GPU的写入，然后绑定到`VkImage`上。