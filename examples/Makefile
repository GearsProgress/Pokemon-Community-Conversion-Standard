# Filter out the unwanted MAKEFLAGS
.PHONY: all clean

all:
	$(MAKE) -C injectGen3 $(MAKECMDGOALS) $(MAKEOVERRIDES)
	$(MAKE) -C scratch $(MAKECMDGOALS) $(MAKEOVERRIDES)

clean:
	$(MAKE) -C injectGen3 clean
	$(MAKE) -C scratch clean	
