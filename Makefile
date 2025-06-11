# === Configurações ===
SRC = test.c
RAWNAME = $(basename $(SRC))
TRACE_DIR = $(RAWNAME)-memorytraces
ABS_TRACE_DIR = $(shell realpath $(TRACE_DIR))
ULIMIT = --ulimit nofile=262144:262144

# === Alvo principal ===
all: check-env compile trace texttrace symbols
	@echo "✅ Tudo pronto! Resultados estão em $(TRACE_DIR)"

# === Checagem de ambiente ===
check-env:
	@command -v docker >/dev/null 2>&1 || { echo >&2 "❌ ERRO: Docker não está instalado ou não está no PATH."; exit 1; }
	@command -v gcc >/dev/null 2>&1 || { echo >&2 "❌ ERRO: gcc não está instalado ou não está no PATH."; exit 1; }
	@test -f $(SRC) || { echo >&2 "❌ ERRO: O arquivo '$(SRC)' não foi encontrado."; exit 1; }

# === Compilação do código fonte ===
compile:
	@echo "🔧 Compilando $(SRC)..."
	mkdir -p $(TRACE_DIR)
	cp $(SRC) $(TRACE_DIR)/
	gcc -o $(TRACE_DIR)/$(RAWNAME).out $(TRACE_DIR)/$(SRC)

# === Execução do Tracergrind ===
trace: compile
	@echo "🐾 Rodando tracergrind..."
	docker run --rm -it $(ULIMIT) -v $(ABS_TRACE_DIR):/home tracergrind \
		-d -d --tool=tracergrind \
		--output=/home/$(RAWNAME).trace \
		/home/$(RAWNAME).out | tee $(TRACE_DIR)/$(RAWNAME).logs

# === Conversão do trace para formato legível ===
texttrace: trace
	@echo "🧠 Convertendo .trace em .texttrace..."
	@if [ -f "$(TRACE_DIR)/$(RAWNAME).trace" ]; then \
		docker run --rm -it $(ULIMIT) -v $(ABS_TRACE_DIR):/home texttrace \
			/home/$(RAWNAME).trace /home/$(RAWNAME).texttrace; \
	else \
		echo "❌ ERRO: $(RAWNAME).trace não encontrado. Execute 'make trace' primeiro."; \
		exit 1; \
	fi

# === Extração de símbolos ELF do binário ===
symbols: compile
	@echo "📦 Extraindo símbolos com readelf..."
	readelf -Wa $(TRACE_DIR)/$(RAWNAME).out | grep -e .text -e main > $(TRACE_DIR)/$(RAWNAME).elf

# === Limpeza dos arquivos gerados ===
clean:
	@echo "🧹 Limpando arquivos gerados..."
	rm -rf $(TRACE_DIR)
	@echo "✔️ Limpeza completa."
