SRC=exemplo.c
RAWNAME=$(basename $(SRC) .c)
TRACE_DIR=$(RAWNAME)-traces

all: compile trace texttrace symbols

compile:
	mkdir -p $(TRACE_DIR)
	cp $(SRC) $(TRACE_DIR)/
	gcc -o $(TRACE_DIR)/$(RAWNAME).out $(TRACE_DIR)/$(SRC)

trace:
	docker run --rm -it -v $(TRACE_DIR):/home tracergrind -d -d --tool=tracergrind --output=/home/$(RAWNAME).trace /home/$(RAWNAME).out | tee $(TRACE_DIR)/$(RAWNAME).logs

texttrace:
	docker run --rm -it -v $(TRACE_DIR):/home texttrace $(RAWNAME).trace $(RAWNAME).texttrace

symbols:
	readelf -Wa $(TRACE_DIR)/$(RAWNAME).out | grep -e .text -e main > $(TRACE_DIR)/$(RAWNAME).elf
