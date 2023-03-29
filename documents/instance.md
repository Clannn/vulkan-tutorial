首先需要通过创建Instance对象来初始化Vulkan库，VkInstanceCreateInfo 的定义如下：

```c++
typedef struct VkInstanceCreateInfo {
    VkStructureType            sType;
    const void*                pNext;
    VkInstanceCreateFlags      flags;
    const VkApplicationInfo*   pApplicationInfo;
    uint32_t                   enabledLayerCount;
    const char* const*         ppEnabledLayerNames;
    uint32_t                   enabledExtensionCount;
    const char* const*         ppEnabledExtensionNames;
} VkInstanceCreateInfo;
```

其中的参数的意义分别是：

- flags用于指示实例的行为，目前只有一个可选值 `VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR`。
- pApplicationInfo 表示应用程序的一些信息，可以为NULL。对于这个东西的作用，vulkan reference的原话是：this information helps implementations recognize behavior inherent to classes of applications.
- enabledLayerCount 表示需要启动的全局layers数量。
- ppEnabledLayerNames 表示每个全局layer的名字，layers按照它们在数组中的位置决定加载顺序，第一个元素离应用程序最近，最后一个元素离驱动程序最近。
- enabledExtensionCount表示需要启动的全局扩展数量。
- ppEnabledExtensionsNames表示每个全局扩展的名字。由于Vulkan的跨平台属性，这里至少需要指定窗口系统对应的扩展。

其中，用于表示应用程序信息的VkApplicationInfo定义如下：

```c++
typedef struct VkApplicationInfo {
    VkStructureType   sType;
    const void*       pNext;
    const char*       pApplicationName;
    uint32_t          applicationVersion;
    const char*       pEngineName;
    uint32_t          engineVersion;
    uint32_t          apiVersion;
} VkApplicationInfo;
```

 除最后一个参数`apiVersion`之外，其他信息都是用户自行设置，目前`apiVersion`的值只能是`VK_API_VERSION_1_X`其中`X`只能选0、1、2、3。`applicationVersion`和`engineVersion`一般通过`VK_MAKE_VERSION(major, minor, patch)`宏命令设置。

Vulkan中的许多信息都是通过结构体而不是函数参数的形式来传递的（ps. 需要传递的参数太多了）。

指定了相关的所有信息之后，就可以调用`vkCreateInstance(&createInfo, nullptr, &instance)`，函数创建一个`VkInstance`，三个参数的意义分别是：

- 指向`VkInstanceCreateInfo`对象的指针
- 指向自定义内存分配的回调函数
- 指向存储新`VkInstance`对象的句柄

从1.3.216 vulkan SDK版本之后，必须开启`VK_KHR_PORTABILITY_ENUMERATION_EXTENSION`扩展并且显式指定`VkInstanceCreateInfo`中的`flags`属性（虽然只有一个值）。我们可以通过glfw提供的函数来获取glfw所需的全局扩展，若SDK不支持，则创建`VkInstance`失败。但有些情况下，有的扩展只是可选项，因此在指定扩展时，我们应该检查以下SDK所支持的扩展。

记得在合适的时候调用`vkDestoryInstance(instance, nullptr)`来显式销毁`VkInstance`实例。