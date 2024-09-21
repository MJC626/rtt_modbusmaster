#include <board.h>
#include <rtthread.h>
#include <rtdevice.h>
#include <agile_modbus.h>
#include <agile_modbus_rtu.h>
#include "uart_rtu.h"

// 线程优先级
#define THREAD_PRIORITY         25
// 线程堆栈大小
#define THREAD_STACK_SIZE       1024
// 线程时间片大小
#define THREAD_TIMESLICE        5

static rt_thread_t modbus_thread = RT_NULL;  // Modbus线程句柄

// Modbus主机发送和接收缓冲区
static uint8_t master_send_buf[AGILE_MODBUS_MAX_ADU_LENGTH];
static uint8_t master_recv_buf[AGILE_MODBUS_MAX_ADU_LENGTH];
// 保存寄存器值的缓冲区
static uint16_t hold_register[40];
static uint16_t input_register[40];
// Modbus线程入口函数
static void modbus_thread_entry(void *parameter)
{
    agile_modbus_rtu_t ctx_rtu;  // Modbus RTU上下文结构体
    agile_modbus_t *ctx = &ctx_rtu._ctx;  // 通用Modbus上下文结构体指针
    agile_modbus_rtu_init(&ctx_rtu, master_send_buf, sizeof(master_send_buf), master_recv_buf, sizeof(master_recv_buf));  // 初始化Modbus RTU
    agile_modbus_set_slave(ctx, 1);  // 设置从设备地址

    int request_counter = 0;  // 请求计数器

        while (1)
        {
            rt_thread_mdelay(100);  // 延迟100毫秒

            switch (request_counter)
            {
                case 0:
                {
                    int send_len = agile_modbus_serialize_read_registers(ctx, 0, 10);  // 序列化读取保持寄存器（起始地址0）的请求
                    send_data(ctx->send_buf, send_len);  // 发送请求数据
                    int read_len = receive_data(ctx->read_buf, ctx->read_bufsz, 1000, 20);  // 接收响应数据

                    if (read_len == 0)
                    {
                        rt_kprintf("1Receive timeout.\n");  // 打印接收超时信息
                        break;
                    }

                    int rc = agile_modbus_deserialize_read_registers(ctx, read_len, hold_register);  // 反序列化读取保持寄存器的响应
                    if (rc < 0)
                    {
                        rt_kprintf("1Receive failed.\n");  // 打印接收失败信息
                        if (rc != -1)
                            rt_kprintf("Error code:%d\n", -128 - rc);  // 打印错误代码
                        break;
                    }

                    rt_kprintf("Hold Registers (Start Address 0):\n");
                    for (int i = 0; i < 10; i++)
                        rt_kprintf("Registers: [%d]: 0x%04X\n", i, hold_register[i]);  // 打印保持寄存器值
                    break;
                }
                case 1:
                {
                    int send_len = agile_modbus_serialize_read_registers(ctx, 20, 10);  // 序列化读取保持寄存器（起始地址10）的请求
                    send_data(ctx->send_buf, send_len);  // 发送请求数据
                    int read_len = receive_data(ctx->read_buf, ctx->read_bufsz, 1000, 20);  // 接收响应数据

                    if (read_len == 0)
                    {
                        rt_kprintf("2Receive timeout.\n");  // 打印接收超时信息
                        break;
                    }

                    int rc = agile_modbus_deserialize_read_registers(ctx, read_len, hold_register);  // 反序列化读取保持寄存器的响应
                    if (rc < 0)
                    {
                        rt_kprintf("2Receive failed.\n");  // 打印接收失败信息
                        if (rc != -1)
                            rt_kprintf("Error code:%d\n", -128 - rc);  // 打印错误代码
                        break;
                    }

                    rt_kprintf("Hold Registers (Start Address 20):\n");
                    for (int i = 0; i < 10; i++)
                        rt_kprintf("Registers: [%d]: 0x%04X\n", i, hold_register[i]);  // 打印保持寄存器值
                    break;
                }
                case 2:
                {
                    int send_len = agile_modbus_serialize_read_input_registers(ctx, 0, 10);  // 序列化读取输入寄存器（起始地址0）的请求
                    send_data(ctx->send_buf, send_len);  // 发送请求数据
                    int read_len = receive_data(ctx->read_buf, ctx->read_bufsz, 1000, 20);  // 接收响应数据

                    if (read_len == 0)
                    {
                        rt_kprintf("3Receive timeout.\n");  // 打印接收超时信息
                        break;
                    }

                    int rc = agile_modbus_deserialize_read_input_registers(ctx, read_len, input_register);  // 反序列化读取输入寄存器的响应
                    if (rc < 0)
                    {
                        rt_kprintf("3Receive failed.\n");  // 打印接收失败信息
                        if (rc != -1)
                            rt_kprintf("Error code:%d\n", -128 - rc);  // 打印错误代码
                        break;
                    }

                    rt_kprintf("Input Registers (Start Address 0):\n");
                    for (int i = 0; i < 10; i++)
                        rt_kprintf("Registers: [%d]: 0x%04X\n", i, input_register[i]);  // 打印输入寄存器值
                    break;
                }
                case 3:
                {
                    int send_len = agile_modbus_serialize_read_input_registers(ctx, 20, 10);  // 序列化读取输入寄存器（起始地址10）的请求
                    send_data(ctx->send_buf, send_len);  // 发送请求数据
                    int read_len = receive_data(ctx->read_buf, ctx->read_bufsz, 1000, 20);  // 接收响应数据

                    if (read_len == 0)
                    {
                        rt_kprintf("4Receive timeout.\n");  // 打印接收超时信息
                        break;
                    }

                    int rc = agile_modbus_deserialize_read_input_registers(ctx, read_len, input_register);  // 反序列化读取输入寄存器的响应
                    if (rc < 0)
                    {
                        rt_kprintf("4Receive failed.\n");  // 打印接收失败信息
                        if (rc != -1)
                            rt_kprintf("Error code:%d\n", -128 - rc);  // 打印错误代码
                        break;
                    }

                    rt_kprintf("Input Registers (Start Address 20):\n");
                    for (int i = 0; i < 10; i++)
                        rt_kprintf("Registers: [%d]: 0x%04X\n", i, input_register[i]);  // 打印输入寄存器值
                    break;
                }
            }

            request_counter = (request_counter + 1) % 4;  // 更新请求计数器，以循环发送四个不同的请求

            rt_kprintf("\n");
        }
}

// 启动Modbus任务的函数
static int modbus_task_start(void)
{
    if (uart_init() != RT_EOK)  // 初始化UART
    {
        rt_kprintf("UART initialization failed.\n");  // 打印UART初始化失败信息
        return -RT_ERROR;
    }

    modbus_thread = rt_thread_create("modbus",  // 创建Modbus线程
                                     modbus_thread_entry,
                                     RT_NULL,
                                     THREAD_STACK_SIZE,
                                     THREAD_PRIORITY,
                                     THREAD_TIMESLICE);

    if (modbus_thread != RT_NULL)
    {
        rt_thread_startup(modbus_thread);  // 启动Modbus线程
        rt_kprintf("Modbus task started.\n");  // 打印Modbus任务启动信息
        return RT_EOK;
    }
    else
    {
        rt_kprintf("Failed to create Modbus task.\n");  // 打印创建Modbus任务失败信息
        return -RT_ERROR;
    }
}

// 停止Modbus任务的函数
static int modbus_task_stop(void)
{
    if (modbus_thread)
    {
        rt_thread_delete(modbus_thread);  // 删除Modbus线程
        modbus_thread = RT_NULL;
        rt_kprintf("Modbus task stopped.\n");  // 打印Modbus任务停止信息
        return RT_EOK;
    }
    else
    {
        rt_kprintf("Modbus task is not running.\n");  // 打印Modbus任务未运行信息
        return -RT_ERROR;
    }
}

// 主函数
int main(void)
{
    const char *device_name = "vcom";
    rt_device_t dev = rt_device_find(device_name);  // 查找设备
    if (dev) {
        rt_console_set_device(device_name);  // 设置控制台设备
    } else {
        rt_kprintf("Failed to find vcom device\n");  // 打印未找到设备信息
    }
    return RT_EOK;
}

// MSH命令
MSH_CMD_EXPORT(modbus_task_start, Start Modbus);  // 导出启动Modbus任务命令
MSH_CMD_EXPORT(modbus_task_stop, Start Modbus);  // 导出停止Modbus任务命令
