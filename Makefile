# Neo6502Poker — moteur C testé en natif, binaire Neo6502 via llvm-mos.
LLVM_MOS ?= $(HOME)/llvm-mos
NEO_CC   := $(LLVM_MOS)/bin/mos-neo6502-clang
HOST_CC  ?= gcc
BUILD    := build

ENGINE_SRC := src/engine/cards.c src/engine/hand.c src/engine/videopoker.c
NEO_SRC    := src/neo/main.c src/neo/display.c

.PHONY: all neo test-engine test clean

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
