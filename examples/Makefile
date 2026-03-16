# Filter out the unwanted MAKEFLAGS
.PHONY: all clean

all:
	$(MAKE) -C deleteGen3 $(MAKECMDGOALS) $(MAKEOVERRIDES)
	$(MAKE) -C injectGen3 $(MAKECMDGOALS) $(MAKEOVERRIDES)
	$(MAKE) -C unlockNationalDex $(MAKECMDGOALS) $(MAKEOVERRIDES)
	$(MAKE) -C isNationalDexUnlocked $(MAKECMDGOALS) $(MAKEOVERRIDES)
	$(MAKE) -C scratch $(MAKECMDGOALS) $(MAKEOVERRIDES)

clean:
	$(MAKE) -C deleteGen3 clean
	$(MAKE) -C injectGen3 clean
	$(MAKE) -C unlockNationalDex clean
	$(MAKE) -C isNationalDexUnlocked clean
	$(MAKE) -C scratch clean	
