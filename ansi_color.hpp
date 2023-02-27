#define SET_GREEN		 "\033[32m"
#define SET_RED			 "\033[31m"
#define SET_LIGHT_YELLOW "\033[93m"
#define SET_YELLOW		 "\033[33m"
#define RESET_COLOR		 "\033[0m"

#define GREEN_TEXT(text)		SET_GREEN text RESET_COLOR
#define RED_TEXT(text)			SET_RED text RESET_COLOR
#define LIGHT_YELLOW_TEXT(text) SET_LIGHT_YELLOW text RESET_COLOR
#define YELLOW_TEXT(text)		SET_YELLOW text RESET_COLOR

#define GREEN_OUTPUT(something)		   SET_GREEN << something << RESET_COLOR
#define RED_OUTPUT(something)		   SET_RED << something << RESET_COLOR
#define LIGHT_YELLOW_OUTPUT(something) SET_LIGHT_YELLOW << something << RESET_COLOR
#define YELLOW_OUTPUT(something)	   SET_YELLOW << something << RESET_COLOR
