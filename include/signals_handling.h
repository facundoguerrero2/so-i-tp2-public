#pragma once

/**
 * @brief sends to current foreground process a singal
 */
void signal_handler(int sig);
/**
 * @brief setup wich signals will be handled by signal_handler
 */
void setup_signal_handlers(void);
