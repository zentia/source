# Instance
全局实例，一般来说只有一个。
# SurfaceKHR
窗口
# PhysicalDevice
物理设备，实际的硬件，显卡、集成显卡
# Device
逻辑设备
# Queue
设备提供的队列，提交给硬件的命令，有的设备会有多个队列
# CommandBuffer
- 命令缓存区，DrawCall先提交到这里。
- CommandPool时创建CommandBuffer的对象池，因为CommandBuffer创建销毁都比较耗。Pool需要绑定到DeviceFamily，所以多个设备的命令没法一起提交。
- CommandBuffer先收集命令，然后用`vkQueueSubmit`提交给设备的Queue。
- CommandBuffer可以创建多个，BeginRenderPass调用的时候穿的CommandBuffer一般是主CommandBuffer，没有传到RenderPass的都是子CommandBuffer，这样多个线程上可以分别处理自己的命令到自己的子CommandBuffer上，然后提交到主CommandBuffer。
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

# Sampler
采样器，就是数据的壳，告诉vulkan具体如何解读数据。但和ImageView不一样，不需要绑定到Image上。

# DescriptorSet
描述符集，shader没法直接访问资源，要通过DescriptorSet来访问，其实就是内存到Shader的映射。DX12叫DescriptorHeap。

# FrameBuffer
最后要画到屏幕上的RT。BeginRenderPass的时候，就要带上FrameBuffer这个参数，这样Vulkan才知道如何画。每个RenderPass都对应一个FrameBuffer，也就说可以创建多个FrameBuffer

# Pipeline
最外层的一个壳，分为图形管线和计算管线。计算管线只有一个阶段，图形管线有多个阶段。