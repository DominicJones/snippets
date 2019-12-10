PARSER_NAME := drvExpression
PARSER_SOURCES := $(PARSER_NAME).lex.cpp $(PARSER_NAME).tab.cpp

# build flex/bison parser on demand
.PHONY: parser
parser:
	bison --defines=$(PARSER_NAME).tab.h --file-prefix=$(PARSER_NAME) $(PARSER_NAME).ypp
	flex --nounistd --always-interactive --header-file=$(PARSER_NAME).lex.h --outfile=$(PARSER_NAME).lex.cpp $(PARSER_NAME).lpp

.PHONY: clean_parser
clean_parser:
	@rm -f $(PARSER_NAME).tab.* $(PARSER_NAME).lex.*
