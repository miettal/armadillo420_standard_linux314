
#ifndef _MX3_UART_H_
#define _MX3_UART_H_

/* UCR1 : UART Control Register 1 */
#define UCR1_UARTEN (BIT(0))

/* UCR2 : UART Control Register 2 */
#define UCR2_SRSTn (BIT(0))
#define UCR2_RXEN  (BIT(1))
#define UCR2_TXEN  (BIT(2))
#define UCR2_WS    (BIT(5))
#define UCR2_IRTS  (BIT(14))

/* UCR3 : UART Control Register 3 */
#define UCR3_RXDMUXSEL (BIT(2))
#define UCR3_RI        (BIT(8))
#define UCR3_DCD       (BIT(9))
#define UCR3_DSR       (BIT(10))

/* UCR4 : UART Control Register 4 */

/* USR1 : UART Status Register 1 */
#define USR1_RRDY (BIT(9))

/* USR2 : UART Status Register 2 */
#define USR2_TXDC (BIT(3))
#define USR2_TXFE (BIT(14))

#endif
