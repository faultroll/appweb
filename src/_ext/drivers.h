//----------------------------------------------------------------------------------------------------------------------------//
/// \file
/// \brief the head file of all drivers include in the project.
/// \details some definitions to access the drivers in the project.
///   the head file is used both in drivers c file and the user application c file
///


#ifndef _DRIVERS_H_
#define _DRIVERS_H_

////////////////////////////////////////////////////////////////////////////////////////////
/// \def DRIVER_PRINTK
/// \brief DRIVER_PRINTK would be used to debug the driver. DEBUG should be defined or undefined to control this.
#if defined(DEBUG)
#define DRIVER_PRINTK(fmt, argv...) printk(fmt,##argv)
#else
#define DRIVER_PRINTK(...)
#endif


////////////////////////////////////////////////////////////////////////////////////////////
/// \brief definitions of the names and major numbers of the devices
#define DEVICE_GPIO_NAME        "gpio_dev"
#define DEVICE_GPIO_MAJOR       230

#define DEVICE_JIFFIES_NAME     "jiffies_dev"
#define DEVICE_JIFFIES_MAJOR    231

#define DEVICE_XIF_NAME         "xif_dev"
#define DEVICE_XIF_MAJOR        232

#define DEVICE_RXIF_NAME        "rxif_dev"
#define DEVICE_RXIF_MAJOR       233

#define DEVICE_BUTTONS_NAME     "buttons_dev"
#define DEVICE_BUTTONS_MAJOR    234

#define DEVICE_LCD_NAME         "lcd_dev"
#define DEVICE_LCD_MAJOR        235

#define DEVICE_BEEP_NAME        "beep_dev"
#define DEVICE_BEEP_MAJOR       237

#define DEVICE_SHT11_NAME       "sht11_dev"
#define DEVICE_SHT11_MAJOR      239

////////////////////////////////////////////////////////////////////////////////////////////
/// Accessing to the gpio_dev
///
/// \details The driver would access all the GPIO pins of the CPU.
///

extern int fd_gpio; ///< the global file device to access to the gpio device

/// GPIO names
#define GPIO_PIN_PB(idx)    (0x20 + idx)
#define GPIO_PIN_PC(idx)    (0x40 + idx)
#define GPIO_PIN_PE(idx)    (0x80 + idx)
#define GPIO_PIN_PG(idx)    (0xC0 + idx)

/// Operations
#define GPIO_OP_INPUT       (1<<8)   // set the pin as input direction
#define GPIO_OP_OUTPUT      (2<<8)   // set the pin as output direction
#define GPIO_OP_WRITE       (3<<8)   // write the pin to the given value 0 or 1
#define GPIO_OP_SET         (4<<8)   // set the pin value to 1
#define GPIO_OP_CLR         (5<<8)   // clear the pin value to 0
#define GPIO_OP_GET         (6<<8)   // get the pin value
#define GPIO_OP_SWAP        (7<<8)   // swap the value of the pin
#define GPIO_OP_DELAY       (8<<8)   // Set the delay time between two operation, return the last delay value (unsigned int)

#define GPIO_TO_OP(op_name)     (op_name&(~0xFF))
#define GPIO_TO_NAME(op_name)   (op_name&0xFF)

/// -------------------------------------------------------------------
/// \def Access to GPIO, the file (device) fd_gpio should be given with a right value.
/// the secondary function of the pin is not implemented here.
/// \example example of the gpio
///    GPIO_INPUT(GPIO_PA(0),1);
///    GPIO_OUTPUT(GPIO_PA(0),1);
///    GPIO_SET(GPIO_PA(0));
///
#define GPIO_INPUT(name, pullup)    ioctl(fd_gpio, GPIO_OP_INPUT + name, pullup)
#define GPIO_OUTPUT(name, value)    ioctl(fd_gpio, GPIO_OP_OUTPUT + name, value)
#define GPIO_SET(name)              ioctl(fd_gpio, GPIO_OP_SET + name, 1)
#define GPIO_CLR(name)              ioctl(fd_gpio, GPIO_OP_CLR + name, 0)
#define GPIO_GET(name)              ioctl(fd_gpio, GPIO_OP_GET + name, 0)
#define GPIO_SWAP(name)             ioctl(fd_gpio, GPIO_OP_SWAP + name, 0)
#define GPIO_DELAY(delay)           ioctl(fd_gpio, GPIO_OP_DELAY + 0, delay)


////////////////////////////////////////////////////////////////////////////////////////////
/// Accessing to the buttons_dev
///
/// \details The driver would access the 6 buttons on board.
///

extern int fd_buttons; ///< the global file device to access to the buttons device

/// BUTTONS names
#define BUTTONS_KEY_NULL    0
#define BUTTONS_KEY_ESC     1
#define BUTTONS_KEY_RIGHT   2
#define BUTTONS_KEY_DOWN    3
#define BUTTONS_KEY_LEFT    4
#define BUTTONS_KEY_UP      5
#define BUTTONS_KEY_ENTER   6

/// Operations
// read the buttons pressed, if return value is BUTTONS_KEY_*, means the button BUTTONS_KEY_* is pressed
#define BUTTONS_READ()   ioctl(fd_buttons, 0, 0) // return value BUTTONS_KEY_*


////////////////////////////////////////////////////////////////////////////////////////////
/// Accessing to the lcd_dev
///
/// \details The driver would access the lcd on the board.
///

extern int fd_lcd; ///< the global file device to access to the lcd device

/// Operations
#define LCD_BACKLIGHT_OFF()  ioctl(fd_lcd, 0xAA, 0)
#define LCD_BACKLIGHT_ON()   ioctl(fd_lcd, 0x55, 0)

#define LCD_WRITE(msg,size)  write(fd_lcd, msg, size) ///< write data to display


////////////////////////////////////////////////////////////////////////////////////////////
/// Accessing to the beep_dev
///
/// \details The driver would access the beep on the board.
///

extern int fd_beep; ///< the global file device to access to the beep device

/// Values of beep,led,relay
#define BEEP_ON     1
#define BEEP_OFF    0

#define LED_ON      1
#define LED_OFF     0

#define RELAY_OPENED 1
#define RELAY_CLOSED 0

// Command
#define BEEP_SET_CMD (0+256)
#define RUN_LED_SET_CMD (1+256)
#define ERR_LED_SET_CMD (2+256)
#define RELAY_SET_CMD (3+256)
#define BEEP_GET_CMD (4+256)
#define RUN_LED_GET_CMD (5+256)
#define ERR_LED_GET_CMD (6+256)
#define RELAY_GET_CMD (7+256)

/// Operations
#define BEEP_SET(s) ioctl(fd_beep, BEEP_SET_CMD, s) // s := BEEP_ON/BEEP_OFF
#define RUN_LED_SET(s) ioctl(fd_beep, RUN_LED_SET_CMD, s) // s := LED_ON/LED_OFF
#define ERR_LED_SET(s) ioctl(fd_beep, ERR_LED_SET_CMD, s) // s := LED_ON/LED_OFF
#define RELAY_SET(s) ioctl(fd_beep, RELAY_SET_CMD, s) // s := RELAY_OPENED/RELAY_CLOSED

#define BEEP_GET() ioctl(fd_beep, BEEP_GET_CMD, 0) // return : BEEP_ON/BEEP_OFF
#define RUN_LED_GET() ioctl(fd_beep, RUN_LED_GET_CMD, 0) // return : LED_ON/LED_OFF
#define ERR_LED_GET() ioctl(fd_beep, ERR_LED_GET_CMD, 0) // return : LED_ON/LED_OFF
#define RELAY_GET() ioctl(fd_beep, RELAY_GET_CMD, 0) // return : RELAY_OPENED/RELAY_CLOSED

////////////////////////////////////////////////////////////////////////////////////////////
/// Accessing to the xif_dev
///
/// \details the file(device) would supply a method to xif_dev ( control interface )
///
#define XIF_FIFO_LENGTH    8 // must be 2^N bits !!!, 1,2,4,8,16,32..
#define XIF_MESSAGE_MAX_LENGTH 128

typedef struct Txif_dev_message
{
	unsigned short length; // length of message
	char data[XIF_MESSAGE_MAX_LENGTH - sizeof(unsigned short)];
	// the last data will always be set to '\0' by the driver xif_dev
} Txif_dev_message;

// Return value
// 0 when fail
// 1 when success
// NOTE: THE fd SHOULD BE SPECIFIED FIRST!!!
#define XIF_READ(fd, msg)   read(fd, msg, sizeof(Txif_dev_message))  ///< read Txif_dev_message to msg, length is specified by Txif_dev_message.length
// NOTE: a '\0' character should be appended to the message for string operation to avoid error.
#define XIF_WRITE(fd, msg)  write(fd, msg, sizeof(Txif_dev_message)) ///< write Txif_dev_message with msg, length is specified by Txif_dev_message.length
#define XIF_CLEAR(fd)       ioctl(fd, 1, 0) ///< clear the messages in buffer


#endif // _DRIVERS_H_
