#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

typedef enum
{
    LOG_TYPE_UNKNOWN,
    LOG_TYPE_LOAD,
    LOG_TYPE_BLOCK,
    LOG_TYPE_MEMORY
} LogType;

typedef struct
{
    int index;
    char file_name[256];
    uint64_t from_address;
    uint64_t to_address;
} LoadLog;

typedef struct
{
    int index;
    char id[32];
    uint64_t thread_id;
    uint64_t start_address;
    uint64_t end_address;
} BlockLog;

typedef struct
{
    int index;
    char exec_id[32];
    uint64_t instruction_address;
    uint64_t start_address;
    int length;
    char mode;
    uint64_t data;
} MemoryLog;

LoadLog load_main_log;
int load_main_log_found = 0; // flag para saber se foi definido

uint64_t endereco_main_carregado = 0;

#define INITIAL_CAPACITY 1000

// LoadLogs
LoadLog *load_logs = NULL;
size_t load_log_count = 0;
size_t load_log_capacity = 0;

// BlockLogs
BlockLog *block_logs = NULL;
size_t block_log_count = 0;
size_t block_log_capacity = 0;

// MemoryLogs
MemoryLog *memory_logs = NULL;
size_t memory_log_count = 0;
size_t memory_log_capacity = 0;

void add_load_log(LoadLog log)
{
    if (load_log_count >= load_log_capacity)
    {
        load_log_capacity = (load_log_capacity == 0) ? INITIAL_CAPACITY : load_log_capacity * 2;
        load_logs = realloc(load_logs, load_log_capacity * sizeof(LoadLog));
    }
    load_logs[load_log_count++] = log;
}

void add_block_log(BlockLog log)
{
    if (block_log_count >= block_log_capacity)
    {
        block_log_capacity = (block_log_capacity == 0) ? INITIAL_CAPACITY : block_log_capacity * 2;
        block_logs = realloc(block_logs, block_log_capacity * sizeof(BlockLog));
    }
    block_logs[block_log_count++] = log;
}

void add_memory_log(MemoryLog log)
{
    if (memory_log_count >= memory_log_capacity)
    {
        memory_log_capacity = (memory_log_capacity == 0) ? INITIAL_CAPACITY : memory_log_capacity * 2;
        memory_logs = realloc(memory_logs, memory_log_capacity * sizeof(MemoryLog));
    }
    memory_logs[memory_log_count++] = log;
}

char get_log_type(const char *line)
{
    if (line[0] == '[' && line[2] == ']')
    {
        return line[1]; // 'L', 'M', 'B'
    }
    return '\0';
}

int parse_load_log(const char *line, int index, LoadLog *log)
{
    const char *pattern = "[L] Loaded %255s from 0x%" SCNx64 " to 0x%" SCNx64;
    int matched = sscanf(line, pattern, log->file_name, &log->from_address, &log->to_address);
    if (matched == 3)
    {
        log->index = index;

        // Verifica se é a linha principal (carregamento do binário do .elf)
        if (!load_main_log_found && strncmp(log->file_name, "/home/", 6) == 0)
        {
            load_main_log = *log; // salva na variável global
            load_main_log_found = 1;
        }

        // Adiciona à lista global de LoadLogs
        add_load_log(*log);

        return 1;
    }
    return 0;
}

int parse_block_log(const char *line, int index, BlockLog *log)
{
    uint64_t thread_id, start_address, end_address;
    int matched = sscanf(line, "[B] EXEC_ID: %31s THREAD_ID: %" SCNx64 " START_ADDRESS: %" SCNx64 " END_ADDRESS: %" SCNx64,
                         log->id, &thread_id, &start_address, &end_address);
    if (matched == 4)
    {
        log->index = index;
        log->thread_id = thread_id;
        log->start_address = start_address;
        log->end_address = end_address;

        // Adiciona à lista global de BlockLogs
        add_block_log(*log);

        return 1;
    }
    return 0;
}

int parse_memory_log(const char *line, int index, MemoryLog *log)
{
    uint64_t ins_addr, start_addr, data;
    int matched = sscanf(line, "[M] EXEC_ID: %31s INS_ADDRESS: %" SCNx64 " START_ADDRESS: %" SCNx64 " LENGTH: %d MODE: %c DATA: %" SCNx64,
                         log->exec_id, &ins_addr, &start_addr, &log->length, &log->mode, &data);
    if (matched == 6)
    {
        log->index = index;
        log->instruction_address = ins_addr;
        log->start_address = start_addr;
        log->data = data;

        // Adiciona à lista global de MemoryLogs
        add_memory_log(*log);

        return 1;
    }
    return 0;
}

void process_log_file(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("Erro ao abrir arquivo");
        return;
    }

    char line[1024];
    int line_index = 0;

    while (fgets(line, sizeof(line), file))
    {
        char log_type = get_log_type(line);

        if (log_type == 'L')
        {
            LoadLog log;
            if (parse_load_log(line, line_index, &log))
            {
                // printf("[L] %s | 0x%" PRIx64 " → 0x%" PRIx64 "\n",
                //        log.file_name, log.from_address, log.to_address);
            }
        }
        else if (log_type == 'B')
        {
            BlockLog log;
            if (parse_block_log(line, line_index, &log))
            {
                // printf("[B] ID: %s | Thread: 0x%" PRIx64 " | 0x%" PRIx64 " → 0x%" PRIx64 "\n",
                //        log.id, log.thread_id, log.start_address, log.end_address);
            }
        }
        else if (log_type == 'M')
        {
            MemoryLog log;
            if (parse_memory_log(line, line_index, &log))
            {
                // printf("[M] Exec: %s | Addr: 0x%" PRIx64 " | Mode: %c | Data: 0x%" PRIx64 "\n",
                //        log.exec_id, log.start_address, log.mode, log.data);
            }
        }

        line_index++;
    }

    fclose(file);
}

int processar_elf(const char *elf_filename)
{
    FILE *file = fopen(elf_filename, "r");
    if (!file)
    {
        perror("Erro ao abrir arquivo ELF");
        return 0;
    }

    char line[512];
    uint64_t text_virtual_address = 0;
    uint64_t main_offset = 0;

    while (fgets(line, sizeof(line), file))
    {
        // Procurar a linha da seção .text
        if (strstr(line, ".text") && strstr(line, "PROGBITS"))
        {
            sscanf(line, "%*s %*s %*s %" SCNx64, &text_virtual_address);
        }

        // Procurar a linha da função main
        if (strstr(line, " main"))
        {
            sscanf(line, "%*d: %" SCNx64, &main_offset);
        }
    }

    fclose(file);

    if (text_virtual_address == 0 || main_offset == 0)
    {
        fprintf(stderr, "Não foi possível localizar .text ou main no ELF\n");
        return 0;
    }

    // Calcular endereço de execução da main (offset relativo + endereço carregado)
    endereco_main_carregado = (main_offset - text_virtual_address) + load_main_log.from_address;

    printf("[INFO] Endereço main original: 0x%" PRIx64 "\n", main_offset);
    printf("[INFO] Endereço .text:         0x%" PRIx64 "\n", text_virtual_address);
    printf("[INFO] Endereço main carregado: 0x%" PRIx64 "\n", endereco_main_carregado);

    return 1;
}

void analisar_logs_main()
{
    printf("\n=== Analisando Logs da função main ===\n");

    printf("\n--- BlockLogs dentro da main ---\n");
    for (size_t i = 0; i < block_log_count; i++)
    {
        uint64_t start = block_logs[i].start_address;
        uint64_t end = block_logs[i].end_address;

        if ((start >= endereco_main_carregado && start <= load_main_log.to_address) ||
            (end >= endereco_main_carregado && end <= load_main_log.to_address))
        {
            printf("[Block %zu] ID: %s, Thread: 0x%" PRIx64 ", Start: 0x%" PRIx64 ", End: 0x%" PRIx64 "\n",
                   i, block_logs[i].id, block_logs[i].thread_id, start, end);
        }
    }

    printf("\n--- MemoryLogs dentro da main ---\n");
    for (size_t i = 0; i < memory_log_count; i++)
    {
        uint64_t ins = memory_logs[i].instruction_address;

        if (ins >= endereco_main_carregado && ins <= load_main_log.to_address)
        {
            printf("[Memory %zu] ExecID: %s, INS_ADDR: 0x%" PRIx64 ", START: 0x%" PRIx64 ", MODE: %c\n",
                   i, memory_logs[i].exec_id, ins,
                   memory_logs[i].start_address, memory_logs[i].mode);
        }
    }
}

void analisar_logs_main_no_offset()
{
    printf("\n=== Analisando Logs da função main (sem offset ELF) ===\n");

    printf("\n--- BlockLogs dentro do intervalo do binário ---\n");
    for (size_t i = 0; i < block_log_count; i++)
    {
        uint64_t start = block_logs[i].start_address;
        uint64_t end = block_logs[i].end_address;

        // Verifica se há interseção do bloco com o intervalo do binário
        if ((start >= load_main_log.from_address && start <= load_main_log.to_address) ||
            (end >= load_main_log.from_address && end <= load_main_log.to_address) ||
            (start <= load_main_log.from_address && end >= load_main_log.to_address)) // bloco englobando o intervalo todo
        {
            printf("[Block %zu] ID: %s, Thread: 0x%" PRIx64 ", Start: 0x%" PRIx64 ", End: 0x%" PRIx64 "\n",
                   i, block_logs[i].id, block_logs[i].thread_id, start, end);
        }
    }

    printf("\n--- MemoryLogs dentro do intervalo do binário ---\n");
    for (size_t i = 0; i < memory_log_count; i++)
    {
        uint64_t ins = memory_logs[i].instruction_address;

        if (ins >= load_main_log.from_address && ins <= load_main_log.to_address)
        {
            printf("[Memory %zu] ExecID: %s, INS_ADDR: 0x%" PRIx64 ", START: 0x%" PRIx64 ", MODE: %c\n",
                   i, memory_logs[i].exec_id, ins,
                   memory_logs[i].start_address, memory_logs[i].mode);
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Uso: %s <arquivo.texttrace> <arquivo.elf>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *texttrace_filename = argv[1];
    const char *elf_filename = argv[2];

    // Processar o .texttrace
    process_log_file(texttrace_filename);

    // Processar o .elf
    if (!processar_elf(elf_filename))
    {
        fprintf(stderr, "Erro ao processar o arquivo ELF: %s\n", elf_filename);
        return EXIT_FAILURE;
    }

    // analisar_logs_main();
    analisar_logs_main_no_offset();

    // Printar os logs
    // printf("\n=== Load Logs (%zu entradas) ===\n", load_log_count);
    // for (size_t i = 0; i < load_log_count; i++)
    // {
    //     printf("[%zu] file: %s, from: 0x%016" PRIx64 ", to: 0x%016" PRIx64 "\n",
    //            i, load_logs[i].file_name, load_logs[i].from_address, load_logs[i].to_address);
    // }

    // printf("\n=== Block Logs (%zu entradas) ===\n", block_log_count);
    // for (size_t i = 0; i < block_log_count; i++)
    // {
    //     printf("[%zu] id: %s, thread: 0x%" PRIx64 ", start: 0x%" PRIx64 ", end: 0x%" PRIx64 "\n",
    //            i, block_logs[i].id, block_logs[i].thread_id, block_logs[i].start_address, block_logs[i].end_address);
    // }

    // printf("\n=== Memory Logs (%zu entradas) ===\n", memory_log_count);
    // for (size_t i = 0; i < memory_log_count; i++)
    // {
    //     printf("[%zu] exec_id: %s, ins_addr: 0x%" PRIx64 ", start: 0x%" PRIx64
    //            ", length: %d, mode: %c, data: 0x%" PRIx64 "\n",
    //            i, memory_logs[i].exec_id, memory_logs[i].instruction_address,
    //            memory_logs[i].start_address, memory_logs[i].length,
    //            memory_logs[i].mode, memory_logs[i].data);
    // }

    // Liberar memória
    free(load_logs);
    free(block_logs);
    free(memory_logs);

    return EXIT_SUCCESS;
}
