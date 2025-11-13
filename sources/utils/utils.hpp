#ifndef UTILS_HPP
# define UTILS_HPP

#include "colors.hpp"

void	printSeparator( void );
void	setColor (const std::string color);
void	resetColor ( void );
void	printTestHeader(std::string test_name);
void	expect(const std::string &msg);
void 	waitForInput();
void	clearTerminal();

#endif
