#include "hw/sysbus.h"
#include "qemu/osdep.h"
#include "qemu/log.h"

// 定义寄存器偏移
#define CONTROL_REG 0x00
#define STATUS_REG  0x04

// 定义外设状态
typedef struct {
    uint32_t control;
    uint32_t status;
} SimpleDeviceState;

typedef struct {
    SysBusDevice parent_obj;
    SimpleDeviceState state; // 外设寄存器
} SimpleDevice;


// 读取回调函数
static uint64_t simple_device_read(void *opaque, hwaddr offset, unsigned size)
{
    SimpleDevice *dev = opaque;
    switch (offset) {
    case CONTROL_REG:
        qemu_log_mask(LOG_UNIMP, "Control register is write-only!\n");
        return 0;
    case STATUS_REG:
        return dev->state.status;
    default:
        qemu_log_mask(LOG_UNIMP, "Invalid read offset: 0x%lx\n", offset);
        return 0;
    }
}

// 写入回调函数
static void simple_device_write(void *opaque, hwaddr offset, uint64_t value, unsigned size)
{
    SimpleDevice *dev = opaque;
    switch (offset) {
    case CONTROL_REG:
        dev->state.control = value;
        qemu_log_mask(LOG_UNIMP, "Control register written: 0x%lx\n", value);
        break;
    default:
        qemu_log_mask(LOG_UNIMP, "Invalid write offset: 0x%lx\n", offset);
        break;
    }
}

// 内存区域映射
static const MemoryRegionOps simple_device_ops = {
    .read = simple_device_read,
    .write = simple_device_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

// 外设初始化
static void simple_device_init(Object *obj)
{
    SimpleDevice *dev = SIMPLE_DEVICE(obj);

    // 初始化寄存器
    dev->state.control = 0;
    dev->state.status = 0;

    // 创建 MMIO 区域
    memory_region_init_io(&dev->parent_obj.mmio,
                          obj, &simple_device_ops,
                          dev, "simple_device", 0x1000);
    sysbus_init_mmio(SYS_BUS_DEVICE(obj), &dev->parent_obj.mmio);
}

// 类型信息定义
static const TypeInfo simple_device_info = {
    .name = "simple-device",
    .parent = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(SimpleDevice),
    .instance_init = simple_device_init,
};

static void simple_device_register_types(void)
{
    type_register_static(&simple_device_info);
}

type_init(simple_device_register_types);

