#include "main.h"
#include <string.h>
#include <stdlib.h>

// pins and clock for the LED
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

// TX and RX buffer configurations
#define BUFFER_SIZE 20
char tx_buffer[BUFFER_SIZE];
char rx_buffer[BUFFER_SIZE];
volatile int tx_head = 0, tx_tail = 0; // TX buffer indices
volatile int rx_head = 0, rx_tail = 0; // RX buffer indices

/* Function Prototypes */
static void SystemClock_Config(void);
HAL_StatusTypeDef COM_Init(UART_HandleTypeDef *);
int __io_putchar(int c);
int __io_getchar(void);
void enqueue_tx(char c);
int dequeue_tx(char *c);
int is_tx_buffer_empty(void);
void enqueue_rx(char c);
int dequeue_rx(char *c);
int is_rx_buffer_empty(void);
void LED2_Init(void);
void LED2_Toggle(void);
void BSP_COM_Init(UART_HandleTypeDef *huart);

int main(void)
{
    /* STM32L4xx HAL library initialization */
    HAL_Init();
    SystemClock_Config();
    LED2_Init();
    BSP_COM_Init(&hDiscoUart);

    srand(HAL_GetTick()); // Seed random number generator with system tick

    printf("Welcome to the Math Quiz!\n\n");

    int correct_answers = 0;
    int total_questions = 5; // Number of questions to ask
    int user_answer, num1, num2, correct_answer;
    char buffer[10]; // Buffer to hold user input

    for (int i = 0; i < total_questions; i++)
    {
        num1 = rand() % 10 + 1;
        num2 = rand() % 10 + 1;
        correct_answer = num1 + num2;

        while (1)
        {
            printf("Question %d: What is %d + %d?\n", i + 1, num1, num2);
            printf("Your answer: ");
            fflush(stdout);

            memset(buffer, 0, sizeof(buffer));
            fgets(buffer, sizeof(buffer), stdin);

            if (sscanf(buffer, "%d", &user_answer) == 1)
            {
                if (user_answer == correct_answer)
                {
                    printf("Correct!\n\n");
                    correct_answers++;
                }
                else
                {
                    printf("Wrong! The correct answer is %d.\n\n", correct_answer);
                }
                break;
            }
            else
            {
                printf("Invalid input. Please enter a valid number.\n\n");
            }
        }
    }

    printf("Quiz complete!\n");
    printf("You got %d out of %d questions correct.\n", correct_answers, total_questions);

    while (1)
    {
        LED2_Toggle(); // Toggle the LED for visual confirmation
        HAL_Delay(500); // Delay for 500ms
    }
}

/* Function Implementations */
void LED2_Init(void)
{
    GPIO_InitTypeDef gpio_init_structure;

    LED2_GPIO_CLK_ENABLE();
    gpio_init_structure.Pin = LED2_PIN;
    gpio_init_structure.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_structure.Pull = GPIO_NOPULL;
    gpio_init_structure.Speed = GPIO_SPEED_FREQ_HIGH;

    HAL_GPIO_Init(LED2_GPIO_PORT, &gpio_init_structure);
}

void LED2_Toggle(void)
{
    HAL_GPIO_TogglePin(LED2_GPIO_PORT, LED2_PIN);
}

void BSP_COM_Init(UART_HandleTypeDef *huart)
{
    COM_Init(huart);
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
        char c;
        if (HAL_UART_Receive(&hDiscoUart, (uint8_t *)&c, 1, 3000) == HAL_OK)
        {
            enqueue_rx(c);
        }
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
    int next = (tx_head + 1) % BUFFER_SIZE;
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
    tx_tail = (tx_tail + 1) % BUFFER_SIZE;
    return 1;
}

/* RX Circular Buffer Operations */
void enqueue_rx(char c)
{
    int next = (rx_head + 1) % BUFFER_SIZE;
    if (next != rx_tail) // Ensure buffer is not full
    {
        rx_buffer[rx_head] = c;
        rx_head = next;
    }
}

int dequeue_rx(char *c)
{
    if (rx_head == rx_tail) // Buffer is empty
        return 0;

    *c = rx_buffer[rx_tail];
    rx_tail = (rx_tail + 1) % BUFFER_SIZE;
    return 1;
}

/* Custom getchar and putchar */
int __io_getchar(void)
{
    char c;
    while (!dequeue_rx(&c)) // Wait for data
        ;
    return c;
}

int __io_putchar(int c)
{
    enqueue_tx((char)c);
    return c;
}

/* System Clock Configuration */
static void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /* Configure the main internal regulator output voltage */
    __HAL_RCC_PWR_CLK_ENABLE();
    HAL_PWR_EnableBkUpAccess();
    HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /* Initialize the CPU, AHB, and APB busses clocks */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
    RCC_OscInitStruct.MSIState = RCC_MSI_ON;
    RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;  // MSI clock = 4 MHz
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
    RCC_OscInitStruct.PLL.PLLM = 1;
    RCC_OscInitStruct.PLL.PLLN = 40; // Multiply by 40
    RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2; // Divide by 2
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
    RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        /* Initialization Error */
        while (1);
    }

    /* Configure the system clock source, AHB/APB prescalers */
    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | 
                                   RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
    {
        /* Initialization Error */
        while (1);
    }
}

