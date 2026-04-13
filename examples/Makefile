# Filter out the unwanted MAKEFLAGS
.PHONY: all clean

all:
	$(MAKE) -C deleteGen3 $(MAKECMDGOALS) $(MAKEOVERRIDES)
	$(MAKE) -C injectGen3 $(MAKECMDGOALS) $(MAKEOVERRIDES)
	$(MAKE) -C unlockNationalDex $(MAKECMDGOALS) $(MAKEOVERRIDES)
	$(MAKE) -C isNationalDexUnlocked $(MAKECMDGOALS) $(MAKEOVERRIDES)
	$(MAKE) -C unlockMysteryEvent $(MAKECMDGOALS) $(MAKEOVERRIDES)
	$(MAKE) -C isMysteryEventUnlocked $(MAKECMDGOALS) $(MAKEOVERRIDES)
	$(MAKE) -C unlockMysteryGift $(MAKECMDGOALS) $(MAKEOVERRIDES)
	$(MAKE) -C isMysteryGiftUnlocked $(MAKECMDGOALS) $(MAKEOVERRIDES)
	$(MAKE) -C scratch $(MAKECMDGOALS) $(MAKEOVERRIDES)

clean:
	$(MAKE) -C deleteGen3 clean
	$(MAKE) -C injectGen3 clean
	$(MAKE) -C unlockNationalDex clean
	$(MAKE) -C isNationalDexUnlocked clean
	$(MAKE) -C unlockMysteryEvent clean
	$(MAKE) -C isMysteryEventUnlocked clean
	$(MAKE) -C unlockMysteryGift clean
	$(MAKE) -C isMysteryGiftUnlocked clean
	$(MAKE) -C scratch clean	
