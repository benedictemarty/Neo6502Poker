# Neo6502Poker — moteur C testé en natif, binaire Neo6502 via llvm-mos.
LLVM_MOS ?= $(HOME)/llvm-mos
NEO_CC   := $(LLVM_MOS)/bin/mos-neo6502-clang
HOST_CC  ?= gcc
BUILD    := build

ENGINE_SRC := src/engine/cards.c src/engine/hand.c src/engine/videopoker.c
NEO_SRC    := src/neo/main.c src/neo/display.c

.PHONY: all neo test-engine test clean run run-emu shot

all: neo

# --- binaire Neo6502 ---------------------------------------------------------
neo: $(BUILD)/poker.neo

$(BUILD)/poker.neo: $(ENGINE_SRC) $(NEO_SRC) src/engine/*.h src/neo/*.h
	@mkdir -p $(BUILD)
	$(NEO_CC) -Os -Wall -Isrc -o $@ $(ENGINE_SRC) $(NEO_SRC)
	@ls -l $@

# --- tests natifs du moteur ---------------------------------------------------
$(BUILD)/test_hand: $(ENGINE_SRC) tests/c/test_hand.c src/engine/*.h
	@mkdir -p $(BUILD)
	$(HOST_CC) -O2 -Wall -Wextra -Isrc -o $@ $(ENGINE_SRC) tests/c/test_hand.c

$(BUILD)/test_videopoker: $(ENGINE_SRC) tests/c/test_videopoker.c src/engine/*.h
	@mkdir -p $(BUILD)
	$(HOST_CC) -O2 -Wall -Wextra -Isrc -o $@ $(ENGINE_SRC) tests/c/test_videopoker.c

test-engine: $(BUILD)/test_hand $(BUILD)/test_videopoker
	@$(BUILD)/test_hand
	@$(BUILD)/test_videopoker

test: test-engine
	python3 -m pytest -q tests

clean:
	rm -rf $(BUILD)

# --- émulateurs -----------------------------------------------------------------
# run  : émulateur officiel `neo` (fenêtre SDL2, jouable) ; le stockage est ./storage relatif au cwd
# shot : Phosphoneo (headless, captures) — utilisé aussi par tests/test_emulator.py
NEO_EMU ?= $(HOME)/Neo6502firmware/bin/neo
EMU     ?= $(HOME)/Phosphoneo/build/phosphoneo
SD      := $(BUILD)/sd

run: $(BUILD)/poker.neo
	@mkdir -p $(BUILD)/storage && cp assets/cards/48x64/cards.bin $(BUILD)/storage/
	cd $(BUILD) && $(NEO_EMU) poker.neo

run-emu: run

shot: $(BUILD)/poker.neo
	@mkdir -p $(SD) && cp assets/cards/48x64/cards.bin $(SD)/
	$(EMU) $(BUILD)/poker.neo --headless --storage $(SD) --cycles 60000000 --type-keys '6000000:MMD' --screenshot $(BUILD)/shot.ppm
