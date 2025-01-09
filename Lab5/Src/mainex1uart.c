#include "main.h"

//  pins and clock for the LED
#define LED2_PIN                         GPIO_PIN_14
#define LED2_GPIO_PORT                   GPIOB
#define LED2_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOB_CLK_ENABLE()
#define LED2_GPIO_CLK_DISABLE()          __HAL_RCC_GPIOB_CLK_DISABLE()

// pins and clocks for UART
#define DISCOVERY_COM1                          USART1
#define DISCOVERY_COM1_CLK_ENABLE()             __HAL_RCC_USART1_CLK_ENABLE()
#define DISCOVERY_COM1_CLK_DISABLE()            __HAL_RCC_USART1_CLK_DISABLE()

#define DISCOVERY_COM1_TX_PIN                   GPIO_PIN_6
#define DISCOVERY_COM1_TX_GPIO_PORT             GPIOB
#define DISCOVERY_COM1_TX_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOB_CLK_ENABLE()   
#define DISCOVERY_COM1_TX_GPIO_CLK_DISABLE()    __HAL_RCC_GPIOB_CLK_DISABLE()  
#define DISCOVERY_COM1_TX_AF                    GPIO_AF7_USART1

#define DISCOVERY_COM1_RX_PIN                   GPIO_PIN_7
#define DISCOVERY_COM1_RX_GPIO_PORT             GPIOB
#define DISCOVERY_COM1_RX_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOB_CLK_ENABLE()   
#define DISCOVERY_COM1_RX_GPIO_CLK_DISABLE()    __HAL_RCC_GPIOB_CLK_DISABLE()  
#define DISCOVERY_COM1_RX_AF                    GPIO_AF7_USART1

UART_HandleTypeDef hDiscoUart;

#define TX_BUFFER_SIZE 128
char tx_buffer[TX_BUFFER_SIZE]; // Circular buffer for TX
volatile int tx_head = 0; // Head index for TX buffer
volatile int tx_tail = 0; // Tail index for TX buffer

int ch;
volatile int done = 0;

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
HAL_StatusTypeDef COM_Init(UART_HandleTypeDef *);
int __io_putchar(int);

/* Function prototypes for circular buffer operations */
void enqueue_tx(char c);
int dequeue_tx(char *c);
int is_tx_buffer_empty(void);

int main(void)
{
    HAL_Init();
    SystemClock_Config();

    hDiscoUart.Instance = DISCOVERY_COM1;
    hDiscoUart.Init.BaudRate = 115200;
    hDiscoUart.Init.WordLength = UART_WORDLENGTH_8B;
    hDiscoUart.Init.StopBits = UART_STOPBITS_1;
    hDiscoUart.Init.Parity = UART_PARITY_NONE;
    hDiscoUart.Init.Mode = UART_MODE_TX_RX;
    hDiscoUart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    hDiscoUart.Init.OverSampling = UART_OVERSAMPLING_16;
    hDiscoUart.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    hDiscoUart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

    if (COM_Init(&hDiscoUart) == HAL_OK)
    {
        for (;;)
        {
            if (done == 1)
            {
                enqueue_tx(ch);
                done = 0;
            }
            if (done == -1)
            {
                enqueue_tx('E');
                done = 0;
            }
        }
    }
    else
    {
        enqueue_tx('o');
        enqueue_tx('h');
        enqueue_tx('!');
    }
}

/* System Clock Configuration */
static void SystemClock_Config(void)
{
    // Clock configuration remains unchanged
}

/* UART Initialization */
HAL_StatusTypeDef COM_Init(UART_HandleTypeDef *huart)
{
    GPIO_InitTypeDef gpio_init_structure;

    DISCOVERY_COM1_TX_GPIO_CLK_ENABLE();
    DISCOVERY_COM1_RX_GPIO_CLK_ENABLE();
    DISCOVERY_COM1_CLK_ENABLE();

    gpio_init_structure.Pin = DISCOVERY_COM1_TX_PIN;
    gpio_init_structure.Mode = GPIO_MODE_AF_PP;
    gpio_init_structure.Speed = GPIO_SPEED_FREQ_HIGH;
    gpio_init_structure.Pull = GPIO_NOPULL;
    gpio_init_structure.Alternate = DISCOVERY_COM1_TX_AF;
    HAL_GPIO_Init(DISCOVERY_COM1_TX_GPIO_PORT, &gpio_init_structure);

    gpio_init_structure.Pin = DISCOVERY_COM1_RX_PIN;
    gpio_init_structure.Mode = GPIO_MODE_AF_PP;
    gpio_init_structure.Alternate = DISCOVERY_COM1_RX_AF;
    HAL_GPIO_Init(DISCOVERY_COM1_RX_GPIO_PORT, &gpio_init_structure);

    huart->Instance = DISCOVERY_COM1;

    __HAL_UART_ENABLE_IT(huart, UART_IT_RXNE); // Enable RX interrupt
    __HAL_UART_ENABLE_IT(huart, UART_IT_TXE);  // Enable TX interrupt
    HAL_UART_Init(huart);

    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0U);
    HAL_NVIC_EnableIRQ(USART1_IRQn);

    return HAL_OK;
}

/* USART1 IRQ Handler */
void USART1_IRQHandler()
{
    if (__HAL_UART_GET_FLAG(&hDiscoUart, UART_FLAG_RXNE)) // RX interrupt
    {
        if (HAL_UART_Receive(&hDiscoUart, (uint8_t *)&ch, 1, 3000) == HAL_OK)
            done = 1;
        else
            done = -1;
    }

    if (__HAL_UART_GET_FLAG(&hDiscoUart, UART_FLAG_TXE)) // TX interrupt
    {
        char c;
        if (dequeue_tx(&c))
        {
            HAL_UART_Transmit(&hDiscoUart, (uint8_t *)&c, 1, 3000);
        }
        else
        {
            __HAL_UART_DISABLE_IT(&hDiscoUart, UART_IT_TXE); // Disable TX interrupt if buffer is empty
        }
    }

    HAL_UART_IRQHandler(&hDiscoUart);
}

/* TX Circular Buffer Operations */
void enqueue_tx(char c)
{
    int next = (tx_head + 1) % TX_BUFFER_SIZE;
    if (next != tx_tail) // Ensure buffer is not full
    {
        tx_buffer[tx_head] = c;
        tx_head = next;
        __HAL_UART_ENABLE_IT(&hDiscoUart, UART_IT_TXE); // Enable TX interrupt
    }
}

int dequeue_tx(char *c)
{
    if (tx_head == tx_tail) // Buffer is empty
        return 0;

    *c = tx_buffer[tx_tail];
    tx_tail = (tx_tail + 1) % TX_BUFFER_SIZE;
    return 1;
}

int is_tx_buffer_empty(void)
{
    return tx_head == tx_tail;
}

int __io_putchar(int c)
{
    enqueue_tx((char)c);
    return c;
}

