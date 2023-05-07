#define SET_GREEN		 "\033[32m"
#define SET_RED			 "\033[31m"
#define SET_LIGHT_YELLOW "\033[93m"
#define SET_YELLOW		 "\033[33m"
#define SET_BLUE		 "\033[34m"
#define SET_PURPLE		 "\033[35m"
#define SET_CYAN		 "\033[36m"
#define SET_WHITE		 "\033[37m"
#define SET_GRAY		 "\033[90m"
#define RESET_COLOR		 "\033[0m"

#define GREEN_TEXT(text)		SET_GREEN text RESET_COLOR
#define RED_TEXT(text)			SET_RED text RESET_COLOR
#define LIGHT_YELLOW_TEXT(text) SET_LIGHT_YELLOW text RESET_COLOR
#define YELLOW_TEXT(text)		SET_YELLOW text RESET_COLOR
#define BLUE_TEXT(text)			SET_BLUE text RESET_COLOR
#define PURPLE_TEXT(text)		SET_PURPLE text RESET_COLOR
#define CYAN_TEXT(text)			SET_CYAN text RESET_COLOR
#define GRAY_TEXT(text)			SET_GRAY text RESET_COLOR
#define WHITE_TEXT(text)		SET_WHITE text RESET_COLOR

#define GREEN_OUTPUT(something)		   SET_GREEN << something << RESET_COLOR
#define RED_OUTPUT(something)		   SET_RED << something << RESET_COLOR
#define LIGHT_YELLOW_OUTPUT(something) SET_LIGHT_YELLOW << something << RESET_COLOR
#define YELLOW_OUTPUT(something)	   SET_YELLOW << something << RESET_COLOR
#define BLUE_OUTPUT(something)		   SET_BLUE << something << RESET_COLOR
#define PURPLE_OUTPUT(something)	   SET_PURPLE << something << RESET_COLOR
#define CYAN_OUTPUT(something)		   SET_CYAN << something << RESET_COLOR
#define GRAY_OUTPUT(something)		   SET_GRAY << something << RESET_COLOR
#define WHITE_OUTPUT(something)		   SET_WHITE << something << RESET_COLOR


#define SET_BOLD		 "\033[1m"
#define UNSET_BOLD       "\033[22m"
#define SET_UNDERLINE	 "\033[4m"
#define UNSET_UNDERLINE  "\033[24m"
#define SET_BLINK		 "\033[5m"
#define UNSER_BLINK		 "\033[25m"
#define SET_REVERSE		 "\033[7m"
#define UNSET_REVERSE	 "\033[27m"

#define BOLD_TEXT(text)			SET_BOLD text UNSET_BOLD
#define UNDERLINE_TEXT(text)	SET_UNDERLINE text UNSET_UNDERLINE
#define BLINK_TEXT(text)		SET_BLINK text UNSER_BLINK
#define REVERSE_TEXT(text)		SET_REVERSE text UNSET_REVERSE

#define BOLD_OUTPUT(something)		SET_BOLD << something << UNSET_BOLD
#define UNDERLINE_OUTPUT(something) SET_UNDERLINE << something << UNSET_UNDERLINE
#define BLINK_OUTPUT(something)		SET_BLINK << something << UNSER_BLINK
#define REVERSE_OUTPUT(something)	SET_REVERSE << something << UNSET_REVERSE
