#include <board.h>
#include <rtthread.h>
#include <rtdevice.h>
#include "uart_rtu.h"

static rt_device_t uart_dev = RT_NULL;
static rt_sem_t _rx_sem = RT_NULL;

// 发送数据
int send_data(uint8_t *buf, int len)
{
    rt_device_write(uart_dev, 0, buf, len);
    return len;
}

// 接收数据
int receive_data(uint8_t *buf, int bufsz, int timeout, int bytes_timeout)
{
    int len = 0;

    while(1)
    {
        rt_sem_control(_rx_sem, RT_IPC_CMD_RESET, RT_NULL);

        int rc = rt_device_read(uart_dev, 0, buf + len, bufsz);
        if(rc > 0)
        {
            timeout = bytes_timeout;
            len += rc;
            bufsz -= rc;
            if(bufsz == 0)
                break;

            continue;
        }

        if(rt_sem_take(_rx_sem, rt_tick_from_millisecond(timeout)) != RT_EOK)
            break;
        timeout = bytes_timeout;
    }

    return len;
}

// 串口回调
static rt_err_t uart_callback(rt_device_t dev, rt_size_t size)
{
    rt_sem_release(_rx_sem);
    return RT_EOK;
}

// 串口初始化
int uart_init(void)
{
    _rx_sem = rt_sem_create("uart2", 0, RT_IPC_FLAG_FIFO);
    if(_rx_sem == RT_NULL)
    {
        rt_kprintf("create rx_sem failed.");
        return -RT_ERROR;
    }
    uart_dev = rt_device_find("uart2");
    if (uart_dev == RT_NULL)
    {
        rt_kprintf("can't find device uart2.");
        rt_sem_delete(_rx_sem);
        return -RT_ERROR;
    }
    else {
        rt_device_open(uart_dev, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX);
        rt_device_control(uart_dev, RT_DEVICE_CTRL_CONFIG, (void *)RT_SERIAL_CONFIG_DEFAULT);
        rt_device_set_rx_indicate(uart_dev, uart_callback);
    }

    return RT_EOK;
}
